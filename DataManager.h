#pragma once
#include <map>
using namespace std;
class DataManager
{
public:
	static map<string, string> messageCodeMap;
private:
	DataManager();
	~DataManager();
};

