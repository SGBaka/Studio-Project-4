#ifndef ENEMY_H
#define ENEMY_H

#include "CharacterObject.h"
#include "Pathfinding.h"
#include "Node.h"
#include "Pathing.h"
#include "MainScene.h"
#include "Sonar.h"
#include <string>

using namespace::std;

class cEnemy : public CharacterObject
{
public:
	enum AI_STATES
	{
		AS_ROAM,
		AS_CHASE,
		AS_IDLE,
		AS_SUSP,
	};

	enum AI_MESSAGES
	{
		AM_TOTAL_MSG,
	};

	enum SOUND_TYPE
	{
		ST_SONAR,
		ST_TOTAL
	};

	int ID;
	AI_STATES AI_STATE;

	void Init(Vector3 position);
	void Update(double dt);

	cEnemy();
	virtual ~cEnemy();

	string pathFind(const int& xStart, const int& yStart, const int& xFinish, const int& yFinish);

	void setWaypoints(void);

	bool executePath(double dt, string& route, float& routeCounter);

	std::string getState(void);

	float GetDistance(float x1, float y1, float x2, float y2);

	bool gotoServe, gotoIdle, gotoRoam, gotoChase, gotoSusp;

	vector<Sonar*> sonarList;
	float sonarCooldown,default_sonarCooldown, chase_sonarCooldown, sonarTimer;
	
	int sonar_radius, sonar_sides, sonar_speed;

	bool isVisible;

	static int smartCount, stupidCount;

	Color color;

	float fadeTimer, fadeDuration;

	float  chase_timer_max;

	float suspDuration;

	Vector3 suspPos;

	float timer;
	int idleTimemin, idleTimemax;
	int map_min_x, map_max_x, map_min_y, map_max_y;
	int vislble_range;

	bool hasPath;
private:

	string route, route2, route3;
	float routeCounter, routeCounter2, routeCounter3;

	CPathing patrolPath;

	Vector3 direction, turnTo;

	float angle;
	bool rotating;
	float rotCounter;
	float timer2, timer3;
	float idleTime;
	int n, m;

	bool hasSetDest, hasSetDest2;
	Sonar sonar;
	bool smart;
	int probability;
	
	SoundEngine SE_Engine;
	ISoundSource *SoundList[ST_TOTAL];
};

#endif