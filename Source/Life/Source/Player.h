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

	int sonarCooldown;
	float sonarTimer;

private:
	int mapWidth, mapHeight;
};


#endif