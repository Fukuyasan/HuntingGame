#pragma once
#include <string>

static std::string sprit(const std::string& str, const char* delim)
{
	std::string spritStr = str;

	// 区切る文字がなければそのまま返す
	if (delim == "") return spritStr;

	// 区切りたい文字の出現位置を取得
	size_t delimIndex = spritStr.find(delim);
	if (delimIndex == std::string::npos) return spritStr;

	// それ以降の文字を削除
	std::string delimedStr = spritStr.erase(delimIndex);

	return delimedStr;
}
