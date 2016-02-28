
#include "maploader.h"

/****************************************************************************/
/*!
\brief
Default constructor
!*/
/****************************************************************************/
MapLoader::MapLoader() : map_height(90), map_width(160)
{
	FILE_LOCATION = "GameData//Maps//001.csv";
}

/****************************************************************************/
/*!
\brief
Default destructor
!*/
/****************************************************************************/
MapLoader::~MapLoader()
{
	for (unsigned i = 0; i < map_data.size(); ++i)
	{
		for (unsigned j = 0; j < map_data[i].size(); ++j)
		{
			std::vector<std::string>().swap(map_data[i]);
		}
		std::vector<std::vector<std::string>>().swap(map_data);
	}
}

/****************************************************************************/
/*!
\brief
check if file exists
\param file
the file location to check for
!*/
/****************************************************************************/
bool MapLoader::fileExists(std::string file)
{
	std::ifstream infile(file);
	return infile.good();
}

/****************************************************************************/
/*!
\brief
loads the map from a file into a 2D vector
\param file
the file location to load map from
!*/
/****************************************************************************/

bool MapLoader::loadMap(std::string file)
{
	if (!fileExists(file))
	{
		return false;
	}

	FILE_LOCATION = file;

	for (unsigned i = 0; i < map_data.size(); ++i)
	{
		for (unsigned j = 0; j < map_data[i].size(); ++j)
		{
			std::vector<std::string>().swap(map_data[i]);
		}
		std::vector<std::vector<std::string>>().swap(map_data);
	}

	std::fstream mapData;
	mapData.open(FILE_LOCATION);

	std::string Line;
	while (std::getline(mapData, Line, '\n'))
	{
		std::istringstream csvStream(Line);
		std::vector<std::string> Column;
		std::string Element;

		while (std::getline(csvStream, Element, ','))
		{
			Column.push_back(Element);
		}
		map_width = Column.size();
		map_data.push_back(Column);

		std::vector<std::string>().swap(Column);
	}	

	worldSize = static_cast<float>(std::stoi(map_data[0][0]));
	worldHeight = static_cast<float>(std::stoi(map_data[0][1]));

	star_one = static_cast<float>(std::stoi(map_data[0][2]));
	star_two = static_cast<float>(std::stoi(map_data[0][3]));
	star_three = static_cast<float>(std::stoi(map_data[0][4]));

	map_height = map_data.size();
	return true;
}

/****************************************************************************/
/*!
\brief
outputs the 2D vector into a file
\param file
the file location to save the map to
!*/
/****************************************************************************/
bool MapLoader::saveMap(std::string file)
{
	bool create = false;
	if (!fileExists(file))
	{
		std::fstream filecreate;
		filecreate.open(file, std::fstream::out);
		filecreate.close(); 
	}
	else
	{
		create = true;
		std::fstream filecreate;
		filecreate.open("GameData//Maps//Problem.csv", std::fstream::out);
		filecreate.close();
	}

	std::fstream mapFile;
	if (create == false)
	{
		mapFile.open(file);
	}
	else
	{	
		mapFile.open("GameData//Maps//Problem.csv");
	}

	for (unsigned i = 0; i < map_data.size(); ++i)
	{
		for (unsigned j = 0; j < map_data[i].size(); ++j)
		{
			if (j != map_data[i].size())
				mapFile << map_data[i][j];

			if (j != map_data[i].size() - 1)
			{
				mapFile << ",";
			}
		}

		if (i < (map_data.size() - 1))
			mapFile << "\n";
	}

	mapFile.close();

	remove(file.c_str());
	rename("GameData//Maps//Problem.csv", file.c_str());
	remove("GameData//Maps//Problem.csv");
	return true;
}

bool MapLoader::saveMap_Creator(std::string file)
{
	if (!fileExists(file))
	{
		std::fstream filecreate;
		filecreate.open(file, std::fstream::out);
		filecreate.close();
	}

	std::fstream mapFile;
	mapFile.open(file);

	for (unsigned i = 0; i < map_data.size(); ++i)
	{
		for (unsigned j = 0; j < map_data[i].size(); ++j)
		{
			mapFile << map_data[i][j];

			if (j != map_data[i].size() - 1)
			{
				mapFile << ",";
			}
		}

		if (i < map_data.size() - 1)
			mapFile << "\n";
	}

	mapFile.close();
	return true;
}

std::string MapLoader::newFile(int difficulty, std::string fileLoc)
{
	std::string mapName = fileLoc;
	int i = 1;

	if (difficulty == 1)
	{
		mapName += "e_";
	}
	else if (difficulty == 2)
	{
		mapName += "m_";
	}
	else if (difficulty == 3)
	{
		mapName += "h_";
	}

	while (fileExists(mapName + std::to_string(static_cast<unsigned long long>(i)) +".csv"))
	{	
		i++;
	}
	saveMap_Creator(mapName + std::to_string(static_cast<unsigned long long>(i)) + ".csv");
	if (difficulty == 1)
	{
		return "e_" + std::to_string(static_cast<unsigned long long>(i));
	}
	else if (difficulty == 2)
	{
		return "m_" + std::to_string(static_cast<unsigned long long>(i));
	}
	else if (difficulty == 3)
	{
		return "h_" + std::to_string(static_cast<unsigned long long>(i));
	}
	return mapName + std::to_string(static_cast<unsigned long long>(i));
}