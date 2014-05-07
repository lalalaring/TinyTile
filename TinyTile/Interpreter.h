#ifndef _INTERPRETER_H
#define _INTERPRETER_H

#include "Types.h"
#include <vector>
#include <stack>
#include <deque>
#include <hash_map>
#include "Opcode.h"
#include "Function.h"
struct ByteCode
{
	OPCODE      opcode;
	Value       v;
};

class CInterpreter
{
public:
	CInterpreter();

public:
	size_t  Generate(ByteCode& code);
	void    FixJMP(size_t pc, size_t dst);
	void    Run();
	void    Execute(ByteCode& code);

	void    Reset();
	void    Pause();
	void    Resume();

private:

private:

	void Handle_OP_LD(ByteCode& code);
	void Handle_OP_LDC(ByteCode& code);
	void Handle_OP_SV(ByteCode& code);
	void Handle_OP_POP(ByteCode& code);
	void Handle_OP_ADD(ByteCode& code);
	void Handle_OP_SUB(ByteCode& code);
	void Handle_OP_MUL(ByteCode& code);
	void Handle_OP_DIV(ByteCode& code);
	void Handle_OP_MOD(ByteCode& code);
	void Handle_OP_CALL(ByteCode& code);
	void Handle_OP_CMP(ByteCode& code);
	void Handle_OP_JMP(ByteCode& code);
	void Handle_OP_JCC(ByteCode& code);
	void Handle_OP_NOP(ByteCode& code);
	void Handle_OP_RET(ByteCode& code);
	void Handle_OP_MAX(ByteCode& code);
	void CallFunction(std::string& name, std::deque<Value>& params);

	void print(std::deque<Value>& params);

private:
	std::vector<ByteCode>   m_ByteCode;
	std::stack<Value>       m_Stack;
	void                    (CInterpreter::*m_Handler[OP_MAX+1])(ByteCode&); //!!可能改变
	std::hash_map<std::string, void (CInterpreter::*)(std::deque<Value>&)> m_InternalFunctions;
	size_t                  m_PC;
	bool                    m_Paused;
};

#endif  // _INTERPRETER_H
