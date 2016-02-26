#include "MPlayer.h"

MPlayer::MPlayer()
: moveSpeed(4)
, sonarCooldown(1)
, sonarTimer(sonarCooldown)
, specialCooldown(2)
, specialTimer(specialCooldown)
, specialDuration(0.2)
, specialROF(0.04)
, specialTimer2(specialROF)
, specialCounter(0)
, isSpecial(false)
, playerID(0)
{
	for (int i = 0; i < 4; ++i)
	{
		moveToDir.push_back(false);
	}

	mapWidth = MultScene::GetInstance()->ML_map.map_width;
	mapHeight = MultScene::GetInstance()->ML_map.map_height;
}

MPlayer::~MPlayer()
{

}

void MPlayer::Init(Vector3 position)
{
	SE_Engine.Init();

	this->position = position;

	LuaScript playerScript("character");
	sonarCooldown = playerScript.get<float>("player.sonar_cooldown");
	sonarTimer = sonarCooldown;
	specialCooldown = playerScript.get<float>("player.special_cooldown");
	specialTimer = specialCooldown;

	LuaScript sound("Sound");
	SoundList[ST_FOOTSTEPS] = SE_Engine.preloadSound(sound.getGameData("sound.footsteps").c_str());
}


void MPlayer::Update(double dt)
{
	if (std::all_of(moveToDir.begin(), moveToDir.end(), [](bool v){return !v; })) // Check if all boolean in vector list are false
	{
		if ((Application::IsKeyPressed('W') && !checkCollision(2) && playerID == 1)
			|| (Application::IsKeyPressed(VK_UP) && !checkCollision(2) && playerID == 2))
			checkMovement(2, dt);
		else if ((Application::IsKeyPressed('A') && !checkCollision(0) && playerID == 1)
			|| (Application::IsKeyPressed(VK_LEFT) && !checkCollision(0) && playerID == 2))
			checkMovement(0, dt);
		else if ((Application::IsKeyPressed('S') && !checkCollision(3) && playerID == 1)
			|| (Application::IsKeyPressed(VK_DOWN) && !checkCollision(3) && playerID == 2))
			checkMovement(3, dt);
		else if ((Application::IsKeyPressed('D') && !checkCollision(1) && playerID == 1)
			|| (Application::IsKeyPressed(VK_RIGHT) && !checkCollision(1) && playerID == 2))
			checkMovement(1, dt);

		if (Application::IsKeyPressed('W') ||
			Application::IsKeyPressed('A') ||
			Application::IsKeyPressed('S') ||
			Application::IsKeyPressed('D') ||
			Application::IsKeyPressed(VK_UP) ||
			Application::IsKeyPressed(VK_DOWN) ||
			Application::IsKeyPressed(VK_LEFT) ||
			Application::IsKeyPressed(VK_RIGHT))
		{
			if (!SE_Engine.isSoundPlaying(SoundList[ST_FOOTSTEPS]))
				SE_Engine.playSound2D(SoundList[ST_FOOTSTEPS]);
		}

		if ((Application::IsKeyPressed('N') && sonarTimer >= sonarCooldown && playerID == 2) || 
			(Application::IsKeyPressed(VK_LBUTTON) && sonarTimer >= sonarCooldown && playerID == 1))
		{
			LuaScript playerScript("character");
			sonarTimer = 0;
			Sonar *SNR;
			SNR = new Sonar();
			SNR->Init(playerScript.get<float>("player.sonar_radius"), playerScript.get<float>("player.sonar_radius2"), playerScript.get<int>("player.sonar_sides"), playerScript.get<float>("player.sonar_speed"));
			SNR->GenerateSonar(position, 1);
			sonarList.push_back(SNR);
		}
		else if ((Application::IsKeyPressed('M') && specialTimer >= specialCooldown && !isSpecial && playerID == 2) 
			|| (Application::IsKeyPressed(VK_RBUTTON) && specialTimer >= specialCooldown && !isSpecial && playerID == 1))
		{
			specialPos = position;
			specialTimer = 0;
			isSpecial = true;
		}
	}
	if (isSpecial && specialCounter < specialDuration)
	{
		if (specialTimer2 >= specialROF)
		{
			LuaScript playerScript("character");
			specialTimer2 = 0;
			Sonar *SNR;
			SNR = new Sonar();
			SNR->Init(playerScript.get<float>("player.special_radius"), playerScript.get<float>("player.special_radius2"), playerScript.get<int>("player.special_sides"), playerScript.get<float>("player.special_speed"));
			SNR->GenerateSonar(specialPos, 2);
			sonarList.push_back(SNR);
		}
	}

	if (sonarTimer < sonarCooldown)
		sonarTimer += dt;

	if (specialTimer < specialCooldown)
		specialTimer += dt;

	if (specialTimer2 < specialROF && isSpecial)
		specialTimer2 += dt;

	if (isSpecial)
		specialCounter += dt;

	if (specialCounter >= specialDuration)
	{
		isSpecial = false;
		specialCounter = 0;
		specialTimer2 = 0;
		specialPos.SetZero();
	}


	for (int i = 0; i < sonarList.size(); ++i)
	{
		sonarList[i]->Update(dt);

		if (sonarList[i]->segmentList.empty())
		{
			delete sonarList[i];
			sonarList.erase(sonarList.begin() + i);
		}
	}
	for (int i = 0; i < moveToDir.size(); ++i)
	{
		checkDirection(i, dt);
	}
}

