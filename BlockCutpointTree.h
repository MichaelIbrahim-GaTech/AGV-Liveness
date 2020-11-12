#pragma once
#include "DirectedAcyclicMultiGraph.h"

enum BlockType {NODE, CASE_I, CASE_II, CASE_III, CASE_IV, CASE_V};
class BlockCutpointTree
{
	DirectedAcyclicMultiGraph* U;
	vector<bool> Explored;

	bool AllExplored();
	int IsExploredArt(int);

	set<int> ConsideredBlocks;
public:
	vector<set<int>> Blocks;
	vector<BlockType> Types;
	// Block parent and children
	vector<int> ParentBlock;
	vector<vector<int>> Children;
	vector<bool> Processed;
	vector<int> ParentAP;

	int GetNextBlockToProcess();
	BlockCutpointTree(DirectedAcyclicMultiGraph* _U, vector<int> art, vector<int> level);
	//BlockCutpointTree(DirectedAcyclicMultiGraph* _U, vector<int> art, vector<int> level, int _subtree);
	BlockCutpointTree(DirectedAcyclicMultiGraph* _U, vector<int> art, vector<int> level, int _subtree, set<int> _subtreeNodes);

	set<int> GetSubTreeNodes(int block);
};

