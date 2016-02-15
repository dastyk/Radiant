#include "ini.h"


ini::ini(const string& path) : _path(path)
{

}


ini::~ini()
{
}

const void ini::Init()
{
	ifstream file;

	file.open(_path, ios::in);
	if (file.is_open())
	{
		ParseData(file);
	}
	file.close();
	return void();
}

const void ini::Shutdown()
{
	ofstream file;

	file.open(_path, ios::out | ios::trunc);
	if (file.is_open())
	{
		WriteData(file);
	}
	file.close();
	return void();
}

string ini::Get(const string& section, const string& name, const string& default_value) const
{
	for (auto s : _sections)
	{
		if (s.name == section)
		{
			for (auto key : s.keys)
			{
				if (key.name == name)
					return key.value;
			}
		}
	}
	return default_value;
}

long ini::GetInteger(const string& section, const string& name, long default_value) const
{
	string valstr = Get(section, name, "");
	const char* value = valstr.c_str();
	char* end;
	// This parses "1234" (decimal) and also "0x4D2" (hex)
	long n = strtol(value, &end, 0);
	return end > value ? n : default_value;
}

double ini::GetReal(const string& section, const string& name, double default_value) const
{
	string valstr = Get(section, name, "");
	const char* value = valstr.c_str();
	char* end;
	double n = strtod(value, &end);
	return end > value ? n : default_value;
}

bool ini::GetBoolean(const string& section, const string& name, bool default_value) const
{
	string valstr = Get(section, name, "");
	// Convert to lower case to make string comparisons case-insensitive
	std::transform(valstr.begin(), valstr.end(), valstr.begin(), ::tolower);
	if (valstr == "true" || valstr == "yes" || valstr == "on" || valstr == "1")
		return true;
	else if (valstr == "false" || valstr == "no" || valstr == "off" || valstr == "0")
		return false;
	else
		return default_value;
}

const void ini::Set(const string& section, const string& name, const string& value)
{
	for (auto& s : _sections)
	{
		if (s.name == section)
		{
			for (auto& key : s.keys)
			{
				if (key.name == name)
				{
					key.value = value;
					return;
				}
			}

			s.keys.push_back(Key(name, value));
			return;
		}
	}

	_sections.push_back(Section(section));
	vector<Key>& k = _sections[_sections.size() - 1].keys;
	k.push_back(Key(name, value));

}

const void ini::SetInteger(const string& section, const string& name, long value)
{
	Set(section, name, to_string(value));
	return void();
}

const void ini::SetReal(const string& section, const string& name, double value)
{
	Set(section, name, to_string(value));
	return void();
}

const void ini::SetBoolean(const string& section, const string& name, bool value)
{
	string a = (value) ? "True" : "False";
	Set(section, name, a);
}

const void ini::ParseData(ifstream& file)
{

	string name;

	char data;
	while (file.get(data))
	{
		if (data == '[')
		{
			name.clear();
			// Read the section name
			file.read(&data, 1);
			while (data != ']')
			{
				name.push_back(data);
				file.read(&data, 1);
				if (name.size() > 50)
					throw "Error";
			}
			_sections.push_back(Section(name));
			getline(file, name);
		}
		else if (data != '\n')
		{
			name.clear();
			while (data != '=')
			{
				name.push_back(data);
				file.read(&data, 1);
				if (name.size() > 50)
					throw "Error";
			}
			vector<Key>& k = _sections[_sections.size() - 1].keys;
			k.push_back(Key(name));
			name.clear();
			getline(file, name);
			k.at(k.size() - 1).value = name;
		}
	}
	return void();
}

const void ini::WriteData(ofstream & file)
{
	for (auto section : _sections)
	{
		file << ("[" + section.name) + "]\n";
		for (auto key : section.keys)
		{
			file << (key.name + "=" + key.value + "\n");
		}
		file << "\n";
	}

	return void();
}
