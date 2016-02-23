/****************************************************************************/
/*!
\file MapScene.cpp
\author Gabriel Wong Choon Jieh
\par email: AuraTigital\@gmail.com
\brief
Main menu for the openGL framework
!*/
/****************************************************************************/
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

const unsigned int MapScene::ui_NUM_LIGHT_PARAMS = MapScene::E_UNI_LIGHT0_EXPONENT - (MapScene::E_UNI_LIGHT0_POSITION - 1/*Minus the enum before this*/);
/******************************************************************************/
/*!
\brief
Default Constructor
*/
/******************************************************************************/
MapScene::MapScene() : ui_NUM_LIGHTS(1)
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

	selectedTile = 0;
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

	LuaScript sound("Sound");
	SoundList[ST_BUTTON_CLICK] = SE_Engine.preloadSound(sound.getGameData("sound.button_click").c_str());
	SoundList[ST_BUTTON_CLICK_2] = SE_Engine.preloadSound(sound.getGameData("sound.button_click2").c_str());

	LEVEL = 2;
	InitSimulation();
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

	P_meshArray[E_GEO_BUTTON_LEFT] = MeshBuilder::GenerateQuad("Button Texture", Color(0.f, 0.f, 0.f), 1.f, 1.f, 1.0f);
	P_meshArray[E_GEO_BUTTON_LEFT]->textureID[0] = LoadTGA(script.getGameData("image.button.left").c_str(), true);

	P_meshArray[E_GEO_BUTTON_RIGHT] = MeshBuilder::GenerateQuad("Button Texture", Color(0.f, 0.f, 0.f), 1.f, 1.f, 1.0f);
	P_meshArray[E_GEO_BUTTON_RIGHT]->textureID[0] = LoadTGA(script.getGameData("image.button.right").c_str(), true);

	P_meshArray[E_GEO_BUTTON_REFRESH] = MeshBuilder::GenerateQuad("Button Texture", Color(0.f, 0.f, 0.f), 1.f, 1.f, 1.0f);
	P_meshArray[E_GEO_BUTTON_REFRESH]->textureID[0] = LoadTGA(script.getGameData("image.button.refresh").c_str(), true);

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

	P_meshArray[E_GEO_SAVE] = MeshBuilder::GenerateQuad("Save button", Color(0.f, 0.f, 0.f), 1.f, 1.f, 1.0f);
	P_meshArray[E_GEO_SAVE]->textureID[0] = LoadTGA(script.getGameData("image.button.Save").c_str(), true);

	//World
	P_meshArray[E_GEO_FLOOR_1] = MeshBuilder::GenerateQuad("Floor Texture", Color(0.f, 0.f, 0.f), 1.f, 1.f, 1.0f);
	P_meshArray[E_GEO_FLOOR_1]->textureID[0] = LoadTGA(script.getGameData("image.tile.floor").c_str(), true);

	P_meshArray[E_GEO_FLOOR_2] = MeshBuilder::GenerateQuad("Floor Texture_White", Color(0.f, 0.f, 0.f), 1.f, 1.f, 1.0f);
	P_meshArray[E_GEO_FLOOR_2]->textureID[0] = LoadTGA(script.getGameData("image.tile.floor_2").c_str(), true);

	P_meshArray[E_GEO_WALL_1] = MeshBuilder::GenerateQuad("Wall Texture", Color(0.f, 0.f, 0.f), 1.f, 1.f, 1.0f);
	P_meshArray[E_GEO_WALL_1]->textureID[0] = LoadTGA(script.getGameData("image.tile.wall").c_str(), true);

	P_meshArray[E_GEO_PLAYER] = MeshBuilder::GenerateQuad("AI enemy", Color(0.f, 0.f, 0.f), 1.f, 1.f, 1.0f);
	P_meshArray[E_GEO_PLAYER]->textureID[0] = LoadTGA(script.getGameData("image.tile.enemy").c_str(), true);

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

	MENU_STATE = 1;

	/*S_MB = new TextButton;
	S_MB->pos.Set(Application::GetWindowWidth()*0.08f, Application::GetWindowHeight()*0.05f, 0.1f);
	S_MB->scale.Set(22, 22, 22);
	S_MB->text = "Restart";
	S_MB->gamestate = 1;
	v_textButtonList.push_back(S_MB);*/

	Button *m_B;
	m_B = new Button;
	m_B->Position.Set(Application::GetWindowWidth()* 0.98f, Application::GetWindowHeight()*0.05f, 0.1f);
	m_B->Scale.Set(18, 18, 18);
	m_B->mesh = P_meshArray[E_GEO_BUTTON_LEFT];
	m_B->ID = BI_BACK;
	m_B->labeltype = Button::LT_NONE;
	m_B->gamestate = 1;
	v_buttonList.push_back(m_B);

	//m_B = new Button;
	//m_B->Position.Set(Application::GetWindowWidth()*0.97f - 50.f, Application::GetWindowHeight()*0.95f, 0.1f);
	//m_B->Scale.Set(20, 20, 20);
	//m_B->mesh = P_meshArray[E_GEO_BUTTON_LEFT];
	//m_B->ID = BI_PREV_MAP;
	//m_B->labeltype = Button::LT_NONE;
	//m_B->gamestate = 1;
	//v_buttonList.push_back(m_B);
	//m_B = new Button;
	//m_B->Position.Set(Application::GetWindowWidth()*0.97f, Application::GetWindowHeight()*0.95f, 0.1f);
	//m_B->Scale.Set(20, 20, 20);
	//m_B->mesh = P_meshArray[E_GEO_BUTTON_RIGHT];
	//m_B->ID = BI_NEXT_MAP;
	//m_B->labeltype = Button::LT_NONE;
	//m_B->gamestate = 1;
	//v_buttonList.push_back(m_B);

	m_B = new Button;
	m_B->Position.Set(Application::GetWindowWidth()*0.98f, Application::GetWindowHeight()*0.05f + 40, 0.1f);
	m_B->Scale.Set(20, 20, 20);
	m_B->mesh = P_meshArray[E_GEO_SAVE];
	m_B->ID = BI_SAVE;
	m_B->labeltype = Button::LT_NONE;
	m_B->gamestate = 1;
	v_buttonList.push_back(m_B);

	//m_B = new Button;
	//m_B->Position.Set(Application::GetWindowWidth()*0.03f + 30.f, Application::GetWindowHeight()*0.85f, 0.1f);
	//m_B->Scale.Set(30, 30, 30);
	//m_B->mesh = P_meshArray[E_GEO_FLOOR_BORDER];
	//m_B->ID = BI_FLOOR;
	//m_B->labeltype = Button::LT_NONE;
	//m_B->gamestate = 1;
	//v_buttonList.push_back(m_B);
	//m_B = new Button;
	//m_B->Position.Set(Application::GetWindowWidth()*0.03f + 30.f, Application::GetWindowHeight()*0.70f, 0.1f);
	//m_B->Scale.Set(30, 30, 30);
	//m_B->mesh = P_meshArray[E_GEO_WALL_BORDER];
	//m_B->ID = BI_WALL;
	//m_B->labeltype = Button::LT_NONE;
	//m_B->gamestate = 1;
	//v_buttonList.push_back(m_B);
	//m_B = new Button;
	//m_B->Position.Set(Application::GetWindowWidth()*0.03f + 30.f, Application::GetWindowHeight()*0.55f, 0.1f);
	//m_B->Scale.Set(30, 30, 30);
	//m_B->mesh = P_meshArray[E_GEO_PLAYER_BORDER];
	//m_B->ID = BI_PLAYER;
	//m_B->labeltype = Button::LT_NONE;
	//m_B->gamestate = 1;
	//v_buttonList.push_back(m_B);
	//m_B = new Button;
	//m_B->Position.Set(Application::GetWindowWidth()*0.03f + 30.f, Application::GetWindowHeight()*0.40f, 0.1f);
	//m_B->Scale.Set(30, 30, 30);
	//m_B->mesh = P_meshArray[E_GEO_ENEMY_BORDER];
	//m_B->ID = BI_ENEMY;
	//m_B->labeltype = Button::LT_NONE;
	//m_B->gamestate = 1;
	//v_buttonList.push_back(m_B);
	//m_B = new Button;
	//m_B->Position.Set(Application::GetWindowWidth()*0.03f + 30.f, Application::GetWindowHeight()*0.25f, 0.1f);
	//m_B->Scale.Set(30, 30, 30);
	//m_B->mesh = P_meshArray[E_GEO_DANGER_BORDER];
	//m_B->ID = BI_DANGER;
	//m_B->labeltype = Button::LT_NONE;
	//m_B->gamestate = 1;
	//v_buttonList.push_back(m_B);
}

