#pragma once

enum OPCODE
{
	OP_LD,    // load variable to stack
	OP_LDC,  // load constant values to stack
	OP_SV,    // save stack value to variable
	OP_POP,   // pop a value from the stack
	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_DIV,
	OP_MOD,
	OP_CALL,
	OP_CMP,
	OP_JMP,
	OP_JCC,
	OP_NOP,
	OP_RET,
	OP_MAX
};