// ShaderCompiler.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#define _CRT_SECURE_NO_WARNINGS
#include "ICompileIterator.h"
#include "DirectCompile.h"
#include "StringUtility.h"
#include  <stdio.h>
#include  <io.h>
#include <vector>
#include "BatchCompile.h"
#include <fstream>
#include <unordered_map>
#include "ShaderUniforms.h"
using namespace std;
using namespace SCompile;


string start = "fxc.exe /nologo";
string shaderTypeCmd = " /T ";
string fullOptimize = " /O3";
string noOptimize = " /Od";
string debuging = " /Zi";
string funcName = " /E ";
string output = " /Fo ";
void GenerateReleaseCommand(
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
	cmdResult += start + shaderTypeCmd + shaderTypeName + fullOptimize + funcName + functionName + output + '\"' + resultFileName + '\"' + " " + '\"' + fileName + '\"';
}
void GenerateDebugCommand(
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
	cmdResult += start + shaderTypeCmd + shaderTypeName + debuging + noOptimize + funcName + functionName + output + '\"' + resultFileName + '\"' + " " + '\"' + fileName + '\"';
}

template <typename T>
void PutIn(std::vector<char>& c, const T& data)
{
	T* cc = &((T&)data);
	size_t siz = c.size();
	c.resize(siz + sizeof(T));
	memcpy(c.data() + siz, cc, sizeof(T));
}
template <>
void PutIn<string>(std::vector<char>& c, const string& data)
{
	PutIn<uint>(c, (uint)data.length());
	size_t siz = c.size();
	c.resize(siz + data.length());
	memcpy(c.data() + siz, data.data(), data.length());
}
template <typename T>
void DragData(ifstream& ifs, T& data)
{
	ifs.read((char*)&data, sizeof(T));
}
template <>
void DragData<string>(ifstream& ifs, string& str)
{
	uint32_t length = 0;
	DragData<uint32_t>(ifs, length);
	str.clear();
	str.resize(length);
	ifs.read(str.data(), length);
}
struct PassCommand
{
	string vertexCommand;
	string fragmentCommand;
};
void CompileShader(
	const string& fileName,
	const string& propertyPath,
	const string& tempFilePath,
	std::vector<char>& resultData,
	bool isDebug)
{
	vector<ShaderVariable> vars;
	vector<PassDescriptor> passDescs;
	GetShaderRootSigData(propertyPath, vars, passDescs);

	resultData.clear();
	resultData.reserve(32768);
	PutIn<uint>(resultData, (uint)vars.size());
	for (auto i = vars.begin(); i != vars.end(); ++i)
	{
		PutIn<string>(resultData, i->name);
		PutIn<ShaderVariableType>(resultData, i->type);
		PutIn<uint>(resultData, i->tableSize);
		PutIn<uint>(resultData, i->registerPos);
		PutIn<uint>(resultData, i->space);
	}
	auto func = [&](const string& command, uint64_t& fileSize)->void
	{
		system(command.c_str());
		fileSize = 0;
		ifstream ifs(tempFilePath, ios::binary);
		if (!ifs) return;
		ifs.seekg(0, ios::end);
		fileSize = ifs.tellg();
		ifs.seekg(0, ios::beg);
		PutIn<uint64_t>(resultData, fileSize);
		if (fileSize == 0) return;
		size_t originSize = resultData.size();
		resultData.resize(fileSize + originSize);
		ifs.read(resultData.data() + originSize, fileSize);
	};
	PutIn<uint>(resultData, (uint)passDescs.size());
	for (auto i = passDescs.begin(); i != passDescs.end(); ++i)
	{
		uint64_t fileSize = 0;
		string vertexCommand, fragmentCommand;
		if (isDebug)
		{
			GenerateDebugCommand(
				fileName, i->vertex, tempFilePath, ShaderType::VertexShader, vertexCommand);
			GenerateDebugCommand(
				fileName, i->fragment, tempFilePath, ShaderType::PixelShader, fragmentCommand);
		}
		else
		{
			GenerateReleaseCommand(
				fileName, i->vertex, tempFilePath, ShaderType::VertexShader, vertexCommand);
			GenerateReleaseCommand(
				fileName, i->fragment, tempFilePath, ShaderType::PixelShader, fragmentCommand);
		}
		PutIn(resultData, i->rasterizeState);
		PutIn(resultData, i->depthStencilState);
		PutIn(resultData, i->blendState);
		func(vertexCommand, fileSize);
		func(fragmentCommand, fileSize);
	}

}

