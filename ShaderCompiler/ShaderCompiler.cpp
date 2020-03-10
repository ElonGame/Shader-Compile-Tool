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
	std::unordered_map<std::string, std::pair<ShaderType, uint>> passMap;
	passMap.reserve(passDescs.size() * 2);
	for (auto i = passDescs.begin(); i != passDescs.end(); ++i)
	{
		auto findFunc = [&](const std::string& name, ShaderType type)->void
		{
			auto ite = passMap.find(name);
			if (ite == passMap.end())
			{
				passMap.insert_or_assign(name, std::pair<ShaderType, uint>(type, (uint)passMap.size()));
			}
		};
		findFunc(i->vertex, ShaderType::VertexShader);
		findFunc(i->fragment, ShaderType::PixelShader);
	}
	std::vector<std::pair<std::string, ShaderType>> functionNames(passMap.size());
	PutIn<uint>(resultData, (uint)passMap.size());
	for (auto ite = passMap.begin(); ite != passMap.end(); ++ite)
	{
		functionNames[ite->second.second] = { ite->first, ite->second.first };
	}
	string commandCache;
	for (uint i = 0; i < functionNames.size(); ++i)
	{
		uint64_t fileSize = 0;
		if (isDebug)
		{
			GenerateDebugCommand(
				fileName, functionNames[i].first, tempFilePath, functionNames[i].second, commandCache);
		}
		else
		{
			GenerateReleaseCommand(
				fileName, functionNames[i].first, tempFilePath, functionNames[i].second, commandCache);
		}
		cout << commandCache << endl;
		func(commandCache, fileSize);
	}

	PutIn<uint>(resultData, (uint)passDescs.size());
	for (auto i = passDescs.begin(); i != passDescs.end(); ++i)
	{
		PutIn(resultData, i->rasterizeState);
		PutIn(resultData, i->depthStencilState);
		PutIn(resultData, i->blendState);
		PutIn<uint>(resultData, passMap[i->vertex].second);
		PutIn<uint>(resultData, passMap[i->fragment].second);
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
		cout << kernelCommand << endl;
		func(kernelCommand, fileSize);
	}
}
void getFiles(string path, vector<string>& files)
{
	intptr_t   hFile = 0;
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					getFiles(p.assign(path).append("\\").append(fileinfo.name), files);
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);

		_findclose(hFile);
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
		cout << "  0: Compile Single File" << endl;
		cout << "  1: Compile Batched File" << endl;
		std::cin >> cmd;
		if (cmd == "exit") return 0;
		else if (cmd.size() == 1) {
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
}
