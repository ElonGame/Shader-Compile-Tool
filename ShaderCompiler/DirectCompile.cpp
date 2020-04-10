#include "DirectCompile.h";
#include <fstream>
using namespace std;
DirectCompile::DirectCompile()
{
RE_CHOOSE:
	cout << "DXC Or FXC? Y for DXC & N for FXC" << endl;
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

std::vector<Command>& DirectCompile::GetCommand()
{
	return commands;
}

void DirectCompile::UpdateCommand()
{
	commands.clear();
	string fileName;
	cout << "Please Input the source shader file name: " << endl;
	cin >> fileName;
	{
		ifstream ifs(fileName);
		if (!ifs)
		{
			return;
		}
	}
	Command c;
	c.fileName = fileName;
	cout << "Please Input the source property file name: " << endl;
	cin >> c.propertyFileName;
	{
		ifstream ifs(c.propertyFileName);
		if (!ifs)
		{
			return;
		}
	}
	c.isDebug = isDebug;
RE_CHOOSE_TYPE:
	cout << "What Type of Shader is it? " << endl;
	cout << "  0 for Vertex & Fragment Shader" << endl;
	cout << "  1 for Compute Shader" << endl;
	string type;
	cin >> type;
	if (type[0] == '0')//VS
	{
		c.isCompute = false;
	}
	else if (type[0] == '1')//Compute
	{
		c.isCompute = true;
	}
	else
	{
		goto RE_CHOOSE_TYPE;
	}
	commands.push_back(c);
}