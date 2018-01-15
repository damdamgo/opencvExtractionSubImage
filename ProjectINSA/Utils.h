#pragma once
#include <iostream>
#include <stdio.h>
#include <vector>
#include <windows.h>
#include "opencvInclude.h"
using namespace std;
class Utils{
public:
	static vector<string> explode(const string& s, const char& c);
	static String GLOBAL_PATH;
	static wstring s2ws(const std::string& s);
	static void read_directory(const std::string& name, std::vector<std::string> &v);
};
