#include "Parser.h"
#include "Lex.h"
#include "Symbol.h"
#include "Interpreter.h"
#include "ERROR.h"
#include "Opcode.h"
CParser::CParser(std::shared_ptr<CLexer> lexer, std::shared_ptr<CInterpreter> vm) :
m_Lexer(lexer),
m_VM(vm),
m_Done(false)
{
}

size_t CParser::GenerateCode(OPCODE op, u32 op_type, s64 op_val, const char* token_info)
{

	FILE* fp = fopen("bytecode.txt", "a+");
	fprintf(fp, "%d\t%f\t%s\n", (u32)op, op_val, token_info);
	fclose(fp);

	ByteCode bc = { (OPCODE)op, (ValueType)op_type };
	if (op_type == VT_NUMBER) {
		bc.v.n.i = op_val;
	}
	else {
		
		bc.v.s = (op_val == 0) ? "" : (char*)op_val;
	}
	return m_VM->Generate(bc);
}

void CParser::FixJMP(size_t pc, size_t dst)
{
	m_VM->FixJMP(pc, dst);
}

void CParser::declaration_()
{
	m_Lexer->SkipToken();
	// var id1, id2 ... ;
	while (1) {
		TokenInfo token = m_Lexer->GetToken();
		if (token.type != TokenType::IDENTIFIER) {
			ReportError(token.line, "Invalid identifier found\n");
			break;
		}
		g_SymbolMgr.AddSymbol(token.value_s);
		m_Lexer->Expect(TokenType::IDENTIFIER);

		TokenInfo id = token;
		token = m_Lexer->GetToken();
		if (token.type == TokenType::SET) {
			// id = expr
			m_Lexer->Expect(TokenType::SET);
			expr_();
			GenerateCode(OPCODE::OP_SV, VT_NUMBER, g_SymbolMgr.GetSymbolIndex(id.value_s), id.value_s.c_str());
		}

		token = m_Lexer->GetToken();
		if (token.type == TokenType::SEMICOLON) {
			break;
		}
		else if (token.type != TokenType::COMMA) {
			ReportError(token.line, "Invalid identifier found\n");
			break;
		}
		m_Lexer->Expect(TokenType::COMMA);
	}
	m_Lexer->Expect(TokenType::SEMICOLON);
}

void CParser::function_params_()
{
	int param_count = 0;
	TokenInfo token = m_Lexer->GetToken();
	if (token.type == TokenType::RPAREN) {
		goto Exit;
	}

	// id1, id2, ... ;
	while (1) {
		expr_();

		param_count++;

		TokenInfo cur_token = m_Lexer->GetToken();
		if (cur_token.type == TokenType::RPAREN) {
			break;
		}
		else if (cur_token.type != TokenType::COMMA) {
			ReportError(cur_token.line, "Invalid identifier found\n");
		}
		m_Lexer->Expect(TokenType::COMMA);
	}

Exit:
	GenerateCode(OPCODE::OP_LDC, VT_NUMBER, param_count, "param count");
}

void CParser::function_params_def_()
{
	TokenInfo token = m_Lexer->GetToken();
	if (token.type == TokenType::RPAREN) {
		return;
	}

	// id1, id2, ... ;
	while (1) {
		TokenInfo cur_token = m_Lexer->GetToken();
		m_Lexer->Expect(TokenType::IDENTIFIER);

		//std::string name = m_FunctionName + "_" + cur_token.value_s;
		size_t index = g_SymbolMgr.AddSymbol(cur_token.value_s);
		GenerateCode(OPCODE::OP_SV, VT_NUMBER, (s64)index, cur_token.value_s.c_str());

		cur_token = m_Lexer->GetToken();
		if (cur_token.type == TokenType::RPAREN) {
			break;
		}
		else if (cur_token.type != TokenType::COMMA) {
			ReportError(cur_token.line, "Invalid identifier found\n");
		}
		m_Lexer->Expect(TokenType::COMMA);
	}
}

