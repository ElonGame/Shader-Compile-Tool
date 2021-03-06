#include "StringUtility.h"
using namespace std;
void IndicesOf(const string& str, const string& sign, vector<int>& v)
{
	v.clear();
	if (str.empty()) return;
	int count = str.length() - sign.length() + 1;
	v.reserve(10);
	for (int i = 0; i < count; ++i)
	{
		bool success = true;
		for (int j = 0; j < sign.length(); ++j)
		{
			if (sign[j] != str[i + j])
			{
				success = false;
				break;
			}
		}
		if (success)
			v.push_back(i);
	}
}

void IndicesOf(const std::string& str, char sign, std::vector<int>& v)
{
	v.clear();
	int count = str.length();
	v.reserve(10);
	for (int i = 0; i < count; ++i)
	{
		if (sign == str[i])
		{
			v.push_back(i);
		}
	}
}

void CutToLine(const char* str, int64_t size, std::vector<std::string>& lines)
{
	lines.clear();
	lines.reserve(32);
	string buffer;
	buffer.reserve(32);
	for (size_t i = 0; i < size; ++i)
	{
		if (str[i] == '\0') break;
		if (!(str[i] == '\n' || str[i] == '\r'))
		{
			buffer.push_back(str[i]);
		}
		else
		{
			if (str[i] == '\n' || (str[i] == '\r' && i < size - 1 && str[i + 1] == '\n'))
			{
				if (!buffer.empty())
					lines.push_back(buffer);
				buffer.clear();
			}
		}
	}
	if (!buffer.empty())
		lines.push_back(buffer);
}

void CutToLine(const std::string& str, std::vector<std::string>& lines)
{
	lines.clear();
	lines.reserve(32);
	string buffer;
	buffer.reserve(32);
	for (size_t i = 0; i < str.length(); ++i)
	{
		if (!(str[i] == '\n' || str[i] == '\r'))
		{
			buffer.push_back(str[i]);
		}
		else
		{
			if (str[i] == '\n' || (str[i] == '\r' && i < str.length() - 1 && str[i + 1] == '\n'))
			{
				if (!buffer.empty())
					lines.push_back(buffer);
				buffer.clear();
			}
		}
	}
	if (!buffer.empty())
		lines.push_back(buffer);
}



void ReadLines(std::ifstream& ifs, std::vector<std::string>& lines)
{
	ifs.seekg(0, ios::end);
	int64_t size = ifs.tellg();
	ifs.seekg(0, ios::beg);
	vector<char> buffer(size + 1);
	ifs.read(buffer.data(), size);
	CutToLine(buffer.data(), size, lines);
}

int GetFirstIndexOf(const std::string& str, char sign)
{
	int count = str.length();
	for (int i = 0; i < count; ++i)
	{
		if (sign == str[i])
		{
			return i;
		}
	}
	return -1;
}

int GetFirstIndexOf(const std::string& str, const std::string& sign)
{
	int count = str.length() - sign.length() + 1;
	for (int i = 0; i < count; ++i)
	{
		bool success = true;
		for (int j = 0; j < sign.length(); ++j)
		{
			if (sign[j] != str[i + j])
			{
				success = false;
				break;
			}
		}
		if (success)
			return i;
	}
	return -1;
}

void Split(const std::string& str, char sign, std::vector<std::string>& v)
{
	vector<int> indices;
	IndicesOf(str, sign, indices);
	v.clear();
	v.reserve(10);
	string s;
	s.reserve(str.size());
	int startPos = 0;
	for (auto index = indices.begin(); index != indices.end(); ++index)
	{
		s.clear();
		for (int i = startPos; i < *index; ++i)
		{
			s.push_back(str[i]);
		}
		startPos = *index + 1;
		if (!s.empty())
			v.push_back(s);
	}
	s.clear();
	for (int i = startPos; i < str.length(); ++i)
	{
		s.push_back(str[i]);
	}
	if (!s.empty())
		v.push_back(s);
}

