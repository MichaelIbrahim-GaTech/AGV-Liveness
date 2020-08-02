#pragma once
#include "MultiGraph.h"
#include "CondensedMultiGraph.h"
#include "ProducerPathBasedMerger.h"
#include "GraphCycles.h"
#include "Cycle.h"

class DirectedAcyclicMultiGraph
{
	CondensedMultiGraph* C;
	vector<vector<int>> nodes;
	vector<int> nodeInDegree;
	vector<bool> major;
	vector<map<int, vector<int>>> collapsedPaths;//from-to, vector of collapsed vertices
	//Get the topological order of the nodes of the DAG
	vector<int> TopologicalOrder();
	vector<bool> GetSn(vector<int> order, int n);
	bool ExistProducerPathBasedMerger(vector<int> _order, vector<bool> Sn, int n, int u, vector<int>& path);
	void GetProducerCycleBasedMerger(vector<int> _order, vector<bool> Sn, int n, int u, vector<int> path, set<int>& mergedNodes);
	//This function collapse non-major nodes
	void CollapseNonMajorNodesPaths();
	void GetMergedVertices(vector<int> _path, vector<int>& _mergedVertices);

	bool SubgraphContainsProducerCycle(vector<int> subgraph, CondensedMultiGraph* _C);
public:
	int nh;
	vector<int> capacities;
	vector<multimap<int, int>> directed;
	vector<multimap<int, int>> reversedEdges;
	DirectedAcyclicMultiGraph() {}
	DirectedAcyclicMultiGraph(const DirectedAcyclicMultiGraph&);
	DirectedAcyclicMultiGraph(CondensedMultiGraph* _C);
	bool TerminalNodesCapacityLessThanAllInEdges();
	bool ExistAPathLeadingToNH(CondensedMultiGraph* _C);
	bool ExistAProducerMerger(CondensedMultiGraph* _C);
	bool ExistACycle(CondensedMultiGraph* _C);
	vector<CondensedMultiGraph> PickATerminalNodeAndCollapseFeasiblePaths();
	bool IsTree();
	vector<int> Hash();
};