// function foo(id1, id2 ...)
// {
//   stmt ;
// }
void CParser::function_()
{
	size_t func_entry = GenerateCode(OPCODE::OP_JMP, VT_NUMBER, 0, "JMP");
	size_t func_begin = GenerateCode(OPCODE::OP_NOP, VT_NUMBER, 0, "NOP");

	m_Lexer->Expect(TokenType::FUNCTION);
	TokenInfo cur_token = m_Lexer->GetToken();
	m_FunctionName = cur_token.value_s;
	g_SymbolMgr.AddFunction(m_FunctionName, func_begin);
	m_Lexer->Expect(TokenType::IDENTIFIER);
	m_Lexer->Expect(TokenType::LPAREN);
	{
		function_params_def_();
	}
	m_Lexer->Expect(TokenType::RPAREN);

	m_Lexer->Expect(TokenType::LCURLY);
	{
		while (m_Lexer->GetToken().type != TokenType::RCURLY) {
			stmt_();
		}
	}
	m_Lexer->Expect(TokenType::RCURLY);
	m_FunctionName = "";

	size_t func_end = GenerateCode(OPCODE::OP_RET, VT_NUMBER, 0, "RET");
	size_t func_exit = GenerateCode(OPCODE::OP_NOP, VT_NUMBER, 0, "NOP");
	FixJMP(func_entry, func_exit);
}

void CParser::expr_()
{
	TokenInfo token = m_Lexer->GetToken();
	if (token.type == TokenType::SEMICOLON) {
		// Empty expression
		return;
	}
	else if (token.type == TokenType::IDENTIFIER) {
		// foo() or id2 = id1
		TokenInfo next_token = m_Lexer->NextToken();
		if (next_token.type == TokenType::LPAREN) {
			// id(param1, param2, param3, ...)  --  function call
			m_Lexer->Expect(TokenType::IDENTIFIER);
			m_Lexer->Expect(TokenType::LPAREN);
			{
				function_params_();
			}
			m_Lexer->Expect(TokenType::RPAREN);

			GenerateCode(OPCODE::OP_CALL, VT_STRING, (s64)token.value_s.c_str(), token.value_s.c_str());
			return;
		}
		else if (token_is_set(next_token)) {
			// id = expr | id += expr  --  set
			m_Lexer->Expect(TokenType::IDENTIFIER);
			m_Lexer->SkipToken();
			expr_();

			GenerateCode(OPCODE::OP_SV, VT_NUMBER, g_SymbolMgr.GetSymbolIndex(token.value_s), token.value_s.c_str());
			return;
		}
	}
	term_();
	more_terms_();
}

void CParser::term_()
{
	factor_();
	more_factors_();
}

void CParser::more_terms_()
{
	TokenInfo token = m_Lexer->GetToken();
	if (token.type == TokenType::ADD || token.type == TokenType::SUB) {
		m_Lexer->SkipToken();
		term_();
		more_terms_();

		OPCODE op = (token.type == TokenType::ADD) ? OPCODE::OP_ADD : OPCODE::OP_SUB;
		GenerateCode(op, VT_NUMBER, 0, "OP");
	}
}

void CParser::factor_()
{
	TokenInfo token = m_Lexer->GetToken();
	if (token.type == TokenType::LPAREN) {
		// '(' expr ')'
		m_Lexer->Expect(TokenType::LPAREN);
		{
			expr_();
		}
		m_Lexer->Expect(TokenType::RPAREN);
		return;
	}
	if (token.type == TokenType::IDENTIFIER || token.type == TokenType::NUMBER || token.type == TokenType::STRING) {
		m_Lexer->SkipToken();

		switch (token.type)
		{
		case TokenType::IDENTIFIER:
			GenerateCode(OPCODE::OP_LD, VT_NUMBER, g_SymbolMgr.GetSymbolIndex(token.value_s), token.value_s.c_str());
			break;
		case TokenType::NUMBER:
			GenerateCode(OPCODE::OP_LDC, VT_NUMBER, token.value_n.n, "number");
			break;
		case TokenType::STRING:
			GenerateCode(OPCODE::OP_LDC, VT_STRING, (s64)token.value_s.c_str(), token.value_s.c_str());
			break;
		}
		return;
	}
	ReportError(token.line, "Invalid factor\n");
}

void CParser::more_factors_()
{
	TokenInfo token = m_Lexer->GetToken();
	if (token.type == TokenType::MUL || token.type == TokenType::DIV || token.type == TokenType::MOD) {
		m_Lexer->SkipToken();
		factor_();
		more_factors_();

		OPCODE op = (token.type == TokenType::MUL) ? OPCODE::OP_MUL : ((token.type == TokenType::DIV) ? OPCODE::OP_DIV : OPCODE::OP_MOD);
		GenerateCode(op, 0, 0, "OP");
	}
}