void Split(const string& str, const string& sign, vector<string>& v)
{
	vector<int> indices;
	IndicesOf(str, sign, indices);
	v.clear();
	v.reserve(10);
	string s;
	s.reserve(str.size());
	int startPos = 0;
	for (auto index = indices.begin(); index != indices.end(); ++index)
	{
		s.clear();
		for (int i = startPos; i < *index; ++i)
		{
			s.push_back(str[i]);
		}
		startPos = *index + 1;
		if (!s.empty())
			v.push_back(s);
	}
	s.clear();
	for (int i = startPos; i < str.length(); ++i)
	{
		s.push_back(str[i]);
	}
	if (!s.empty())
		v.push_back(s);
}
void GetDataFromAttribute(const std::string& str, std::string& result)
{
	int firstIndex = GetFirstIndexOf(str, '[');
	result.clear();
	if (firstIndex < 0) return;
	result.reserve(5);
	for (int i = firstIndex + 1; str[i] != ']' && i < str.length(); ++i)
	{
		result.push_back(str[i]);
	}
}

void GetDataFromBrackets(const std::string& str, std::string& result)
{
	int firstIndex = GetFirstIndexOf(str, '(');
	result.clear();
	if (firstIndex < 0) return;
	result.reserve(5);
	for (int i = firstIndex + 1; str[i] != ')' && i < str.length(); ++i)
	{
		result.push_back(str[i]);
	}
}

inline constexpr void mtolower(char& c)
{
	if ((c >= 'A') && (c <= 'Z'))
		c = c + ('a' - 'A');
}
inline constexpr void mtoupper(char& c)
{
	if ((c >= 'a') && (c <= 'z'))
		c = c + ('A' - 'a');
}

void ToLower(std::string& str)
{
	char* c = str.data();
	const uint size = str.length();
	for (uint i = 0; i < size; ++i)
	{
		mtolower(c[i]);
	}
}
void ToUpper(std::string& str)
{
	char* c = str.data();
	const uint size = str.length();
	for (uint i = 0; i < size; ++i)
	{
		mtoupper(c[i]);
	}
}

int64_t StringToInt(const std::string& str)
{
	if (str.empty()) return 0;
	int64_t v;
	int64_t result = 0;
	uint start = 0;
	if (str[0] == '-')
	{
		v = -1;
		start = 1;
	}
	else v = 1;
	for (; start < str.size(); ++start)
	{
		result = result * 10 + ((int)str[start] - 48);
	}
	return result * v;
}

double StringToFloat(const std::string& str)
{
	if (str.empty()) return 0;
	double v;
	double result = 0;
	double afterPoint = 1;
	uint start = 0;
	if (str[0] == '-')
	{
		v = -1;
		start = 1;
	}
	else v = 1;
	for (; start < str.size(); ++start)
	{
		char cur = str[start];
		if (cur == '.')
		{
			start++;
			break;
		}
		result = result * 10 + ((int64_t)cur - 48);
	}
	for (; start < str.size(); ++start)
	{
		afterPoint *= 0.1;
		result += afterPoint * (double)((int)str[start] - 48);
	}
	return result * v;
}

bool StringEqual(const std::string& a, const std::string& b)
{
	char* aPtr = ((std::string&)a).data();
	char* bPtr = ((std::string&)b).data();
	if (a.length() != b.length()) return false;
	size_t fullLen = a.length();
	if (fullLen >= sizeof(uint64_t) * 2)
	{
		size_t longSize = fullLen / sizeof(uint64_t);
		uint64_t* longAPtr = (uint64_t*)aPtr;
		uint64_t* longBPtr = (uint64_t*)bPtr;
		uint64_t* end = longAPtr + longSize;
		for (; longAPtr != end; longAPtr++)
		{
			if (*longAPtr != *longBPtr) return false;
			longBPtr++;
		}
		char* aChar = (char*)longAPtr;
		char* bChar = (char*)longBPtr;
		size_t charSize = fullLen % sizeof(uint64_t);
		for (uint i = 0; i < charSize; ++i)
		{
			if (aChar[i] != bChar[i]) return false;
		}
	}
	else
	{
		size_t longSize = fullLen / sizeof(uint32_t);
		uint32_t* longAPtr = (uint32_t*)aPtr;
		uint32_t* longBPtr = (uint32_t*)bPtr;
		uint32_t* end = longAPtr + longSize;
		for (; longAPtr != end; longAPtr++)
		{
			if (*longAPtr != *longBPtr) return false;
			longBPtr++;
		}
		char* aChar = (char*)longAPtr;
		char* bChar = (char*)longBPtr;
		size_t charSize = fullLen % sizeof(uint32_t);
		for (uint i = 0; i < charSize; ++i)
		{
			if (aChar[i] != bChar[i]) return false;
		}
	}
	return true;
}