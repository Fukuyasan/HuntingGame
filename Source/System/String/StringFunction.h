#pragma once
#include <string>

static std::string sprit(std::string& str, const char* delim)
{
	// ��؂镶�����Ȃ���΂��̂܂ܕԂ�
	if (delim == "") return str;

	// ��؂肽�������̏o���ʒu���擾
	size_t delimIndex = str.find(delim);
	if (delimIndex == -1) return str;

	// ����ȍ~�̕������폜
	std::string delimedStr = str.erase(delimIndex);

	return delimedStr;
}