void CParser::if_()
{
	// if '(' expr ')' '{' stmt '}'
	m_Lexer->Expect(TokenType::IF);
	m_Lexer->Expect(TokenType::LPAREN);
	{
		expr_();
		TokenInfo token = m_Lexer->GetToken();
		if (token_is_comp(token)) {
			m_Lexer->SkipToken();
			expr_();
			GenerateCode(OPCODE::OP_CMP, VT_NUMBER, (u32)token.type, "CMP");
		}
	}
	m_Lexer->Expect(TokenType::RPAREN);

	size_t pc_if = GenerateCode(OPCODE::OP_JCC, VT_NUMBER, 0, "JCC");

	m_Lexer->Expect(TokenType::LCURLY);
	{
		do {
			stmt_();
		} while (m_Lexer->GetToken().type != TokenType::RCURLY);
	}
	m_Lexer->Expect(TokenType::RCURLY);

	size_t pc_else = GenerateCode(OPCODE::OP_JMP, VT_NUMBER, 0, "JMP");

	size_t dst_if = GenerateCode(OPCODE::OP_NOP, VT_NUMBER, 0, "NOP");
	FixJMP(pc_if, dst_if);

	// ['else' '{' stmt '}']
	if (m_Lexer->GetToken().type == TokenType::ELSE) {
		m_Lexer->Expect(TokenType::ELSE);
		stmt_();
	}
	size_t dst_else = GenerateCode(OPCODE::OP_NOP, VT_NUMBER, 0, "NOP");
	FixJMP(pc_else, dst_else);
}

void CParser::while_()
{
	size_t dst_begin = GenerateCode(OPCODE::OP_NOP, VT_NUMBER, 0, "NOP");

	// while '(' expr ')' '{' stmt '}'
	m_Lexer->Expect(TokenType::WHILE);
	m_Lexer->Expect(TokenType::LPAREN);
	{
		expr_();
		TokenInfo token = m_Lexer->GetToken();
		if (token_is_comp(token)) {
			m_Lexer->SkipToken();
			expr_();
			GenerateCode(OPCODE::OP_CMP, VT_NUMBER, (u32)token.type, "CMP");
		}
	}
	m_Lexer->Expect(TokenType::RPAREN);

	size_t pc_while = GenerateCode(OPCODE::OP_JCC, VT_NUMBER, 0, "JCC");

	m_Lexer->Expect(TokenType::LCURLY);
	{
		do {
			stmt_();
		} while (m_Lexer->GetToken().type != TokenType::RCURLY);
	}
	m_Lexer->Expect(TokenType::RCURLY);
	GenerateCode(OPCODE::OP_JMP, VT_NUMBER, dst_begin, "JMP");

	size_t dst_end = GenerateCode(OPCODE::OP_NOP, VT_NUMBER, 0, "NOP");
	FixJMP(pc_while, dst_end);
}

void CParser::return_()
{
	m_Lexer->Expect(TokenType::RETURN);
	{
		TokenInfo token = m_Lexer->GetToken();
		if (token.type != TokenType::SEMICOLON) {
			expr_();
		}
	}
	m_Lexer->Expect(TokenType::SEMICOLON);
}

void CParser::stmt_()
{
	TokenInfo token = m_Lexer->GetToken();
	if (token.type == TokenType::LCURLY) {
		m_Lexer->Expect(TokenType::LCURLY);
		{
			stmt_();
		}
		m_Lexer->Expect(TokenType::RCURLY);
	}
	else if (token.type == TokenType::VAR) {
		declaration_();
	}
	else if (token.type == TokenType::IF) {
		if_();
	}
	else if (token.type == TokenType::WHILE) {
		while_();
	}
	else if (token.type == TokenType::RETURN) {
		return_();
	}
	else {
		expr_();
		m_Lexer->Expect(TokenType::SEMICOLON);
	}
}

void CParser::Parse()
{
	while (1) {
		TokenInfo token = m_Lexer->GetToken();
		if (token.type == TokenType::TEOF) {
			break;
		}
		if (token.type == TokenType::FUNCTION) {
			function_();
		}
		else {
			stmt_();
		}
	}
}
