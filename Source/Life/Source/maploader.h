/****************************************************************************/
/*!
\file Maploader.h
\author Gabriel Wong Choon Jieh
\par email: AuraTigital\@gmail.com
\brief
Handles loading a map into a 2D vector
!*/
/****************************************************************************/
#ifndef MAPLOADER_H
#define MAPLOADER_H

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>

class MapLoader
{
private:

	std::string FILE_LOCATION;

public:
	MapLoader();
	~MapLoader();

	unsigned int map_height;
	unsigned int map_width;
	float worldSize;
	float worldHeight;
	float star_one; // Passing Grade
	float star_two;
	float star_three;

	bool fileExists(std::string file);
	bool loadMap(std::string file);
	bool saveMap(std::string file);
	bool saveMap_Creator(std::string file);
	std::string newFile(int difficulty, std::string fileLoc);
	std::vector<std::vector<std::string>> map_data;
};
#endif