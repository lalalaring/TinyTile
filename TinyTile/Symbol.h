#ifndef _SYMBOL_H
#define _SYMBOL_H

#include "Types.h"

#include <string>
#include <hash_map>

class CSymbolMap
{
public:
	size_t      AddSymbol(std::string& name);
	bool        SetValue(std::string& name, const Value& value);
	bool        SetValue(int index, Value v);
	Value       GetValue(int index);
	int         GetSymbolIndex(std::string& name);
	void        Clear();

	void        AddFunction(std::string& name, size_t index);
	size_t      GetFunction(std::string& name);

private:
	std::hash_map<std::string, int>   m_SymbolIndex, m_Functions;
	std::vector<Value>                m_Symbols;
};
extern CSymbolMap g_SymbolMgr;

#endif  // _SYMBOL_H
