/**************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2005 - 2018 Media Design School

File Name	:	INIParser.h
Description	:	System to read INI files and parse information to the program
Author   	:	Zheng Yu Bu
mail		:	zheng.yu5315@mediadesign.school.nz
**************************************************/

#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

#pragma once

class INIParser
{
private:
	std::map<std::string, std::string> mapPairs;

public:
	INIParser();

	~INIParser();

	bool LoadIniFile(const char* _pcFilename);
	bool AddValue(const char* _pcSection, const char* _pcKey, const char* _pcValue);
	bool GetStringValue(const char* _pcSection, const char* _pcKey, std::string& _rStrValue);
	bool GetIntValue(const char* _pcSection, const char* _pcKey, int& _riValue);
	bool GetFloatValue(const char* _pcSection, const char* _pcKey, float& _rfValue);
	bool GetBoolValue(const char* _pcSection, const char* _pcKey, bool& _rbValue);
};