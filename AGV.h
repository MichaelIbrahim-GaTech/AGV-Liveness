#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <stack>
#include <string>
#include <queue>
#include "MultiGraph.h"
#include "CondensedMultiGraph.h"
#include "DirectedAcyclicMultiGraph.h"
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
	AGV(string file, bool condensed);
	bool IsLive();
};

