#pragma once
#include <string>
#include <vector>
typedef unsigned int uint;
void IndicesOf(const std::string& str, const std::string& sign, std::vector<uint>& v);
void IndicesOf(const std::string& str, char, std::vector<uint>& v);
int GetFirstIndexOf(const std::string& str, const std::string& sign);
int GetFirstIndexOf(const std::string& str, char sign);
void Split(const std::string& str, const std::string& sign, std::vector<std::string>& v);
void Split(const std::string& str, char sign, std::vector<std::string>& v);
void GetDataFromAttribute(const std::string& str, std::string& result);
void GetDataFromBrackets(const std::string& str, std::string& result);
int StringToInteger(const std::string& str);