#include "BatchCompile.h"
#include <iostream>
#include <fstream>
#include "StringUtility.h"
using namespace std;
BatchCompile::BatchCompile()
{
RE_CHOOSE:
	cout << "Release Or Debug? Y for Release & N for Debug" << endl;
	string s;
	cin >> s;
	if (s.size() != 1 || ((s[0] != 'Y') && s[0] != 'N' && s[0] != 'y' && s[0] != 'n'))
	{
		goto RE_CHOOSE;
	}
	if (s[0] == 'Y' || s[0] == 'y')
		isDebug = false;
	else isDebug = true;
}
void BatchCompile::UpdateCommand()
{
	Command c;
	c.isDebug = isDebug;
	commands.clear();
	cout << "Please input batch look-up file: " << endl;
	cin >> lookUpFilePath;
	ifstream ifs(lookUpFilePath);
	if (!ifs)
	{
		cout << "Look-up File Not Exists!" << endl;
		return;
	}
	char cArray[512];
	vector<string> splitedCommands;
	while (true)
	{
		ifs.getline(cArray, 511);
		if (strlen(cArray) == 0) return;
		string s = cArray;
		Split(s, ' ', splitedCommands);
		if (splitedCommands.size() != 3)
			continue;
		if (splitedCommands[0] == "compute")
		{
			c.isCompute = true;

		}
		else if (splitedCommands[0] == "shader")
		{
			c.isCompute = false;
		}
		c.fileName = splitedCommands[1];
		c.propertyFileName = splitedCommands[2];
		commands.push_back(c);
	}
}
std::vector<Command>& BatchCompile::GetCommand()
{
	Command c;
	c.isDebug = isDebug;
	commands.clear();
	ifstream ifs(lookUpFilePath);
	if (!ifs)
	{
		cout << "Look-up File Not Exists!" << endl;
		return commands;
	}
	char cArray[512];
	vector<string> splitedCommands;
	while (true)
	{
		ifs.getline(cArray, 511);
		if (strlen(cArray) == 0) return commands;
		string s = cArray;
		Split(s, ' ', splitedCommands);
		if (splitedCommands.size() != 3)
			continue;
		if (splitedCommands[0] == "compute")
		{
			c.isCompute = true;

		}
		else if (splitedCommands[0] == "shader")
		{
			c.isCompute = false;
		}
		c.fileName = splitedCommands[1];
		c.propertyFileName = splitedCommands[2];
		commands.push_back(c);
	}
	return commands;
}