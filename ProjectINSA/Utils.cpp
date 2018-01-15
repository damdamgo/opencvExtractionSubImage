#include "stdafx.h"
#include "Utils.h"
/*
Utils pour le projet
*/
vector<string> Utils::explode(const string& s, const char& c)
{
	string buff{ "" };
	vector<string> v;

	for (auto n : s)
	{
		if (n != c) buff += n; else
			if (n == c && buff != "") { v.push_back(buff); buff = ""; }
	}
	if (buff != "") v.push_back(buff);

	return v;
}

String Utils::GLOBAL_PATH = "C:/Users/Poste/Desktop/opencv/ProjectINSA";

wstring Utils::s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

void  Utils::read_directory(const std::string& name, std::vector<std::string> &v)
{
	std::string pattern(name);
	pattern.append("\\*");
	WIN32_FIND_DATA data;
	HANDLE hFind;
	if ((hFind = FindFirstFile(Utils::s2ws(pattern).c_str(), &data)) != INVALID_HANDLE_VALUE) {
		do {
			wstring ws(data.cFileName);
			string str(ws.begin(), ws.end());
			v.push_back(str);
		} while (FindNextFile(hFind, &data) != 0);
		FindClose(hFind);
	}
}