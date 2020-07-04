#include "KLObject.h"

KLObject::KLObject()
{
	K = -1;
	L = -1;
	AssociatedEdge = -1;
	Considered = false;
	prev = NULL;
}

KLObject::KLObject(const KLObject& _kl)
{
	K = _kl.K;
	L = _kl.L;
	AssociatedEdge = _kl.AssociatedEdge;
	Considered = _kl.Considered;
	prev = _kl.prev;
}

bool operator < (const KLObject& lhs, const KLObject& rhs)
{
	return ((lhs.K <= rhs.K) && (lhs.L <= rhs.L));
}

bool operator > (const KLObject& lhs, const KLObject& rhs)
{
	return ((lhs.K >= rhs.K) && (lhs.L >= rhs.L));
}