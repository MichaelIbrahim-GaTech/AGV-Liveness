#pragma once
#include <iostream>
#include <vector>
#include <set>
#include <stack>
#include <string>
#include <math.h>
using namespace std;
#define NIL -1
class Graph
{
	static vector<set<int>> getTranspose(vector<set<int>> _d);
	static vector<int> DFSUtil(int v, bool visited[], const vector<set<int>>& _d);
	static void fillOrder(int v, bool visited[], stack<int>& Stack, const vector<set<int>>& _d);
	static vector<pair<int, int>> bridgeUtil(int* time, int v, bool visited[], int disc[], int low[], int parent[], const vector<set<int>>& _d);
public:
	static vector<vector<int>> GetStronglyConnectedComponents(vector<set<int>> _d);
	static vector<pair<int, int>> bridge(vector<set<int>> _d);
};

