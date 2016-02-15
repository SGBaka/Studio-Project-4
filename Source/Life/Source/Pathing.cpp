#include "Pathing.h"
#include "MainScene.h"


CPathing::CPathing(void) : location(0)
{
	for (std::vector<Vector3>::iterator it = WayPointList.begin(); it != WayPointList.end(); ++it)
	{
		Vector3 go = (Vector3)*it;
		go.SetZero();
	}

	for (std::vector<Vector3>::iterator it = WayPointTileList.begin(); it != WayPointTileList.end(); ++it)
	{
		Vector3 go = (Vector3)*it;
		go.SetZero();
	}
}


CPathing::~CPathing(void)
{
}

void CPathing::setWayPoints(int amount, ...)
{
	Vector3 temp;

	va_list wayPoints;
	va_start(wayPoints, amount);

	for (short itr = 0; itr < amount; ++itr)
	{
		//Set Point to current Parameter;
		int Point = va_arg(wayPoints, int);

		//Loop through column
		for (unsigned i = MainScene::GetInstance()->ML_map.map_height - 1; i > 0; --i)
		{
			for (unsigned j = 0; j < MainScene::GetInstance()->ML_map.map_width; ++j)
			{
				//Process Tiles
				if (isdigit(MainScene::GetInstance()->ML_map.map_data[i][j][0]))
				{
					if (stoi(MainScene::GetInstance()->ML_map.map_data[i][j]) == Point)
					{
						//Get position of waypoint
						temp.Set(j * MainScene::GetInstance()->ML_map.worldSize*2.f,
							(MainScene::GetInstance()->ML_map.map_height - i) *
							MainScene::GetInstance()->ML_map.worldSize*2.f, 0);

						//Set Way Points
						WayPointList.push_back(temp);
						WayPointTileList.push_back(Vector3(static_cast<float>(j), static_cast<float>(i), 0.f));
					}
				}
			}
		}
	}

	va_end(wayPoints);
}