#include "Player.h"

Player::Player() 
: moveSpeed(250)
{
	for (int i = 0; i < 4; ++i)
	{
		moveToDir.push_back(false);
	}

	mapWidth = MainScene::GetInstance()->ML_map.map_width;
	mapHeight = MainScene::GetInstance()->ML_map.map_height;
}

Player::~Player()
{

}

void Player::Init(Vector3 position)
{
	this->position = position;
}


void Player::Update(double dt)
{
	if (std::all_of(moveToDir.begin(), moveToDir.end(), [](bool v){return !v;})) // Check if all boolean in vector list are false
	{
		if (Application::IsKeyPressed('W') && !checkCollision(2))
			checkMovement(2, dt);
		else if (Application::IsKeyPressed('A') && !checkCollision(0))
			checkMovement(0, dt);
		else if (Application::IsKeyPressed('S') && !checkCollision(3))
			checkMovement(3, dt);
		else if (Application::IsKeyPressed('D') && !checkCollision(1))
			checkMovement(1, dt);
	}

	for (int i = 0; i < moveToDir.size(); ++i)
	{
		checkDirection(i, dt);
	}
}

bool Player::checkCollision(int mode)
{
	switch (mode)
	{
	case 0:
		return (MainScene::GetInstance()->ML_map.map_data[currTile.y][currTile.x - 1] == "1");
		break;
	case 1:
		return (MainScene::GetInstance()->ML_map.map_data[currTile.y][currTile.x + 1] == "1");
		break;
	case 2:
		return (MainScene::GetInstance()->ML_map.map_data[currTile.y - 1][currTile.x] == "1");
		break;
	case 3:
		return (MainScene::GetInstance()->ML_map.map_data[currTile.y + 1][currTile.x] == "1");
		break;
	default:
		return false;
		break;
	}
}

void Player::checkDirection(int mode, double dt)
{
	if (moveToDir[mode])
	{
		movePlayer(mode, dt);

		float check;

		if (mode <= 1)
			check = (float)position.x / (MainScene::GetInstance()->ML_map.worldSize * 2);
		else
			check = (float)position.y / (MainScene::GetInstance()->ML_map.worldSize * 2);

		if (check == (int)check)
		{
			moveToDir[mode] = false;
		}
	}
}

void Player::checkMovement(int mode, double dt)
{
	movePlayer(mode, dt);

	float check;

	if (mode <= 1)
		check = (float)position.x / (MainScene::GetInstance()->ML_map.worldSize * 2);
	else
		check = (float)position.y / (MainScene::GetInstance()->ML_map.worldSize * 2);

	if (moveToDir[mode] == false)
	{
		switch (mode)
		{
		case 0:
			currTile.x--;
			break;
		case 1:
			currTile.x++;
			break;
		case 2:
			currTile.y--;
			break;
		case 3:
			currTile.y++;
			break;
		}
	}

	if (check != (int)check)
		moveToDir[mode] = true;

}

void Player::movePlayer(int mode, double dt)
{
	switch (mode)
	{
	case 0:
		position.x -= roundUp(moveSpeed * dt, 2);
		break;
	case 1:
		position.x += roundUp(moveSpeed * dt, 2);
		break;
	case 2:
		position.y += roundUp(moveSpeed * dt, 2);
		break;
	case 3:
		position.y -= roundUp(moveSpeed * dt, 2);
		break;
	}
}

int Player::roundUp(int numToRound, int multiple)
{
	if (multiple == 0)
		return numToRound;

	int remainder = numToRound % multiple;
	if (remainder == 0)
		return numToRound;

	return numToRound + multiple - remainder;
}
