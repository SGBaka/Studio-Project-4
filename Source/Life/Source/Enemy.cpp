#include "Enemy.h"
#include "Player.h"
#include "MyMath.h"

int cEnemy::AI_counter = 0;

cEnemy::cEnemy()
: route("")
, routeCounter(0)
, route2("")
, routeCounter2(0)
, route3("")
, routeCounter3(0)
, patrolPath(CPathing())
, direction(0, -1, 0)
, angle(0)
, turnTo(0, 0, 0)
, rotating(false)
, rotCounter(0)
, hasSetDest(false)
, gotoServe(false)
, gotoIdle(false)
, gotoRoam(false)
, timer(0)
, timer2(0)
, sonarCooldown(1)
, sonarTimer(sonarCooldown)
, gotoChase(false)
, hasSetDest2(false)
, isVisible(false)
, smart(false)
, probability(0)
, idleTime(0)
, idleTimemin(0)
, idleTimemax(0)
, color(0,0,0)
, fadeTimer(0)
, fadeDuration(0.2)
, gotoSusp(false)
, suspDuration(0)
, suspPos(0,0,0)
{
	name = "Enemy";

	n = MainScene::GetInstance()->ML_map.map_width;
	m = MainScene::GetInstance()->ML_map.map_height;

	for (int i = 1; i < m; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			if (!isdigit(MainScene::GetInstance()->ML_map.map_data[i][j][0]))
				Map[j][i] = 0;
			else
				Map[j][i] = stoi(MainScene::GetInstance()->ML_map.map_data[i][j]);
		}
	}
}

cEnemy::~cEnemy()
{
	AI_counter = 0;
}

