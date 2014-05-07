#include "Symbol.h"

CSymbolMap g_SymbolMgr;

size_t CSymbolMap::AddSymbol(std::string& name)
{
	Value symbol = { VT_UNKNOWN };
	m_Symbols.push_back(symbol);
	size_t index = m_Symbols.size() - 1;
	m_SymbolIndex[name] = index;
	return index;
}

bool CSymbolMap::SetValue(std::string& name, const Value& value)
{
	auto it = m_SymbolIndex.find(name);
	if (it == m_SymbolIndex.end()) {
		return false;
	}
	m_Symbols[it->second] = value;
	return true;
}

Value CSymbolMap::GetValue(int index)
{
	Value v = { VT_UNKNOWN };
	if (index < 0 || index >(int)m_Symbols.size()) {
		return v;
	}
	return m_Symbols[index];
}

bool CSymbolMap::SetValue(int index, Value v)
{
	if (index < 0 || index >(int)m_Symbols.size()) {
		return false;
	}
	m_Symbols[index] = v;
	return true;
}

int CSymbolMap::GetSymbolIndex(std::string& name)
{
	auto it = m_SymbolIndex.find(name);
	if (it == m_SymbolIndex.end()) {
		return -1;
	}
	return it->second;
}

void CSymbolMap::Clear()
{
	m_Symbols.clear();
	m_SymbolIndex.clear();
	m_Functions.clear();
}

void CSymbolMap::AddFunction(std::string& name, size_t index)
{
	m_Functions[name] = index;
}

size_t CSymbolMap::GetFunction(std::string& name)
{
	auto it = m_Functions.find(name);
	if (it != m_Functions.end()) {
		return it->second;
	}
	return -1;
}
