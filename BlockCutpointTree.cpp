#include "BlockCutpointTree.h"

bool BlockCutpointTree::AllExplored()
{
	for (int i = 0; i < Explored.size(); i++)
	{
		if (!Explored[i])
			return false;
	}
	return true;
}

int BlockCutpointTree::IsExploredArt(int current)
{
	for (map<int, int>::iterator itr = U->directed[current].begin(); itr != U->directed[current].end(); itr++)
	{
		if (!Explored[itr->first])
		{
			return itr->first;
		}
	}
	for (map<int, int>::iterator itr = U->reversedEdges[current].begin(); itr != U->reversedEdges[current].end(); itr++)
	{
		if (!Explored[itr->first])
		{
			return itr->first;
		}
	}
	return -1;
}

BlockCutpointTree::BlockCutpointTree(DirectedAcyclicMultiGraph* _U, vector<int> art, vector<int> level)
	: Explored(_U->directed.size(), false)
{
	U = _U;

	vector<int> ArtBlock(_U->directed.size(), -1);
	vector<bool> IsArt(_U->directed.size(), false);

	for (int i = 0; i < art.size(); i++)
	{
		IsArt[art[i]] = true;
	}
	stack<int> ToExploreArt;
	if (!IsArt[_U->nh])
	{
		set<int> Block0;
		stack<int> ToExplore0;
		Block0.insert(_U->nh);
		ToExplore0.push(_U->nh);
		Explored[_U->nh] = true;
		while (!ToExplore0.empty())
		{
			int current = ToExplore0.top();
			ToExplore0.pop();
			for (map<int, int>::iterator itr = _U->directed[current].begin(); itr != _U->directed[current].end(); itr++)
			{
				if (!Explored[itr->first])
				{
					Block0.insert(itr->first);
					Explored[itr->first] = true;
					if (IsArt[itr->first])
					{
						ArtBlock[itr->first] = 0;
						ToExploreArt.push(itr->first);
					}
					else
					{
						ToExplore0.push(itr->first);
					}
				}
			}
			for (map<int, int>::iterator itr = _U->reversedEdges[current].begin(); itr != _U->reversedEdges[current].end(); itr++)
			{
				if (!Explored[itr->first])
				{
					Block0.insert(itr->first);
					Explored[itr->first] = true;
					if (IsArt[itr->first])
					{
						ArtBlock[itr->first] = 0;
						ToExploreArt.push(itr->first);
					}
					else
					{
						ToExplore0.push(itr->first);
					}
				}
			}
		}
		Blocks.push_back(Block0);
	}
	else
	{
		set<int> Block0;
		Block0.insert(_U->nh);
		Blocks.push_back(Block0);
		ArtBlock[_U->nh] = 0;
		ToExploreArt.push(_U->nh);
		Explored[_U->nh] = true;
	}
	// do the iterative algorithm
	while (!ToExploreArt.empty())
	{
		int current = ToExploreArt.top();
		int next = IsExploredArt(current);
		if (next == -1) // fully explored articulation point
		{
			ToExploreArt.pop();
		}
		else
		{
			if (IsArt[next] && (level[next] != level[current]))
			{
				set<int> Block;
				Block.insert(current);
				Block.insert(next);
				Explored[current] = true;
				Explored[next] = true;
				ArtBlock[next] = Blocks.size();
				Blocks.push_back(Block);
				ToExploreArt.push(next);
			}
			else
			{
				int WhereToPushBlock = Blocks.size();
				set<int> Block;
				Block.insert(current);
				Block.insert(next);
				Explored[current] = true;
				Explored[next] = true;
				if (IsArt[next] && (ArtBlock[next] == -1))
					ArtBlock[next] = WhereToPushBlock;
				stack<int> Explore;
				Explore.push(next);
				while (!Explore.empty())
				{
					int node = Explore.top();
					Explore.pop();

					
					for (map<int, int>::iterator itr = _U->directed[node].begin(); itr != _U->directed[node].end(); itr++)
					{
						if (Block.find(itr->first) != Block.end())
							continue;
						if (IsArt[node])
						{
							if (level[node] != level[itr->first])
								continue;
						}
						if (!Explored[itr->first])
						{
							Block.insert(itr->first);
							Explored[itr->first] = true;
							if (IsArt[itr->first])
							{
								ArtBlock[itr->first] = WhereToPushBlock;
								ToExploreArt.push(itr->first);
							}
							Explore.push(itr->first);
							
						}
					}
					for (map<int, int>::iterator itr = _U->reversedEdges[node].begin(); itr != _U->reversedEdges[node].end(); itr++)
					{
						if (Block.find(itr->first) != Block.end())
							continue;
						if (IsArt[node])
						{
							if (level[node] != level[itr->first])
								continue;
						}
						if (!Explored[itr->first])
						{
							Block.insert(itr->first);
							Explored[itr->first] = true;
							if (IsArt[itr->first])
							{
								ArtBlock[itr->first] = WhereToPushBlock;
								ToExploreArt.push(itr->first);
							}
							Explore.push(itr->first);

						}
					}
				}
				Blocks.push_back(Block);
			}
		}
	}
	// classify the blocks
	for (int i = 0; i < Blocks.size(); i++)
	{
		Types.push_back(BlockType::NODE);
	}
	for (int i = 0; i < Blocks.size(); i++)
	{
		if (Blocks[i].size() == 1)
		{
			Types[i] = BlockType::NODE;
		}
		else if (Blocks[i].size() == 2) // Block Types: I & II
		{
			Types[i] = BlockType::CASE_II;

			set<int>::iterator first = Blocks[i].begin();
			set<int>::iterator second = first++;
			if (IsArt[*first] && !IsArt[*second])
			{
				for (map<int, int>::iterator itr = _U->directed[*first].begin(); itr != _U->directed[*first].end(); itr++)
				{
					if (itr->first == *second)
					{
						Types[i] = BlockType::CASE_I;
						break;
					}
				}
			}
			else if (!IsArt[*first] && IsArt[*second])
			{
				for (map<int, int>::iterator itr = _U->directed[*second].begin(); itr != _U->directed[*second].end(); itr++)
				{
					if (itr->first == *first)
					{
						Types[i] = BlockType::CASE_I;
						break;
					}
				}
			}
			else if (ArtBlock[*first] < ArtBlock[*second])// first is the parent ap 
			{
				for (map<int, int>::iterator itr = _U->directed[*first].begin(); itr != _U->directed[*first].end(); itr++)
				{
					if (itr->first == *second)
					{
						Types[i] = BlockType::CASE_I;
						break;
					}
				}
			}
			else
			{
				for (map<int, int>::iterator itr = _U->directed[*second].begin(); itr != _U->directed[*second].end(); itr++)
				{
					if (itr->first == *first)
					{
						Types[i] = BlockType::CASE_I;
						break;
					}
				}
			}

		}
		else // Block Types III, IV & V
		{
			if (Blocks[i].find(_U->nh) != Blocks[i].end()) // Block Type V
			{
				Types[i] = BlockType::CASE_V;
			}
			else // BlockTypes III & IV
			{
				Types[i] = BlockType::CASE_IV;
				int parentAP;
				for (set<int>::iterator itr = Blocks[i].begin(); itr != Blocks[i].end(); itr++)
				{
					if (IsArt[*itr])
					{
						if (ArtBlock[*itr] < i)
						{
							parentAP = *itr;
						}
					}
				}
				for (map<int, int>::iterator itr = _U->directed[parentAP].begin(); itr != _U->directed[parentAP].end(); itr++)
				{
					if(Blocks[i].find(itr->first) != Blocks[i].end())
					{
						Types[i] = BlockType::CASE_III;
						break;
					}
				}

			}
		}
	}
	for (int i = 0; i < Blocks.size(); i++)
	{
		ParentBlock.push_back(-1);
		Children.push_back(vector<int>());
		Processed.push_back(false);
		ParentAP.push_back(-1);
	}
	for (int i = 0; i < Blocks.size(); i++)
	{
		for (set<int>::iterator itr = Blocks[i].begin(); itr != Blocks[i].end(); itr++)
		{
			if (IsArt[*itr])
			{
				if (ArtBlock[*itr] < i)
				{
					ParentBlock[i] = ArtBlock[*itr];
					Children[ArtBlock[*itr]].push_back(i);
					ParentAP[i] = *itr;
					break;
				}
			}
		}
	}
	for (int i = 0; i < Blocks.size(); i++)
		ConsideredBlocks.insert(i);
}