void cEnemy::Init(Vector3 position)
{

	LuaScript enemyScript("enemy");
	chase_timer_max = enemyScript.get<float>("enemy.chase_timer_max");//3
	default_sonarCooldown = enemyScript.get<float>("enemy.default_sonarCooldown");//1
	sonarTimer = default_sonarCooldown;
	chase_sonarCooldown = enemyScript.get<float>("enemy.chase_sonarCooldown");//0.5
	sonar_radius = enemyScript.get<int>("enemy.sonar_radius");//70
	sonar_speed = enemyScript.get<int>("enemy.sonar_speed");//1
	sonar_sides = enemyScript.get<int>("enemy.sonar_sides");//40
	idleTimemin = enemyScript.get<int>("enemy.idleTimemin");//1
	idleTimemax = enemyScript.get<int>("enemy.idleTimemax");//5
	fadeDuration = enemyScript.get<float>("enemy.fadeDuration");//0.2
	suspDuration = enemyScript.get<float>("enemy.suspDuration");//0
	map_min_x = enemyScript.get<int>("enemy.map_min_x");//1
	map_max_x = enemyScript.get<int>("enemy.map_max_x");//28
	map_min_y = enemyScript.get<int>("enemy.map_min_y");//2
	map_max_y = enemyScript.get<int>("enemy.map_max_y");//19
	vislble_range = enemyScript.get<int>("enemy.vislble_range");//3
	//suspPos
	//probability(0)
	AI_STATE = AS_ROAM;
	this->position = position;
	rotation = 0.f;

	if (AI_counter < 2)
	{
		this->smart = true;
		setWaypoints();
		AI_counter++;
	}
	else
	{
		Vector3 temp;

		do {
			temp.x = Math::RandIntMinMax(map_min_x, map_max_x);
			temp.y = Math::RandIntMinMax(map_min_y, map_max_y);
		} while (MainScene::GetInstance()->ML_map.map_data[temp.y][temp.x] == "1");

		patrolPath.WayPointTileList.push_back(temp);
		this->smart = false;
		AI_counter++;
	}

	if (route.empty())
	{
		route = pathFind(currTile.x, currTile.y,
			patrolPath.WayPointTileList[patrolPath.location].x,
			patrolPath.WayPointTileList[patrolPath.location].y);
	}

}
float cEnemy::GetDistance(float x1, float y1, float x2, float y2)
{
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}
void cEnemy::Update(double dt)
{
	if ((isVisible && (currTile - MainScene::GetInstance()->player_ptr->currTile).Length() <= vislble_range) || MainScene::GetInstance()->toggleVisible/* || AI_STATE == AS_CHASE*/)
	{
		if (fadeTimer < fadeDuration)
			fadeTimer += dt;

		color.r = (fadeTimer / fadeDuration);
	}
	else if (fadeTimer >= 0 && color.r >= 0)
	{
		fadeTimer -= dt;
		color.r = (fadeTimer / fadeDuration);
	}

	else
	{
		fadeTimer = 0;
		color.r = 0;
	}

	if (AI_STATE == AS_CHASE)
	{
		sonarCooldown = chase_sonarCooldown;
	}

	else
		sonarCooldown = default_sonarCooldown;


	if (AI_STATE != AS_IDLE)
	{
		if (sonarTimer < sonarCooldown)
			sonarTimer += dt;

		if (sonarTimer >= sonarCooldown)
		{
			sonarTimer = 0;

			Sonar *SNR;
			SNR = new Sonar();
			SNR->Init(sonar_radius, 0, sonar_sides, sonar_speed);
			SNR->GenerateSonar(position, 3);
			sonarList.push_back(SNR);
		}
	}

	for (int i = 0; i < sonarList.size(); ++i)
	{
		sonarList[i]->Update(dt);

		if (AI_STATE == AS_CHASE)
			sonarList[i]->alertType = 2;
		else if (AI_STATE == AS_SUSP)
			sonarList[i]->alertType = 3;
		else 
			sonarList[i]->alertType = 1;

			if (sonarList[i]->radius >= sonarList[i]->maxRad)
			{
				delete sonarList[i];
				sonarList.erase(sonarList.begin() + i);
			}
	}

	switch (AI_STATE)
	{

	case cEnemy::AS_ROAM:
		gotoRoam = false;

		if (currTile.x == patrolPath.WayPointTileList[patrolPath.location].x &&
			currTile.y == patrolPath.WayPointTileList[patrolPath.location].y &&
			routeCounter == 0)
		{
			if (!(rand() % 5))
			{
				gotoIdle = true;
				idleTime = Math::RandFloatMinMax(idleTimemin, idleTimemax);
			}
				
			else
			{
				patrolPath.location++;

				if (!smart)
				{
					Vector3 temp;

					do {
						temp.x = Math::RandIntMinMax(map_min_x, map_max_x);
						temp.y = Math::RandIntMinMax(map_min_y, map_max_y);
					} while (MainScene::GetInstance()->ML_map.map_data[temp.y][temp.x] == "1");

					patrolPath.WayPointTileList.push_back(temp);
				}

				if (patrolPath.location >= patrolPath.WayPointTileList.size() && smart)
					patrolPath.location = 0;

				route = pathFind(currTile.x, currTile.y,
					patrolPath.WayPointTileList[patrolPath.location].x,
					patrolPath.WayPointTileList[patrolPath.location].y);
			}

		}

		executePath(dt, route, routeCounter);

		if (gotoIdle && routeCounter == 0)
		{
			AI_STATE = AS_IDLE;
			route = "";
		}
		if (gotoChase && routeCounter == 0)
		{
			AI_STATE = AS_CHASE;
			route = "";
		}
		if (gotoSusp && routeCounter == 0)
		{
			AI_STATE = AS_SUSP;
			route = "";
		}

		break;

	case cEnemy::AS_CHASE:
		timer += dt;
		gotoChase = false;
		isVisible = true;

		if (timer <= chase_timer_max && routeCounter2 == 0)
		{
			route2 = pathFind(currTile.x, currTile.y, MainScene::GetInstance()->player_ptr->currTile.x, MainScene::GetInstance()->player_ptr->currTile.y);

		}
		else if (timer > chase_timer_max)
		{
			gotoRoam = true;
		}

		executePath(dt, route2, routeCounter2);

		if (gotoRoam && routeCounter2 == 0)
		{
			timer = 0;

			if (smart)
			{
				setWaypoints();
				patrolPath.location = 0;
			}
			
			route2 = "";

			route = pathFind(currTile.x, currTile.y,
				patrolPath.WayPointTileList[patrolPath.location].x,
				patrolPath.WayPointTileList[patrolPath.location].y);

			AI_STATE = AS_ROAM;
		}
		break;
	case cEnemy::AS_IDLE:
		timer2 += dt;
		gotoIdle = false;
		
		if (timer2 > idleTime)
		{
			gotoRoam = true;
			timer2 = 0;
		}

		if (gotoChase || (currTile - MainScene::GetInstance()->player_ptr->currTile).Length() <= 1)
		{
			AI_STATE = AS_CHASE;
		}

		if (gotoRoam)
		{
			AI_STATE = AS_ROAM;
		}

		if (gotoSusp)
		{
			AI_STATE = AS_SUSP;
		}

		break;

	case cEnemy::AS_SUSP:

		gotoSusp = false;

		timer3 += dt;

		if (timer3 <= suspDuration && routeCounter3 == 0)
		{
			route3 = pathFind(currTile.x, currTile.y, suspPos.x, suspPos.y);
		}

		else if (timer3 > suspDuration)
		{
			gotoRoam = true;
			timer3 = 0;
		}

		executePath(dt, route3, routeCounter3);

		if (gotoRoam && routeCounter3 == 0)
		{
			suspPos.SetZero();

			//Comment out if you want enemies to return to patrol path (suspicion)
			if (smart)
			{
				setWaypoints();
				patrolPath.location = 0;
			}

			route3 = "";

			route = pathFind(currTile.x, currTile.y,
				patrolPath.WayPointTileList[patrolPath.location].x,
				patrolPath.WayPointTileList[patrolPath.location].y);

			AI_STATE = AS_ROAM;
		}

		if (gotoChase && routeCounter3 == 0)
		{
			suspPos.SetZero();

			AI_STATE = AS_CHASE;
			route3 = "";
		}

		break;

	default:
		break;
	}
}

