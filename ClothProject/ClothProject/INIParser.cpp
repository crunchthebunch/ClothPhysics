#include "INIParser.h"

INIParser::INIParser()
{
	//Constructor
}

INIParser::~INIParser()
{
	//Destructor
}

bool INIParser::LoadIniFile(const char* _pcFilename)
{
	//Load INI file
	std::string line;
	std::string section;
	std::string key;
	std::string value;
	bool isSection = false;
	bool isKey = false;
	bool isValue = false;
	bool isComment = false;

	std::ifstream iniFile(_pcFilename);
	if (iniFile.is_open())
	{
		while (getline(iniFile, line)) //Looping through the lines
		{
			//Get rid of spaces
			line.erase(std::remove(line.begin(), line.end(), ' '), line.end()); //Moves data (spaces) to the back, then erases from there to the end of the line

			for (std::string::iterator it = line.begin(); it != line.end(); ++it) //Looping through characters in the line
			{
				if (*it == ';')
				{
					isComment = true;
					break;
				}

				if (isComment == false) //If it is not a comment
				{
					//Get Section
					if (*it == '[' && isSection == false)
					{
						section = "";
						isSection = true;
					}

					if (isSection == true)
					{
						if (*it != ']')
						{
							if (*it != '[')
							{
								section += *it;
							}
						}
					}

					//Get Key and Value
					if (isSection == false && *it != '[' && *it != ']')
					{
						if (*it != '=' && isKey == false && isValue == false)
						{
							key = "";
							isKey = true;
						}

						if (isKey == true)
						{
							if (*it != '=')
							{
								key += *it;
							}
							else
							{
								isKey = false;
								value = "";
								isValue = true;
							}
						}
						if (isValue == true)
						{
							if (*it != '=')
							{
								value += *it;
							}
						}
					}
				}

			}
			if (isComment == false && isSection == false)
			{
				AddValue(section.c_str(), key.c_str(), value.c_str());
			}
			isSection = false;
			isValue = false;
			isComment = false;
		}
		iniFile.close();
	}

	return (false);
}

bool INIParser::AddValue(const char* _pcSection, const char* _pcKey, const char* _pcValue)
{
	std::string sectionKey;
	sectionKey = _pcSection;
	sectionKey += "|";
	sectionKey += _pcKey;

	//std::cout << sectionKey << ": " << _pcValue << std::endl;

	mapPairs.insert(std::pair<std::string, std::string>(sectionKey, _pcValue));
	return (false);
}

//Get Functions
bool INIParser::GetStringValue(const char* _pcSection, const char* _pcKey, std::string& _rStrValue)
{
	std::string stringValue;
	std::map<std::string, std::string>::iterator it; //Create an iterator for the map

	std::string sectionKey;
	sectionKey = _pcSection;
	sectionKey += "|";
	sectionKey += _pcKey;

	it = mapPairs.find(sectionKey);
	if (it != mapPairs.end()) //If the iterator isn't at the end (which would not be a usable value)
	{
		stringValue = it->second; //Gets the second of <string, string> iterator, which is the value
		_rStrValue = stringValue;
		return (true);
	}

	return (false);
}

bool INIParser::GetIntValue(const char* _pcSection, const char* _pcKey, int& _riValue)
{
	int intValue;
	std::map<std::string, std::string>::iterator it; //Create an iterator for the map

	std::string sectionKey;
	sectionKey = _pcSection;
	sectionKey += "|";
	sectionKey += _pcKey;

	it = mapPairs.find(sectionKey);
	if (it != mapPairs.end()) //If the iterator isn't at the end (which would not be a usable value)
	{
		intValue = std::stoi(it->second); //Gets the second of <string, string> iterator, which is the value, then convert to the get-type
		_riValue = intValue;
		return (true);
	}

	return (false);
}

bool INIParser::GetFloatValue(const char* _pcSection, const char* _pcKey, float& _rfValue)
{
	float floatValue;
	std::map<std::string, std::string>::iterator it; //Create an iterator for the map

	std::string sectionKey;
	sectionKey = _pcSection;
	sectionKey += "|";
	sectionKey += _pcKey;

	it = mapPairs.find(sectionKey);
	if (it != mapPairs.end()) //If the iterator isn't at the end (which would not be a usable value)
	{
		floatValue = std::stof(it->second); //Gets the second of <string, string> iterator, which is the value, then convert to the get-type
		_rfValue = floatValue;
		return (true);
	}

	return (false);
}

bool INIParser::GetBoolValue(const char* _pcSection, const char* _pcKey, bool& _rbValue)
{

	bool boolValue = false;
	std::map<std::string, std::string>::iterator it; //Create an iterator for the map
	std::string itString;

	std::string sectionKey;
	sectionKey = _pcSection;
	sectionKey += "|";
	sectionKey += _pcKey;

	it = mapPairs.find(sectionKey);
	if (it != mapPairs.end()) //If the iterator isn't at the end (which would not be a usable value)
	{
		itString = it->second;

		for (int i = 0; i < (int)itString.length(); i++)
		{
			itString[i] = tolower(itString[i]); //Convert all chars in string to be lower case
		}

		if (itString == "true" || itString == "1")
		{
			_rbValue = true;
			return (true);
		}
		else if (itString == "false" || itString == "0")
		{
			_rbValue = false;
			return (true);
		}
	}
	return (false);
}