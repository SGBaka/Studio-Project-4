
#include "OtherDLC.h"

std::string Virtual_Keyboard(double dt, std::string input)
{
	std::string output = input;
	char temp_string;
	static bool any_press = false;
	static float keyboard_timer = 0.0f;

	keyboard_timer -= (float)dt;
	if (any_press == false)
	{
		for (int i = VK_LBUTTON; i <= VK_OEM_CLEAR; i++)
		{
			//std::cout << i << std::endl;
			if ((GetAsyncKeyState(i) & 0x8001) != 0)
			{
				keyboard_timer = 0.1f;
				any_press = true;
				if (i == VK_BACK)
				{
					output = input.substr(0, input.size() - 1);
				}
				else
				{
					// Letter && Number
					if (input.size() < 15)
					{
						if (i >= 48 && i <= 57) // Numbers
						{
							temp_string = i;
							std::stringstream ss;
							std::string s;

							ss << temp_string;
							s = ss.str();
							output = input + s;
						}
						else if (i >= 65 && i <= 90) // Letters
						{
							temp_string = i + 32;
							std::stringstream ss;
							std::string s;

							ss << temp_string;
							s = ss.str();
							output = input + s;
						}
						else if (i == 189)
						{
							output = input + "_";
						}
					}
				}
			}
		}
	}
	else if (any_press == true)
	{
		bool any_key = false;
		for (int i = VK_LBUTTON; i <= VK_OEM_CLEAR; i++)
		{
			//std::cout << i << std::endl;
			if ((GetAsyncKeyState(i) & 0x8001) != 0)
			{
				any_key = true;
			}
		}

		if (any_key == false || keyboard_timer < 0)
		{
			any_press = false;
		}
	}

	return output;
}

std::string ExePath() {
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	std::string newBuffer = std::string(buffer).substr(0, pos);
	pos = newBuffer.find_last_of("\\/");

	return newBuffer.substr(0, pos);
}

std::vector<std::string> get_all_files_name_within_folder(std::string folder)
{
	std::vector<std::string> files_name;

	char search_path[200];
	std::sprintf(search_path, "%s/*.*", folder.c_str());
	//WIN32_FIND_DATA fd;
	LPWIN32_FIND_DATAA fd;
	HANDLE hFind = ::FindFirstFileA(search_path, fd);

	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (!(fd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				files_name.push_back(fd->cFileName);
			}
		} while (::FindNextFileA(hFind, fd));
		::FindClose(hFind);
	}
	return files_name;
}

std::vector<std::string> get_all_folder_name_within_folder(std::string folder)
{
	std::vector<std::string> folders_name;

	char search_path[200];
	std::sprintf(search_path, "%s/*.*", folder.c_str());
	//WIN32_FIND_DATA fd;
	LPWIN32_FIND_DATAA fd;
	HANDLE hFind = ::FindFirstFileA(search_path, fd);

	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (!(fd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			}
			else
				folders_name.push_back(fd->cFileName);
		} while (::FindNextFileA(hFind, fd));
		::FindClose(hFind);
	}
	return folders_name;
}