// ShaderCompiler.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "ICompileIterator.h"
#include "DirectCompile.h"
#include "StringUtility.h"
#include  <stdio.h>
#include  <io.h>
#include <vector>
#include <fstream>
using namespace std;


string start = "fxc.exe /nologo";
string shaderTypeCmd = " /T ";
string fullOptimize = " /O3";
string noOptimize = " /Od";
string debuging = " /Zi";
string funcName = " /E ";
string output = " /Fo ";

void GenerateReleaseCommand(
	const string& filePath,
	const string& fileName,
	const string& functionName,
	const string& resultFileName,
	ShaderType shaderType,
	string& cmdResult
) {
	string shaderTypeName;
	switch (shaderType)
	{
	case ShaderType::ComputeShader:
		shaderTypeName = "cs_5_1";
		break;
	case ShaderType::VertexShader:
		shaderTypeName = "vs_5_1";
		break;
	case ShaderType::HullShader:
		shaderTypeName = "hs_5_1";
		break;
	case ShaderType::DomainShader:
		shaderTypeName = "ds_5_1";
		break;
	case ShaderType::GeometryShader:
		shaderTypeName = "gs_5_1";
		break;
	case ShaderType::PixelShader:
		shaderTypeName = "ps_5_1";
		break;
	default:
		shaderTypeName = " ";
		break;
	}
	cmdResult.clear();
	cmdResult.reserve(50);
	cmdResult += start + shaderTypeCmd + shaderTypeName + fullOptimize + funcName + functionName + output + '\"' + filePath + resultFileName + '\"' + " " + '\"' + fileName + '\"';
}
void GenerateDebugCode(
	const string& filePath,
	const string& fileName,
	const string& functionName,
	const string& resultFileName,
	ShaderType shaderType,
	string& cmdResult
) {
	string shaderTypeName;
	switch (shaderType)
	{
	case ShaderType::ComputeShader:
		shaderTypeName = "cs_5_1";
		break;
	case ShaderType::VertexShader:
		shaderTypeName = "vs_5_1";
		break;
	case ShaderType::HullShader:
		shaderTypeName = "hs_5_1";
		break;
	case ShaderType::DomainShader:
		shaderTypeName = "ds_5_1";
		break;
	case ShaderType::GeometryShader:
		shaderTypeName = "gs_5_1";
		break;
	case ShaderType::PixelShader:
		shaderTypeName = "ps_5_1";
		break;
	default:
		shaderTypeName = " ";
		break;
	}
	cmdResult.clear();
	cmdResult.reserve(50);
	cmdResult += start + shaderTypeCmd + shaderTypeName + debuging + noOptimize + funcName + functionName + output + '\"' + filePath + resultFileName + '\"' + " " + '\"' + fileName + '\"';
}


typedef unsigned int uint;
void TryCreateDirectory(string& path)
{
	vector<uint> slashIndex;
	slashIndex.reserve(20);
	for (uint i = 0; i < path.length(); ++i)
	{
		if (path[i] == '/' || path[i] == '\\')
		{
			slashIndex.push_back(i);
			path[i] = '\\';
		}
	}
	if (slashIndex.empty()) return;
	string command;
	command.reserve(slashIndex[slashIndex.size() - 1] + 3);
	uint startIndex = 0;
	for (uint i = 0; i < slashIndex.size(); ++i)
	{
		uint value = slashIndex[i];
		for (uint x = startIndex; x < value; ++x)
		{
			command += path[x];
		}
		if (_access(command.data(), 0) == -1)
		{
			std::system(("md " + command).data());
		}
		startIndex = slashIndex[i];
	}
}
int main()
{
	string cmd;
	string sonCmd;
	string results;
	unique_ptr<DirectCompile> dc;
	vector<Command>* cmds = nullptr;
	while (true)
	{
		cout << "Choose Compiling Mode: " << endl;
		cout << "0. Compile Single File" << endl;
		std::cin >> cmd;
		if (cmd.size() != 1) continue;

		if (cmd[0] == '0')
		{
			dc = std::unique_ptr<DirectCompile>(new DirectCompile());
			dc->UpdateCommand();
			cout << "Input anything to execute the command" << endl;
			std::cin >> sonCmd;
			cmds = &dc->GetCommand();
		}
	EXECUTE:
		for (auto i = cmds->begin(); i != cmds->end(); ++i)
		{
			TryCreateDirectory(i->filePath);
			if (i->isDebug)
			{

				GenerateDebugCode(
					i->filePath,
					i->fileName,
					i->functionName,
					i->resultFileName,
					i->shaderType,
					results);

			}
			else
			{
				GenerateReleaseCommand(
					i->filePath,
					i->fileName,
					i->functionName,
					i->resultFileName,
					i->shaderType,
					results);
			}
			cout << results << endl;
			std::system(results.c_str());
		}
		cout << "Want to repeat the command again? Y for true" << endl;
		std::cin >> sonCmd;
		if (sonCmd.length() == 1 && (sonCmd[0] == 'y' || sonCmd[0] == 'Y'))
		{
			goto EXECUTE;
		}
	}
}