//BlockCutpointTree::BlockCutpointTree(DirectedAcyclicMultiGraph* _U, vector<int> art, vector<int> level, int _subtree)
//	:BlockCutpointTree(_U, art, level)
//{
//	ConsideredBlocks.clear();
//	//find first occurence of the parent ap called _subtree
//	for (int i = 0; i < Blocks.size(); i++)
//	{
//		if (Blocks[i].find(_subtree) != Blocks[i].end())
//		{
//			stack<int> Explore;
//			Explore.push(i);
//			while (!Explore.empty())
//			{
//				int block = Explore.top();
//				Explore.pop();
//				ConsideredBlocks.insert(block);
//				for (int itr = 0; itr < Children[block].size(); itr++)
//					Explore.push(Children[block][itr]);
//			}
//			return;
//		}
//	}
//
//}

BlockCutpointTree::BlockCutpointTree(DirectedAcyclicMultiGraph* _U, vector<int> art, vector<int> level, int _subtree, set<int> _subtreeNodes)
	:BlockCutpointTree(_U, art, level)
{
	ConsideredBlocks.clear();
	for (int i = 0; i < Blocks.size(); i++)
	{
		set<int> intersect;
		set_intersection(Blocks[i].begin(), Blocks[i].end(), _subtreeNodes.begin(), _subtreeNodes.end(),
			std::inserter(intersect, intersect.begin()));
		if (intersect.size() == Blocks[i].size())
			ConsideredBlocks.insert(i);
	}
}

