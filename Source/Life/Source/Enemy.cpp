#include "Enemy.h"
#include "Player.h"
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
, gotoNavi(false)
, gotoRoam(false)

, gotoChase(false)
, hasSetDest2(false)
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

}

void cEnemy::Init(Vector3 position)
{
	AI_STATE = AS_ROAM;
	this->position = position;
	setWaypoints();
	rotation = 0.f;
	srand((unsigned int)time(NULL));
}
float cEnemy::GetDistance(float x1, float y1, float x2, float y2)
{
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}
void cEnemy::Update(double dt)
{

	if (route.empty())
	{
		patrolPath.location = 0;

		route = pathFind(currTile.x, currTile.y,
			patrolPath.WayPointTileList[patrolPath.location].x,
			patrolPath.WayPointTileList[patrolPath.location].y);
	}
	if ((gotoNavi || gotoRoam) && routeCounter == 0 && routeCounter2 == 0 && routeCounter3 == 0)
	{
		if (gotoNavi)
		{
			route3 = "";
			routeCounter3 = 0;
			hasSetDest2 = false;
		}
		
		else if (gotoRoam)
		{
			route = "";
			routeCounter = 0;
			patrolPath.location = 0;
			AI_STATE = AS_ROAM;
		}
	}
	//cout << AI_STATE << endl;
	switch (AI_STATE)
	{

	case cEnemy::AS_ROAM:
	
		gotoRoam = false;

		if (currTile.x == patrolPath.WayPointTileList[patrolPath.location].x &&
			currTile.y == patrolPath.WayPointTileList[patrolPath.location].y &&
			routeCounter == 0 && rotating == false)
		{
			patrolPath.location++;

			if (patrolPath.location >= patrolPath.WayPointTileList.size())
				patrolPath.location = 0;

			route = pathFind(currTile.x, currTile.y,
				patrolPath.WayPointTileList[patrolPath.location].x,
				patrolPath.WayPointTileList[patrolPath.location].y);

		}
	
		executePath(dt, route, routeCounter);

		if (gotoChase && routeCounter == 0)
		{
			AI_STATE = AS_CHASE;
			route2 = "";
			routeCounter2 = 0;
			hasSetDest = false;
		}
		break;
	case cEnemy::AS_CHASE:
		timer += dt;
		gotoChase = false;
		if (!MainScene::GetInstance()->player_ptr->sonarList.empty())
		{
			//	cout << "hit" << endl;
			for (int i = 0; i < MainScene::GetInstance()->player_ptr->sonarList.size(); i++)
			{
				if (MainScene::GetInstance()->player_ptr->sonarList[i] != NULL)
				{
					if (timer < 3 && routeCounter2 == 0)
					{
						route2 = pathFind(currTile.x, currTile.y, MainScene::GetInstance()->player_ptr->currTile.x, MainScene::GetInstance()->player_ptr->currTile.y);

					}
					else
					{
						timer = 0;
					}
				}
			}
		}
		executePath(dt, route2, routeCounter2);
		/*
		if (gotoReturn && routeCounter == 0)
		{
			AI_STATE = AS_RETURN;
		}*/
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

				//if (AI_STATE == AS_CHASE)
				//{
				//	speed = 220;
				//}

				//else if (AI_STATE == AS_SUSP)
				//{
				//	speed = 120;
				//}

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
	}	
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