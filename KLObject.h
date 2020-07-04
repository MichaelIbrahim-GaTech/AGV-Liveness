#pragma once
#include <iostream>
using namespace std;
class KLObject
{
public:
	KLObject();
	KLObject(const KLObject& _kl);
	int K;
	int L;
	bool Considered;
	int AssociatedEdge;
	KLObject* prev;
	friend bool operator < (const KLObject& lhs, const KLObject& rhs);
	friend bool operator > (const KLObject& lhs, const KLObject& rhs);
};