int BlockCutpointTree::GetNextBlockToProcess()
{
	// first check the easier problem, CASE_I, CASE_V, CASE_III, CASE_II, CASE_IV
	vector<BlockType> ProblemPriority;
	ProblemPriority.push_back(BlockType::CASE_I);
	ProblemPriority.push_back(BlockType::CASE_V);
	ProblemPriority.push_back(BlockType::CASE_III);
	ProblemPriority.push_back(BlockType::CASE_IV);
	ProblemPriority.push_back(BlockType::CASE_II);
	for (int p = 0; p < ProblemPriority.size(); p++)
	{
		for (set<int>::iterator i = ConsideredBlocks.begin(); i != ConsideredBlocks.end(); i++)
		{
			if (!Processed[*i])
			{
				if (Types[*i] == ProblemPriority[p])
				{
					bool leaf = Children[*i].empty();
					if (leaf)
						return *i;
					// check for a node with all children processed
					bool AllChildrenProcessed = true;
					for (vector<int>::iterator itr = Children[*i].begin(); itr != Children[*i].end(); itr++)
					{
						if (!Processed[*itr])
						{
							AllChildrenProcessed = false;
							break;
						}
					}
					if (AllChildrenProcessed)
						return *i;
				}
			}
		}
	}
	return -1;
}

set<int> BlockCutpointTree::GetSubTreeNodes(int block)
{
	set<int> nodes;
	stack<int> Explore;
	Explore.push(block);
	while (!Explore.empty())
	{
		int b = Explore.top();
		Explore.pop();
		for (set<int>::iterator itr = Blocks[b].begin(); itr != Blocks[b].end(); itr++)
			nodes.insert(*itr);
		for (int i = 0; i < Children[b].size(); i++)
			Explore.push(Children[b][i]);
	}
	return nodes;
}