#include "Interpreter.h"
#include "Symbol.h"
#include "Lex.h"
#include "ERROR.h"

#define GET_STACK_TOP() m_Stack.top(); m_Stack.pop();

CInterpreter::CInterpreter() : m_PC(0), m_Paused(false)
{

	m_Handler[OPCODE::OP_LD] = &CInterpreter::Handle_OP_LD;
	m_Handler[OPCODE::OP_LDC] = &CInterpreter::Handle_OP_LDC;
	m_Handler[OPCODE::OP_SV] = &CInterpreter::Handle_OP_SV;
	m_Handler[OPCODE::OP_POP] = &CInterpreter::Handle_OP_POP;
	m_Handler[OPCODE::OP_ADD] = &CInterpreter::Handle_OP_ADD;
	m_Handler[OPCODE::OP_SUB] = &CInterpreter::Handle_OP_SUB;
	m_Handler[OPCODE::OP_MUL] = &CInterpreter::Handle_OP_MUL;
	m_Handler[OPCODE::OP_DIV] = &CInterpreter::Handle_OP_DIV;
	m_Handler[OPCODE::OP_MOD] = &CInterpreter::Handle_OP_MOD;
	m_Handler[OPCODE::OP_CALL] = &CInterpreter::Handle_OP_CALL;
	m_Handler[OPCODE::OP_CMP] = &CInterpreter::Handle_OP_CMP;
	m_Handler[OPCODE::OP_JMP] = &CInterpreter::Handle_OP_JMP;
	m_Handler[OPCODE::OP_JCC] = &CInterpreter::Handle_OP_JCC;
	m_Handler[OPCODE::OP_NOP] = &CInterpreter::Handle_OP_NOP;
	m_Handler[OPCODE::OP_RET] = &CInterpreter::Handle_OP_RET;
	m_Handler[OPCODE::OP_MAX] = &CInterpreter::Handle_OP_MAX;

	m_InternalFunctions["print"] = &CInterpreter::print;
}

size_t CInterpreter::Generate(ByteCode& code)
{
	m_ByteCode.push_back(code);
	return m_ByteCode.size() - 1;
}

void CInterpreter::FixJMP(size_t pc, size_t dst)
{
	if (pc >= m_ByteCode.size() || dst >= m_ByteCode.size()) {
		return;
	}
	m_ByteCode[pc].v.n.i = dst;
}

void CInterpreter::Run()
{
	while (1) {
		if (m_Paused) {
			continue;
		}

		if (m_PC >= m_ByteCode.size()) {
			break;
		}

		ByteCode code = m_ByteCode[m_PC];
		Execute(code);
		m_PC++;
	}
}

void CInterpreter::Execute(ByteCode& code)
{
	if (code.opcode < 0 || code.opcode >= OPCODE::OP_MAX) {
		return;
	}
	(this->*(m_Handler[code.opcode]))(code);
}

void CInterpreter::Reset()
{
	m_PC = 0;
	m_Paused = false;
	while (!m_Stack.empty()) {
		m_Stack.pop();
	}
}

void CInterpreter::Pause()
{
	m_Paused = true;
}

void CInterpreter::Resume()
{
	m_Paused = false;
}

//////////////////////////////////////////////////////////////////////////
void CInterpreter::Handle_OP_LD(ByteCode& code)
{
	Value v = g_SymbolMgr.GetValue((int)code.v.n.i);
	if (v.type == VT_UNKNOWN) {
		ReportError(-1, "Unknown variable\n");
	}
	m_Stack.push(v);
}

void CInterpreter::Handle_OP_LDC(ByteCode& code)
{
	m_Stack.push(code.v);
}

void CInterpreter::Handle_OP_SV(ByteCode& code)
{
	Value v = GET_STACK_TOP();
	if (!g_SymbolMgr.SetValue((int)code.v.n.i, v)) {
		ReportError(-1, "Unknown variable\n");
	}
}

void CInterpreter::Handle_OP_POP(ByteCode& code)
{
	m_Stack.pop();
}

void CInterpreter::Handle_OP_ADD(ByteCode& code)
{
	if (m_Stack.size() < 2) {
		ReportError(-1, "Invalid expression\n");
	}

	Value v1 = GET_STACK_TOP();
	Value v2 = GET_STACK_TOP();

	if (v1.type == VT_UNKNOWN || v2.type == VT_UNKNOWN || v1.type != v2.type) {
		ReportError(-1, "Invalid variable value\n");
	}
	Value v = { v1.type };
	if (v1.type == VT_STRING && v2.type == VT_STRING) {
		v.s = v2.s + v1.s;
	}
	else {
		v.n.i = v1.n.i + v2.n.i;
	}
	m_Stack.push(v);
}