void CompileComputeShader(
	const string& fileName,
	const string& propertyPath,
	const string& tempFilePath,
	std::vector<char>& resultData,
	bool isDebug)
{
	vector<ComputeShaderVariable> vars;
	vector<string> passDescs;
	GetComputeShaderRootSigData(propertyPath, vars, passDescs);
	resultData.clear();
	resultData.reserve(32768);
	PutIn<uint>(resultData, (uint)vars.size());
	for (auto i = vars.begin(); i != vars.end(); ++i)
	{
		PutIn<string>(resultData, i->name);
		PutIn<ComputeShaderVariable::Type>(resultData, i->type);
		PutIn<uint>(resultData, i->tableSize);
		PutIn<uint>(resultData, i->registerPos);
		PutIn<uint>(resultData, i->space);
	}
	auto func = [&](const string& command, uint64_t& fileSize)->void
	{
		system(command.c_str());
		fileSize = 0;
		ifstream ifs(tempFilePath, ios::binary);
		if (!ifs) return;
		ifs.seekg(0, ios::end);
		fileSize = ifs.tellg();
		ifs.seekg(0, ios::beg);
		PutIn<uint64_t>(resultData, fileSize);
		if (fileSize == 0) return;
		size_t originSize = resultData.size();
		resultData.resize(fileSize + originSize);
		ifs.read(resultData.data() + originSize, fileSize);
	};
	PutIn<uint>(resultData, (uint)passDescs.size());
	for (auto i = passDescs.begin(); i != passDescs.end(); ++i)
	{
		uint64_t fileSize = 0;
		string kernelCommand;
		if (isDebug)
		{
			GenerateDebugCommand(
				fileName, *i, tempFilePath, ShaderType::ComputeShader, kernelCommand);
		}
		else
		{
			GenerateReleaseCommand(
				fileName, *i, tempFilePath, ShaderType::ComputeShader, kernelCommand);
		}
		func(kernelCommand, fileSize);
	}
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
	unique_ptr<ICompileIterator> dc;
	vector<Command>* cmds = nullptr;
	std::vector<char> outputData;
	while (true)
	{
		cout << "Choose Compiling Mode: " << endl;
		cout << "  0. Compile Single File" << endl;
		cout << "  1. Compile Batched File" << endl;
		std::cin >> cmd;
		if (cmd == "exit") return 0;
		if (cmd.size() != 1) continue;
		if (cmd[0] == '0')
		{
			dc = std::unique_ptr<ICompileIterator>(new DirectCompile());
		}
		else if (cmd[0] == '1')
		{
			dc = std::unique_ptr<ICompileIterator>(new BatchCompile());
		}
		else
			dc = nullptr;

		if (dc)
		{
			dc->UpdateCommand();
			cout << "Input anything to execute the command" << endl;
			std::cin >> sonCmd;
			cmds = &dc->GetCommand();
		}
	EXECUTE:
		static string temp = ".temp.cso";
		string pathFolder = "CompileResult\\";
		TryCreateDirectory(pathFolder);
		for (auto i = cmds->begin(); i != cmds->end(); ++i)
		{
			uint32_t maxSize = 0;
			if (i->isCompute)
			{
				CompileComputeShader(i->fileName, i->propertyFileName, temp, outputData, i->isDebug);
			}
			else
			{
				CompileShader(i->fileName, i->propertyFileName, temp, outputData, i->isDebug);
			}
			ofstream ofs(pathFolder + i->fileName + ".cso", ios::binary);
			ofs.write(outputData.data(), outputData.size());
		}
		remove(temp.c_str());
		cout << "Want to repeat the command again? Y for true" << endl;
		std::cin >> sonCmd;
		if (sonCmd.length() == 1 && (sonCmd[0] == 'y' || sonCmd[0] == 'Y'))
		{
			goto EXECUTE;
		}
	}
}
