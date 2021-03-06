#ifndef PLAYER_H
#define PLAYER_H

#include "CharacterObject.h"
#include "MainScene.h"
#include "Application.h"
#include <algorithm> 
#include "Sonar.h"

using namespace::std;

class Player : public CharacterObject
{
public:

	enum SOUND_TYPE
	{
		ST_FOOTSTEPS,
		ST_N_SONAR,
		ST_S_SONAR,
		ST_TOTAL
	};

	Player();
	~Player();

	void Init(Vector3 position);
	void Update(double dt);

	void movePlayer(int mode, double dt);	// 0 = left, 1 = right, 2 = up, 3 = down
	void checkMovement(int mode, double dt);
	void checkDirection(int mode, double dt);

	bool checkCollision(int mode);

	int roundUp(int numToRound, int factor);

	vector<bool> moveToDir;

	float moveSpeed;

	vector<Sonar*> sonarList;

	float sonarCooldown, specialCooldown;
	float sonarTimer, specialTimer;

	float specialDuration, specialROF, specialTimer2, specialCounter, sonarRad, specialRad;
	Vector3 specialPos;

	bool isSpecial;

private:
	int mapWidth, mapHeight;

	SoundEngine SE_Engine;
	ISoundSource *SoundList[ST_TOTAL];
};


#endif