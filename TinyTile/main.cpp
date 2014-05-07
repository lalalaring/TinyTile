#include "Lex.h"
#include "Interpreter.h"
#include "Parser.h"
int main()
{
	std::shared_ptr<CLexer>       m_Lexer;
	std::shared_ptr<CParser>      m_Parser;
	std::shared_ptr<CInterpreter> m_VM;
	m_Lexer.reset(new CLexer);
	m_VM.reset(new CInterpreter);
	m_Parser.reset(new CParser(m_Lexer, m_VM));
	m_Lexer->LoadFile("1.txt");
	m_Lexer->DumpTokes();
	m_Parser->Parse();
	m_VM->Run();
	system("pause");
	return 0;
}