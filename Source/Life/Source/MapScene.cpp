

#include "MapScene.h"
#include "GL\glew.h"

#include <cstdlib>
#include <ctime>

#include "shader.hpp"
#include "Mtx44.h"
#include "MyMath.h"
#include <math.h>

#include "Application.h"
#include "SceneManager.h"
#include "MeshBuilder.h"
#include "Utility.h"
#include <sstream>
#include "Pathfinding.h"
#include "Enemy.h"
//#include <vld.h>

MapScene* MapScene::Instance = NULL;
static bool Auto_MapName = true;

const unsigned int MapScene::ui_NUM_LIGHT_PARAMS = MapScene::E_UNI_LIGHT0_EXPONENT - (MapScene::E_UNI_LIGHT0_POSITION - 1/*Minus the enum before this*/);
/******************************************************************************/
/*!
\brief
Default Constructor
*/
/******************************************************************************/
MapScene::MapScene() : ui_NUM_LIGHTS(1), hasPlacedPlayer(false), playerTile(0, 0, 0), enemyCounter(0), enemyID(50)
{

}

/******************************************************************************/
/*!
\brief
Destructor
*/
/******************************************************************************/
MapScene::~MapScene()
{

}

/******************************************************************************/
/*!
\brief
Initialize default variables, create meshes, lighting
*/
/******************************************************************************/
void MapScene::Init()
{
	SE_Engine.Init();
	f_fov = 0.f;
	f_mouseSensitivity = 0.f;
	//CUR_STATE = S_CREATE;

	selectedTile = BI_FLOOR;
	selTilePos = (0, 0, 0);

	InitMeshList();
	//Starting position of translations and initialize physics

	// Init Camera
	camera.Init(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, -100.0f), Vector3(0.0f, 1.0f, 0.0f));

	MousePosX = 0.f;
	MousePosY = 0.f;
	InitMenu();
	enemyID = 50;

	f_fov_target = f_fov;

	switch (CUR_STATE)
	{
	case MapScene::MT_CREATE:
		MENU_STATE = MT_CREATE;
		break;
	case MapScene::MT_EDIT:
		MENU_STATE = MT_EDIT;
		break;
	case MapScene::MT_NEW:
		MENU_STATE = MT_NEW;
		break;
	case MapScene::MT_REPLACE:
		MENU_STATE = MT_REPLACE;
	default:
		break;
	}

	LuaScript sound("Sound");
	SoundList[ST_BUTTON_CLICK] = SE_Engine.preloadSound(sound.getGameData("sound.button_click").c_str());
	SoundList[ST_BUTTON_CLICK_2] = SE_Engine.preloadSound(sound.getGameData("sound.button_click2").c_str());

	LuaScript fileLoc("GameData");
	OutputFolder = ExePath() + "\\life\\" + fileLoc.get<string>("file_directory");

	file_Directory = fileLoc.get<string>("file_directory") + "\\";

	LEVEL = 2;
	InitSimulation();

	temp = false;
	temp_total_string = "";
	delete_file = false;
	delete_timer = 0.0f;
	load_file = false;
	load_timer = 0.0f;
}


/******************************************************************************/
/*!
\brief
Sets the state type
*/
/******************************************************************************/
void MapScene::setState(MAP_TYPE MT)
{
	CUR_STATE = MT;
}

/******************************************************************************/
/*!
\brief
Initialize shaders
*/
/******************************************************************************/
void MapScene::InitShaders()
{
	Application::SetCursor(true);
	// Init VBO here
	// Set background color to whatever
	glClearColor(0.1f, 0.1f, 0.1f, 0.0f);

	//Enable depth buffer and depth testing
	glEnable(GL_DEPTH_TEST);

	//Enable back face culling
	glEnable(GL_CULL_FACE);

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Default to fill mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Generate a default VAO for now
	glGenVertexArrays(1, &u_m_vertexArrayID);
	glBindVertexArray(u_m_vertexArrayID);

	// Initialize Materials
	InitShadersAndLights();

	//Set projection matrix to perspective mode
	Mtx44 projection;
	projection.SetToPerspective(f_fov, static_cast<double>(Application::GetWindowWidth()) / static_cast<double>(Application::GetWindowHeight()), 0.01f, 10000.0f); //FOV, Aspect Ratio, Near plane, Far plane
	projectionStack.LoadMatrix(projection);
}