bool MPlayer::checkCollision(int mode)
{
	switch (mode)
	{
	case 0:
		return (MultScene::GetInstance()->ML_map.map_data[currTile.y][currTile.x - 1] == "1");
		break;
	case 1:
		return (MultScene::GetInstance()->ML_map.map_data[currTile.y][currTile.x + 1] == "1");
		break;
	case 2:
		return (MultScene::GetInstance()->ML_map.map_data[currTile.y - 1][currTile.x] == "1");
		break;
	case 3:
		return (MultScene::GetInstance()->ML_map.map_data[currTile.y + 1][currTile.x] == "1");
		break;
	default:
		return false;
		break;
	}
}

void MPlayer::checkDirection(int mode, double dt)
{
	if (moveToDir[mode])
	{
		movePlayer(mode, dt);

		float check;

		if (mode <= 1)
			check = (float)position.x / (MultScene::GetInstance()->ML_map.worldSize * 2);
		else
			check = (float)position.y / (MultScene::GetInstance()->ML_map.worldSize * 2);

		if (check == (int)check)
		{
			moveToDir[mode] = false;
		}
	}
}

void MPlayer::checkMovement(int mode, double dt)
{
	movePlayer(mode, dt);

	float check;

	if (mode <= 1)
		check = (float)position.x / (MultScene::GetInstance()->ML_map.worldSize * 2);
	else
		check = (float)position.y / (MultScene::GetInstance()->ML_map.worldSize * 2);

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

void MPlayer::movePlayer(int mode, double dt)
{
	switch (mode)
	{
	case 0:
		position.x -= moveSpeed;
		topLeft.x -= moveSpeed;
		bottomRight.x -= moveSpeed;
		break;
	case 1:
		position.x += moveSpeed;
		topLeft.x += moveSpeed;
		bottomRight.x += moveSpeed;
		break;
	case 2:
		position.y += moveSpeed;
		topLeft.y += moveSpeed;
		bottomRight.y += moveSpeed;
		break;
	case 3:
		position.y -= moveSpeed;
		topLeft.y -= moveSpeed;
		bottomRight.y -= moveSpeed;
		break;
	}
}

int MPlayer::roundUp(int numToRound, int factor)
{
	vector<int> factors;

	for (int i = 2; i <= factor; ++i)
	{
		while (factor % i == 0)
		{
			factor /= i;
			factors.push_back(i);
		}
	}

	for (int i = 0; i < factors.size(); ++i)
	{
		if (numToRound <= factors[i])
		{
			numToRound = factors[i];
			return numToRound;
		}
	}
}
