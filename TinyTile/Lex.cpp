#include "Lex.h"
#include <sstream>
#include "ERROR.h"
//#include <math.h>
#define IS_ALPHA(c)  (c == '_' || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
#define IS_NUMBER(c) (c >= '0' && c <= '9')

char* CLexer::Trim(char* buf)
{
	while (*buf == ' ' ||
		*buf == '\t' ||
		*buf == '\r' ||
		*buf == '\n') {
		if (*buf == '\n') {
			m_Lines++;
		}
		buf++;
	}
	return buf;
}

char* CLexer::ScanToken(char* file_buf, TokenInfo& token)
{

	char* buf = Trim(file_buf);
	if (*buf == 0) {
		token.type = TokenType::TEOF;
		token.line = m_Lines;
		return buf;
	}
	int i = 0, d = 0;
	switch (*buf)
	{
	case ',':
		token.type = TokenType::COMMA;
		token.value_s.push_back(*buf++);
		break;

	case ';':
		token.type = TokenType::SEMICOLON;
		token.value_s.push_back(*buf++);
		break;

	case '(':
		token.type = TokenType::LPAREN;
		token.value_s.push_back(*buf++);
		break;

	case ')':
		token.type = TokenType::RPAREN;
		token.value_s.push_back(*buf++);
		break;

	case '{':
		token.type = TokenType::LCURLY;
		token.value_s.push_back(*buf++);
		break;

	case '}':
		token.type = TokenType::RCURLY;
		token.value_s.push_back(*buf++);
		break;
	case '+':
		if (buf[1] == '=') {
			token.type = TokenType::ADDSET;
			token.value_s.push_back(*buf++);
			token.value_s.push_back(*buf++);
		}
		else if (buf[1] == '+') {
			token.type = TokenType::INC;
			token.value_s.push_back(*buf++);
			token.value_s.push_back(*buf++);
		}
		else {
			token.type = TokenType::ADD;
			token.value_s.push_back(*buf++);
		}
		break;

	case '-':
		if (buf[1] == '=') {
			token.type = TokenType::SUBSET;
			token.value_s.push_back(*buf++);
			token.value_s.push_back(*buf++);
		}
		else if (buf[1] == '-') {
			token.type = TokenType::DEC;
			token.value_s.push_back(*buf++);
			token.value_s.push_back(*buf++);
		}
		//else if (IS_NUMBER(buf[1]))
		//{
		//	//Õý¸ººÅ
		//	//token.type = TokenType::SIG;
		//	//token.value_s.push_back(*buf++);
		//	//token.value_s.push_back(*buf++);
		//}
		else {
			token.type = TokenType::SUB;
			token.value_s.push_back(*buf++);
		}
		break;

	case '*':
		if (buf[1] == '=') {
			token.type = TokenType::MULSET;
			token.value_s.push_back(*buf++);
			token.value_s.push_back(*buf++);
		}
		else {
			token.type = TokenType::MUL;
			token.value_s.push_back(*buf++);
		}
		break;

	case '/':
		if (buf[1] == '=') {
			token.type = TokenType::DIVSET;
			token.value_s.push_back(*buf++);
			token.value_s.push_back(*buf++);
		}
		else {
			token.type = TokenType::DIV;
			token.value_s.push_back(*buf++);
		}
		break;

	case '%':
		if (buf[1] == '=') {
			token.type = TokenType::MODSET;
			token.value_s.push_back(*buf++);
			token.value_s.push_back(*buf++);
		}
		else {
			token.type = TokenType::MOD;
			token.value_s.push_back(*buf++);
		}
		break;

	case '&':
		if (buf[1] == '=') {
			token.type = TokenType::ANDSET;
			token.value_s.push_back(*buf++);
			token.value_s.push_back(*buf++);
		}
		else if (buf[1] == '&') {
			token.type = TokenType::ANDAND;
			token.value_s.push_back(*buf++);
			token.value_s.push_back(*buf++);
		}
		else {
			token.type = TokenType::AND;
			token.value_s.push_back(*buf++);
		}
		break;

	case '|':
		if (buf[1] == '=') {
			token.type = TokenType::ORSET;
			token.value_s.push_back(*buf++);
			token.value_s.push_back(*buf++);
		}
		else if (buf[1] == '&') {
			token.type = TokenType::OROR;
			token.value_s.push_back(*buf++);
			token.value_s.push_back(*buf++);
		}
		else {
			token.type = TokenType::OR;
			token.value_s.push_back(*buf++);
		}
		break;

	case '^':
		if (buf[1] == '=') {
			token.type = TokenType::XORSET;
			token.value_s.push_back(*buf++);
			token.value_s.push_back(*buf++);
		}
		else {
			token.type = TokenType::XOR;
			token.value_s.push_back(*buf++);
		}
		break;

	case '!':
		if (buf[1] == '=') {
			token.type = TokenType::NOTEQUAL;
			token.value_s.push_back(*buf++);
			token.value_s.push_back(*buf++);
		}
		else {
			token.type = TokenType::NOT;
			token.value_s.push_back(*buf++);
		}
		break;

	case '=':
		if (buf[1] == '=') {
			token.type = TokenType::EQUAL;
			token.value_s.push_back(*buf++);
			token.value_s.push_back(*buf++);
		}
		else {
			token.type = TokenType::SET;
			token.value_s.push_back(*buf++);
		}
		break;

	case '"':
	{
		token.type = TokenType::STRING;
		int str_len = 0;
		buf++;
		while (*buf != '"') {
			token.value_s.push_back(*buf++);
		}
		buf++;
	}
		break;

	case '\'':
	{
		token.type = TokenType::NUMBER;
		int str_len = 0;
		char cur_ch = 0;
		s64 value = 0;
		buf++;
		while (*buf != '\'') {
			if (*buf == '\\') {
				// escape character
				buf++;
				if (*buf == 't') {
					cur_ch = '\t';
				}
				else if (*buf == 'r') {
					cur_ch = '\r';
				}
				else if (*buf == 'n') {
					cur_ch = '\n';
				}
				else if (*buf == 'b') {
					cur_ch = ' ';
				}
				else {
					cur_ch = *buf;
				}
				buf++;
			}
			else {
				cur_ch = *buf++;
			}
			str_len++;
			value <<= 8;
			value |= cur_ch;
			if (str_len > 4) {
				ReportError(m_Lines, "Compiling error\n");
			}
		}
		token.value_n.n = value;
		std::stringstream st;
		st << "0x" << std::hex << token.value_n.n;
		token.value_s = st.str();
		buf++;
	}
		break;

	case '<':
		if (buf[1] == '<') {
			token.type = TokenType::LEFTSHIFT;
			token.value_s.push_back(*buf++);
			token.value_s.push_back(*buf++);
		}
		else if (buf[1] == '=') {
			token.type = TokenType::LEQUAL;
			token.value_s.push_back(*buf++);
			token.value_s.push_back(*buf++);
		}
		else {
			token.type = TokenType::LESS;
			token.value_s.push_back(*buf++);
		}
		break;

	case '>':
		if (buf[1] == '>') {
			token.type = TokenType::RIGHTSHIFT;
			token.value_s.push_back(*buf++);
			token.value_s.push_back(*buf++);
		}
		else if (buf[1] == '=') {
			token.type = TokenType::GEQUAL;
			token.value_s.push_back(*buf++);
			token.value_s.push_back(*buf++);
		}
		else {
			token.type = TokenType::GREAT;
			token.value_s.push_back(*buf++);
		}
		break;

	default:
		if (IS_ALPHA(*buf)) {
			if (buf[0] == 'e' &&
				buf[1] == 'l' &&
				buf[2] == 's' &&
				buf[3] == 'e' &&
				!IS_ALPHA(buf[4])) {
				token.value_s = "else";
				token.type = TokenType::ELSE;
				buf += 4;
			}
			else if (buf[0] == 'f' &&
				buf[1] == 'o' &&
				buf[2] == 'r' &&
				!IS_ALPHA(buf[3])) {
				token.value_s = "for";
				token.type = TokenType::FOR;
				buf += 3;
			}
			else if (buf[0] == 'f' &&
				buf[1] == 'u' &&
				buf[2] == 'n' &&
				buf[3] == 'c' &&
				buf[4] == 't' &&
				buf[5] == 'i' &&
				buf[6] == 'o' &&
				buf[7] == 'n' &&
				!IS_ALPHA(buf[8])) {
				token.value_s = "function";
				token.type = TokenType::FUNCTION;
				buf += 8;
			}
			else if (buf[0] == 'i' &&
				buf[1] == 'f' &&
				!IS_ALPHA(buf[2])) {
				token.value_s = "if";
				token.type = TokenType::IF;
				buf += 2;
			}
			else if (buf[0] == 'r' &&
				buf[1] == 'e' &&
				buf[2] == 't' &&
				buf[3] == 'u' &&
				buf[4] == 'r' &&
				buf[5] == 'n' &&
				!IS_ALPHA(buf[6])) {
				token.value_s = "return";
				token.type = TokenType::RETURN;
				buf += 6;
			}
			else if (buf[0] == 'v' &&
				buf[1] == 'a' &&
				buf[2] == 'r' &&
				!IS_ALPHA(buf[3])) {
				token.value_s = "var";
				token.type = TokenType::VAR;
				buf += 3;
			}
			else if (buf[0] == 'w' &&
				buf[1] == 'h' &&
				buf[2] == 'i' &&
				buf[3] == 'l' &&
				buf[4] == 'e' &&
				!IS_ALPHA(buf[5])) {
				token.value_s = "while";
				token.type = TokenType::WHILE;
				buf += 5;
			}
			else {
				token.type = TokenType::IDENTIFIER;
				while (IS_ALPHA(*buf) || IS_NUMBER(*buf)) {
					token.value_s.push_back(*buf++);
				}
			}
		}
		else if (IS_NUMBER(*buf)) {
			i = 0;
			if (buf[0] == '0' && (buf[1] == 'X' || buf[1] == 'x')) {
				buf += 2;
				while (1) {
					if (IS_NUMBER(*buf)) {
						d = *buf++ - '0';
					}
					else if (*buf >= 'A' && *buf <= 'Z') {
						d = *buf++ - 'A' + 10;
					}
					else if (*buf >= 'a' && *buf <= 'z') {
						d = *buf++ - 'a' + 10;
					}
					else {
						break;
					}
					i = (i << 4) + d;
				}
				buf--;
			}
			else {
				while (IS_NUMBER(*buf))
				{
					i = i * 10 + *buf++ - '0';		
				}
			}
			token.type = TokenType::NUMBER;
			token.value_n.n = i;
			std::stringstream st;
			st << "0x" << std::hex << token.value_n.n;
			token.value_s = st.str();
			break;
		}
		else {
			ReportError(m_Lines, "Compiling error\n");
		}
		break;
	}
	token.line = m_Lines;
	return buf;
}
void CLexer::LoadFile(char* file)
{
	m_Tokes.clear();
	m_Lines = 1;

	FILE* fp = fopen(file, "rb");
	if (fp == 0) {
		return;
	}

	fseek(fp, 0, SEEK_END);
	u32 file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char* fbuf = new char[file_size + 1];
	fread(fbuf, 1, file_size, fp);
	fbuf[file_size] = 0;

	char* buf = fbuf;
	do {
		TokenInfo token = { TokenType::UNKNOWN_TOKEN };
		buf = ScanToken(buf, token);
		m_Tokes.push_back(token);
	} while (*buf);

	delete fbuf;

	m_CurToken = m_Tokes.begin();
	fclose(fp);
}

void CLexer::DumpTokes()
{
	Tokens::iterator it = m_Tokes.begin();
	for (; it != m_Tokes.end(); it++) {
		printf("Line%04d: %d %s\n", it->line, it->type, it->value_s.c_str());
	}
}

TokenInfo CLexer::GetToken()
{
	TokenInfo token = { TokenType::TEOF };
	if (m_CurToken == m_Tokes.end()) {
		return token;
	}
	return *m_CurToken;
}

TokenInfo CLexer::NextToken()
{
	TokenInfo token = { TokenType::TEOF };
	if (m_CurToken == m_Tokes.end()) {
		return token;
	}
	auto it = m_CurToken;
	if (++it == m_Tokes.end()) {
		return token;
	}
	return *it;
}

CLexer& CLexer::SkipToken()
{
	m_CurToken++;
	return *this;
}

void CLexer::Expect(TokenType type)
{
	if (m_CurToken == m_Tokes.end()) {
		ReportError(m_CurToken->line, "Unexpected file end\n");
	}
	if (type != m_CurToken->type) {
		ReportError(m_CurToken->line, "Unexpected token: %s\n", m_CurToken->value_s);
	}
	m_CurToken++;
}