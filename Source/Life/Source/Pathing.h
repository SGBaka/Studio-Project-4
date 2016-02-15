#pragma once

#include "Vector3.h"
#include <vector>
#include <cstdarg>

using namespace::std;

class CPathing
{
public:
	CPathing(void);
	~CPathing(void);

	//Set Points
	void setWayPoints(int amount, ...);

	//Vector List to store Way Points
	std::vector<Vector3> WayPointList;
	std::vector<Vector3> WayPointTileList;
	short location;

};