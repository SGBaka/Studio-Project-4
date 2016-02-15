#include "Node.h"

CNode::CNode()
{
	xPos = 0; yPos = 0; level = 0; priority = 0;
}


CNode::CNode(int xP, int yP, int lvl, int p)
{
	xPos = xP; yPos = yP; level = lvl; priority = p;
}


CNode::~CNode(void)
{
}

void CNode::updatePriority(const int& xDes, const int& yDes)
{
	// A*
	priority = level + estimate(xDes, yDes) * 10;
}

void CNode::nextLevel(const int & i)
{
	level += 10;
}

const int& CNode::estimate(const int& xDes, const int& yDes) const
{
	static int xd, yd, d;
	xd = xDes - xPos;
	yd = yDes - yPos;

	// Euclidian Distance
	d = static_cast<int>(sqrt(static_cast<float>(xd * xd + yd * yd)));

	// Manhattan distance
	//d=abs(xd)+abs(yd);

	// Chebyshev distance
	//d=max(abs(xd), abs(yd));

	return(d);
}

bool operator<(const CNode& a, const CNode& b)
{
	return a.getPriority() > b.getPriority();
}
