#pragma once
#include <string>

static std::string sprit(std::string& str, const char* delim)
{
	// 区切る文字がなければそのまま返す
	if (delim == "") return str;

	// 区切りたい文字の出現位置を取得
	size_t delimIndex = str.find(delim);
	if (delimIndex == -1) return str;

	// それ以降の文字を削除
	std::string delimedStr = str.erase(delimIndex);

	return delimedStr;
}
