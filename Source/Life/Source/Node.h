#pragma once
#include <math.h>

class CNode
{

private:
	int xPos, yPos;
	int level;
	int priority;

public:
	CNode();
	CNode(int xP, int yP, int lvl, int p);
	~CNode(void);

	int getPosX() const { return xPos; }
	int getPosY() const { return yPos; }
	int getLevel() const { return level; }
	int getPriority() const { return priority; }

	void updatePriority(const int& xDes, const int& yDes);

	// give better priority to going strait instead of diagonally
	void nextLevel(const int & i); // i: direction

	// Estimation function for the remaining distance to the goal.
	const int& estimate(const int& xDes, const int& yDes) const;

	friend bool operator<(const CNode& a, const CNode& b);
};