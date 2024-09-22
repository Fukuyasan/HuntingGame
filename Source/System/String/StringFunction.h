#pragma once
#include <string>

static std::string sprit(std::string& str, const char* delim)
{
	// ‹æØ‚é•¶š‚ª‚È‚¯‚ê‚Î‚»‚Ì‚Ü‚Ü•Ô‚·
	if (delim == "") return str;

	// ‹æØ‚è‚½‚¢•¶š‚ÌoŒ»ˆÊ’u‚ğæ“¾
	size_t delimIndex = str.find(delim);
	if (delimIndex == -1) return str;

	// ‚»‚êˆÈ~‚Ì•¶š‚ğíœ
	std::string delimedStr = str.erase(delimIndex);

	return delimedStr;
}