/******************************************************************************/
/*!
\brief
Initializes the meshes that is in the P_meshArray
*/
/******************************************************************************/
void MapScene::InitMeshList()
{
	for (unsigned i = 0; i < E_GEO_TOTAL; ++i)
	{
		P_meshArray[i] = NULL;
	}

	LuaScript script("tga");

	P_meshArray[E_GEO_AXES] = MeshBuilder::GenerateAxes("AXES", 10000, 10000, 10000);

	//Text
	P_meshArray[E_GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	P_meshArray[E_GEO_TEXT]->textureID[0] = LoadTGA(script.getGameData("image.font.mainmenu").c_str(), false, false);

	P_meshArray[E_GEO_BUTTON_REFRESH] = MeshBuilder::GenerateQuad("Button Texture", Color(0.f, 0.f, 0.f), 1.f, 1.f, 1.0f);
	P_meshArray[E_GEO_BUTTON_REFRESH]->textureID[0] = LoadTGA(script.getGameData("image.button.refresh").c_str(), true);

	P_meshArray[E_GEO_BUTTON_LEFT] = MeshBuilder::GenerateQuad("Button Texture", Color(0.f, 0.f, 0.f), 1.f, 1.f, 1.0f);
	P_meshArray[E_GEO_BUTTON_LEFT]->textureID[0] = LoadTGA(script.getGameData("image.button.left").c_str(), true);

	P_meshArray[E_GEO_BUTTON_SAVE] = MeshBuilder::GenerateQuad("Save button", Color(0.f, 0.f, 0.f), 1.f, 1.f, 1.0f);
	P_meshArray[E_GEO_BUTTON_SAVE]->textureID[0] = LoadTGA(script.getGameData("image.button.Save").c_str(), true);

	P_meshArray[E_GEO_BUTTON_LOAD] = MeshBuilder::GenerateQuad("Load button", Color(0.f, 0.f, 0.f), 1.f, 1.f, 1.0f);
	P_meshArray[E_GEO_BUTTON_LOAD]->textureID[0] = LoadTGA(script.getGameData("image.button.load").c_str(), true);

	P_meshArray[E_GEO_BUTTON_DIRECTORY] = MeshBuilder::GenerateQuad("Directory Button", Color(0.f, 0.f, 0.f), 1.f, 1.f, 1.0f);
	P_meshArray[E_GEO_BUTTON_DIRECTORY]->textureID[0] = LoadTGA(script.getGameData("image.button.directory").c_str(), true);

	//Tile Selecteion (Bordered)
	P_meshArray[E_GEO_WALL_BORDER] = MeshBuilder::GenerateQuad("Wall (Border)", Color(0.f, 0.f, 0.f), 1.f, 1.f, 1.0f);
	P_meshArray[E_GEO_WALL_BORDER]->textureID[0] = LoadTGA(script.getGameData("image.button.Wall_1_Border").c_str(), true);

	P_meshArray[E_GEO_FLOOR_BORDER] = MeshBuilder::GenerateQuad("Floor (Border)", Color(0.f, 0.f, 0.f), 1.f, 1.f, 1.0f);
	P_meshArray[E_GEO_FLOOR_BORDER]->textureID[0] = LoadTGA(script.getGameData("image.button.Floor_1_Border").c_str(), true);

	P_meshArray[E_GEO_PLAYER_BORDER] = MeshBuilder::GenerateQuad("Player (Border)", Color(0.f, 0.f, 0.f), 1.f, 1.f, 1.0f);
	P_meshArray[E_GEO_PLAYER_BORDER]->textureID[0] = LoadTGA(script.getGameData("image.button.Pilot_Border").c_str(), true);

	P_meshArray[E_GEO_ENEMY_BORDER] = MeshBuilder::GenerateQuad("Enemy (Border)", Color(0.f, 0.f, 0.f), 1.f, 1.f, 1.0f);
	P_meshArray[E_GEO_ENEMY_BORDER]->textureID[0] = LoadTGA(script.getGameData("image.button.Servant_Border").c_str(), true);

	P_meshArray[E_GEO_DANGER_BORDER] = MeshBuilder::GenerateQuad("Danger (Border)", Color(0.f, 0.f, 0.f), 1.f, 1.f, 1.0f);
	P_meshArray[E_GEO_DANGER_BORDER]->textureID[0] = LoadTGA(script.getGameData("image.button.Danger_Border").c_str(), true);

	P_meshArray[E_GEO_WIN_BORDER] = MeshBuilder::GenerateQuad("Win (Border)", Color(0.f, 0.f, 0.f), 1.f, 1.f, 1.0f);
	P_meshArray[E_GEO_WIN_BORDER]->textureID[0] = LoadTGA(script.getGameData("image.button.Win_Border").c_str(), true);

	//World
	/*P_meshArray[E_GEO_FLOOR_1] = MeshBuilder::GenerateQuad("Floor Texture", Color(0.f, 0.f, 0.f), 1.f, 1.f, 1.0f);
	P_meshArray[E_GEO_FLOOR_1]->textureID[0] = LoadTGA(script.getGameData("image.tile.floor").c_str(), true);

	P_meshArray[E_GEO_FLOOR_2] = MeshBuilder::GenerateQuad("Floor Texture_White", Color(0.f, 0.f, 0.f), 1.f, 1.f, 1.0f);
	P_meshArray[E_GEO_FLOOR_2]->textureID[0] = LoadTGA(script.getGameData("image.tile.floor_2").c_str(), true);

	P_meshArray[E_GEO_WALL_1] = MeshBuilder::GenerateQuad("Wall Texture", Color(0.f, 0.f, 0.f), 1.f, 1.f, 1.0f);
	P_meshArray[E_GEO_WALL_1]->textureID[0] = LoadTGA(script.getGameData("image.tile.wall").c_str(), true);

	P_meshArray[E_GEO_PLAYER] = MeshBuilder::GenerateQuad("AI enemy", Color(0.f, 0.f, 0.f), 1.f, 1.f, 1.0f);
	P_meshArray[E_GEO_PLAYER]->textureID[0] = LoadTGA(script.getGameData("image.tile.enemy").c_str(), true);*/

	P_meshArray[E_GEO_POPUP] = MeshBuilder::GenerateQuad("Pop Up Background", Color(0.f, 0.f, 0.f), 1.f, 1.f, 1.0f);
	P_meshArray[E_GEO_POPUP]->textureID[0] = LoadTGA(script.getGameData("image.background.popup").c_str(), true);

	P_meshArray[E_GEO_LINE] = MeshBuilder::GenerateQuad("Ring Segment", Color(1.f, 0.f, 0.f), 1, 1);
}

/******************************************************************************/
/*!
\brief
Initializes menu
*/
/******************************************************************************/
void MapScene::InitMenu(void)
{
	UIColor.Set(0.48235f, 0.70196f, 1.f);
	UIColorPressed.Set(0.9098f, 0.41568f, 0.94117f);
	UIColorEnemy.Set(0.0f, 0.8f, 0.4f);
	//TextButton* S_MB;

	Button *m_B;
	m_B = new Button;
	m_B->Position.Set(Application::GetWindowWidth()* 0.025f, Application::GetWindowHeight()*0.05f, 0.1f);
	m_B->Scale.Set(19, 19, 19);
	m_B->mesh = P_meshArray[E_GEO_BUTTON_LEFT];
	m_B->ID = BI_BACK;
	m_B->labeltype = Button::LT_NONE;
	m_B->gamestate = MT_CREATE;
	v_buttonList.push_back(m_B);

	m_B = new Button;
	m_B->Position.Set(Application::GetWindowWidth()*0.025f, Application::GetWindowHeight()*0.12f, 0.1f);
	m_B->Scale.Set(20, 20, 20);
	m_B->mesh = P_meshArray[E_GEO_BUTTON_REFRESH];
	m_B->ID = BI_REFRESH;
	m_B->labeltype = Button::LT_NONE;
	m_B->gamestate = MT_CREATE;
	v_buttonList.push_back(m_B);

	m_B = new Button;
	m_B->Position.Set(Application::GetWindowWidth()*0.025f, Application::GetWindowHeight()*0.19f, 0.1f);
	m_B->Scale.Set(20, 20, 20);
	m_B->mesh = P_meshArray[E_GEO_BUTTON_SAVE];
	m_B->ID = BI_SAVE;
	m_B->labeltype = Button::LT_NONE;
	m_B->gamestate = MT_CREATE;
	v_buttonList.push_back(m_B);

	m_B = new Button;
	m_B->Position.Set(Application::GetWindowWidth()*0.025f, Application::GetWindowHeight()*0.26f, 0.1f);
	m_B->Scale.Set(20, 20, 20);
	m_B->mesh = P_meshArray[E_GEO_BUTTON_LOAD];
	m_B->ID = BI_LOAD;
	m_B->labeltype = Button::LT_NONE;
	m_B->gamestate = MT_CREATE;
	v_buttonList.push_back(m_B);

	m_B = new Button;
	m_B->Position.Set(Application::GetWindowWidth()*0.025f, Application::GetWindowHeight()*0.33f, 0.1f);
	m_B->Scale.Set(20, 20, 20);
	m_B->mesh = P_meshArray[E_GEO_BUTTON_DIRECTORY];
	m_B->ID = BI_DELETE;
	m_B->labeltype = Button::LT_NONE;
	m_B->gamestate = MT_CREATE;
	v_buttonList.push_back(m_B);

	// Tiles
	m_B = new Button;
	m_B->Position.Set(Application::GetWindowWidth()*0.025f, Application::GetWindowHeight()*0.80f, 0.1f);
	m_B->Scale.Set(20, 20, 20);
	m_B->mesh = P_meshArray[E_GEO_FLOOR_BORDER];
	m_B->ID = BI_FLOOR;
	m_B->labeltype = Button::LT_NONE;
	m_B->gamestate = MT_CREATE;
	v_buttonList.push_back(m_B);

	m_B = new Button;
	m_B->Position.Set(Application::GetWindowWidth()*0.025f, Application::GetWindowHeight()*0.72f, 0.1f);
	m_B->Scale.Set(20, 20, 20);
	m_B->mesh = P_meshArray[E_GEO_WALL_BORDER];
	m_B->ID = BI_WALL;
	m_B->labeltype = Button::LT_NONE;
	m_B->gamestate = MT_CREATE;
	v_buttonList.push_back(m_B);

	m_B = new Button;
	m_B->Position.Set(Application::GetWindowWidth()*0.025f, Application::GetWindowHeight()*0.64f, 0.1f);
	m_B->Scale.Set(20, 20, 20);
	m_B->mesh = P_meshArray[E_GEO_PLAYER_BORDER];
	m_B->ID = BI_PLAYER;
	m_B->labeltype = Button::LT_NONE;
	m_B->gamestate = MT_CREATE;
	v_buttonList.push_back(m_B);

	m_B = new Button;
	m_B->Position.Set(Application::GetWindowWidth()*0.025f, Application::GetWindowHeight()*0.56f, 0.1f);
	m_B->Scale.Set(20, 20, 20);
	m_B->mesh = P_meshArray[E_GEO_ENEMY_BORDER];
	m_B->ID = BI_ENEMY;
	m_B->labeltype = Button::LT_NONE;
	m_B->gamestate = MT_CREATE;
	v_buttonList.push_back(m_B);

	m_B = new Button;
	m_B->Position.Set(Application::GetWindowWidth()*0.025f, Application::GetWindowHeight()*0.48f, 0.1f);
	m_B->Scale.Set(20, 20, 20);
	m_B->mesh = P_meshArray[E_GEO_DANGER_BORDER];
	m_B->ID = BI_DANGER;
	m_B->labeltype = Button::LT_NONE;
	m_B->gamestate = MT_CREATE;
	v_buttonList.push_back(m_B);

	m_B = new Button;
	m_B->Position.Set(Application::GetWindowWidth()*0.025f, Application::GetWindowHeight()*0.40f, 0.1f);
	m_B->Scale.Set(20, 20, 20);
	m_B->mesh = P_meshArray[E_GEO_WIN_BORDER];
	m_B->ID = BI_WIN;
	m_B->labeltype = Button::LT_NONE;
	m_B->gamestate = MT_CREATE;
	v_buttonList.push_back(m_B);

	TextButton* S_MB;
	LuaScript buttonScript("button");
	int total_button;

	float offsetX = 0.0f;
	float offsetY = 0.0f;
	for (int i = 48; i <= 95; i++)
	{
		if ((i >= 48 && i <= 57) || (i >= 65 && i <= 90) || i == 95) // Numbers
		{
			char key = i;
			std::stringstream ss;
			ss << key;

			float offX = 0.0f;
			if (offsetY < 3)
			{
				offX = 0.275f + (offsetX / 20) + 0.025f;
			}
			else
			{
				offX = 0.28f + (offsetX / 20) + 0.025f;
			}
			float offY = 0.5f - (offsetY / 20);
			//cout << "X:[" << offsetX << "] Y:[" << offsetY << "]" << endl;
			//cout << "Letter:[" << ss.str() << "] offX:[" << offX << "] offY:[" << offY << "]" << endl;

			S_MB = new TextButton;
			S_MB->pos.Set(Application::GetWindowWidth() * offX, Application::GetWindowHeight() * offY, 0.5f);
			S_MB->scale.Set(35, 35, 1);
			S_MB->text = ss.str();
			S_MB->gamestate = MT_LOAD;
			v_textButtonList.push_back(S_MB);

			offsetX++;
			if (offsetX >= 10.0f)
			{
				offsetX = 0.0f;
				offsetY++;
			}

			if (i == 95)
			{
				key = 8;
				std::stringstream ss;
				ss << key;
				offX = 0.28f + (offsetX / 20) + 0.025f;

				S_MB = new TextButton;
				S_MB->pos.Set(Application::GetWindowWidth() * offX, Application::GetWindowHeight() * offY, 0.5f);
				S_MB->scale.Set(35, 35, 1);
				S_MB->text = "BACK";
				S_MB->gamestate = MT_LOAD;
				v_textButtonList.push_back(S_MB);
			}
		}
	}

	// Editor replace(repalce and new map)
	total_button = buttonScript.get<int>("editor_replace.total_button");
	for (int i = 1; i <= total_button; i++)
	{
		std::string buttonName = "editor_replace.textbutton_" + std::to_string(static_cast<unsigned long long>(i)) + ".";

		S_MB = new TextButton;
		S_MB->pos.Set(Application::GetWindowWidth()* (buttonScript.get<float>(buttonName + "posX") + 0.025f), Application::GetWindowHeight()* buttonScript.get<float>(buttonName + "posY"), 0.1f);
		S_MB->scale.Set(buttonScript.get<float>(buttonName + "scale"), buttonScript.get<float>(buttonName + "scale"), buttonScript.get<float>(buttonName + "scale"));
		S_MB->text = buttonScript.get<std::string>(buttonName + "text");
		S_MB->gamestate = MT_REPLACE;
		v_textButtonList.push_back(S_MB);
	}

	//Editor Difficulty
	total_button = buttonScript.get<int>("editor_difficulty.total_button");
	for (int i = 1; i <= total_button; i++)
	{
		std::string buttonName = "editor_difficulty.textbutton_" + std::to_string(static_cast<unsigned long long>(i)) + ".";

		S_MB = new TextButton;
		S_MB->pos.Set(Application::GetWindowWidth()* (buttonScript.get<float>(buttonName + "posX") + 0.025f), Application::GetWindowHeight()*buttonScript.get<float>(buttonName + "posY"), 0.1f);
		S_MB->scale.Set(buttonScript.get<float>(buttonName + "scale"), buttonScript.get<float>(buttonName + "scale"), buttonScript.get<float>(buttonName + "scale"));
		S_MB->text = buttonScript.get<std::string>(buttonName + "text");
		S_MB->gamestate = MT_DIFFICULTY;
		v_textButtonList.push_back(S_MB);
	}

	//Editor Load Confirmation
	total_button = buttonScript.get<int>("editor_load.total_button");
	for (int i = 1; i <= total_button; i++)
	{
		std::string buttonName = "editor_load.textbutton_" + std::to_string(static_cast<unsigned long long>(i)) + ".";

		S_MB = new TextButton;
		S_MB->pos.Set(Application::GetWindowWidth()* (buttonScript.get<float>(buttonName + "posX") + 0.045f), Application::GetWindowHeight()*buttonScript.get<float>(buttonName + "posY"), 0.1f);
		S_MB->scale.Set(buttonScript.get<float>(buttonName + "scale"), buttonScript.get<float>(buttonName + "scale"), buttonScript.get<float>(buttonName + "scale"));
		S_MB->text = buttonScript.get<std::string>(buttonName + "text");
		S_MB->gamestate = MT_LOAD;
		v_textButtonList.push_back(S_MB);
	}

	//Editor Delete Confirmation
	int total_option = buttonScript.get<int>("editor_button.button_4.total_option");
	for (int i = 1; i <= total_option; i++)
	{
		std::string buttonName = "editor_button.button_4.option_" + std::to_string(static_cast<unsigned long long>(i)) + ".";

		S_MB = new TextButton;
		S_MB->pos.Set(Application::GetWindowWidth()* (buttonScript.get<float>(buttonName + "posX") + 0.025f), Application::GetWindowHeight()*buttonScript.get<float>(buttonName + "posY"), 0.1f);
		S_MB->scale.Set(buttonScript.get<float>(buttonName + "scale"), buttonScript.get<float>(buttonName + "scale"), buttonScript.get<float>(buttonName + "scale"));
		S_MB->text = buttonScript.get<std::string>(buttonName + "text");
		S_MB->gamestate = MT_DELETE;
		v_textButtonList.push_back(S_MB);
	}
}

/******************************************************************************/
/*!
\brief
Initializes simulation
*/
/******************************************************************************/
bool MapScene::InitSimulation(void)
{
	if (remove("GameData//Maps//temp_file.csv") != 0)
	{
		//std::cout << "Unable to remove" << std::endl;
	}

	//Init Character/world stuff here
	if (!InitLevel(LEVEL))
	{
		return false;
	}

	f_timer = 0.f;
	i_SimulationSpeed = 1;
	return true;
}

/******************************************************************************/
/*!
\brief
Initializes a level
\param level
the level to load
*/
/******************************************************************************/
bool MapScene::InitLevel(int level)
{
	if (MENU_STATE == MT_CREATE)
	{
		newMapName = "";
		enemyCounter = 0;

		ML_map.map_width = 30;
		ML_map.map_height = 22;

		std::cout << ML_map.map_width << ", " << ML_map.map_height << "\n";

		/*std::cout << "\nLoading map...\n";


		std::cout << "Map Size: ";
		std::cout << ML_map.map_width << ", " << ML_map.map_height << "\n";*/

		//Deletes everything from the world
		while (GO_List.size() > 0)
		{
			GameObject *GO = GO_List.back();
			if (GO != NULL)
			{
				delete GO;
				GO = NULL;
			}
			GO_List.pop_back();
		}

		ML_map.map_data.clear();

		for (unsigned y = ML_map.map_height - 1; y > 0; --y)
		{
			for (unsigned x = 0; x < ML_map.map_width; ++x)
			{
				mapArray.push_back("0");
			}

			ML_map.map_data.push_back(mapArray);
			mapArray.clear();
		}

		ML_map.map_data[0][0] = "20";
		ML_map.map_data[0][2] = "40";
		ML_map.map_data[0][3] = "30";
		ML_map.map_data[0][4] = "20";

		std::stringstream ss;
		ss << "GameData//Maps//temp_file.csv";
		ML_map.saveMap_Creator(ss.str());
		ML_map.loadMap(ss.str());
		for (unsigned y = ML_map.map_height - 1; y > 0; --y)
		{
			for (unsigned x = 0; x < ML_map.map_width; ++x)
			{
				if (ML_map.map_data[y][x] == "0")
				{
					GameObject *GO;
					GO = new GameObject();
					GO->Init(Vector3(x*ML_map.worldSize*2.f, (ML_map.map_height - y)*ML_map.worldSize*2.f, -0.5f));
					GO->scale.Set(ML_map.worldSize, ML_map.worldSize, 1);
					GO->enableCollision = false;
					GO->mesh = P_meshArray[E_GEO_FLOOR_BORDER];
					GO_List.push_back(GO);
				}
			}
		}
	}

	if (MENU_STATE == MT_EDIT)
	{
		MENU_STATE = MT_CREATE;
		enemyCounter = 0;
		newMapName = temp_total_string;
		for (unsigned y = ML_map.map_height - 1; y > 0; --y)
		{
			for (unsigned x = 0; x < ML_map.map_width; ++x)
			{
				GameObject *GO;
				GO = new GameObject();
				GO->Init(Vector3(x*ML_map.worldSize*2.f, (ML_map.map_height - y)*ML_map.worldSize*2.f, -0.5f));
				GO->scale.Set(ML_map.worldSize, ML_map.worldSize, 1);
				GO->enableCollision = false;
				GO->mesh = P_meshArray[E_GEO_FLOOR_BORDER];
				GO_List.push_back(GO);

				if (ML_map.map_data[y][x] == "P")
				{
					GameObject *GO;
					GO = new GameObject();
					GO->Init(Vector3(x*ML_map.worldSize*2.f, (ML_map.map_height - y)*ML_map.worldSize*2.f, -0.5f));
					GO->scale.Set(ML_map.worldSize, ML_map.worldSize, 1);
					GO->enableCollision = false;
					GO->mesh = P_meshArray[E_GEO_PLAYER_BORDER];
					GO_List.push_back(GO);
				}
				if (ML_map.map_data[y][x] == "1")
				{
					GameObject *GO;
					GO = new GameObject();
					GO->Init(Vector3(x*ML_map.worldSize*2.f, (ML_map.map_height - y)*ML_map.worldSize*2.f, -0.5f));
					GO->scale.Set(ML_map.worldSize, ML_map.worldSize, 1);
					GO->enableCollision = false;
					GO->mesh = P_meshArray[E_GEO_WALL_BORDER];
					GO_List.push_back(GO);
				}
				if (ML_map.map_data[y][x] == "2")
				{
					GameObject *GO;
					GO = new GameObject();
					GO->Init(Vector3(x*ML_map.worldSize*2.f, (ML_map.map_height - y)*ML_map.worldSize*2.f, -0.5f));
					GO->scale.Set(ML_map.worldSize, ML_map.worldSize, 1);
					GO->enableCollision = false;
					GO->mesh = P_meshArray[E_GEO_DANGER_BORDER];
					GO_List.push_back(GO);
				}
				if (ML_map.map_data[y][x] == "3")
				{
					GameObject *GO;
					GO = new GameObject();
					GO->Init(Vector3(x*ML_map.worldSize*2.f, (ML_map.map_height - y)*ML_map.worldSize*2.f, -0.5f));
					GO->scale.Set(ML_map.worldSize, ML_map.worldSize, 1);
					GO->enableCollision = false;
					GO->mesh = P_meshArray[E_GEO_WIN_BORDER];
					GO_List.push_back(GO);
				}
				if (isdigit(ML_map.map_data[y][x][0]))
				{
					if (stoi(ML_map.map_data[y][x]) >= 50)
					{
						GameObject *GO;
						GO = new GameObject();
						GO->Init(Vector3(x*ML_map.worldSize*2.f, (ML_map.map_height - y)*ML_map.worldSize*2.f, -0.5f));
						GO->scale.Set(ML_map.worldSize, ML_map.worldSize, 1);
						GO->enableCollision = false;
						GO->mesh = P_meshArray[E_GEO_ENEMY_BORDER];
						GO_List.push_back(GO);

						enemyCounter++;
					}
				}
			}
		}
	}
	v3_2DCam.x = -static_cast<float>(Application::GetWindowWidth()) * 0.535f;
	v3_2DCam.y = -static_cast<float>(Application::GetWindowHeight()) * 0.5f;

	v3_2DCam.x += ML_map.map_width*ML_map.worldSize;
	v3_2DCam.y += ML_map.map_height*ML_map.worldSize;
	return true;
}

/******************************************************************************/
/*!
\brief
Calculates tile position based on world position
\param Worldpos
the world position
\return
returns the tile position
*/
/******************************************************************************/
Vector3 MapScene::calTilePos(Vector3 Worldpos)
{
	if (Worldpos != 0)
	{
		Worldpos.x = roundf(Worldpos.x / ((ML_map.worldSize) * 2.f));
		Worldpos.y = roundf((Worldpos.y) / ((ML_map.worldSize) * 2.f));
		Worldpos.y -= ML_map.map_height;
		Worldpos.y = -Worldpos.y;
	}
	else
	{
		Worldpos.SetZero();
	}

	return Worldpos;
}

/******************************************************************************/
/*!
\brief
Calculates world position based on tile position
\param Tilepos
the tile position
\return
returns the world position
*/
/******************************************************************************/
Vector3 MapScene::calWorldPos(Vector3 Tilepos)
{
	Tilepos.x = Tilepos.x * ML_map.worldSize * 2.f;
	Tilepos.y = (ML_map.map_height - Tilepos.y) * ML_map.worldSize * 2.f;
	return Tilepos;
}

/******************************************************************************/
/*!
\brief
Gets the button
*/
/******************************************************************************/
TextButton* MapScene::FetchTB(std::string name)
{
	for (std::vector<TextButton*>::iterator it = v_textButtonList.begin(); it != v_textButtonList.end(); ++it)
	{
		TextButton *S_MB = (TextButton *)*it;
		if (S_MB != NULL)
		{
			if (S_MB->text == name && S_MB->gamestate == MENU_STATE)
			{
				return S_MB;
			}
		}
	}

	return NULL;
}

/******************************************************************************/
/*!
\brief
Gets the button
*/
/******************************************************************************/
Button* MapScene::FetchBUTTON(int ID)
{
	for (std::vector<Button*>::iterator it = v_buttonList.begin(); it != v_buttonList.end(); ++it)
	{
		Button *m_B = (Button *)*it;
		if (m_B != NULL)
		{
			if (m_B->gamestate == MENU_STATE && m_B->ID == ID)
			{
				return m_B;
			}
		}
	}
	return NULL;
}

/******************************************************************************/
/*!
\brief
Update button state
*/
/******************************************************************************/
void MapScene::UpdateTextButtons(void)
{
	for (std::vector<TextButton*>::iterator it = v_textButtonList.begin(); it != v_textButtonList.end(); ++it)
	{
		TextButton *S_MB = (TextButton *)*it;
		if (S_MB->gamestate == MENU_STATE)
		{
			if (intersect2D((S_MB->pos + Vector3(S_MB->text.length() * (S_MB->scale.x) - S_MB->scale.x, S_MB->scale.y*0.4f, 0)), S_MB->pos + Vector3(-S_MB->scale.x*0.5f, -(S_MB->scale.y*0.4f), 0), Vector3(MousePosX, MousePosY, 0)))
			{
				S_MB->active = true;
				if (S_MB->text != "BACK")
					S_MB->color = UIColorPressed;
				else
					S_MB->color = UIColor;
			}
			else
			{
				S_MB->active = false;
				if (S_MB->text != "BACK")
					S_MB->color = UIColor;
				else
					S_MB->color = UIColorPressed;
			}
		}
	}
}

/******************************************************************************/
/*!
\brief
Update button state
*/
/******************************************************************************/
void MapScene::UpdateButtons(void)
{
	for (std::vector<Button*>::iterator it = v_buttonList.begin(); it != v_buttonList.end(); ++it)
	{
		Button *m_B = (Button *)*it;

		if (intersect2D(m_B->Position + m_B->Scale, m_B->Position - m_B->Scale, Vector3(MousePosX, MousePosY, 0)))
		{
			m_B->active = true;
			m_B->color = UIColorPressed;
		}
		else
		{
			m_B->active = false;
			m_B->color = UIColor;
		}
	}
}

/******************************************************************************/
/*!
\brief
Animations, controls
*/
/******************************************************************************/
void MapScene::Update(double dt)	//TODO: Reduce complexity of MapScene::Update()
{
	delete_timer -= static_cast<float>(dt);
	load_timer -= static_cast<float>(dt);
	dt *= static_cast<double>(i_SimulationSpeed);
	f_timer += static_cast<float>(dt);
	//Mouse Section
	double x, y;
	Application::GetMousePos(x, y);

	MousePosX = static_cast<float>(x) / Application::GetWindowWidth() * Application::GetWindowWidth();// +v3_2DCam.x;
	MousePosY = (Application::GetWindowHeight() - static_cast<float>(y)) / Application::GetWindowHeight() * Application::GetWindowHeight();// +v3_2DCam.y;

	MousePosX2 = static_cast<float>(x) / Application::GetWindowWidth() * Application::GetWindowWidth() + v3_2DCam.x;
	MousePosY2 = (Application::GetWindowHeight() - static_cast<float>(y)) / Application::GetWindowHeight() * Application::GetWindowHeight() +v3_2DCam.y;

	static bool bLButtonState = false;
	static bool bRButtonState = false;

	UpdateTextButtons();
	UpdateButtons();

	if (f_fov != f_fov_target)
	{
		float diff = f_fov_target - f_fov;

		if (diff < 0.01 && diff > -0.01)
		{
			f_fov = f_fov_target;
		}
		else
		{
			f_fov += diff * static_cast<float>(dt)* 10.f;
		}

		editFOV(f_fov);
	}

	for (unsigned i = 0; i < GO_List.size(); ++i)
	{
		CharacterObject *CO = dynamic_cast<CharacterObject*>(GO_List[i]);

		if (CO != NULL)
		{
			CO->Update(dt);
		}
	}

	switch (MENU_STATE)
	{
	case MT_NEW:
	{
				   static bool isEscPressed = false;
				   if (Application::IsKeyPressed(VK_ESCAPE) && !isEscPressed)
				   {
					   isEscPressed = true;
				   }
				   else if (!Application::IsKeyPressed(VK_ESCAPE) && isEscPressed)
				   {
					   isEscPressed = false;
				   }

				   msg_timer += static_cast<float>(dt);
				   if (msg_timer > 2.f || Application::IsKeyPressed(VK_LBUTTON))
				   {
					   msg_timer = 0;
					   MENU_STATE = MT_CREATE;
				   }
				   break;
	}
		break;

	case MT_REPLACE:
	{
					   static bool isEscPressed = false;
					   if (Application::IsKeyPressed(VK_ESCAPE) && !isEscPressed)
					   {
						   isEscPressed = true;
					   }
					   else if (!Application::IsKeyPressed(VK_ESCAPE) && isEscPressed)
					   {
						   isEscPressed = false;
					   }

					   if (!bLButtonState && Application::IsKeyPressed(VK_LBUTTON))
					   {
						   bLButtonState = true;
					   }
					   if (bLButtonState && !Application::IsKeyPressed(VK_LBUTTON))
					   {
						   bLButtonState = false;

						   LuaScript nameScript("button");

						   if (FetchTB(nameScript.get<std::string>("editor_replace.textbutton_1.text"))->active)
						   {
							   std::stringstream ss;
							   ss << file_Directory << newMapName << ".csv";
							   ML_map.saveMap_Creator(ss.str());
							   MENU_STATE = MT_NEW;
						   }
						   else if (FetchTB(nameScript.get<std::string>("editor_replace.textbutton_2.text"))->active)
						   {
							   MENU_STATE = MT_DIFFICULTY;
						   }
						   else if (FetchTB(nameScript.get<std::string>("editor_replace.textbutton_3.text"))->active)
						   {
							   MENU_STATE = MT_CREATE;
						   }
					   }
	}
		break;
	case MT_DIFFICULTY:
	{
						  static bool isEscPressed = false;
						  if (Application::IsKeyPressed(VK_ESCAPE) && !isEscPressed)
						  {
							  isEscPressed = true;
						  }
						  else if (!Application::IsKeyPressed(VK_ESCAPE) && isEscPressed)
						  {
							  isEscPressed = false;
						  }

						  if (!bLButtonState && Application::IsKeyPressed(VK_LBUTTON))
						  {
							  bLButtonState = true;
						  }
						  if (bLButtonState && !Application::IsKeyPressed(VK_LBUTTON))
						  {
							  bLButtonState = false;

							  LuaScript nameScript("button");

							  if (FetchTB(nameScript.get<std::string>("editor_difficulty.textbutton_1.text"))->active)
							  {
								  // Easy
								  if (CreateDirectoryA(OutputFolder.c_str(), NULL))
								  {
									  cout << "new folder" << endl;
									  newMapName = ML_map.newFile(1, file_Directory);
								  }
								  else if (ERROR_ALREADY_EXISTS == GetLastError())
								  {
									  cout << "already exist" << endl;
									  newMapName = ML_map.newFile(1, file_Directory);
								  }
								  else
								  {
									  cout << "Failed" << endl;
								  }
								  MENU_STATE = MT_NEW;
							  }
							  else if (FetchTB(nameScript.get<std::string>("editor_difficulty.textbutton_2.text"))->active)
							  {
								  // Medium
								  if (CreateDirectoryA(OutputFolder.c_str(), NULL))
								  {
									  cout << "new folder" << endl;
									  newMapName = ML_map.newFile(2, file_Directory);
								  }
								  else if (ERROR_ALREADY_EXISTS == GetLastError())
								  {
									  cout << "already exist" << endl;
									  newMapName = ML_map.newFile(2, file_Directory);
								  }
								  else
								  {
									  cout << "Failed" << endl;
								  }
								  MENU_STATE = MT_NEW;
							  }
							  else if (FetchTB(nameScript.get<std::string>("editor_difficulty.textbutton_3.text"))->active)
							  {
								  // Hard
								  if (CreateDirectoryA(OutputFolder.c_str(), NULL))
								  {
									  cout << "new folder" << endl;
									  newMapName = ML_map.newFile(3, file_Directory);
								  }
								  else if (ERROR_ALREADY_EXISTS == GetLastError())
								  {
									  cout << "already exist" << endl;
									  newMapName = ML_map.newFile(3, file_Directory);
								  }
								  else
								  {
									  cout << "Failed" << endl;
								  }
								  MENU_STATE = MT_NEW;
							  }
							  else if (FetchTB(nameScript.get<std::string>("editor_difficulty.textbutton_4.text"))->active)
							  {
								  MENU_STATE = MT_CREATE;
							  }
						  }
	}
		break;

	case MT_LOAD:
	{
					temp_total_string = Virtual_Keyboard(dt, temp_total_string);
					static bool isEscPressed = false;
					if (Application::IsKeyPressed(VK_ESCAPE) && !isEscPressed)
					{
						isEscPressed = true;
					}
					else if (!Application::IsKeyPressed(VK_ESCAPE) && isEscPressed)
					{
						isEscPressed = false;
					}

					static bool isEnterPressed = false;
					if (Application::IsKeyPressed(VK_RETURN) && !isEnterPressed)
					{
						isEnterPressed = true;
					}
					else if (!Application::IsKeyPressed(VK_RETURN) && isEnterPressed)
					{
						isEnterPressed = false;

						std::stringstream ss;
						ss << file_Directory + temp_total_string + ".csv";
						temp = ML_map.loadMap(ss.str());

						if (temp == false)
						{
							cout << "File Not Found" << std::endl;
							MENU_STATE = MT_LOAD;
							load_file = true;
							load_timer = 2.0f;
						}
						else
						{
							MENU_STATE = MT_EDIT;
							InitSimulation();
						}
					}

					if (!bLButtonState && Application::IsKeyPressed(VK_LBUTTON))
					{
						bLButtonState = true;
					}
					if (bLButtonState && !Application::IsKeyPressed(VK_LBUTTON))
					{
						bLButtonState = false;

						LuaScript nameScript("button");

						if (FetchTB(nameScript.get<std::string>("editor_load.textbutton_1.text"))->active)
						{
							std::stringstream ss;
							ss << file_Directory + temp_total_string + ".csv";
							temp = ML_map.loadMap(ss.str());

							if (temp == false)
							{
								cout << "File Not Found" << std::endl;
								MENU_STATE = MT_LOAD;
								load_file = true;
								load_timer = 2.0f;
							}
							else
							{
								MENU_STATE = MT_EDIT;
								InitSimulation();
							}
							break;
						}
						else if (FetchTB(nameScript.get<std::string>("editor_load.textbutton_2.text"))->active)
						{
							MENU_STATE = MT_CREATE;
							break;
						}
						else if (FetchTB("BACK")->active)
						{
							temp_total_string = temp_total_string.substr(0, temp_total_string.size() - 1);
							break;
						}

						if (temp_total_string.size() < 15)
						{
							for (int i = 48; i <= 95; i++)
							{
								if ((i >= 48 && i <= 57) || (i >= 65 && i <= 90) || i == 95) // Numbers
								{
									char key = i;
									std::stringstream ss;
									ss << key;

									if (FetchTB(ss.str())->active)
									{
										if (i >= 65 && i < 90)
										{
											key = i + 32;
											std::stringstream ss2;
											ss2 << key;

											temp_total_string += ss2.str();
										}
										else
										{
											temp_total_string += ss.str();
										}
										break;
									}
								}
							}
						}
					}

					if (load_timer < 0)
					{
						load_file = false;
					}
	}
		break;

	case MT_DELETE:
	{
					  static bool isEscPressed = false;
					  if (Application::IsKeyPressed(VK_ESCAPE) && !isEscPressed)
					  {
						  isEscPressed = true;
					  }
					  else if (!Application::IsKeyPressed(VK_ESCAPE) && isEscPressed)
					  {
						  isEscPressed = false;
					  }

					  static bool isEnterPressed = false;
					  if (Application::IsKeyPressed(VK_RETURN) && !isEnterPressed)
					  {
						  isEnterPressed = true;
					  }
					  else if (!Application::IsKeyPressed(VK_RETURN) && isEnterPressed)
					  {
						  isEnterPressed = false;

						  std::stringstream ss;
						  ss << file_Directory << newMapName << ".csv";
						  string temp = ss.str();
						  if (remove(temp.c_str()) != 0)
						  {
							  delete_file = true;
							  delete_timer = 2.0f;
						  }

						  if (delete_file == false)
						  {
							  MENU_STATE = MT_CREATE;
							  InitSimulation();
						  }
					  }

					  if (!bLButtonState && Application::IsKeyPressed(VK_LBUTTON))
					  {
						  bLButtonState = true;
					  }
					  if (bLButtonState && !Application::IsKeyPressed(VK_LBUTTON))
					  {
						  bLButtonState = false;

						  LuaScript nameScript("button");

						  if (FetchTB(nameScript.get<std::string>("editor_button.button_4.option_1.text"))->active)
						  {
							  std::stringstream ss;
							  ss << file_Directory << newMapName << ".csv";
							  string temp = ss.str();
							  if (remove(temp.c_str()) != 0)
							  {
								  delete_file = true;
								  delete_timer = 2.0f;
							  }

							  if (delete_file == false)
							  {
								  MENU_STATE = MT_CREATE;
								  InitSimulation();
							  }
						  }
						  else if (FetchTB(nameScript.get<std::string>("editor_button.button_4.option_2.text"))->active)
						  {
							  MENU_STATE = MT_CREATE;
						  }
					  }

					  if (delete_timer < 0)
					  {
						  delete_file = false;
					  }
	}
		break;

	case MT_CREATE:
	case MT_EDIT:
	default:
		//select Tiles
		if (Application::IsKeyPressed('1'))
			selectedTile = BI_FLOOR;
		if (Application::IsKeyPressed('2'))
			selectedTile = BI_WALL;
		if (Application::IsKeyPressed('3'))
			selectedTile = BI_PLAYER;
		if (Application::IsKeyPressed('4'))
			selectedTile = BI_WIN;
		if (Application::IsKeyPressed('5'))
			selectedTile = BI_DANGER;
		if (Application::IsKeyPressed('6'))
			selectedTile = BI_ENEMY;

		static bool isEscPressed = false;
		if (Application::IsKeyPressed(VK_ESCAPE) && !isEscPressed)
		{
			isEscPressed = true;

		}
		else if (!Application::IsKeyPressed(VK_ESCAPE) && isEscPressed)
		{
			isEscPressed = false;
			SceneManager::Instance()->replace(SceneManager::S_MAIN_MENU);
			if (remove("GameData//Maps//temp_file.csv") != 0)
			{
				//std::cout << "Unable to remove" << std::endl;
			}
			return;
		}
		if (!bRButtonState && Application::IsKeyPressed(VK_RBUTTON))
		{
			bRButtonState = true;
		}
		if (bRButtonState && !Application::IsKeyPressed(VK_RBUTTON))
		{
			bRButtonState = false;
		}
		if (!bLButtonState && Application::IsKeyPressed(VK_LBUTTON))
		{
			bLButtonState = true;
		}
		if (bLButtonState && !Application::IsKeyPressed(VK_LBUTTON))
		{
			bLButtonState = false;
			if (FetchBUTTON(BI_BACK)->active)
			{
				SceneManager::Instance()->replace(SceneManager::S_MAIN_MENU);
				if (remove("GameData//Maps//temp_file.csv") != 0)
				{
					std::cout << "Unable to remove" << std::endl;
				}
				return;
			}
			else if (FetchBUTTON(BI_REFRESH)->active)
			{
				MENU_STATE = MT_CREATE;
				InitSimulation();
			}
			else if (FetchBUTTON(BI_SAVE)->active)
			{
				if (Auto_MapName == false)
				{
					std::cout << "Map Name: ";
					std::cin >> newMapName;

					std::stringstream ss;
					ss << "GameData//Maps//" << newMapName << ".csv";
					ML_map.saveMap_Creator(ss.str());
				}
				else
				{
					if (newMapName != "")
					{
						MENU_STATE = MT_REPLACE;
					}
					else
					{
						MENU_STATE = MT_DIFFICULTY;
					}
				}
			}
			else if (FetchBUTTON(BI_DELETE)->active)
			{
				MENU_STATE = MT_DELETE;
				delete_file = false;
			}
			else if (FetchBUTTON(BI_LOAD)->active)
			{
				MENU_STATE = MT_LOAD;
				load_file = false;
				temp_total_string = "";
			}
			else if (FetchBUTTON(BI_WALL)->active)
			{
				selectedTile = BI_WALL;
			}
			else if (FetchBUTTON(BI_FLOOR)->active)
			{
				selectedTile = BI_FLOOR;
			}
			else if (FetchBUTTON(BI_PLAYER)->active)
			{
				selectedTile = BI_PLAYER;
			}
			else if (FetchBUTTON(BI_ENEMY)->active)
			{
				selectedTile = BI_ENEMY;
			}
			else if (FetchBUTTON(BI_DANGER)->active)
			{
				selectedTile = BI_DANGER;
			}
			else if (FetchBUTTON(BI_WIN)->active)
			{
				selectedTile = BI_WIN;
			}
		}
		if (bLButtonState)
			placeTile(selectedTile);
		if (bRButtonState)
			placeTile(BI_FLOOR);
		break;
	}

	//float f_camSpeed = 10.f;

	//if (Application::IsKeyPressed(VK_CONTROL))
	//{
	//	f_camSpeed *= 0.5f;
	//}
	//if (Application::IsKeyPressed(VK_SHIFT))
	//{
	//	f_camSpeed *= 2.f;
	//}
	//if (Application::IsKeyPressed('W'))
	//{
	//	v3_2DCam.y += f_camSpeed;//static_cast<float>(dt) * f_camSpeed;
	//}
	//if (Application::IsKeyPressed('S'))
	//{
	//	v3_2DCam.y -= f_camSpeed;//static_cast<float>(dt) * f_camSpeed;
	//}
	//if (Application::IsKeyPressed('A'))
	//{
	//	v3_2DCam.x -= f_camSpeed;//static_cast<float>(dt) * f_camSpeed;
	//}
	//if (Application::IsKeyPressed('D'))
	//{
	//	v3_2DCam.x += f_camSpeed;//static_cast<float>(dt) * f_camSpeed;
	//}
}

/******************************************************************************/
/*!
\brief
places a tile
\param selectedTile
ID of tile to be placed
*/
/******************************************************************************/
void MapScene::placeTile(int selectedTile)
{
	GameObject *GO;
	GO = new GameObject();
	selTilePos = calTilePos(Vector3(MousePosX2, MousePosY2));

	//selTilePos.x -= 1;
	//selTilePos.y -= 2.f;

	//cout << selTilePos << endl;
	//cout << selWorldPos << endl;
	if (selTilePos.x < ML_map.map_width && selTilePos.x > -1 && selTilePos.y < ML_map.map_height && selTilePos.y > 0)
	{
		GO->currTile.Set(selTilePos.x, selTilePos.y);
		GO->Init(Vector3(selTilePos.x*ML_map.worldSize*2.f, (ML_map.map_height - selTilePos.y)*ML_map.worldSize*2.f, -0.5f));
		GO->scale.Set(ML_map.worldSize, ML_map.worldSize, 1);

		if (isdigit(ML_map.map_data[selTilePos.y][selTilePos.x][0]))
		{
			if (stoi(ML_map.map_data[selTilePos.y][selTilePos.x]) >= 50)
			{
				enemyCounter--;
			}
		}

		switch (selectedTile)
		{
		case BI_FLOOR:
			GO->mesh = MapScene::GetInstance()->P_meshArray[MapScene::E_GEO_FLOOR_BORDER];
			ML_map.map_data[selTilePos.y][selTilePos.x] = "0";
			MapScene::GetInstance()->GO_List.push_back(GO);
			break;
		case BI_WALL:
			GO->mesh = MapScene::GetInstance()->P_meshArray[MapScene::E_GEO_WALL_BORDER];
			ML_map.map_data[selTilePos.y][selTilePos.x] = "1";
			MapScene::GetInstance()->GO_List.push_back(GO);
			break;
		case BI_PLAYER:
			GO->mesh = MapScene::GetInstance()->P_meshArray[MapScene::E_GEO_PLAYER_BORDER];
			if (hasPlacedPlayer)
			{
				ML_map.map_data[playerTile.y][playerTile.x] = "0";

				for (int i = 0; i < GO_List.size(); ++i)
				{
					if (GO_List[i]->currTile == playerTile)
					{
						delete GO_List[i];
						GO_List.erase(GO_List.begin() + i);
					}
				}
			}
			ML_map.map_data[selTilePos.y][selTilePos.x] = "P";
			playerTile.Set(selTilePos.x, selTilePos.y);
			hasPlacedPlayer = true;
			MapScene::GetInstance()->GO_List.push_back(GO);
			break;
		case BI_WIN:
			GO->mesh = MapScene::GetInstance()->P_meshArray[MapScene::E_GEO_WIN_BORDER];
			ML_map.map_data[selTilePos.y][selTilePos.x] = "3";
			MapScene::GetInstance()->GO_List.push_back(GO);
			break;
		case BI_DANGER:
			GO->mesh = MapScene::GetInstance()->P_meshArray[MapScene::E_GEO_DANGER_BORDER];
			ML_map.map_data[selTilePos.y][selTilePos.x] = "2";
			MapScene::GetInstance()->GO_List.push_back(GO);
			break;
		case BI_ENEMY:
			if (enemyCounter < 4)
			{
				GO->mesh = MapScene::GetInstance()->P_meshArray[MapScene::E_GEO_ENEMY_BORDER];
				ML_map.map_data[selTilePos.y][selTilePos.x] = std::to_string(enemyID);

				enemyID++;
				enemyCounter++;

				MapScene::GetInstance()->GO_List.push_back(GO);
			}
		}
	}
	//for ()
}

/******************************************************************************/
/*!
\brief
edits FOV
\param newFOV
new fov to change to
*/
/******************************************************************************/
void MapScene::editFOV(float &newFOV)
{
	Mtx44 proj;
	proj.SetToPerspective(newFOV, static_cast<double>(Application::GetWindowWidth()) / static_cast<double>(Application::GetWindowHeight()), 0.1f, 10000.0f);
	projectionStack.LoadMatrix(proj);
}

/******************************************************************************/
/*!
\brief
Update FOV
*/
/******************************************************************************/
void MapScene::UpdateFOV()
{
	if (Application::IsKeyPressed('K'))
	{
		if (f_fov >= 44.f && f_fov < 110.f)
		{
			f_fov += 1.f;
		}
	}
	if (Application::IsKeyPressed('L'))
	{
		if (f_fov <= 111.f && f_fov > 45.f)
		{
			f_fov -= 1.f;
		}
	}

	Mtx44 proj;
	proj.SetToPerspective(f_fov, Application::GetWindowWidth() / Application::GetWindowHeight(), 0.1f, 10000.0f);
	projectionStack.LoadMatrix(proj);
}

/******************************************************************************/
/*!
\brief
Initializes all the Shaders & Lights
*/
/******************************************************************************/
void MapScene::InitShadersAndLights(void)
{
	LuaScript scriptshader("Shader");

	//Load vertex and fragment shaders
	u_m_programID = LoadShaders(scriptshader.getGameData("shader.vertex").c_str(), scriptshader.getGameData("shader.fragment").c_str());
	glUseProgram(u_m_programID);

	// Get a handle for our "colorTexture" uniform
	u_m_parameters[E_UNI_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(u_m_programID, "colorTextureEnabled");
	u_m_parameters[E_UNI_COLOR_TEXTURE] = glGetUniformLocation(u_m_programID, "colorTexture");

	// Get a handle for our "textColor" uniform
	u_m_parameters[E_UNI_TEXT_ENABLED] = glGetUniformLocation(u_m_programID, "textEnabled");
	u_m_parameters[E_UNI_TEXT_COLOR] = glGetUniformLocation(u_m_programID, "textColor");

	// Get a handle for our "MVP" uniform
	u_m_parameters[E_UNI_MVP] = glGetUniformLocation(u_m_programID, "MVP");
	u_m_parameters[E_UNI_MODELVIEW] = glGetUniformLocation(u_m_programID, "MV");
	u_m_parameters[E_UNI_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(u_m_programID, "MV_inverse_transpose");
	u_m_parameters[E_UNI_MATERIAL_AMBIENT] = glGetUniformLocation(u_m_programID, "material.kAmbient");
	u_m_parameters[E_UNI_MATERIAL_DIFFUSE] = glGetUniformLocation(u_m_programID, "material.kDiffuse");
	u_m_parameters[E_UNI_MATERIAL_SPECULAR] = glGetUniformLocation(u_m_programID, "material.kSpecular");
	u_m_parameters[E_UNI_MATERIAL_SHININESS] = glGetUniformLocation(u_m_programID, "material.kShininess");

	std::string code[ui_NUM_LIGHT_PARAMS] =
	{
		"lights[0].position_cameraspace",
		"lights[0].color",
		"lights[0].power",
		"lights[0].kC",
		"lights[0].kL",
		"lights[0].kQ",
		"lights[0].type",
		"lights[0].spotDirection",
		"lights[0].cosCutoff",
		"lights[0].cosInner",
		"lights[0].exponent"
	};

	for (size_t i = 0; i < ui_NUM_LIGHTS; ++i)		// For every light
	{
		if (i != 0)
		{
			// Loop to update array index of strings
			for (size_t param = 0; param < ui_NUM_LIGHT_PARAMS; ++param)		// For every param for each light
			{
				code[param][7] = static_cast<char>(i + 48) /*convert into char*/;
			}
		}

		u_m_parameters[E_UNI_LIGHT0_POSITION + i * ui_NUM_LIGHT_PARAMS] = glGetUniformLocation(u_m_programID, code[0].c_str());
		u_m_parameters[E_UNI_LIGHT0_COLOR + i * ui_NUM_LIGHT_PARAMS] = glGetUniformLocation(u_m_programID, code[1].c_str());
		u_m_parameters[E_UNI_LIGHT0_POWER + i * ui_NUM_LIGHT_PARAMS] = glGetUniformLocation(u_m_programID, code[2].c_str());
		u_m_parameters[E_UNI_LIGHT0_KC + i * ui_NUM_LIGHT_PARAMS] = glGetUniformLocation(u_m_programID, code[3].c_str());
		u_m_parameters[E_UNI_LIGHT0_KL + i * ui_NUM_LIGHT_PARAMS] = glGetUniformLocation(u_m_programID, code[4].c_str());
		u_m_parameters[E_UNI_LIGHT0_KQ + i * ui_NUM_LIGHT_PARAMS] = glGetUniformLocation(u_m_programID, code[5].c_str());
		u_m_parameters[E_UNI_LIGHT0_TYPE + i * ui_NUM_LIGHT_PARAMS] = glGetUniformLocation(u_m_programID, code[6].c_str());
		u_m_parameters[E_UNI_LIGHT0_SPOTDIRECTION + i * ui_NUM_LIGHT_PARAMS] = glGetUniformLocation(u_m_programID, code[7].c_str());
		u_m_parameters[E_UNI_LIGHT0_COSCUTOFF + i * ui_NUM_LIGHT_PARAMS] = glGetUniformLocation(u_m_programID, code[8].c_str());
		u_m_parameters[E_UNI_LIGHT0_COSINNER + i * ui_NUM_LIGHT_PARAMS] = glGetUniformLocation(u_m_programID, code[9].c_str());
		u_m_parameters[E_UNI_LIGHT0_EXPONENT + i * ui_NUM_LIGHT_PARAMS] = glGetUniformLocation(u_m_programID, code[10].c_str());
	}

	u_m_parameters[E_UNI_LIGHTENABLED] = glGetUniformLocation(u_m_programID, "lightEnabled");
	u_m_parameters[E_UNI_NUMLIGHTS] = glGetUniformLocation(u_m_programID, "numLights");

	glUniform1i(u_m_parameters[E_UNI_NUMLIGHTS], ui_NUM_LIGHTS);

	u_m_parameters[U_UNI_GLOW] = glGetUniformLocation(u_m_programID, "glow");
	u_m_parameters[U_UNI_GLOW_COLOR] = glGetUniformLocation(u_m_programID, "glowColor");

	glUniform1i(u_m_parameters[U_UNI_GLOW], 0);

	//Main Lighting
	P_lightsArray[0].type = LIGHT_POINT;
	P_lightsArray[0].position.Set(80, 350, 100);
	P_lightsArray[0].color.Set(1.f, 1.f, 0.98f);
	P_lightsArray[0].power = 1.5f;
	P_lightsArray[0].kC = 1.0f;
	P_lightsArray[0].kL = 0.0005f;
	P_lightsArray[0].kQ = 0.000001f;
	P_lightsArray[0].cosCutoff = cos(Math::DegreeToRadian(50));
	P_lightsArray[0].cosInner = cos(Math::DegreeToRadian(20));
	P_lightsArray[0].exponent = 3.f;
	P_lightsArray[0].spotDirection.Set(0.f, 1.f, 0.f);

	// Make sure you pass uniform parameters after glUseProgram()
	for (size_t i = 0; i < ui_NUM_LIGHTS; ++i)
	{
		glUniform1i(u_m_parameters[E_UNI_LIGHT0_TYPE + ui_NUM_LIGHT_PARAMS * i], P_lightsArray[i].type);
		glUniform3fv(u_m_parameters[E_UNI_LIGHT0_COLOR + ui_NUM_LIGHT_PARAMS * i], 1, &P_lightsArray[i].color.r);
		glUniform1f(u_m_parameters[E_UNI_LIGHT0_POWER + ui_NUM_LIGHT_PARAMS * i], P_lightsArray[i].power);
		glUniform1f(u_m_parameters[E_UNI_LIGHT0_KC + ui_NUM_LIGHT_PARAMS * i], P_lightsArray[i].kC);
		glUniform1f(u_m_parameters[E_UNI_LIGHT0_KL + ui_NUM_LIGHT_PARAMS * i], P_lightsArray[i].kL);
		glUniform1f(u_m_parameters[E_UNI_LIGHT0_KQ + ui_NUM_LIGHT_PARAMS * i], P_lightsArray[i].kQ);
		glUniform1f(u_m_parameters[E_UNI_LIGHT0_COSCUTOFF + ui_NUM_LIGHT_PARAMS * i], P_lightsArray[i].cosCutoff);
		glUniform1f(u_m_parameters[E_UNI_LIGHT0_COSINNER + ui_NUM_LIGHT_PARAMS * i], P_lightsArray[i].cosInner);
		glUniform1f(u_m_parameters[E_UNI_LIGHT0_EXPONENT + ui_NUM_LIGHT_PARAMS * i], P_lightsArray[i].exponent);
	}
}

/******************************************************************************/
/*!
\brief
Renders mesh

\param mesh - pointer to a mesh that will be rendered
\param enableLight - should the mesh rendered be affected by light?
*/
/******************************************************************************/
void MapScene::RenderMesh(Mesh *mesh, bool enableLight, float Glow, Color GlowColor)
{
	glUniform1i(u_m_parameters[U_UNI_GLOW], static_cast<GLint>(Glow));
	glUniform3fv(u_m_parameters[U_UNI_GLOW_COLOR], 1, &GlowColor.r);

	Mtx44 MVP, modelView, modelView_inverse_transpose;

	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(u_m_parameters[E_UNI_MVP], 1, GL_FALSE, &MVP.a[0]);
	if (enableLight)
	{
		glUniform1i(u_m_parameters[E_UNI_LIGHTENABLED], 1);
		modelView = viewStack.Top() * modelStack.Top();
		glUniformMatrix4fv(u_m_parameters[E_UNI_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);
		modelView_inverse_transpose = modelView.GetInverse().GetTranspose();
		glUniformMatrix4fv(u_m_parameters[E_UNI_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE, &modelView_inverse_transpose.a[0]);

		//load material
		glUniform3fv(u_m_parameters[E_UNI_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
		glUniform3fv(u_m_parameters[E_UNI_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
		glUniform3fv(u_m_parameters[E_UNI_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
		glUniform1f(u_m_parameters[E_UNI_MATERIAL_SHININESS], mesh->material.kShininess);
	}
	else
	{
		glUniform1i(u_m_parameters[E_UNI_LIGHTENABLED], 0);
	}

	if (mesh->textureID[0] > 0)
	{
		glUniform1i(u_m_parameters[E_UNI_COLOR_TEXTURE_ENABLED], 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->textureID[0]);
		glUniform1i(u_m_parameters[E_UNI_COLOR_TEXTURE], 0);
	}
	else
	{
		glUniform1i(u_m_parameters[E_UNI_COLOR_TEXTURE_ENABLED], 0);
	}
	mesh->Render(); //this line should only be called once 
	if (mesh->textureID[0] > 0)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

/******************************************************************************/
/*!
\brief
Renders text

\param mesh - Pointer to a mesh with a font texture
\param text - String to be printed
\param color - The colour of the text to be printed
*/
/******************************************************************************/
void MapScene::RenderText(Mesh* mesh, std::string text, Color color)
{
	if (!mesh || mesh->textureID <= 0) //Proper error check
		return;

	glDisable(GL_DEPTH_TEST);
	glUniform1i(u_m_parameters[E_UNI_TEXT_ENABLED], 1);
	glUniform3fv(u_m_parameters[E_UNI_TEXT_COLOR], 1, &color.r);
	glUniform1i(u_m_parameters[E_UNI_LIGHTENABLED], 0);
	glUniform1i(u_m_parameters[E_UNI_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID[0]);
	glUniform1i(u_m_parameters[E_UNI_COLOR_TEXTURE], 0);
	for (unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(i * 1.0f, 0, 0); //1.0f is the spacing of each character, you may change this value
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(u_m_parameters[E_UNI_MVP], 1, GL_FALSE, &MVP.a[0]);

		mesh->Render((unsigned)text[i] * 6, 6);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(u_m_parameters[E_UNI_TEXT_ENABLED], 0);
	glEnable(GL_DEPTH_TEST);
}

/******************************************************************************/
/*!
\brief
Renders text around the center

\param mesh - Pointer to a mesh with a font texture
\param text - String to be printed
\param color - The colour of the text to be printed
*/
/******************************************************************************/
void MapScene::RenderTextCenter(Mesh* mesh, std::string text, Color color)
{
	modelStack.PushMatrix();
	modelStack.Translate(-(text.size() / 2.0f), 0.0f, 0.0f);
	RenderText(mesh, text, color);
	modelStack.PopMatrix();
}

/******************************************************************************/
/*!
\brief
Renders text around the center on the screen

\param mesh - Pointer to a mesh with a font texture
\param text - String to be printed
\param color - The colour of the text to be printed
*/
/******************************************************************************/
void MapScene::RenderTextCenterOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
{
	modelStack.PushMatrix();
	x -= ((text.length() - 1.5f) / 2.0f) * size;
	modelStack.Translate(x, y, 0);
	modelStack.Scale(size, size, size);
	RenderTextOnScreen(mesh, text, color);
	modelStack.PopMatrix();
}

/******************************************************************************/
/*!
\brief
Renders text on screen
*/
/******************************************************************************/
void MapScene::RenderTextOnScreen(Mesh* mesh, std::string text, Color color)
{
	glUniform1i(u_m_parameters[U_UNI_GLOW], static_cast<GLint>(0));
	if (!mesh || mesh->textureID <= 0) //Proper error check
		return;

	glDisable(GL_DEPTH_TEST);

	//Add these code just after glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, Application::GetWindowWidth(), 0, Application::GetWindowHeight(), -10, 10); //size of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	//modelStack.LoadIdentity(); //Reset modelStack

	glUniform1i(u_m_parameters[E_UNI_TEXT_ENABLED], 1);
	glUniform3fv(u_m_parameters[E_UNI_TEXT_COLOR], 1, &color.r);
	glUniform1i(u_m_parameters[E_UNI_LIGHTENABLED], 0);
	glUniform1i(u_m_parameters[E_UNI_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID[0]);
	glUniform1i(u_m_parameters[E_UNI_COLOR_TEXTURE], 0);
	for (unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(i * 1.0f, 0, 0); //1.0f is the spacing of each character, you may change this value
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(u_m_parameters[E_UNI_MVP], 1, GL_FALSE, &MVP.a[0]);

		mesh->Render((unsigned)text[i] * 6, 6);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(u_m_parameters[E_UNI_TEXT_ENABLED], 0);

	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();

	glEnable(GL_DEPTH_TEST);
}

/******************************************************************************/
/*!
\brief
Renders a mesh on screen
*/
/******************************************************************************/
void MapScene::RenderMeshOnScreen(Mesh* mesh, float Glow, Color GlowColor)
{
	glUniform1i(u_m_parameters[U_UNI_GLOW], static_cast<GLint>(Glow));
	glUniform3fv(u_m_parameters[U_UNI_GLOW_COLOR], 1, &GlowColor.r);

	glDisable(GL_DEPTH_TEST);
	glUniform1i(u_m_parameters[E_UNI_LIGHTENABLED], 0);

	if (mesh->textureID > 0)
	{
		glUniform1i(u_m_parameters[E_UNI_COLOR_TEXTURE_ENABLED], 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->textureID[0]);
		glUniform1i(u_m_parameters[E_UNI_COLOR_TEXTURE], 0);
	}
	else
		glUniform1i(u_m_parameters[E_UNI_COLOR_TEXTURE_ENABLED], 0);

	Mtx44 ortho;
	ortho.SetToOrtho(0, Application::GetWindowWidth(), 0, Application::GetWindowHeight(), -10, 10); //size of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode

	Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(u_m_parameters[E_UNI_MVP], 1, GL_FALSE, &MVP.a[0]);

	mesh->Render();

	projectionStack.PopMatrix();
	viewStack.PopMatrix();

	if (mesh->textureID > 0)
		glBindTexture(GL_TEXTURE_2D, 0);

	glUniform1i(u_m_parameters[E_UNI_TEXT_ENABLED], 0);
	glEnable(GL_DEPTH_TEST);
	glUniform1i(u_m_parameters[U_UNI_GLOW], 0);
}

/******************************************************************************/
/*!
\brief
Renders the menu buttons
*/
/******************************************************************************/
void MapScene::RenderTextButtons(void)
{
	for (unsigned i = 0; i < v_textButtonList.size(); ++i)
	{
		TextButton *S_MB = v_textButtonList[i];
		if ((S_MB->gamestate == MENU_STATE))
		{
			modelStack.PushMatrix();
			modelStack.Translate(S_MB->pos);
			modelStack.Scale(S_MB->scale);
			RenderTextOnScreen(P_meshArray[E_GEO_TEXT], S_MB->text, S_MB->color);
			modelStack.PopMatrix();
		}
	}
}

/******************************************************************************/
/*!
\brief
Renders the menu buttons
*/
/******************************************************************************/
void MapScene::RenderButtons(void)
{
	LuaScript button_tip("button");
	for (unsigned i = 0; i < v_buttonList.size(); ++i)
	{
		Button *m_B = v_buttonList[i];
		if ((m_B->gamestate == MENU_STATE))
		{
			modelStack.PushMatrix();
			modelStack.Translate(m_B->Position);

			modelStack.PushMatrix();
			modelStack.Scale(m_B->Scale);
			if (m_B->ID < BI_WALL)
			{
				RenderMeshOnScreen(m_B->mesh, 10.f, m_B->color);
			}
			else
			{
				if (m_B->active == false)
					RenderMeshOnScreen(m_B->mesh);
				else
					RenderMeshOnScreen(m_B->mesh, 5.f, m_B->color);
			}

			if (m_B->labeltype == Button::LT_BUTTON)
			{
				RenderTextCenterOnScreen(P_meshArray[E_GEO_TEXT], m_B->label, m_B->color);
			}
			modelStack.PopMatrix();

			std::string button_tipname = "editor_button.button_" + std::to_string(static_cast<unsigned long long>(m_B->ID)) + ".";

			if (m_B->active == true)
			{
				modelStack.PushMatrix();
				modelStack.Translate(Application::GetWindowWidth() * 0.03f, 0.0f, 0.0f);
				modelStack.Scale(m_B->Scale);
				RenderTextOnScreen(P_meshArray[E_GEO_TEXT], button_tip.get<std::string>(button_tipname + "text"), UIColor);
				modelStack.PopMatrix();
			}
			modelStack.PopMatrix();
		}
	}
}

/******************************************************************************/
/*!
\brief
Renders the scene
*/
/******************************************************************************/
void MapScene::Render()
{
	// Render VBO here
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (size_t i = 0; i < ui_NUM_LIGHTS; ++i)
	{
		if (P_lightsArray[i].type == LIGHT_DIRECTIONAL)
		{
			Vector3 lightDir(P_lightsArray[i].position.x, P_lightsArray[i].position.y, P_lightsArray[i].position.z);
			Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
			glUniform3fv(u_m_parameters[E_UNI_LIGHT0_POSITION + (i * ui_NUM_LIGHT_PARAMS)], 1, &lightDirection_cameraspace.x);
		}
		else if (P_lightsArray[i].type == LIGHT_SPOT)
		{
			Position lightPosition_cameraspace = viewStack.Top() * P_lightsArray[i].position;
			glUniform3fv(u_m_parameters[E_UNI_LIGHT0_POSITION + (i * ui_NUM_LIGHT_PARAMS)], 1, &lightPosition_cameraspace.x);
			Vector3 spotDirection_cameraspace = viewStack.Top() * P_lightsArray[i].spotDirection;
			glUniform3fv(u_m_parameters[E_UNI_LIGHT0_SPOTDIRECTION + (i * ui_NUM_LIGHT_PARAMS)], 1, &spotDirection_cameraspace.x);
		}
		else
		{
			Position lightPosition_cameraspace = viewStack.Top() * P_lightsArray[i].position;
			glUniform3fv(u_m_parameters[E_UNI_LIGHT0_POSITION + (i * ui_NUM_LIGHT_PARAMS)], 1, &lightPosition_cameraspace.x);
		}

		Position lightPosition_cameraspace = viewStack.Top() * P_lightsArray[i].position;
		glUniform3fv(u_m_parameters[E_UNI_LIGHT0_POSITION + (i * ui_NUM_LIGHT_PARAMS)], 1, &lightPosition_cameraspace.x);
	}

	viewStack.LoadIdentity();

	viewStack.LookAt(
		camera.position.x, camera.position.y, camera.position.z,
		camera.target.x, camera.target.y, camera.target.z,
		camera.up.x, camera.up.y, camera.up.z
		);

	modelStack.LoadIdentity();

	modelStack.PushMatrix();
	modelStack.Translate(-v3_2DCam);
	RenderGO();
	modelStack.PopMatrix();

	RenderUI();
	RenderTextButtons();
	RenderButtons();
}

/******************************************************************************/
/*!
\brief
Renders the user interface
*/
/******************************************************************************/
void MapScene::RenderUI()
{
	std::stringstream ss2;
	ss2 << "Tile:";
	modelStack.PushMatrix();
	modelStack.Translate(Application::GetWindowWidth() * 0.01f, Application::GetWindowHeight() * 0.975f - ML_map.worldSize, 0);
	modelStack.Scale(15, 15, 15);
	RenderTextOnScreen(P_meshArray[E_GEO_TEXT], ss2.str(), UIColor);
	modelStack.PopMatrix();

	if (selectedTile == BI_FLOOR)
	{
		modelStack.PushMatrix();
		modelStack.Translate(Application::GetWindowWidth() * 0.025f, Application::GetWindowHeight() * 0.975f - 50.f, 0);
		modelStack.Scale(18, 18, 18);
		RenderMeshOnScreen(P_meshArray[E_GEO_FLOOR_BORDER], 0, 0);
		modelStack.PopMatrix();
	}
	else if (selectedTile == BI_WALL)
	{
		modelStack.PushMatrix();
		modelStack.Translate(Application::GetWindowWidth() * 0.025f, Application::GetWindowHeight() * 0.975f - 50.f, 0);
		modelStack.Scale(18, 18, 18);
		RenderMeshOnScreen(P_meshArray[E_GEO_WALL_BORDER], 0, 0);
		modelStack.PopMatrix();
	}
	else if (selectedTile == BI_PLAYER)
	{
		modelStack.PushMatrix();
		modelStack.Translate(Application::GetWindowWidth() * 0.025f, Application::GetWindowHeight() * 0.975f - 50.f, 0);
		modelStack.Scale(18, 18, 18);
		RenderMeshOnScreen(P_meshArray[E_GEO_PLAYER_BORDER], 0, 0);
		modelStack.PopMatrix();
	}
	else if (selectedTile == BI_WIN)
	{
		modelStack.PushMatrix();
		modelStack.Translate(Application::GetWindowWidth() * 0.025f, Application::GetWindowHeight() * 0.975f - 50.f, 0);
		modelStack.Scale(18, 18, 18);
		RenderMeshOnScreen(P_meshArray[E_GEO_WIN_BORDER], 0, 0);
		modelStack.PopMatrix();
	}
	else if (selectedTile == BI_DANGER)
	{
		modelStack.PushMatrix();
		modelStack.Translate(Application::GetWindowWidth() * 0.025f, Application::GetWindowHeight() * 0.975f - 50.f, 0);
		modelStack.Scale(18, 18, 18);
		RenderMeshOnScreen(P_meshArray[E_GEO_DANGER_BORDER], 0, 0);
		modelStack.PopMatrix();
	}
	else if (selectedTile == BI_ENEMY)
	{
		modelStack.PushMatrix();
		modelStack.Translate(Application::GetWindowWidth() * 0.025f, Application::GetWindowHeight() * 0.975f - 50.f, 0);
		modelStack.Scale(18, 18, 18);
		RenderMeshOnScreen(P_meshArray[E_GEO_ENEMY_BORDER], 0, 0);
		modelStack.PopMatrix();
	}

	switch (MENU_STATE)
	{
	case MT_NEW:
	{
				   modelStack.PushMatrix();
				   modelStack.Translate(Application::GetWindowWidth() * 0.525f, Application::GetWindowHeight() * 0.5f, 0);
				   modelStack.Scale(Application::GetWindowWidth() * 0.3f, Application::GetWindowHeight() * 0.35f, 1);
				   RenderMeshOnScreen(P_meshArray[E_GEO_POPUP]);
				   modelStack.PopMatrix();

				   std::stringstream ss;
				   ss << "File Name: [" << newMapName << ".csv]";
				   modelStack.PushMatrix();
				   modelStack.Translate(Application::GetWindowWidth() * 0.525f, Application::GetWindowHeight() * 0.5f, 0);
				   modelStack.Scale(25, 25, 1);
				   RenderTextCenterOnScreen(P_meshArray[E_GEO_TEXT], ss.str(), UIColor);
				   modelStack.PopMatrix();
	}
		break;

	case MT_REPLACE:
	{
					   modelStack.PushMatrix();
					   modelStack.Translate(Application::GetWindowWidth() * 0.525f, Application::GetWindowHeight() * 0.5f, 0);
					   modelStack.Scale(Application::GetWindowWidth() * 0.3f, Application::GetWindowHeight() * 0.35f, 1);
					   RenderMeshOnScreen(P_meshArray[E_GEO_POPUP]);
					   modelStack.PopMatrix();

					   std::stringstream ss;
					   ss << "Replace [" << newMapName << ".csv]?";
					   modelStack.PushMatrix();
					   modelStack.Translate(Application::GetWindowWidth() * 0.525f, Application::GetWindowHeight() * 0.7f, 0);
					   modelStack.Scale(35, 35, 1);
					   RenderTextCenterOnScreen(P_meshArray[E_GEO_TEXT], ss.str(), UIColorPressed);
					   modelStack.PopMatrix();
	}
		break;

	case MT_DIFFICULTY:
	{
					   modelStack.PushMatrix();
					   modelStack.Translate(Application::GetWindowWidth() * 0.525f, Application::GetWindowHeight() * 0.5f, 0);
					   modelStack.Scale(Application::GetWindowWidth() * 0.3f, Application::GetWindowHeight() * 0.35f, 1);
					   RenderMeshOnScreen(P_meshArray[E_GEO_POPUP]);
					   modelStack.PopMatrix();

					   LuaScript buttonScript("button");

					   std::string buttonName = "editor_difficulty.";

					   modelStack.PushMatrix();
					   modelStack.Translate(Application::GetWindowWidth() * 0.525f, Application::GetWindowHeight() * buttonScript.get<float>(buttonName + "posY"), 0.1f);
				       modelStack.Scale(buttonScript.get<float>(buttonName + "scale"), buttonScript.get<float>(buttonName + "scale"), buttonScript.get<float>(buttonName + "scale"));
					   RenderTextCenterOnScreen(P_meshArray[E_GEO_TEXT], buttonScript.get<std::string>(buttonName + "text"), UIColorPressed);
					   modelStack.PopMatrix();
	}
		break;

	case MT_DELETE:
	{
					  modelStack.PushMatrix();
					  modelStack.Translate(Application::GetWindowWidth() * 0.525f, Application::GetWindowHeight() * 0.5f, 0);
					  modelStack.Scale(Application::GetWindowWidth() * 0.3f, Application::GetWindowHeight() * 0.35f, 1);
					  RenderMeshOnScreen(P_meshArray[E_GEO_POPUP]);
					  modelStack.PopMatrix();

					  LuaScript buttonScript("button");

					  std::string buttonName = "editor_button.button_4.";
					  std::string delete_name = buttonScript.get<std::string>(buttonName + "text_onscreen") + newMapName + buttonScript.get<std::string>(buttonName + "text_onscreen2");

					  modelStack.PushMatrix();
					  modelStack.Translate(Application::GetWindowWidth() * 0.525f, Application::GetWindowHeight() * buttonScript.get<float>(buttonName + "posY"), 0.1f);
					  modelStack.Scale(buttonScript.get<float>(buttonName + "scale"), buttonScript.get<float>(buttonName + "scale"), buttonScript.get<float>(buttonName + "scale"));
					  RenderTextCenterOnScreen(P_meshArray[E_GEO_TEXT], delete_name, UIColorPressed);
					  modelStack.PopMatrix();

					  if (delete_file == true)
					  {
						  buttonName = "editor_button.button_4.text_failed.";
						  delete_name = buttonScript.get<std::string>(buttonName + "text");

						  modelStack.PushMatrix();
						  modelStack.Translate(Application::GetWindowWidth() * 0.525f, Application::GetWindowHeight() * buttonScript.get<float>(buttonName + "posY"), 0.1f);
						  modelStack.Scale(buttonScript.get<float>(buttonName + "scale"), buttonScript.get<float>(buttonName + "scale"), buttonScript.get<float>(buttonName + "scale"));
						  RenderTextCenterOnScreen(P_meshArray[E_GEO_TEXT], delete_name, UIColorPressed);
						  modelStack.PopMatrix();
					  }
	}
		break;
		
	case MT_LOAD:
	{
					modelStack.PushMatrix();
					modelStack.Translate(Application::GetWindowWidth() * 0.525f, Application::GetWindowHeight() * 0.5f, 0);
					modelStack.Scale(Application::GetWindowWidth() * 0.3f, Application::GetWindowHeight() * 0.35f, 1);
					RenderMeshOnScreen(P_meshArray[E_GEO_POPUP]);
					modelStack.PopMatrix();

					LuaScript buttonScript("button");

					std::string buttonName = "editor_load.";

					modelStack.PushMatrix();
					modelStack.Translate(Application::GetWindowWidth()*(buttonScript.get<float>(buttonName + "posX") + 0.025f), Application::GetWindowHeight()*buttonScript.get<float>(buttonName + "posY"), 0.1f);
					modelStack.Scale(buttonScript.get<float>(buttonName + "scale"), buttonScript.get<float>(buttonName + "scale"), buttonScript.get<float>(buttonName + "scale"));
					RenderTextOnScreen(P_meshArray[E_GEO_TEXT], buttonScript.get<std::string>(buttonName + "text"), UIColor);
					modelStack.PopMatrix();

					std::stringstream ss3;
					ss3 << temp_total_string;
					modelStack.PushMatrix();
					modelStack.Translate(Application::GetWindowWidth()*(buttonScript.get<float>(buttonName + "posX") + 0.025f), Application::GetWindowHeight() * 0.65f, 0);
					modelStack.Scale(35, 35, 1);
					RenderTextOnScreen(P_meshArray[E_GEO_TEXT], ss3.str(), UIColorPressed);
					modelStack.PopMatrix();

					if (load_file == true)
					{
						buttonName = "editor_load.text_failed.";
						std::string button_name = buttonScript.get<std::string>(buttonName + "text");

						modelStack.PushMatrix();
						modelStack.Translate(Application::GetWindowWidth() * 0.525f, Application::GetWindowHeight() * buttonScript.get<float>(buttonName + "posY"), 0.1f);
						modelStack.Scale(buttonScript.get<float>(buttonName + "scale"), buttonScript.get<float>(buttonName + "scale"), buttonScript.get<float>(buttonName + "scale"));
						RenderTextCenterOnScreen(P_meshArray[E_GEO_TEXT], button_name, UIColorPressed);
						modelStack.PopMatrix();
					}
	}
		break;
	}

	//AI Status
	for (unsigned i = 0; i < GO_List.size(); ++i)
	{
		CharacterObject *CO = dynamic_cast<CharacterObject*>(GO_List[i]);
		if (CO != NULL)
		{
			//
		}
	}
}

/******************************************************************************/
/*!
\brief
Renders the all gameobjects
*/
/******************************************************************************/
void MapScene::RenderGO()
{
	//Temporary bandaid solution (render CO last)

	for (unsigned i = 0; i < GO_List.size(); i++)
	{
		CharacterObject *CO = dynamic_cast<CharacterObject*>(GO_List[i]);

		if (GO_List[i]->active)
		{
			if (CO == NULL)
			{
				modelStack.PushMatrix();
				modelStack.Translate(GO_List[i]->position);
				modelStack.Rotate(GO_List[i]->rotation, 0, 0, 1);
				modelStack.Scale(GO_List[i]->scale);
				RenderMeshOnScreen(GO_List[i]->mesh);
				modelStack.PopMatrix();
			}
		}
	}

	for (unsigned i = 0; i < GO_List.size(); i++)
	{
		CharacterObject *CO = dynamic_cast<CharacterObject*>(GO_List[i]);

		if (GO_List[i]->active)
		{
			if (CO != NULL)
			{
				modelStack.PushMatrix();
				modelStack.Translate(CO->position);
				modelStack.Rotate(CO->rotation, 0, 0, 1);
				modelStack.Scale(CO->scale);
				RenderMeshOnScreen(CO->mesh);
				modelStack.PopMatrix();

				if (CO->Holding != NULL)
				{
					modelStack.PushMatrix();
					modelStack.Translate(CO->position);
					modelStack.Rotate(CO->rotation, 0, 0, 1);
					modelStack.Translate(0, ML_map.worldSize*0.5f, 0);
					modelStack.Scale((CO->Holding->scale));
					RenderMeshOnScreen(CO->Holding->mesh);
					modelStack.PopMatrix();
				}
			}
		}
		else if (CO != NULL)
		{
			modelStack.PushMatrix();
			modelStack.Translate(Application::GetWindowWidth() * 0.025f, Application::GetWindowHeight() * 0.975f - 20.f, 0);
			modelStack.Scale(20, 20, 20);
			RenderTextOnScreen(P_meshArray[E_GEO_TEXT], CO->getState(), UIColor);
			modelStack.PopMatrix();
		}
	}
}

void MapScene::CO_attach(CharacterObject *CO, GameObject *GO)
{
	GO->active = false;
	CO->Holding = GO;
}

void MapScene::CO_drop(CharacterObject *CO)
{
	CO->Holding->position = CO->position;
	CO->Holding->active = true;
	CO->Holding = NULL;
}

/******************************************************************************/
/*!
\brief
Clears memory upon exit
*/
/******************************************************************************/
void MapScene::Exit()
{
	SE_Engine.Exit();

	while (GO_List.size() > 0)
	{
		GameObject *GO = GO_List.back();
		if (GO != NULL)
		{
			delete GO;
			GO = NULL;
		}
		GO_List.pop_back();
	}

	while (v_textButtonList.size() > 0)
	{
		TextButton *S_MB = v_textButtonList.back();
		if (S_MB != NULL)
		{
			delete S_MB;
		}
		S_MB = NULL;
		v_textButtonList.pop_back();
	}

	while (v_buttonList.size() > 0)
	{
		Button *m_B = v_buttonList.back();
		if (m_B != NULL)
		{
			delete m_B;
		}
		m_B = NULL;
		v_buttonList.pop_back();
	}

	for (unsigned i = 0; i < E_GEO_TOTAL; ++i)
	{
		if (P_meshArray[i] != NULL)
		{
			delete P_meshArray[i];
		}
		P_meshArray[i] = NULL;
	}
}

/******************************************************************************/
/*!
\brief
Clears shaders upon exit
*/
/******************************************************************************/
void MapScene::CleanShaders(void)
{
	glDeleteVertexArrays(1, &u_m_vertexArrayID);
	glDeleteProgram(u_m_programID);
}

MapScene* MapScene::GetInstance()
{
	if (MapScene::Instance == NULL)
	{
		MapScene::Instance = new MapScene();
	}

	return MapScene::Instance;
}

void MapScene::Destroy()
{
	if (MapScene::Instance != NULL)
	{
		delete MapScene::Instance;
		MapScene::Instance = NULL;
	}
	//std::remove(newMapName);
}