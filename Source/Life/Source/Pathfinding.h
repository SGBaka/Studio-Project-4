#ifndef PATHFINDING_H
#define PATHFINDING_H

#include <iostream>
#include <iomanip>
#include <queue>
#include <string>
#include <math.h>
#include <ctime>
using namespace std;

//int n = 0; // horizontal size of the map
//int m = 0; // vertical size size of the map
static int Map[1000][1000];
static int closed_nodes_map[1000][1000]; // map of closed (tried-out) nodes
static int open_nodes_map[1000][1000]; // map of open (not-yet-tried) nodes
static int dir_map[1000][1000]; // map of directions
const int dir = 4; // number of possible directions to go at any position
// if dir==4
static int dx[dir] = { 1, 0, -1, 0 };
static int dy[dir] = { 0, 1, 0, -1 };
// if dir==8
//static int dx[dir]={1, 1, 0, -1, -1, -1, 0, 1};
//static int dy[dir]={0, 1, 1, 1, 0, -1, -1, -1};

#endif