/******************************************************************************/
/*!
\brief
Initializes simulation
*/
/******************************************************************************/
bool MapScene::InitSimulation(void)
{
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

	std::stringstream ss;
	ss << "GameData//Maps//temp_file.csv";
	ML_map.saveMap(ss.str());
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

	LuaScript buttonScript("button");

	//for (unsigned y = ML_map.map_height - 1; y > 0; --y)
	//{
	//	for (unsigned x = 0; x < ML_map.map_width; ++x)
	//	{
	//		if (ML_map.map_data[y][x] != "1")
	//		{
	//			GameObject *GO;
	//			GO = new GameObject();
	//			GO->Init(Vector3(x*ML_map.worldSize*2.f, (ML_map.map_height - y)*ML_map.worldSize*2.f, -0.5f));
	//			GO->scale.Set(ML_map.worldSize, ML_map.worldSize, 1);
	//			GO->enableCollision = false;
	//			GO->mesh = P_meshArray[E_GEO_FLOOR_2];
	//			GO_List.push_back(GO);

	//			if (ML_map.map_data[y][x] == "P")
	//			{
	//				GameObject *GO;
	//				GO = new GameObject();
	//				GO->Init(Vector3(x*ML_map.worldSize*2.f, (ML_map.map_height - y)*ML_map.worldSize*2.f, -0.5f));
	//				GO->scale.Set(ML_map.worldSize, ML_map.worldSize, 1);
	//				GO->enableCollision = false;
	//				GO->mesh = P_meshArray[E_GEO_PLAYER];
	//				GO_List.push_back(GO);
	//			}
	//			continue;
	//		}

	//		else
	//		{
	//			GameObject *GO;
	//			GO = new GameObject();
	//			GO->Init(Vector3(x*ML_map.worldSize*2.f, (ML_map.map_height - y)*ML_map.worldSize*2.f, -0.5f));
	//			GO->scale.Set(ML_map.worldSize, ML_map.worldSize, 1);
	//			GO->enableCollision = false;
	//			GO->mesh = P_meshArray[E_GEO_WALL_1];
	//			GO_List.push_back(GO);
	//		}
	//	}
	//}

	v3_2DCam.x = -static_cast<float>(Application::GetWindowWidth()) * 0.5f;
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
		Worldpos.x = static_cast<int>(Worldpos.x / ((ML_map.worldSize) * 2.f));
		Worldpos.y = static_cast<int>((Worldpos.y) / ((ML_map.worldSize) * 2.f));
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
				S_MB->color = UIColorPressed;
			}
			else
			{
				S_MB->active = false;
				S_MB->color = UIColor;
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
	dt *= static_cast<double>(i_SimulationSpeed);
	f_timer += static_cast<float>(dt);
	//Mouse Section
	double x, y;
	Application::GetMousePos(x, y);
	MousePosX = static_cast<float>(x) / Application::GetWindowWidth() * Application::GetWindowWidth();
	MousePosY = (Application::GetWindowHeight() - static_cast<float>(y)) / Application::GetWindowHeight() * Application::GetWindowHeight();

	static bool bLButtonState = false;

	UpdateTextButtons();
	UpdateButtons();

	if (Application::IsKeyPressed('P'))
	{
		std::cout << "Map Name: ";
		std::cin >> newMapName;

		std::stringstream ss;
		ss << "GameData//Maps//" << newMapName << ".csv";
		ML_map.saveMap(ss.str());
	}
	//select Tiles
	if (Application::IsKeyPressed('1'))
		selectedTile = 0;
	if (Application::IsKeyPressed('2'))
		selectedTile = 1;
	if (Application::IsKeyPressed('3'))
		selectedTile = 2;
	if (Application::IsKeyPressed('4'))
		selectedTile = 3;
	if (Application::IsKeyPressed('5'))
		selectedTile = 4;
	if (Application::IsKeyPressed('6'))
		selectedTile = 5;

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
			std::cout << "Unable to remove" << std::endl;
		}
		return;
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
		else if (FetchBUTTON(BI_SAVE)->active)
		{
			cout << "New Map: " << endl;
			cin >> newMapName;

			std::stringstream ss;
			ss << "GameData//Maps//" << newMapName << ".csv";
			ML_map.saveMap(ss.str());
		}
		//else if (FetchBUTTON(BI_PREV_MAP)->active)
		//{
		//	if (i_SimulationSpeed > 1)
		//	{
		//		--i_SimulationSpeed;
		//	}
		//}
		//else if (FetchBUTTON(BI_NEXT_MAP)->active)
		//{
		//	if (i_SimulationSpeed < 5)
		//	{
		//		++i_SimulationSpeed;
		//	}
		//}
	}

	if (bLButtonState)
		placeTile(selectedTile);

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
	selTilePos = calTilePos(Vector3(MousePosX, MousePosY));
	selTilePos.x -= 8;
	selTilePos.y += 2;
	
	//cout << selTilePos << endl;
	//cout << selWorldPos << endl;
	if (selTilePos.x < ML_map.map_width && selTilePos.x > -1 && selTilePos.y < ML_map.map_height && selTilePos.y > -1)
	{
		GO->Init(Vector3(selTilePos.x*ML_map.worldSize*2.f, (ML_map.map_height - selTilePos.y)*ML_map.worldSize*2.f, -0.5f));
		GO->scale.Set(ML_map.worldSize, ML_map.worldSize, 1);

		switch (selectedTile)
		{
		case 0:
			GO->mesh = MapScene::GetInstance()->P_meshArray[MapScene::E_GEO_FLOOR_BORDER];
			ML_map.map_data[selTilePos.y][selTilePos.x] = "0";
			break;
		case 1:
			GO->mesh = MapScene::GetInstance()->P_meshArray[MapScene::E_GEO_WALL_BORDER];
			ML_map.map_data[selTilePos.y][selTilePos.x] = "1";
			break;
		case 2:
			GO->mesh = MapScene::GetInstance()->P_meshArray[MapScene::E_GEO_PLAYER_BORDER];
			ML_map.map_data[selTilePos.y][selTilePos.x] = "P";
			break;
		case 3:
			GO->mesh = MapScene::GetInstance()->P_meshArray[MapScene::E_GEO_WIN_BORDER];
			ML_map.map_data[selTilePos.y][selTilePos.x] = "3";
			break;
		case 4:
			GO->mesh = MapScene::GetInstance()->P_meshArray[MapScene::E_GEO_DANGER_BORDER];
			ML_map.map_data[selTilePos.y][selTilePos.x] = "2";
			break;
		case 5:
			GO->mesh = MapScene::GetInstance()->P_meshArray[MapScene::E_GEO_ENEMY_BORDER];
			ML_map.map_data[selTilePos.y][selTilePos.x] = std::to_string(enemyID);
			enemyID++;
		}
		MapScene::GetInstance()->GO_List.push_back(GO);
	}
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
	for (unsigned i = 0; i < v_buttonList.size(); ++i)
	{
		Button *m_B = v_buttonList[i];
		if ((m_B->gamestate == MENU_STATE))
		{
			modelStack.PushMatrix();
			modelStack.Translate(m_B->Position);
			modelStack.Scale(m_B->Scale);
			RenderMeshOnScreen(m_B->mesh);

			if (m_B->labeltype == Button::LT_BUTTON)
			{
				RenderTextCenterOnScreen(P_meshArray[E_GEO_TEXT], m_B->label, m_B->color);
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
	std::stringstream ss;
	ss << "RunTime " << f_timer;
	modelStack.PushMatrix();
	modelStack.Translate(Application::GetWindowWidth() * 0.025f, Application::GetWindowHeight() * 0.9815f, 0);
	modelStack.Scale(15, 15, 15);
	RenderTextOnScreen(P_meshArray[E_GEO_TEXT], ss.str(), UIColor);
	modelStack.PopMatrix();

	std::stringstream ss2;
	ss2 << "Tile:";
	modelStack.PushMatrix();
	modelStack.Translate(Application::GetWindowWidth() * 0.96f, Application::GetWindowHeight() * 0.975f - 80.f, 0);
	modelStack.Scale(10, 10, 10);
	RenderTextOnScreen(P_meshArray[E_GEO_TEXT], ss2.str(), UIColor);
	modelStack.PopMatrix();

	if (selectedTile == 0)
	{
		modelStack.PushMatrix();
		modelStack.Translate(Application::GetWindowWidth() * 0.98f, Application::GetWindowHeight() * 0.975f - 50.f, 0);
		modelStack.Scale(18, 18, 18);
		RenderMeshOnScreen(P_meshArray[E_GEO_FLOOR_BORDER], 0, 0);
		modelStack.PopMatrix();
	}
	else if (selectedTile == 1)
	{
		modelStack.PushMatrix();
		modelStack.Translate(Application::GetWindowWidth() * 0.98f, Application::GetWindowHeight() * 0.975f - 50.f, 0);
		modelStack.Scale(18, 18, 18);
		RenderMeshOnScreen(P_meshArray[E_GEO_WALL_BORDER], 0, 0);
		modelStack.PopMatrix();
	}
	else if (selectedTile == 2)
	{
		modelStack.PushMatrix();
		modelStack.Translate(Application::GetWindowWidth() * 0.98f, Application::GetWindowHeight() * 0.975f - 50.f, 0);
		modelStack.Scale(18, 18, 18);
		RenderMeshOnScreen(P_meshArray[E_GEO_PLAYER_BORDER], 0, 0);
		modelStack.PopMatrix();
	}
	else if (selectedTile == 3)
	{
		modelStack.PushMatrix();
		modelStack.Translate(Application::GetWindowWidth() * 0.98f, Application::GetWindowHeight() * 0.975f - 50.f, 0);
		modelStack.Scale(18, 18, 18);
		RenderMeshOnScreen(P_meshArray[E_GEO_WIN_BORDER], 0, 0);
		modelStack.PopMatrix();
	}
	else if (selectedTile == 4)
	{
		modelStack.PushMatrix();
		modelStack.Translate(Application::GetWindowWidth() * 0.98f, Application::GetWindowHeight() * 0.975f - 50.f, 0);
		modelStack.Scale(18, 18, 18);
		RenderMeshOnScreen(P_meshArray[E_GEO_DANGER_BORDER], 0, 0);
		modelStack.PopMatrix();
	}
	else if (selectedTile == 5)
	{
		modelStack.PushMatrix();
		modelStack.Translate(Application::GetWindowWidth() * 0.98f, Application::GetWindowHeight() * 0.975f - 50.f, 0);
		modelStack.Scale(18, 18, 18);
		RenderMeshOnScreen(P_meshArray[E_GEO_ENEMY_BORDER], 0, 0);
		modelStack.PopMatrix();
	}

	/*std::stringstream ss3;
	ss3 << "2";
	modelStack.PushMatrix();
	modelStack.Translate(Application::GetWindowWidth() * 0.98f, Application::GetWindowHeight() * 0.975f - 240.f, 0);
	modelStack.Scale(20, 20, 20);
	RenderTextOnScreen(P_meshArray[E_GEO_TEXT], ss3.str(), UIColor);
	modelStack.PopMatrix();

	std::stringstream ss4;
	ss4 << "3";
	modelStack.PushMatrix();
	modelStack.Translate(Application::GetWindowWidth() * 0.98f, Application::GetWindowHeight() * 0.975f - 350.f, 0);
	modelStack.Scale(20, 20, 20);
	RenderTextOnScreen(P_meshArray[E_GEO_TEXT], ss4.str(), UIColor);
	modelStack.PopMatrix();

	std::stringstream ss5;
	ss5 << "4";
	modelStack.PushMatrix();
	modelStack.Translate(Application::GetWindowWidth() * 0.98f, Application::GetWindowHeight() * 0.975f - 460.f, 0);
	modelStack.Scale(20, 20, 20);
	RenderTextOnScreen(P_meshArray[E_GEO_TEXT], ss5.str(), UIColor);
	modelStack.PopMatrix();

	std::stringstream ss6;
	ss6 << "5";
	modelStack.PushMatrix();
	modelStack.Translate(Application::GetWindowWidth() * 0.98f, Application::GetWindowHeight() * 0.975f - 570.f, 0);
	modelStack.Scale(20, 20, 20);
	RenderTextOnScreen(P_meshArray[E_GEO_TEXT], ss6.str(), UIColor);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(Application::GetWindowWidth() * 0.98f, Application::GetWindowHeight() * 0.975f - 100.f, 0);
	modelStack.Scale(18, 18, 18);
	RenderMeshOnScreen(P_meshArray[E_GEO_FLOOR_BORDER], 0, 0);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(Application::GetWindowWidth() * 0.98f, Application::GetWindowHeight() * 0.975f - 210.f, 0);
	modelStack.Scale(18, 18, 18);
	RenderMeshOnScreen(P_meshArray[E_GEO_WALL_BORDER], 0, 0);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(Application::GetWindowWidth() * 0.98f, Application::GetWindowHeight() * 0.975f - 320.f, 0);
	modelStack.Scale(18, 18, 18);
	RenderMeshOnScreen(P_meshArray[E_GEO_PLAYER_BORDER], 0, 0);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(Application::GetWindowWidth() * 0.98f, Application::GetWindowHeight() * 0.975f - 430.f, 0);
	modelStack.Scale(18, 18, 18);
	RenderMeshOnScreen(P_meshArray[E_GEO_ENEMY_BORDER], 0, 0);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(Application::GetWindowWidth() * 0.98f, Application::GetWindowHeight() * 0.975f - 540.f, 0);
	modelStack.Scale(18, 18, 18);
	RenderMeshOnScreen(P_meshArray[E_GEO_DANGER_BORDER], 0, 0);
	modelStack.PopMatrix();*/

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