#pragma once
#include <iostream>
#include <vector>
#include <set>
#include <stack>
#include <string>
#include <math.h>
using namespace std;
#define NIL -1
#define INFINITY 10000000
class Graph
{
	static vector<set<int>> getTranspose(const vector<set<int>>& _d);
	static vector<int> DFSUtil(int v, bool visited[], const vector<set<int>>& _d);
	static void fillOrder(int v, bool visited[], stack<int>& Stack, const vector<set<int>>& _d);
	static vector<pair<int, int>> bridgeUtil(int* time, int v, bool visited[], int disc[], int low[], int parent[], const vector<set<int>>& _d);
	static void topologicalSortUtil(int v, bool visited[], stack<int>& Stack, const vector<set<int>>& _d);
	static bool isCyclicUtil(int v, bool visited[], int parent, const vector<set<int>>& _d);
	static void GetAllPathsUtil(int u, bool visited[], int path[], int& path_index, const vector<set<int>>& _d, vector<vector<int>>& _paths);
public:
	static vector<vector<int>> GetStronglyConnectedComponents(const vector<set<int>>& _d);
	static vector<pair<int, int>> bridge(const vector<set<int>>& _d);
	static vector<int> topologicalSort(const vector<set<int>>& _d);
	static bool isTree(const vector<set<int>>& _d);
	static void GetAllPaths(int s, const vector<set<int>>& _d, vector<vector<int>>& _paths);
};

