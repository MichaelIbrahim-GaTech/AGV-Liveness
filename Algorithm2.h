#pragma once
#include "InTree.h"
#include "TneTree.h"

class Algorithm2
{
public:
	static InTree MacroMergerTree(InTree it);
	static TneTree MacroMergerTree(TneTree it);
	static bool MacroMerger(InTree it, vector<int>& MSEQ);

};

