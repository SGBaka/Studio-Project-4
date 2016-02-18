#include "EditAvatar.h"

EditAvatar::EditAvatar()
: moveSpeed(5)
, selectedTile(0)
{
	for (int i = 0; i < 4; ++i)
	{
		moveToDir.push_back(false);
	}

	mapWidth = MapScene::GetInstance()->ML_map.map_width;
	mapHeight = MapScene::GetInstance()->ML_map.map_height;
}

EditAvatar::~EditAvatar()
{

}

void EditAvatar::Init(Vector3 position)
{
	this->position = position;
}


void EditAvatar::Update(double dt)
{
	if (std::all_of(moveToDir.begin(), moveToDir.end(), [](bool v){return !v; })) // Check if all boolean in vector list are false
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
	
	if (Application::IsKeyPressed('0'))
		selectedTile = 0;
	if (Application::IsKeyPressed('1'))
		selectedTile = 1;
	if (Application::IsKeyPressed('2'))
		selectedTile = 2;
	if (Application::IsKeyPressed('R'))
		placeTile(selectedTile);
	if (Application::IsKeyPressed('P'))
		saveMap();

	for (int i = 0; i < moveToDir.size(); ++i)
	{
		checkDirection(i, dt);
	}
	//cout << selectedTile << endl;
}

bool EditAvatar::checkCollision(int mode)
{
	/*switch (mode)
	{
	case 0:
		return (MapScene::GetInstance()->ML_map.map_data[currTile.y][currTile.x - 1] == "1");
		break;
	case 1:
		return (MapScene::GetInstance()->ML_map.map_data[currTile.y][currTile.x + 1] == "1");
		break;
	case 2:
		return (MapScene::GetInstance()->ML_map.map_data[currTile.y - 1][currTile.x] == "1");
		break;
	case 3:
		return (MapScene::GetInstance()->ML_map.map_data[currTile.y + 1][currTile.x] == "1");
		break;
	default:
		return false;
		break;
	}*/
	return false;
}

void EditAvatar::checkDirection(int mode, double dt)
{
	if (moveToDir[mode])
	{
		movePlayer(mode, dt);

		float check;

		if (mode <= 1)
			check = (float)position.x / (MapScene::GetInstance()->ML_map.worldSize * 2);
		else
			check = (float)position.y / (MapScene::GetInstance()->ML_map.worldSize * 2);

		if (check == (int)check)
		{
			moveToDir[mode] = false;
		}
	}
}

void EditAvatar::checkMovement(int mode, double dt)
{
	movePlayer(mode, dt);

	float check;

	if (mode <= 1)
		check = (float)position.x / (MapScene::GetInstance()->ML_map.worldSize * 2);
	else
		check = (float)position.y / (MapScene::GetInstance()->ML_map.worldSize * 2);

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

void EditAvatar::movePlayer(int mode, double dt)
{
	switch (mode)
	{
	case 0:
		position.x -= roundUp(moveSpeed, 20);
		break;
	case 1:
		position.x += roundUp(moveSpeed, 20);
		break;
	case 2:
		position.y += roundUp(moveSpeed, 20);
		break;
	case 3:
		position.y -= roundUp(moveSpeed, 20);
		break;
	}
}

void EditAvatar::placeTile(int mode)
{
	GameObject *GO;
	GO = new GameObject();
	GO->Init(Vector3(position.x, position.y, -0.5f));
	GO->scale.Set(MapScene::GetInstance()->ML_map.worldSize, MapScene::GetInstance()->ML_map.worldSize, 1);
	switch (mode)
	{
	case 0:
		GO->mesh = MapScene::GetInstance()->P_meshArray[MapScene::E_GEO_FLOOR_1];
		break;
	case 1:
		GO->mesh = MapScene::GetInstance()->P_meshArray[MapScene::E_GEO_WALL_1];
		break;
	case 2:
		GO->mesh = MapScene::GetInstance()->P_meshArray[MapScene::E_GEO_PLAYER];
		break;
	/*case 3:
		GO->mesh = MapScene::GetInstance()->P_meshArray[MapScene::E_GEO_ENEMY];
		break;
	case 4:
		GO->mesh = MapScene::GetInstance()->P_meshArray[MapScene::E_GEO_WINPOINT];
		break;
	case 5:
		GO->mesh = MapScene::GetInstance()->P_meshArray[MapScene::E_GEO_WAYPOINT];
		break;
	case 6:
		GO->mesh = MapScene::GetInstance()->P_meshArray[MapScene::E_GEO_DANGERPOINT];
		break;*/
	}

	MapScene::GetInstance()->GO_List.push_back(GO);
}

void EditAvatar::selectTile(int mode)
{
	switch (mode)
	{
	case 0:
		MapScene::GetInstance()->P_meshArray[MapScene::E_GEO_FLOOR_1];
		break;
	case 1:
		MapScene::GetInstance()->P_meshArray[MapScene::E_GEO_WALL_1];
		break;
	case 2:
		MapScene::GetInstance()->P_meshArray[MapScene::E_GEO_PLAYER];
		break;
	//case 3:
	//	MapScene::GetInstance()->P_meshArray[MapScene::E_GEO_ENEMY];
	//	break;
	//case 4:
	//	MapScene::GetInstance()->P_meshArray[MapScene::E_GEO_WINPOINT];
	//	break;
	//case 5:
	//	MapScene::GetInstance()->P_meshArray[MapScene::E_GEO_WAYPOINT];
	//	break;
	//case 6:
	//	MapScene::GetInstance()->P_meshArray[MapScene::E_GEO_DANGERPOINT];
	//	break;
	}
}

void EditAvatar::saveMap(void)
{
	int level = 0;
	LuaScript scriptlevel("maps");
	std::cout << "Save as Level :  " << endl;
	std::cin >> level;
	std::string luaName = "map.map.level_" + std::to_string(static_cast<unsigned long long>(level));
	MapScene::GetInstance()->ML_map.saveMap(scriptlevel.getGameData(luaName.c_str()));
	std::cout << "Saved!" << endl;
}

int EditAvatar::roundUp(int numToRound, int factor)
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