string cEnemy::pathFind(const int& xStart, const int& yStart, const int& xFinish, const int& yFinish)
{
	// function returns a route in the form of a string (series of directions)
	// 0 = Right
	// 1 = Down
	// 2 = Left
	// 3 = Up

	static priority_queue<CNode> myQueue[2];
	static int queueIndex;
	static CNode* tempNode1;
	static CNode* tempNode2;
	static int i, j, x, y, xdx, ydy;
	static char c;

	queueIndex = 0;

	//reset node maps
	for (y = 0; y < m; y++)
	{
		for (x = 0; x < n; x++)
		{
			closed_nodes_map[x][y] = 0;
			open_nodes_map[x][y] = 0;
		}
	}

	//create start node and put into open list
	tempNode1 = new CNode(xStart, yStart, 0, 0);
	tempNode1->updatePriority(xFinish, yFinish);

	myQueue[queueIndex].push(*tempNode1);
	open_nodes_map[x][y] = tempNode1->getPriority();

	//A* search
	while (!myQueue[queueIndex].empty())
	{
		//get current node w/ highest priority
		tempNode1 = new CNode(myQueue[queueIndex].top().getPosX(), myQueue[queueIndex].top().getPosY(),
			myQueue[queueIndex].top().getLevel(), myQueue[queueIndex].top().getPriority());

		x = tempNode1->getPosX();
		y = tempNode1->getPosY();

		myQueue[queueIndex].pop(); //remove node from open list
		open_nodes_map[x][y] = 0;
		closed_nodes_map[x][y] = 1;

		//stop searching when goal is reached
		if (x == xFinish && y == yFinish)
		{
			//generate the path
			string path = "";

			while (!(x == xStart && y == yStart))
			{
				j = dir_map[x][y];
				c = '0' + (j + dir / 2) % dir;
				path = c + path;
				x += dx[j];
				y += dy[j];
			}

			delete tempNode1;

			while (!myQueue[queueIndex].empty())
				myQueue[queueIndex].pop();

			return path;
		}

		//generate moves in all directions
		for (i = 0; i < dir; i++)
		{
			xdx = x + dx[i];
			ydy = y + dy[i];

			//state the "unwalkable" tiles in the if statement here
			if (!(xdx < 0 || xdx > n - 1 || ydy < 0 || ydy > m - 1 || closed_nodes_map[xdx][ydy] == 1 || Map[xdx][ydy] == 1 ))
			{
				//generate child node
				tempNode2 = new CNode(xdx, ydy, tempNode1->getLevel(), tempNode1->getPriority());
				tempNode2->nextLevel(i);
				tempNode2->updatePriority(xFinish, yFinish);

				//add if not already in open list
				if (open_nodes_map[xdx][ydy] == 0)
				{
					//update priority
					open_nodes_map[xdx][ydy] = tempNode2->getPriority();
					myQueue[queueIndex].push(*tempNode2);
					//update direction
					dir_map[xdx][ydy] = (i + dir / 2) % dir;
				}

				else if (open_nodes_map[xdx][ydy] > tempNode2->getPriority())
				{
					// update the priority info
					open_nodes_map[xdx][ydy] = tempNode2->getPriority();
					// update the parent direction info
					dir_map[xdx][ydy] = (i + dir / 2) % dir;
					//replace the node
					while (!(myQueue[queueIndex].top().getPosX() == xdx &&
						myQueue[queueIndex].top().getPosY() == ydy))
					{
						myQueue[1 - queueIndex].push(myQueue[queueIndex].top());
						myQueue[queueIndex].pop();
					}

					myQueue[queueIndex].pop();

					if (myQueue[queueIndex].size() > myQueue[1 - queueIndex].size())
						queueIndex = 1 - queueIndex;

					while (!myQueue[queueIndex].empty())
					{
						myQueue[1 - queueIndex].push(myQueue[queueIndex].top());
						myQueue[queueIndex].pop();
					}

					queueIndex = 1 - queueIndex;
					myQueue[queueIndex].push(*tempNode2);
				}

				else
					delete tempNode2;
			}
		}

		delete tempNode1;
	}

	return ""; // no path found
}

