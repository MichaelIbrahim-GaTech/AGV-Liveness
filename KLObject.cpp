#include "KLObject.h"


bool operator < (const KLObject& lhs, const KLObject& rhs)
{
	return ((lhs.K <= rhs.K) && (lhs.L <= rhs.L));
}

bool operator > (const KLObject& lhs, const KLObject& rhs)
{
	return ((lhs.K >= rhs.K) && (lhs.L >= rhs.L));
}