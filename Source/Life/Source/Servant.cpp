#include "Servant.h"

cServant::cServant() : route(""), routeCounter(0), route2(""), routeCounter2(0), route3(""), routeCounter3(0), patrolPath(CPathing()), direction(0, -1, 0), angle(0), turnTo(0, 0, 0),
rotating(false), rotCounter(0), hasSetDest(false), hasFood(false), gotoServe(false), gotoNavi(false), gotoRoam(false), hasSetDest2(false)
{
	name = "Servant";
	f_HungerMeter = 100.f;
	f_PeeMeter = 100.f;
	b_NeedtoEat = false;
	b_NeedtoPee = false;

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

cServant::~cServant()
{

}

void cServant::Init(Vector3 position)
{
	AI_STATE = AS_ROAM;
	this->position = position;
	setWaypoints();
	rotation = 0.f;
	srand((unsigned int)time(NULL));
}

void cServant::Update(double dt)
{

	if (route.empty())
	{
		patrolPath.location = 0;

		route = pathFind(currTile.x, currTile.y,
			patrolPath.WayPointTileList[patrolPath.location].x,
			patrolPath.WayPointTileList[patrolPath.location].y);
	}


	if ((gotoNavi || gotoRoam) && routeCounter == 0 && routeCounter2 == 0 && routeCounter3 == 0 && !hasFood)
	{
		if (gotoNavi)
		{
			route3 = "";
			routeCounter3 = 0;
			hasSetDest2 = false;
			AI_STATE = AS_NAVI;
		}
		
		else if (gotoRoam)
		{
			route = "";
			routeCounter = 0;
			patrolPath.location = 0;
			AI_STATE = AS_ROAM;
		}
	}

	switch (AI_STATE)
	{

	case cServant::AS_ROAM:

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

		if (gotoServe && routeCounter == 0)
		{
			route2 = "";
			routeCounter2 = 0;
			hasSetDest = false;
			AI_STATE = AS_SERVE;
		}

		break;

	case cServant::AS_SERVE:

		break;

	case cServant::AS_BATHROOM:

		break;

	case cServant::AS_EATING:
		break;

	case cServant::AS_BOMB:
		break;

	case cServant::AS_NAVI:
		break;

	default:
		break;
	}
}

string cServant::pathFind(const int& xStart, const int& yStart, const int& xFinish, const int& yFinish)
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

bool cServant::executePath(double dt, string& route, float& routeCounter)
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

				routeCounter += (this->direction * dt * speed).Length();

				if (routeCounter >= MainScene::GetInstance()->ML_map.worldSize * 2)
				{
					this->position.x -= this->direction.x * routeCounter;
					this->position.y -= -this->direction.y * routeCounter;

					this->position.x += this->direction.x * (MainScene::GetInstance()->ML_map.worldSize * 2);
					this->position.y += -this->direction.y * (MainScene::GetInstance()->ML_map.worldSize * 2);

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

void cServant::setWaypoints(void)
{
	patrolPath.setWayPoints(4, 100, 101, 102, 103);
}

std::string cServant::getState(void)
{
	switch (AI_STATE)
	{
	case cServant::AS_ROAM:
		return "Roam";
		break;
	case cServant::AS_SERVE:
		return "Serve";
		break;
	case cServant::AS_BATHROOM:
		return "Bathroom";
		break;
	case cServant::AS_BOMB:
		return "Bomb";
		break;
	case cServant::AS_EATING:
		return "Eat";
		break;
	case cServant::AS_NAVI:
		return "Navigation (Role Change)";
		break;
	default:
		return "Undefined";
		break;
	}
}