bool cEnemy::executePath(double dt, string& route, float& routeCounter)
{
	for (int i = 0; i < route.length(); ++i)
	{
		char temp = route[i];

		if (temp != '4')
		{
			switch (temp)
			{
			case '0':
				this->turnTo = Vector3(1, 0, 0);
				break;

			case '1':
				this->turnTo = Vector3(0, -1, 0);
				break;

			case '2':
				this->turnTo = Vector3(-1, 0, 0);
				break;

			case '3':
				this->turnTo = Vector3(0, 1, 0);
				break;
			}

			Vector3 directionReal;
			directionReal.x = direction.x;

			if (direction.y != 0)
				directionReal.y = -direction.y;

			float dot = this->turnTo.x * directionReal.x + this->turnTo.y * directionReal.y;
			float det = this->turnTo.x * directionReal.y - this->turnTo.y * directionReal.x;
			angle = atan2(det, dot) * 57.2957795;

			if (rotCounter < abs(angle) && routeCounter == 0)
				this->rotating = true;
			else
			{
				if (angle > 0)
					this->rotation += rotCounter;
				else
					this->rotation -= rotCounter;

				this->rotation -= angle;

				this->rotCounter = 0;

				this->rotating = false;
			}

			float rotspeed = 500;

			//if (AI_STATE == AS_CHASE)
			//{
			//	rotspeed = 800;
			//}

			if (this->rotating == true)
			{
				if (angle > 0)
				{
					this->rotation -= rotspeed * dt;
				}
				else
				{
					this->rotation += rotspeed * dt;
				}

				this->rotCounter += rotspeed * dt;

				if (this->rotation > 360)
					this->rotation -= 360;

				if (this->rotation < -360)
					this->rotation += 360;
			}
			else
			{
				this->direction.x = this->turnTo.x;
				this->direction.y = -this->turnTo.y;


				switch (temp)
				{
				case '0':
					rotation = -90;
					break;

				case '1':
					rotation = 180;
					break;

				case '2':
					rotation = 90;
					break;

				case '3':
					rotation = 0;
					break;
				}


				float speed = 100;

				if (AI_STATE == AS_CHASE)
				{
					speed = 250;
				}

				else if (AI_STATE == AS_ROAM)
				{
					speed = 100;
				}

				this->position.x += this->direction.x * dt * speed;
				this->position.y += -this->direction.y * dt * speed;

				this->topLeft.x += this->direction.x * dt * speed;
				this->topLeft.y += -this->direction.y * dt * speed;

				this->bottomRight.x += this->direction.x * dt * speed;
				this->bottomRight.y += -this->direction.y * dt * speed;

				routeCounter += (this->direction * dt * speed).Length();

				if (routeCounter >= MainScene::GetInstance()->ML_map.worldSize * 2)
				{
					this->position.x -= this->direction.x * routeCounter;
					this->position.y -= -this->direction.y * routeCounter;

					this->topLeft.x -= this->direction.x * routeCounter;
					this->topLeft.y -= -this->direction.y * routeCounter;

					this->bottomRight.x -= this->direction.x * routeCounter;
					this->bottomRight.y -= -this->direction.y * routeCounter;

					this->position.x += this->direction.x * (MainScene::GetInstance()->ML_map.worldSize * 2);
					this->position.y += -this->direction.y * (MainScene::GetInstance()->ML_map.worldSize * 2);

					this->topLeft.x += this->direction.x * (MainScene::GetInstance()->ML_map.worldSize * 2);
					this->topLeft.y += -this->direction.y * (MainScene::GetInstance()->ML_map.worldSize * 2);

					this->bottomRight.x += this->direction.x * (MainScene::GetInstance()->ML_map.worldSize * 2);
					this->bottomRight.y += -this->direction.y * (MainScene::GetInstance()->ML_map.worldSize * 2);

					route[i] = '4';
					routeCounter = 0;
				}

				if (routeCounter == 0)
				{
					currTile.x += this->direction.x;
					currTile.y += this->direction.y;
				}
			}
			break;
		}
	}

	return route.find_first_not_of(route[0]) == std::string::npos;

}

