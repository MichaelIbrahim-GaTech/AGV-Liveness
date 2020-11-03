#include "Algorithm3.h"

bool Algorithm3::MergingSource(TneTree tne, int edge, vector<int>& source)
{
	if (tne._InTreeCapacitiesList[0] < tne._InTreeEdgeCapacitiesList[edge]) // infeasible
		return false;
	tne.MergeWithRoot(edge);
	tne = Algorithm2::MacroMergerTree(tne);
	tne.MarkAllEdgesAsUnprocessed();

	if (tne.ASourceNodeCouldBeMergedWithRoot(source))
		return true;
	for (int i = 0; i < tne._RootChildren.size(); i++)
	{
		if (MergingSource(tne, tne._RootChildren[i], source))
			return true;
	}
	return false;
}

bool Algorithm3::MergingSource(TneTree tne, vector<int>& Path)
{
	pair<int, int>source;
	if (MergingSource(tne, 1, Path))
	{
		//reverse(Path.begin(), Path.end());
		return true;
	}
	return false;
}