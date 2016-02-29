

#ifndef OTHERDLC_H
#define OTHERDLC_H

#include "Vector3.h"
#include "MyMath.h"
#include <iostream>
#include <vector>
#include <string>
#include <Windows.h>
#include <sstream>

std::string Virtual_Keyboard(double dt, std::string input);
std::string ExePath();
std::vector<std::string> get_all_files_name_within_folder(std::string folder);
std::vector<std::string> get_all_folder_name_within_folder(std::string folder);

#endif