void cEnemy::setWaypoints(void)
{
	/*
	switch (ID)
	{
	case 50:
		patrolPath.setWayPoints(4, 100, 101, 102, 103);
		break;
	case 51:
		patrolPath.setWayPoints(4, 104, 105, 106, 107);
		break;
	default:
		break;
	}*/
	patrolPath.WayPointTileList.clear();
	
	Vector3 tile_topleft = currTile, tile_topright = currTile, tile_bottomleft = currTile, tile_bottomright = currTile, temp = currTile;
	for (int i = 0; i < 3; i++)
	{
		temp.x += 1;
		if (MainScene::GetInstance()->ML_map.map_data[temp.y][temp.x] == "0")
			tile_topright.x = temp.x;
		else
			temp.x--;

		temp.y -= 1;
		if (MainScene::GetInstance()->ML_map.map_data[temp.y][temp.x] == "0")
			tile_topright.y -= 1;
		else
			temp.y++;
	}
	patrolPath.WayPointTileList.push_back(tile_topright);
	temp = currTile;
	for (int i = 0; i < 3; i++)
	{
		temp.x -= 1;
		if (MainScene::GetInstance()->ML_map.map_data[temp.y][temp.x] == "0")
			tile_topleft.x -= 1;
		else
			temp.x++;
		temp.y -= 1;
		if (MainScene::GetInstance()->ML_map.map_data[temp.y][temp.x] == "0")
			tile_topleft.y -= 1;
		else
			temp.y++;
	}
	patrolPath.WayPointTileList.push_back(tile_topleft);
	temp = currTile;
	for (int i = 0; i < 3; i++)
	{
		temp.x -= 1;
		if (MainScene::GetInstance()->ML_map.map_data[temp.y][temp.x] == "0")
			tile_bottomleft.x -= 1;
		else
			temp.x++;
		temp.y += 1;
		if (MainScene::GetInstance()->ML_map.map_data[temp.y][temp.x] == "0")
			tile_bottomleft.y += 1;
		else
			temp.y--;
	}
	patrolPath.WayPointTileList.push_back(tile_bottomleft);
	temp = currTile;
	for (int i = 0; i < 3; i++)
	{
		temp.x += 1;
		if (MainScene::GetInstance()->ML_map.map_data[temp.y][temp.x] == "0")
			tile_bottomright.x += 1;
		else
			temp.x--;
		temp.y += 1;
		if (MainScene::GetInstance()->ML_map.map_data[temp.y][temp.x] == "0")
			tile_bottomright.y += 1;
		else
			temp.y--;
	}
	patrolPath.WayPointTileList.push_back(tile_bottomright);

	if (patrolPath.WayPointTileList.size() == 1)
		patrolPath.WayPointTileList.push_back(currTile);

}

std::string cEnemy::getState(void)
{
	switch (AI_STATE)
	{
	case cEnemy::AS_ROAM:
		return "Roam";
		break;
	default:
		return "Undefined";
		break;
	}
}