#pragma once
#include <iostream>
#include <stdio.h>
#include <vector>
#include <windows.h>
#include "opencvInclude.h"
#include "Utils.h"

using namespace std;

class MatchingPattern {
	//Map qui contient le nom du label en clé et en valeur les informations necessaire pour le matching
	map<string, pair<Mat,pair<Mat, std::vector<KeyPoint>>>> descriptorPattern;
public : 
	void buildKeyPoint();
	String matchWithMath(Mat match);
};
