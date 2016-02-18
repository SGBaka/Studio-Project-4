#ifndef PLAYER_H
#define PLAYER_H

#include "CharacterObject.h"
#include "MapScene.h"
#include "Application.h"
#include <algorithm> 

using namespace::std;

class EditAvatar : public CharacterObject
{
public:
	EditAvatar();
	~EditAvatar();

	void Init(Vector3 position);
	void Update(double dt);

	void movePlayer(int mode, double dt);	// 0 = left, 1 = right, 2 = up, 3 = down
	void selectTile(int mode);	
	/*	Insert tiles according to numbers;
		0 = Empty Space
		1 = Solid Wall
		2 = Player
		3 = Enemy
		4 = Winpoint
		5 = Waypoint
		6 = Danger Zone
	*/
	void placeTile(int mode);
	void saveMap(void);
	void checkMovement(int mode, double dt);
	void checkDirection(int mode, double dt);

	bool checkCollision(int mode);

	int roundUp(int numToRound, int factor);

	vector<bool> moveToDir;

	float moveSpeed;
	int selectedTile;

private:
	int mapWidth, mapHeight;
};
#endif