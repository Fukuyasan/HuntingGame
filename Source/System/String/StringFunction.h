#pragma once
#include <string>

static std::string sprit(const std::string& str, const char* delim)
{
	std::string spritStr = str;

	// ��؂镶�����Ȃ���΂��̂܂ܕԂ�
	if (delim == "") return spritStr;

	// ��؂肽�������̏o���ʒu���擾
	size_t delimIndex = spritStr.find(delim);
	if (delimIndex == std::string::npos) return spritStr;

	// ����ȍ~�̕������폜
	std::string delimedStr = spritStr.erase(delimIndex);

	return delimedStr;
}