#define OP_ARITH(op) \
	if (m_Stack.size() < 2) { \
	ReportError(-1, "Invalid expression\n"); \
	} \
	\
	Value v1 = GET_STACK_TOP(); \
	Value v2 = GET_STACK_TOP(); \
	\
	if (v1.type != VT_NUMBER || v1.type != v2.type) { \
	ReportError(-1, "Invalid variable value\n"); \
	} \
	Value v = { v1.type }; \
	v.n.i = v2.n.i op v1.n.i; \
	m_Stack.push(v);

void CInterpreter::Handle_OP_SUB(ByteCode& code)
{
	OP_ARITH(-);
}

void CInterpreter::Handle_OP_MUL(ByteCode& code)
{
	OP_ARITH(*);
}

void CInterpreter::Handle_OP_DIV(ByteCode& code)
{
	OP_ARITH(/ );
}

void CInterpreter::Handle_OP_MOD(ByteCode& code)
{
	OP_ARITH(%);
}

void CInterpreter::Handle_OP_CALL(ByteCode& code)
{
	Value v = GET_STACK_TOP();
	if (v.type != VT_NUMBER) {
		ReportError(-1, "Invalid call\n");
	}

	std::deque<Value> tmp;
	for (int i = 0; i < (int)v.n.i; i++) {
		Value v_ = GET_STACK_TOP();
		tmp.push_front(v_);
	}

	CallFunction(code.v.s, tmp);
}
//Fuction fu = new Fuction();
void CInterpreter::CallFunction(std::string& name, std::deque<Value>& params)
{
	auto it = m_InternalFunctions.find(name);
	if (it == m_InternalFunctions.end()) {
		// call others
		size_t f = g_SymbolMgr.GetFunction(name);
		if (f == -1) {
			ReportError(-1, "Cannot find function: %s", name.c_str());
		}
		Value ret_addr = { VT_NUMBER, m_PC };
		m_Stack.push(ret_addr);
		for (int i = (int)params.size() - 1; i >= 0; i--) {
			m_Stack.push(params[i]);
		}
		m_PC = f;
		return;
	}
	//(fu->*(it->second))(params);
}

void CInterpreter::Handle_OP_CMP(ByteCode& code)
{
	Value v2 = GET_STACK_TOP();
	Value v1 = GET_STACK_TOP();
	if (v1.type != v2.type) {
		ReportError(-1, "Mismatching type.");
	}
	bool is_number = (v1.type == VT_NUMBER);
	int res = 0;
	switch (code.v.n.i)
	{
	case (s64)TokenType::EQUAL:
		res = (is_number ? v1.n.i == v2.n.i : v1.s == v2.s) ? 1 : 0;
		break;
	case (s64)TokenType::NOTEQUAL:
		res = (is_number ? v1.n.i != v2.n.i : v1.s != v2.s) ? 1 : 0;
		break;
	case (s64)TokenType::LESS:
		res = (is_number ? v1.n.i < v2.n.i : v1.s < v2.s) ? 1 : 0;
		break;
	case (s64)TokenType::GREAT:
		res = (is_number ? v1.n.i > v2.n.i : v1.s > v2.s) ? 1 : 0;
		break;
	case (s64)TokenType::LEQUAL:
		res = (is_number ? v1.n.i <= v2.n.i : v1.s <= v2.s) ? 1 : 0;
		break;
	case (s64)TokenType::GEQUAL:
		res = (is_number ? v1.n.i >= v2.n.i : v1.s >= v2.s) ? 1 : 0;
		break;
	}
	Value v = { VT_NUMBER };
	v.n.i = res;
	m_Stack.push(v);
}

void CInterpreter::Handle_OP_JMP(ByteCode& code)
{
	m_PC = (size_t)code.v.n.i;
}

void CInterpreter::Handle_OP_JCC(ByteCode& code)
{
	Value v = GET_STACK_TOP();
	bool res = false;
	if (v.type == VT_NUMBER && v.n.i) {
		res = true;
	}
	if (v.type == VT_STRING && v.s != "") {
		res = true;
	}
	if (!res) {
		m_PC = (size_t)code.v.n.i;
	}
}

void CInterpreter::Handle_OP_NOP(ByteCode& code)
{
}

void CInterpreter::Handle_OP_RET(ByteCode& code)
{
	Value v = GET_STACK_TOP();
	m_PC = (size_t)v.n.i;
}

void CInterpreter::Handle_OP_MAX(ByteCode& code)
{
#if 0
	fuck off
#endif
}


void CInterpreter::print(std::deque<Value>& params)
{
	if (params.size() < 1) {
		return;
	}

	for (size_t i = 0; i < params.size(); i++) {
		Value& v = params[i];
		if ((v.type == VT_STRING)) {
			std::cout << v.s;
		}
		else {
			std::cout << v.n.i;
		}
	}
	std::cout << std::endl;
}




