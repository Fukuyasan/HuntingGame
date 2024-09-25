#pragma once
#include <string>

static std::string sprit(const std::string& str, const char* delim)
{
	std::string spritStr = str;

	// ‹æØ‚é•¶š‚ª‚È‚¯‚ê‚Î‚»‚Ì‚Ü‚Ü•Ô‚·
	if (delim == "") return spritStr;

	// ‹æØ‚è‚½‚¢•¶š‚ÌoŒ»ˆÊ’u‚ğæ“¾
	size_t delimIndex = spritStr.find(delim);
	if (delimIndex == std::string::npos) return spritStr;

	// ‚»‚êˆÈ~‚Ì•¶š‚ğíœ
	std::string delimedStr = spritStr.erase(delimIndex);

	return delimedStr;
}
