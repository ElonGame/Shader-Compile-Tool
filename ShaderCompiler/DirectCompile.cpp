#include "DirectCompile.h";
using namespace std;
DirectCompile::DirectCompile()
{
	cout << "Choose a Directory Path:" << endl;
	cin >> path;
	if (!path.empty() && (path[path.length() - 1] != '/' || path[path.length() - 1] != '\\'))
		path += '\\';
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

std::vector<Command>& DirectCompile::GetCommand()
{
	return commands;
}

void DirectCompile::UpdateCommand()
{
	commands.clear();
	string fileName;
	string resultName;
	cout << "Please Input the source shader file name: " << endl;
	cin >> fileName;
	cout << "Plase Input the destnation shader file name: " << endl;
	cin >> resultName;
	Command c;
	c.filePath = path;
	c.fileName = fileName;
	c.resultFileName = resultName;
	c.isDebug = isDebug;
RE_CHOOSE_TYPE:
	cout << "What Type of Shader is it? " << endl;
	cout << "  0 for Vertex & Fragment Shader" << endl;
	cout << "  1 for Compute Shader" << endl;
	string type;
	cin >> type;
	if (type[0] == '0')//VS
	{
		string vs, ps;
		cout << "Please input the Vertex Shader Name: " << endl;
		cin >> vs;
		cout << "Plase input the PS Shader Name: " << endl;
		cin >> ps;
		
		c.shaderType = ShaderType::VertexShader;
		c.functionName = vs;
		commands.push_back(c);
		c.shaderType = ShaderType::PixelShader;
		c.functionName = ps;
		commands.push_back(c);
	}
	else if (type[0] == '1')//Compute
	{
		string cs;
		cout << "Please input the kernel names, input \"0\" for end" << endl;
		c.shaderType = ShaderType::ComputeShader;
		string kernelName;
		cin >> kernelName;
		while (!(kernelName.length() >= 1 && kernelName[0] == '0'))
		{
			c.functionName = kernelName;
			commands.push_back(c);
			cin >> kernelName;
		}
	}
	else
	{
		goto RE_CHOOSE_TYPE;
	}

}