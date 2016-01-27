#ifndef _INI_H_
#define _INI_H_

#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
using namespace std;

struct Key
{
	string name;
	string value;
	Key(string name)
	{
		this->name = name;
	}
	Key(string name, string value)
	{
		this->name = name;
		this->value = value;
	}
};
struct Section
{
	string name;
	vector<Key> keys;

	Section(string name)
	{
		this->name = name;
	}

};

class ini
{
public:
	ini(string path);
	~ini();
	const void Init();
	const void Shutdown();

	string Get(string section, string name, string default_value)const;

	long GetInteger(string section, string name, long default_value)const;

	double GetReal(string section, string name, double default_value)const;

	bool GetBoolean(string section, string name, bool default_value)const;



	const void Set(string section, string name, string value);

	const void SetInteger(string section, string name, long value);

	const void SetReal(string section, string name, double value);

	const void SetBoolean(string section, string name, bool value);

private:

	const void ParseData(ifstream& file);
	const void WriteData(ofstream& file);
	vector<Section> _sections;
	string _path;
	int _datasize;
};
#endif