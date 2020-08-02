#pragma once
#include <map>
#include <vector>
#include "Graph.h"
using namespace std;
class Cycle
{
	int n;
	vector<int> capacities;
	vector<map<int, int>> directed;
	vector<map<int, int>> reversedEdges;

	bool PerformFeasibleMergers(bool direction);
	bool PerformProducerMergers(bool direction);

	bool IsCollapsable(bool direction);
	void Merge(int n1, int n2, int _capacity);
	bool AllEdgesInOneDirection();
public:
	//Cycle(){};
	Cycle(vector<int> _capacities, vector<map<int, int>> _directed);
	bool IsProducer();
};

