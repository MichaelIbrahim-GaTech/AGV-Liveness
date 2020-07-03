#pragma once
class KLObject
{
public:
	int K;
	int L;
	bool Considered;
	int AssociatedEdge;
	KLObject* prev;
	friend bool operator < (const KLObject& lhs, const KLObject& rhs);
	friend bool operator > (const KLObject& lhs, const KLObject& rhs);
};

