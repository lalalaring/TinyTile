
#include "Types.h"
#include "Token.h"
#include <vector>

struct TokenInfo {
	TokenType    type;
	std::string   value_s;
	union number
	{
		s64         n;
		f64			f;
	}value_n;
	int           line;
};
typedef std::vector<TokenInfo> Tokens;

class CLexer
{
public:
	void        LoadFile(char* file);
	void        DumpTokes();

	TokenInfo   GetToken();
	TokenInfo   NextToken();
	CLexer&     SkipToken();
	void        Expect(TokenType type);

private:
	char*       Trim(char* buf);
	char*       ScanToken(char* file_buf, TokenInfo& token);

public:
	Tokens            m_Tokes;
	Tokens::iterator  m_CurToken;
	int               m_Lines;
};

inline bool token_is_set(TokenInfo& token)
{
	return token.type >= TokenType::SET && token.type <= TokenType::XORSET;
}

inline bool token_is_comp(TokenInfo& token)
{
	return token.type >= TokenType::EQUAL && token.type <= TokenType::GEQUAL;
}
