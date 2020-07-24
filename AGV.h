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
#include "TreeLiveness.h"
using namespace std;
class AGV
{
	int nh;
	MultiGraph G;
	vector<map<int,int>> A;
	MultiGraph Ghat;
	//CondensedMultiGraph C_Ghat;

	vector<vector<vector<int>>> ExploredConfigurations;
	bool ExploredBefore(int _nodes, vector<int> _hash);

public:
	AGV(string file);
	AGV(string file, bool condensed);
	bool IsLive();
};

