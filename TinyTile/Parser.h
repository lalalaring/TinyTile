#ifndef _PARSER_H
#define _PARSER_H

#include "Types.h"
#include <memory>
#include "Opcode.h"
class  CLexer;
class  CInterpreter;
struct TokenInfo;
class CParser
{
public:
	CParser(std::shared_ptr<CLexer> lexer, std::shared_ptr<CInterpreter> vm);

public:
	void      Parse();

private:
	void      declaration_();
	void      function_params_();
	void      function_params_def_();

	void      function_();

	void      stmt_();
	void      if_();
	void      while_();
	void      return_();

	void      expr_();
	void      term_();
	void      more_terms_();
	void      factor_();
	void      more_factors_();

private:
	size_t    GenerateCode(OPCODE op, u32 op_type, s64 op_val, const char* token_info);
	void      FixJMP(size_t pc, size_t dst);

private:
	std::shared_ptr<CLexer>         m_Lexer;
	std::shared_ptr<CInterpreter>   m_VM;
	bool                            m_Done;

	std::string                     m_FunctionName;
};

#endif  // _PARSER_H
