#ifndef MPLAYER_H
#define MPLAYER_H

#include "CharacterObject.h"
#include "MultScene.h"
#include "Application.h"
#include <algorithm> 
#include "Sonar.h"

using namespace::std;

class MPlayer : public CharacterObject
{
public:

	enum SOUND_TYPE
	{
		ST_FOOTSTEPS,
		ST_TOTAL
	};

	MPlayer();
	~MPlayer();

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

	float specialDuration, specialROF, specialTimer2, specialCounter;
	Vector3 specialPos;

	bool isSpecial;
	float playerID;

private:
	int mapWidth, mapHeight;

	SoundEngine SE_Engine;
	ISoundSource *SoundList[ST_TOTAL];
};


#endif