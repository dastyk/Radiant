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
	Key(const string& sname) : name(sname)
	{
	}
	Key(const string& sname, const string& svalue): name(sname), value(svalue)
	{
	}
};
struct Section
{
	string name;
	vector<Key> keys;

	Section(const string& sname) : name(sname)
	{
	}

};

class ini
{
public:
	ini(const string& path);
	~ini();
	const void Init();
	const void Shutdown();

	string Get(const string& section, const string& name, const string& default_value)const;

	long GetInteger(const string& section, const string& name, long default_value)const;

	double GetReal(const string& section, const string& name, double default_value)const;

	bool GetBoolean(const string& section, const string& name, bool default_value)const;



	const void Set(const string& section, const string& name, const string& value);

	const void SetInteger(const string& section, const string& name, long value);

	const void SetReal(const string& section, const string& name, double value);

	const void SetBoolean(const string& section, const string& name, bool value);

private:

	const void ParseData(ifstream& file);
	const void WriteData(ofstream& file);
	vector<Section> _sections;
	string _path;
	int _datasize;
};
#endif