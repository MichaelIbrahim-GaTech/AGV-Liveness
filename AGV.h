#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <stack>
#include <string>
#include "MultiGraph.h"
#include "CondensedMultiGraph.h"
using namespace std;
class AGV
{
	int nh;
	MultiGraph G;
	vector<map<int,int>> A;
	MultiGraph Ghat;
	//CondensedMultiGraph C_Ghat;

public:
	AGV(string file);
	bool IsLive();
};

