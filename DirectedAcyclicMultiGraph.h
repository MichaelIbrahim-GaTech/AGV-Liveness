#pragma once
#include "MultiGraph.h"
#include "CondensedMultiGraph.h"
#include "ProducerPathBasedMerger.h"
#include "GraphCycles.h"
#include "Cycle.h"

#include "InTree.h"
#include "Algorithm2.h"
#include "Algorithm3.h"

class DirectedAcyclicMultiGraph
{
	vector<int> nodeInDegree;
	vector<bool> major;
	vector<map<int, vector<int>>> TNEs; // to keep track of all feasible source merging T(n,e)
	//Get the topological order of the nodes of the DAG
	vector<int> TopologicalOrder();
	vector<bool> GetSn(vector<int> order, int n);
	bool ExistProducerPathBasedMerger(vector<int> _order, vector<bool> Sn, int n, int u, vector<int>& path);
	void GetProducerCycleBasedMerger(vector<int> _order, vector<bool> Sn, int n, int u, vector<int> path, set<int>& mergedNodes);
	//This function collapse non-major nodes
	void CollapseNonMajorNodesPaths();

	bool SubgraphContainsProducerCycle(vector<int> subgraph, CondensedMultiGraph* _C);
public:
	int nh;
	CondensedMultiGraph* C;
	vector<vector<int>> nodes;
	vector<int> capacities;
	vector<multimap<int, int>> directed;
	vector<multimap<int, int>> reversedEdges;
	vector<map<int, vector<int>>> collapsedPaths;//from-to, vector of collapsed vertices
	void GetMergedVertices(vector<int> _path, vector<int>& _mergedVertices);
	int GetNodeFromVertexNumber(int _vertex);

	DirectedAcyclicMultiGraph() {}
	DirectedAcyclicMultiGraph(const DirectedAcyclicMultiGraph&);
	DirectedAcyclicMultiGraph(CondensedMultiGraph* _C);
	bool TerminalNodesCapacityLessThanAllInEdges();
	bool ExistAPathLeadingToNH(CondensedMultiGraph* _C);
	bool ExistAProducerMerger(CondensedMultiGraph* _C);
	bool ExistAProducerMergerEdge(CondensedMultiGraph* _C);
	bool ExistACycleOld(CondensedMultiGraph* _C);
	vector<CondensedMultiGraph> PickATerminalNodeAndCollapseFeasiblePaths();
	bool IsTree();
	vector<int> Hash();

	//call Algorithm2
	bool IsThereATerminalNodeThatDoesntHaveMaximalCapacity(CondensedMultiGraph* _C);
	//call Algorithm3
	bool ExistAMeregerSequenceForATerminalNode(CondensedMultiGraph* _C, bool& _terminate);
	//implements Algorithm4 in NAHS paper
	void Algorithm4_NAHS(CondensedMultiGraph* _C);

	// This function is used by Case I of Algorithm 6, it performs a merger if it is feasible.
	bool ExistAFeasibleMergerEdge(int from, int to, CondensedMultiGraph* _C);


	vector<int> GetArticulationPoints(vector<int>& Level);
};

