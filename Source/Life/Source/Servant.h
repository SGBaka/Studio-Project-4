#ifndef SERVANT_H
#define SERVANT_H

#include "CharacterObject.h"
#include "Pathfinding.h"
#include "Node.h"
#include "Pathing.h"
#include "MainScene.h"
#include <string>

using namespace::std;

class cServant : public CharacterObject
{
public:
	enum AI_STATES
	{
		AS_ROAM,
		AS_SERVE,
		AS_BATHROOM,
		AS_EATING,
		AS_BOMB,
		AS_NAVI,
	};

	enum AI_MESSAGES
	{
		AM_PILOT_FOOD,
		AM_COPILOT_FOOD,
		AM_COPILOT_TAKEOVER,
		AM_TOTAL_MSG,
	};

	AI_STATES AI_STATE;

	void Init(Vector3 position);
	void Update(double dt);

	cServant();
	virtual ~cServant();

	string pathFind(const int& xStart, const int& yStart, const int& xFinish, const int& yFinish);

	void setWaypoints(void);

	bool executePath(double dt, string& route, float& routeCounter);

	std::string getState(void);
private:

	string route, route2, route3;
	float routeCounter, routeCounter2, routeCounter3;

	CPathing patrolPath;

	Vector3 direction, turnTo;

	float angle;
	bool rotating;
	float rotCounter;

	int n, m;

	bool hasSetDest, hasSetDest2, hasFood;

	bool gotoServe, gotoNavi, gotoRoam;
};

#endif