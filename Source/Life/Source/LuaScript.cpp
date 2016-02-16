#include "LuaScript.h"

LuaScript::LuaScript(const std::string& filename) {
	L = luaL_newstate();
	std::string LUALOC = "Lua//" + filename + ".lua";

	if (luaL_loadfile(L, LUALOC.c_str()) || lua_pcall(L, 0, 0, 0))
	{
		std::cout << "Error: failed to load (" << filename << ")" << std::endl;
		L = 0;
	}

	if (L)
		luaL_openlibs(L);
}

LuaScript::~LuaScript() {
	if (L)
		lua_close(L);
}

void LuaScript::printError(const std::string& variableName, const std::string& reason)
{
	std::cout << "Error: can't get [" << variableName << "]. " << reason << std::endl;
}

std::vector<int> LuaScript::getIntVector(const std::string& name) {
	std::vector<int> v;
	lua_gettostack(name.c_str());
	if (lua_isnil(L, -1)) { // array is not found
		return std::vector<int>();
	}
	lua_pushnil(L);
	while (lua_next(L, -2)) {
		v.push_back((int)lua_tonumber(L, -1));
		lua_pop(L, 1);
	}
	clean();
	return v;
}

std::vector<std::string> LuaScript::getTableKeys(const std::string& name) {
	std::string code =
		"function getKeys(name) "
		"s = \"\""
		"for k, v in pairs(_G[name]) do "
		"    s = s..k..\",\" "
		"    end "
		"return s "
		"end"; // function for getting table keys
	luaL_loadstring(L, code.c_str()); // execute code
	lua_pcall(L, 0, 0, 0);
	lua_getglobal(L, "getKeys"); // get function
	lua_pushstring(L, name.c_str());
	lua_pcall(L, 1, 1, 0); // execute function
	std::string test = lua_tostring(L, -1);
	std::vector<std::string> strings;
	std::string temp = "";
	std::cout << "TEMP:" << test << std::endl;
	for (unsigned int i = 0; i < test.size(); i++) {
		if (test.at(i) != ',') {
			temp += test.at(i);
		}
		else {
			strings.push_back(temp);
			temp = "";
		}
	}
	clean();
	return strings;
}

std::string LuaScript::getGameData(const std::string& variableName)
{
	std::string file = "";
	std::string variable = variableName;
	int i = 0;
	while (variableName[i] != '.')
	{
		file += variableName[i];
		variable.erase(variable.begin());
		i++;
	}

	i++;
	variable.erase(variable.begin());

	if (lua_gettostack(variable))
	{ // variable succesfully on top of stack
		std::string s = "null";
		std::string filename = "null";

		if (lua_isstring(L, -1))
		{
			filename = std::string(lua_tostring(L, -1));

			s = "GameData//";

			if (file == "sound")
			{
				s += "Sounds//";
				file = "";

				while (variableName[i] != '.')
				{
					file += variableName[i];
					i++;
				}

				if (file == "ambience")
				{
					s += "Ambience//";
				}
				else if (file == "effect")
				{
					s += "Effects//";
				}
				else if (file == "other")
				{
					s += "Other//";
				}
				else if (file == "ui")
				{
					s += "UI//";
				}
				else if (file == "weapon")
				{
					s += "Weapons//";
				}
			}
			else if (file == "image")
			{
				s += "Image//";
				file = "";

				while (variableName[i] != '.')
				{
					file += variableName[i];
					i++;
				}

				if (file == "font")
				{
					s += "Font//";
				}
				else if (file == "tile")
				{
					s += "Tile//";
				}
				else
				{
					s += "UI//";

					if (file == "button")
					{
						s += "Button//";
					}
				}
			}
			else if (file == "map")
			{
				s += "Maps//";
			}
			else if (file == "shader")
			{
				s += "Shader//";
			}

			s += filename;
		}
		else
		{
			printError(variableName, "Not a string");
		}
		return s;
	}
	else
	{
		return "null";
	}
}