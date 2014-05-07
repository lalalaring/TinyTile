#pragma once

enum class TokenType
{
	VAR,
	FUNCTION,
	WHILE,
	FOR,
	IF,
	ELSE,
	RETURN,
	COMMA,		//,
	SEMICOLON,	//;
	LPAREN,		//(
	RPAREN,		//)
	LCURLY,		//{
	RCURLY,		//}
	ADD,
	SUB,
	MUL,
	DIV,
	MOD,		//%
	AND,		//&
	OR,			//|
	NOT,		//!
	ANDAND,		//&&
	OROR,		//||
	XOR,		//^
	LEFTSHIFT,	//<<
	RIGHTSHIFT,	//>>

	EQUAL,
	NOTEQUAL,
	LESS,
	GREAT,
	LEQUAL,
	GEQUAL,

	INC,		//++
	DEC,		//--
	//SIG,			//-

	SET,
	ADDSET,
	SUBSET,
	MULSET,
	DIVSET,
	MODSET,
	ANDSET,
	ORSET,
	XORSET,

	IDENTIFIER,
	NUMBER,
	STRING,
	UNKNOWN_TOKEN,
	TEOF,
	MAX_SYMBOL

};