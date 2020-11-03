#include "Algorithm2.h"

InTree Algorithm2::MacroMergerTree(InTree it)
{
	if (it._size == 1)
		return it;
	int nextChild = it.GetNextUnprocessedEdge();
	while (nextChild != -1)
	{
		if (it._InTreeCapacitiesList[0] >= it._InTreeEdgeCapacitiesList[nextChild]) // feasible
		{
			if (it._InTreeCapacitiesList[nextChild] >= it._InTreeEdgeCapacitiesList[nextChild])//producer merger
			{
				it.MergeWithRoot(nextChild);
				it.MarkAllEdgesAsUnprocessed();
			}
			else // feasible non-producer merger
			{
				InTree temp = it;
				temp.MergeWithRoot(nextChild);
				temp = MacroMergerTree(temp);
				if (temp._InTreeCapacitiesList[0] >= it._InTreeCapacitiesList[0])
				{
					it = temp;
					it.MarkAllEdgesAsUnprocessed();
				}
				else
				{
					it._Processed[nextChild] = true;
				}
			}
		}
		else // infeasible basic merger
		{
			it._Processed[nextChild] = true;
		}
		nextChild = it.GetNextUnprocessedEdge();
	}
	return it;
}

TneTree Algorithm2::MacroMergerTree(TneTree it)
{
	if (it._size == 1)
		return it;
	int nextChild = it.GetNextUnprocessedEdge();
	while (nextChild != -1)
	{
		if (it._InTreeCapacitiesList[0] >= it._InTreeEdgeCapacitiesList[nextChild]) // feasible
		{
			if (it._InTreeCapacitiesList[nextChild] >= it._InTreeEdgeCapacitiesList[nextChild])//producer merger
			{
				it.MergeWithRoot(nextChild);
				it.MarkAllEdgesAsUnprocessed();
			}
			else // feasible non-producer merger
			{
				TneTree temp = it;
				temp.MergeWithRoot(nextChild);
				temp = MacroMergerTree(temp);
				if (temp._InTreeCapacitiesList[0] >= it._InTreeCapacitiesList[0])
				{
					it = temp;
					it.MarkAllEdgesAsUnprocessed();
				}
				else
				{
					it._Processed[nextChild] = true;
				}
			}
		}
		else // infeasible basic merger
		{
			it._Processed[nextChild] = true;
		}
		nextChild = it.GetNextUnprocessedEdge();
	}
	return it;
}

bool Algorithm2::MacroMerger(InTree it, vector<int>& MSEQ)
{
	InTree temp = MacroMergerTree(it);
	if (temp._InTreeList[0].size() > 1)
	{
		MSEQ = temp._InTreeList[0];
		return true;
	}
	return false;
}