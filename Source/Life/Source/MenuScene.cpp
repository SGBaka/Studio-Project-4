
#include "MenuScene.h"
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
//#include <vld.h>

const unsigned int MenuScene::ui_NUM_LIGHT_PARAMS = MenuScene::E_UNI_LIGHT0_EXPONENT - (MenuScene::E_UNI_LIGHT0_POSITION - 1/*Minus the enum before this*/);
/******************************************************************************/
/*!
\brief
Default Constructor
*/
/******************************************************************************/
MenuScene::MenuScene() : ui_NUM_LIGHTS(1)
{

}

/******************************************************************************/
/*!
\brief
Destructor
*/
/******************************************************************************/
MenuScene::~MenuScene()
{

}

/******************************************************************************/
/*!
\brief
Assigns file to values
*/
/******************************************************************************/
void MenuScene::assignsave(bool save)
{
	SH_1.assign(f_fov, 70.f, 1, save);
	if (save)
	{
		f_mouseSensitivity *= 100.f;
	}

	SH_1.assign(f_mouseSensitivity, 100.f, 2, save);
	f_mouseSensitivity *= 0.01f;
	SH_1.assign(us_control[E_CTRL_MOVE_FRONT], 'W', 3, save);
	SH_1.assign(us_control[E_CTRL_MOVE_BACK], 'S', 4, save);
	SH_1.assign(us_control[E_CTRL_MOVE_LEFT], 'A', 5, save);
	SH_1.assign(us_control[E_CTRL_MOVE_RIGHT], 'D', 6, save);
	SH_1.assign(us_control[E_CTRL_MOVE_SPRINT], VK_SHIFT, 7, save);
	SH_1.assign(us_control[E_CTRL_MOVE_WALK], VK_CONTROL, 8, save);
	SH_1.assign(us_control[E_CTRL_MOVE_JUMP], VK_SPACE, 9, save);
	SH_1.assign(us_control[E_CTRL_INTERACT], 'E', 10, save);
	SH_1.assign(us_control[E_CTRL_THROW], VK_RBUTTON, 11, save);
	SH_1.assign(us_control[E_CTRL_ATTACK], VK_LBUTTON, 12, save);
	SH_1.assign(us_control[E_CTRL_AIM], VK_MBUTTON, 13, save);
	SH_1.assign(us_control[E_CTRL_ABILITY_1], 'V', 14, save);
	SH_1.assign(us_control[E_CTRL_ABILITY_2], 'B', 15, save);
	SH_1.assign(us_control[E_CTRL_CAMERA_TOGGLE], 'C', 16, save);
	SH_1.assign(Graphics, GRA_MAX, 17, save);
	SH_1.assign(currentLevel, 1, 18, save);

	SH_1.saveData();
}

/******************************************************************************/
/*!
\brief
Initialize default variables, create meshes, lighting
*/
/******************************************************************************/
void MenuScene::Init()
{
	SE_Engine.Init();
	f_fov = 0.f;
	f_mouseSensitivity = 0.f;
	Graphics = GRA_MAX;

	for (size_t i = 0; i < E_CTRL_TOTAL; ++i)
	{
		us_control[i] = 0;
	}

	LuaScript scriptSH("GameData");
	SH_1.init(scriptSH.getGameData("gamedata.gamedata").c_str());
	loader.init("Lua//save.txt");
	assignsave(false);

	InitMeshList();
	//Starting position of translations and initialize physics

	// Init Camera
	camera.Init(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, -100.0f), Vector3(0.0f, 1.0f, 0.0f));

	f_timer = 0.f;
	MousePosX = 0.f;
	MousePosY = 0.f;
	v3_MenuCam.SetZero();
	InitMenu();

	f_fov_target = f_fov;

	switch (CUR_MENU)
	{
	case MenuScene::MT_MAIN_MENU:
		MENU_STATE = E_M_MAIN;
		break;
	case MenuScene::MT_MAIN_MENU_SPLASH:
		MENU_STATE = E_M_SPLASH;
		break;
	case MenuScene::MT_MAIN_MENU_SELECTION:
		MENU_STATE = E_M_SELECTION;
		break;
	case MenuScene::MT_MAIN_MENU_GAMEMODE:
		MENU_STATE = E_M_GAMEMODE;
		break;
	case MenuScene::MT_MAIN_MENU_OPTION:
		MENU_STATE = E_M_OPTIONS;
		break;
	case MenuScene::MT_MAIN_MENU_FIND:
		MENU_STATE = E_M_FIND;
		break;
	case MenuScene::MT_MAIN_MENU_SKIP:
		MENU_STATE = E_M_SKIP;
		break;
	case MenuScene::MT_PAUSE_MENU:
		MENU_STATE = E_M_OPTIONS;
		break;
	case MenuScene::MT_END_MENU:
		MENU_STATE = E_M_END;
		break;
	case MenuScene::MT_SONAR:
		MENU_STATE = E_M_SONAR;
		break;
	case MenuScene::MT_ENEMY:
		MENU_STATE = E_M_ENEMY;
		break;
	case MenuScene::MT_ZONE:
		MENU_STATE = E_M_ZONE;
		break;
	default:
		break;
	}
	LuaScript fileLoc("GameData");
	OutputFolder = ExePath() + "\\life\\" + fileLoc.get<string>("file_directory");
	file_Directory = fileLoc.get<string>("file_directory") + "\\";
	temp_total_string = "";

	LuaScript sound("Sound");
	SoundList[ST_BUTTON_CLICK] = SE_Engine.preloadSound(sound.getGameData("sound.button_click").c_str());
	SoundList[ST_BUTTON_CLICK_2] = SE_Engine.preloadSound(sound.getGameData("sound.button_click2").c_str());
	SoundList[ST_BGM] = SE_Engine.preloadSound(sound.getGameData("sound.backgroundO").c_str());

	file_found_timer = 0.0f;
}

/******************************************************************************/
/*!
\brief
Sets the menu type
*/
/******************************************************************************/
void MenuScene::setMenu(MENU_TYPE MT)
{
	CUR_MENU = MT;
}

/******************************************************************************/
/*!
\brief
Initialize shaders
*/
/******************************************************************************/
void MenuScene::InitShaders()
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
void MenuScene::InitMeshList()
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

	P_meshArray[E_GEO_BACKGROUND_MAIN] = MeshBuilder::GenerateQuad("Background_Main", Color(1.0f, 1.0f, 1.0f), static_cast<float>(Application::GetWindowWidth() / 2), static_cast<float>(Application::GetWindowHeight() / 2), 1.0f);
	P_meshArray[E_GEO_BACKGROUND_MAIN]->textureID[0] = LoadTGA(script.getGameData("image.background.background_main").c_str(), true, false);

	P_meshArray[E_GEO_BACKGROUND_OPTION] = MeshBuilder::GenerateQuad("Background_Option", Color(1.0f, 1.0f, 1.0f), static_cast<float>(Application::GetWindowWidth() / 2), static_cast<float>(Application::GetWindowHeight() / 2), 1.0f);
	P_meshArray[E_GEO_BACKGROUND_OPTION]->textureID[0] = LoadTGA(script.getGameData("image.background.background_option").c_str(), true, false);

	P_meshArray[E_GEO_BACKGROUND_CREDIT] = MeshBuilder::GenerateQuad("Background_Credit", Color(1.0f, 1.0f, 1.0f), static_cast<float>(Application::GetWindowWidth() / 2), static_cast<float>(Application::GetWindowHeight() / 2), 1.0f);
	P_meshArray[E_GEO_BACKGROUND_CREDIT]->textureID[0] = LoadTGA(script.getGameData("image.background.background_credit").c_str(), true, false);

	P_meshArray[E_GEO_BACKGROUND_TUTORIAL] = MeshBuilder::GenerateQuad("Background_Tutorial", Color(1.0f, 1.0f, 1.0f), static_cast<float>(Application::GetWindowWidth() / 2), static_cast<float>(Application::GetWindowHeight() / 2), 1.0f);
	P_meshArray[E_GEO_BACKGROUND_TUTORIAL]->textureID[0] = LoadTGA(script.getGameData("image.background.background_tutorial").c_str(), true, false);

	P_meshArray[E_GEO_BACKGROUND_END] = MeshBuilder::GenerateQuad("Background_End", Color(1.0f, 1.0f, 1.0f), static_cast<float>(Application::GetWindowWidth() / 2), static_cast<float>(Application::GetWindowHeight() / 2), 1.0f);
	P_meshArray[E_GEO_BACKGROUND_END]->textureID[0] = LoadTGA(script.getGameData("image.background.background_end").c_str(), true, false);

	P_meshArray[E_GEO_SPLASH] = MeshBuilder::GenerateQuad("Splash", Color(1.0f, 1.0f, 1.0f), static_cast<float>(Application::GetWindowWidth() / 2), static_cast<float>(Application::GetWindowHeight() / 2), 1.0f);
	P_meshArray[E_GEO_SPLASH]->textureID[0] = LoadTGA(script.getGameData("image.background.splash").c_str(), true);

	P_meshArray[E_GEO_LOADING_BACKGROUND] = MeshBuilder::GenerateQuad("Loading Screen", Color(1.0f, 1.0f, 1.0f), static_cast<float>(Application::GetWindowWidth() / 2), static_cast<float>(Application::GetWindowHeight() / 2), 1.0f);
	P_meshArray[E_GEO_LOADING_BACKGROUND]->textureID[0] = LoadTGA(script.getGameData("image.background.load").c_str(), true);

	P_meshArray[E_GEO_BUTTON_BACK] = MeshBuilder::GenerateQuad("Button Texture", Color(0.f, 0.f, 0.f), 1.f, 1.f, 1.0f);
	P_meshArray[E_GEO_BUTTON_BACK]->textureID[0] = LoadTGA(script.getGameData("image.button.back").c_str(), true);

	P_meshArray[E_GEO_BUTTON_LEFT] = MeshBuilder::GenerateQuad("Button Texture", Color(0.f, 0.f, 0.f), 1.f, 1.f, 1.0f);
	P_meshArray[E_GEO_BUTTON_LEFT]->textureID[0] = LoadTGA(script.getGameData("image.button.left").c_str(), true);

	P_meshArray[E_GEO_BUTTON_RIGHT] = MeshBuilder::GenerateQuad("Button Texture", Color(0.f, 0.f, 0.f), 1.f, 1.f, 1.0f);
	P_meshArray[E_GEO_BUTTON_RIGHT]->textureID[0] = LoadTGA(script.getGameData("image.button.right").c_str(), true);

	P_meshArray[E_GEO_STAR] = MeshBuilder::GenerateQuad("Star", Color(0.f, 0.f, 0.f), 1.f, 1.f, 1.0f);
	P_meshArray[E_GEO_STAR]->textureID[0] = LoadTGA(script.getGameData("image.background.star").c_str(), true);

	P_meshArray[E_GEO_SONAR] = MeshBuilder::GenerateQuad("Sonar Screen", Color(1.0f, 1.0f, 1.0f), static_cast<float>(Application::GetWindowWidth() / 2), static_cast<float>(Application::GetWindowHeight() / 2), 1.0f);
	P_meshArray[E_GEO_SONAR]->textureID[0] = LoadTGA(script.getGameData("image.background.background_sonar").c_str(), true);

	P_meshArray[E_GEO_ENEMY] = MeshBuilder::GenerateQuad("Enemy Screen", Color(1.0f, 1.0f, 1.0f), static_cast<float>(Application::GetWindowWidth() / 2), static_cast<float>(Application::GetWindowHeight() / 2), 1.0f);
	P_meshArray[E_GEO_ENEMY]->textureID[0] = LoadTGA(script.getGameData("image.background.background_enemy").c_str(), true);

	P_meshArray[E_GEO_ZONE] = MeshBuilder::GenerateQuad("Zone Screen", Color(1.0f, 1.0f, 1.0f), static_cast<float>(Application::GetWindowWidth() / 2), static_cast<float>(Application::GetWindowHeight() / 2), 1.0f);
	P_meshArray[E_GEO_ZONE]->textureID[0] = LoadTGA(script.getGameData("image.background.background_zone").c_str(), true);
}

/******************************************************************************/
/*!
\brief
Initializes menu
*/
/******************************************************************************/
void MenuScene::InitMenu(void)
{
	TextButton* S_MB;
	Button *m_B;
	UIColor.Set(0.4f, 0.4f, 0.4f);
	UIColorPressed.Set(0.9f, 0.9f, 0.9f);

	v3_Menupos[E_M_SPLASH].Set(0, 0, 0);
	v3_Menupos[E_M_MAIN].Set(0, 0, 0);
	v3_Menupos[E_M_LOADING] = v3_Menupos[E_M_MAIN];
	v3_Menupos[E_M_GAMEMODE].Set(4000, 0, 0);
	v3_Menupos[E_M_SELECTION].Set(8000, 0, 0);
	v3_Menupos[E_M_SKIP].Set(12000, 0, 0);
	v3_Menupos[E_M_FIND].Set(4000, -2000, 0);
	v3_Menupos[E_M_OPTIONS].Set(0, -2000, 0);
	v3_Menupos[E_M_MAP].Set(-4000, 0, 0);
	v3_Menupos[E_M_END].Set(0, 2000, 0);
	v3_Menupos[E_M_SONAR].Set(0, 0, 0);
	v3_Menupos[E_M_ENEMY].Set(0, 0, 0);
	v3_Menupos[E_M_ZONE].Set(0, 0, 0);

	transcomplete = false;

	if (CUR_MENU == MT_END_MENU)
		v3_MenuCam = v3_Menupos[E_M_END] - Vector3(0, 80, 0);

	LuaScript buttonScript("button");

	int total_option = 0;
	int total_button = 0;

	// Main Menu (Start Game / Option / Map Editor / Quit)
	total_button = buttonScript.get<int>("main.total_button");
	for (int i = 1; i <= total_button; i++)
	{
		std::string buttonName = "main.textbutton_" + std::to_string(static_cast<unsigned long long>(i)) + ".";

		S_MB = new TextButton;
		S_MB->pos.Set(Application::GetWindowWidth()* buttonScript.get<float>(buttonName + "posX"), Application::GetWindowHeight()* buttonScript.get<float>(buttonName + "posY"), 0.1f);
		S_MB->scale.Set(buttonScript.get<float>(buttonName + "scale"), buttonScript.get<float>(buttonName + "scale"), buttonScript.get<float>(buttonName + "scale"));
		S_MB->text = buttonScript.get<std::string>(buttonName + "text");
		S_MB->gamestate = E_M_MAIN;
		v_textButtonList.push_back(S_MB);
	}

	// Game Mode Menu (Single Player Game / Multi-Player Game)
	total_button = buttonScript.get<int>("main_gamemode.total_button");
	for (int i = 1; i <= total_button; i++)
	{
		std::string buttonName = "main_gamemode.option_" + std::to_string(static_cast<unsigned long long>(i)) + ".";

		S_MB = new TextButton;
		S_MB->pos.Set(Application::GetWindowWidth()*buttonScript.get<float>(buttonName + "posX"), Application::GetWindowHeight()*buttonScript.get<float>(buttonName + "posY"), 0.1f);
		S_MB->scale.Set(buttonScript.get<float>(buttonName + "scale"), buttonScript.get<float>(buttonName + "scale"), buttonScript.get<float>(buttonName + "scale"));
		S_MB->text = buttonScript.get<std::string>(buttonName + "text");
		S_MB->gamestate = E_M_GAMEMODE;
		v_textButtonList.push_back(S_MB);
	}

	// Selection Menu (New Game / Load Game)
	total_button = buttonScript.get<int>("main_selection.total_button");
	for (int i = 1; i <= total_button; i++)
	{
		std::string buttonName = "main_selection.option_" + std::to_string(static_cast<unsigned long long>(i)) + ".";

		S_MB = new TextButton;
		S_MB->pos.Set(Application::GetWindowWidth()*buttonScript.get<float>(buttonName + "posX"), Application::GetWindowHeight()*buttonScript.get<float>(buttonName + "posY"), 0.1f);
		S_MB->scale.Set(buttonScript.get<float>(buttonName + "scale"), buttonScript.get<float>(buttonName + "scale"), buttonScript.get<float>(buttonName + "scale"));
		S_MB->text = buttonScript.get<std::string>(buttonName + "text");
		S_MB->gamestate = E_M_SELECTION;
		v_textButtonList.push_back(S_MB);
	}

	// Skip Tutorial Menu (Skip / No)
	total_button = buttonScript.get<int>("main_skip.total_button");
	for (int i = 1; i <= total_button; i++)
	{
		std::string buttonName = "main_skip.option_" + std::to_string(static_cast<unsigned long long>(i)) + ".";

		S_MB = new TextButton;
		S_MB->pos.Set(Application::GetWindowWidth()*buttonScript.get<float>(buttonName + "posX"), Application::GetWindowHeight()*buttonScript.get<float>(buttonName + "posY"), 0.1f);
		S_MB->scale.Set(buttonScript.get<float>(buttonName + "scale"), buttonScript.get<float>(buttonName + "scale"), buttonScript.get<float>(buttonName + "scale"));
		S_MB->text = buttonScript.get<std::string>(buttonName + "text");
		S_MB->gamestate = E_M_SKIP;
		v_textButtonList.push_back(S_MB);
	}

	// Option Menu (Difficulty Selection)
	total_option = buttonScript.get<int>("option.total_option");
	for (int i = 1; i <= total_option; i++)
	{
		std::string buttonName = "option.option_" + std::to_string(static_cast<unsigned long long>(i)) + ".";

		int total_button = buttonScript.get<int>(buttonName + "total_button");
		for (int j = 1; j <= total_button; j++)
		{
			buttonName = "option.option_" + std::to_string(static_cast<unsigned long long>(i)) + ".textbutton_" + std::to_string(static_cast<unsigned long long>(j)) + ".";

			S_MB = new TextButton;
			S_MB->pos.Set(Application::GetWindowWidth()*buttonScript.get<float>(buttonName + "posX"), Application::GetWindowHeight()*buttonScript.get<float>(buttonName + "posY"), 0.1f);
			S_MB->scale.Set(buttonScript.get<float>(buttonName + "scale"), buttonScript.get<float>(buttonName + "scale"), buttonScript.get<float>(buttonName + "scale"));
			S_MB->text = buttonScript.get<std::string>(buttonName + "text");
			S_MB->gamestate = E_M_OPTIONS;
			v_textButtonList.push_back(S_MB);
		}
	}

	// End Menu (Back / Retry / Next Level)
	total_button = buttonScript.get<int>("end_screen.total_button");
	for (int i = 1; i <= total_button; i++)
	{
		std::string buttonName = "end_screen.textbutton_" + std::to_string(static_cast<unsigned long long>(i)) + ".";

		S_MB = new TextButton;
		S_MB->pos.Set(Application::GetWindowWidth()* buttonScript.get<float>(buttonName + "posX"), Application::GetWindowHeight()* buttonScript.get<float>(buttonName + "posY"), 0.1f);
		S_MB->scale.Set(buttonScript.get<float>(buttonName + "scale"), buttonScript.get<float>(buttonName + "scale"), buttonScript.get<float>(buttonName + "scale"));
		S_MB->text = buttonScript.get<std::string>(buttonName + "text");
		S_MB->gamestate = E_M_END;
		v_textButtonList.push_back(S_MB);
	}
}

/******************************************************************************/
/*!
\brief
Gets the button
*/
/******************************************************************************/
TextButton* MenuScene::FetchTB(std::string name)
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
Button* MenuScene::FetchBUTTON(int ID)
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
void MenuScene::UpdateTextButtons(void)
{
	for (std::vector<TextButton*>::iterator it = v_textButtonList.begin(); it != v_textButtonList.end(); ++it)
	{
		TextButton *S_MB = (TextButton *)*it;
		if (S_MB->gamestate == MENU_STATE)
		{
			Vector3 offset = v3_Menupos[MENU_STATE];

			if (intersect2D((S_MB->pos + Vector3(S_MB->text.length() * (S_MB->scale.x) - S_MB->scale.x, S_MB->scale.y*0.4f, 0)) + offset, S_MB->pos + Vector3(-S_MB->scale.x*0.5f, -(S_MB->scale.y*0.4f), 0) + offset, Vector3(MousePosX, MousePosY, 0)))
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
void MenuScene::UpdateButtons(void)
{
	for (std::vector<Button*>::iterator it = v_buttonList.begin(); it != v_buttonList.end(); ++it)
	{
		Button *m_B = (Button *)*it;

		Vector3 offset = v3_Menupos[MENU_STATE];

		if (intersect2D(m_B->Position + offset + m_B->Scale, m_B->Position + offset - m_B->Scale, Vector3(MousePosX, MousePosY, 0)))
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
void MenuScene::Update(double dt)	//TODO: Reduce complexity of MenuScene::Update()
{
	f_timer += static_cast<float>(dt);
	file_found_timer -= static_cast<float>(dt);
	//Mouse Section
	double x, y;
	Application::GetMousePos(x, y);
	MousePosX = static_cast<float>(x) / Application::GetWindowWidth() * Application::GetWindowWidth() + v3_MenuCam.x;
	MousePosY = (Application::GetWindowHeight() - static_cast<float>(y)) / Application::GetWindowHeight() * Application::GetWindowHeight() + v3_MenuCam.y;//*/

	static bool bLButtonState = false;

	UpdateTextButtons();
	UpdateButtons();

	if (v3_MenuCam != v3_Menupos[MENU_STATE])
	{
		transcomplete = false;

		Vector3 diff = v3_Menupos[MENU_STATE] - v3_MenuCam;
		v3_MenuCam += diff * static_cast<float>(dt)* 10.f;

		if (diff.x < 0.15f && diff.y < 0.15f && diff.x > -0.15f && diff.y > -0.15f)
		{
			v3_MenuCam = v3_Menupos[MENU_STATE];
		}
	}
	else
	{
		transcomplete = true;
	}

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

	switch (MENU_STATE)
	{
	case E_M_LOADING:
	{
						if (SE_Engine.isSoundPlaying(SoundList[ST_BGM]))
							SE_Engine.stopAllSounds();

						SceneManager::Instance()->replace(SceneManager::S_GAME);
						break;
	}
	case E_M_SPLASH:
	{
					   if (SE_Engine.isSoundPlaying(SoundList[ST_BGM]))
						   SE_Engine.stopAllSounds();

					   if (f_timer > 2.f || Application::IsKeyPressed(VK_LBUTTON))
					   {
						   MENU_STATE = E_M_MAIN;
					   }
					   break;
	}
	case E_M_MAIN:
	{
					 if (!SE_Engine.isSoundPlaying(SoundList[ST_BGM]))
						 SE_Engine.playSound2D(SoundList[ST_BGM], 1);

					 static bool mRButtonPressed = false;
					 if (Application::IsKeyPressed(VK_RBUTTON) && !mRButtonPressed)
					 {
						 mRButtonPressed = true;
					 }
					 if (!Application::IsKeyPressed(VK_RBUTTON) && mRButtonPressed)
					 {
						 mRButtonPressed = false;
					 }

					 if (!bLButtonState && Application::IsKeyPressed(VK_LBUTTON))
					 {
						 bLButtonState = true;
					 }
					 if (bLButtonState && !Application::IsKeyPressed(VK_LBUTTON))
					 {
						 bLButtonState = false;

						 LuaScript nameScript("button");

						 if (FetchTB(nameScript.get<std::string>("main.textbutton_1.text"))->active)
						 {
							 PREV_STATE = MENU_STATE;
							 MENU_STATE = E_M_GAMEMODE;
							 setMenu(MT_MAIN_MENU_GAMEMODE);
							 SE_Engine.playSound2D(SoundList[ST_BUTTON_CLICK_2]);
							 transcomplete = false;
						 }
						 else if (FetchTB(nameScript.get<std::string>("main.textbutton_2.text"))->active)
						 {
							 SceneManager::Instance()->replace(SceneManager::S_EDITOR_NEW);
						 }
						 else if (FetchTB(nameScript.get<std::string>("main.textbutton_3.text"))->active)
						 {
							 PREV_STATE = MENU_STATE;
							 MENU_STATE = E_M_OPTIONS;
							 setMenu(MT_MAIN_MENU_OPTION);
							 SE_Engine.playSound2D(SoundList[ST_BUTTON_CLICK_2]);
							 transcomplete = false;
						 }
						 else if (FetchTB(nameScript.get<std::string>("main.textbutton_4.text"))->active)
						 {
							 Application::closeApplication();
						 }
					 }
					 break;
	}
	case E_M_END:
	{
					if (SE_Engine.isSoundPlaying(SoundList[ST_BGM]))
						SE_Engine.stopAllSounds();

					static bool mRButtonPressed = false;
					if (Application::IsKeyPressed(VK_RBUTTON) && !mRButtonPressed)
					{
						mRButtonPressed = true;
					}
					if (!Application::IsKeyPressed(VK_RBUTTON) && mRButtonPressed)
					{
						mRButtonPressed = false;
					}

					if (!bLButtonState && Application::IsKeyPressed(VK_LBUTTON))
					{
						bLButtonState = true;
					}
					if (bLButtonState && !Application::IsKeyPressed(VK_LBUTTON))
					{
						bLButtonState = false;

						LuaScript nameScript("button");

						if (FetchTB(nameScript.get<std::string>("end_screen.textbutton_1.text"))->active)
						{
							SE_Engine.playSound2D(SoundList[ST_BUTTON_CLICK]);
							SceneManager::Instance()->pop();
							SceneManager::Instance()->pop();
							SceneManager::Instance()->push(SceneManager::S_MAIN_MENU);
						}
						else if (FetchTB(nameScript.get<std::string>("end_screen.textbutton_2.text"))->active)
						{
							SE_Engine.playSound2D(SoundList[ST_BUTTON_CLICK]);
							MainScene::GetInstance()->InitSimulation();
							SceneManager::Instance()->pop();

						}
						else if (FetchTB(nameScript.get<std::string>("end_screen.textbutton_3.text"))->active)
						{
							SE_Engine.playSound2D(SoundList[ST_BUTTON_CLICK]);
							MainScene::GetInstance()->LEVEL++;
							MainScene::GetInstance()->InitSimulation();
							loader.assign(MainScene::GetInstance()->LEVEL, 0, 1, true);
							loader.saveData();
							SceneManager::Instance()->pop();
						}
					}
					break;
	}
	case E_M_GAMEMODE:
	{
						 if (!SE_Engine.isSoundPlaying(SoundList[ST_BGM]))
							 SE_Engine.playSound2D(SoundList[ST_BGM], 1);

						 static bool mRButtonPressed = false;
						 if (Application::IsKeyPressed(VK_RBUTTON) && !mRButtonPressed)
						 {
							 mRButtonPressed = true;
						 }
						 if (!Application::IsKeyPressed(VK_RBUTTON) && mRButtonPressed)
						 {
							 mRButtonPressed = false;

							 SE_Engine.playSound2D(SoundList[ST_BUTTON_CLICK]);
							 PREV_STATE = MENU_STATE;
							 MENU_STATE = E_M_MAIN;
							 setMenu(MT_MAIN_MENU);
							 transcomplete = false;
						 }

						 static bool mLButtonPressed = false;
						 if (!mLButtonPressed && Application::IsKeyPressed(VK_LBUTTON))
						 {
							 mLButtonPressed = true;
						 }
						 if (mLButtonPressed && !Application::IsKeyPressed(VK_LBUTTON))
						 {
							 mLButtonPressed = false;
							 LuaScript nameScript("button");

							 if (FetchTB(nameScript.get<std::string>("main_gamemode.option_1.text"))->active)
							 {
								 PREV_STATE = MENU_STATE;
								 MENU_STATE = E_M_SELECTION;
								 setMenu(MT_MAIN_MENU_SELECTION);
								 SE_Engine.playSound2D(SoundList[ST_BUTTON_CLICK_2]);
								 transcomplete = false;
							 }
							 else if (FetchTB(nameScript.get<std::string>("main_gamemode.option_2.text"))->active)
							 {
								 // --! For Troy's Multiplayer Mode !--

								 //PREV_STATE = MENU_STATE;
								 //MENU_STATE = E_M_LOADING;

							 }
							 else if (FetchTB(nameScript.get<std::string>("main_gamemode.option_3.text"))->active)
							 {
								 PREV_STATE = MENU_STATE;
								 MENU_STATE = E_M_FIND;
								 setMenu(MT_MAIN_MENU_FIND);
								 SE_Engine.playSound2D(SoundList[ST_BUTTON_CLICK_2]);
								 temp_total_string = "";
							 }
						 }
						 break;
	}

	case E_M_SELECTION:
	{
						  if (!SE_Engine.isSoundPlaying(SoundList[ST_BGM]))
							  SE_Engine.playSound2D(SoundList[ST_BGM], 1);

						  static bool mRButtonPressed = false;
						  if (Application::IsKeyPressed(VK_RBUTTON) && !mRButtonPressed)
						  {
							  mRButtonPressed = true;
						  }
						  if (!Application::IsKeyPressed(VK_RBUTTON) && mRButtonPressed)
						  {
							  mRButtonPressed = false;

							  SE_Engine.playSound2D(SoundList[ST_BUTTON_CLICK]);
							  PREV_STATE = MENU_STATE;
							  MENU_STATE = E_M_GAMEMODE;
							  setMenu(MT_MAIN_MENU_GAMEMODE);
							  transcomplete = false;
						  }

						  static bool mLButtonPressed = false;
						  if (!mLButtonPressed && Application::IsKeyPressed(VK_LBUTTON))
						  {
							  mLButtonPressed = true;
						  }
						  if (mLButtonPressed && !Application::IsKeyPressed(VK_LBUTTON))
						  {
							  mLButtonPressed = false;
							  LuaScript nameScript("button");

							  if (FetchTB(nameScript.get<std::string>("main_selection.option_1.text"))->active)
							  {
								  PREV_STATE = MENU_STATE;
								  MENU_STATE = E_M_SKIP;
								  setMenu(MT_MAIN_MENU_SKIP);
								  SE_Engine.playSound2D(SoundList[ST_BUTTON_CLICK_2]);
								  transcomplete = false;
							  }
							  else if (FetchTB(nameScript.get<std::string>("main_selection.option_2.text"))->active)
							  {
								  MainScene::GetInstance()->LEVEL = stoi(loader.Data[0]);
								  PREV_STATE = MENU_STATE;
								  MENU_STATE = E_M_LOADING;

							  }
						  }
						  break;
	}
	case E_M_SKIP:
	{
					 if (!SE_Engine.isSoundPlaying(SoundList[ST_BGM]))
						 SE_Engine.playSound2D(SoundList[ST_BGM], 1);

					 static bool mRButtonPressed = false;
					 if (Application::IsKeyPressed(VK_RBUTTON) && !mRButtonPressed)
					 {
						 mRButtonPressed = true;
					 }
					 if (!Application::IsKeyPressed(VK_RBUTTON) && mRButtonPressed)
					 {
						 mRButtonPressed = false;

						 SE_Engine.playSound2D(SoundList[ST_BUTTON_CLICK]);
						 PREV_STATE = MENU_STATE;
						 MENU_STATE = E_M_SELECTION;
						 setMenu(MT_MAIN_MENU_SELECTION);
						 transcomplete = false;
					 }

					 static bool mLButtonPressed = false;
					 if (!mLButtonPressed && Application::IsKeyPressed(VK_LBUTTON))
					 {
						 mLButtonPressed = true;
					 }
					 if (mLButtonPressed && !Application::IsKeyPressed(VK_LBUTTON))
					 {
						 mLButtonPressed = false;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
						 LuaScript nameScript("button");

						 // Skip Tutorial
						 if (FetchTB(nameScript.get<std::string>("main_skip.option_1.text"))->active)
						 {
							 MainScene::GetInstance()->LEVEL = 2;
							 PREV_STATE = MENU_STATE;
							 MENU_STATE = E_M_LOADING;
						 }
						 // Not Skipping
						 else if (FetchTB(nameScript.get<std::string>("main_skip.option_2.text"))->active)
						 {
							 MainScene::GetInstance()->LEVEL = 1;
							 PREV_STATE = MENU_STATE;
							 MENU_STATE = E_M_LOADING;
						 }
					 }
					 break;
	}
	case E_M_FIND:
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

					 static bool mRButtonPressed = false;
					 if (Application::IsKeyPressed(VK_RBUTTON) && !mRButtonPressed)
					 {
						 mRButtonPressed = true;
					 }
					 if (!Application::IsKeyPressed(VK_RBUTTON) && mRButtonPressed)
					 {
						 mRButtonPressed = false;

						 SE_Engine.playSound2D(SoundList[ST_BUTTON_CLICK]);
						 PREV_STATE = MENU_STATE;
						 MENU_STATE = E_M_GAMEMODE;
						 setMenu(MT_MAIN_MENU_GAMEMODE);
						 transcomplete = false;
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
						 bool temp = ML_map.loadMap(ss.str());

						 if (temp == false)
						 {
							 cout << "File Not Found" << std::endl;
							 file_found = true;
							 file_found_timer = 0.5f;
						 }
						 else
						 {
							 MainScene::GetInstance()->LEVEL = 0;
							 MainScene::GetInstance()->LEVELNAME = ss.str();

							 PREV_STATE = MENU_STATE;
							 MENU_STATE = E_M_LOADING;
						 }
					 }

					 if (!bLButtonState && Application::IsKeyPressed(VK_LBUTTON))
					 {
						 bLButtonState = true;
					 }
					 if (bLButtonState && !Application::IsKeyPressed(VK_LBUTTON))
					 {
						 bLButtonState = false;
					 }

					 if (file_found == true && file_found_timer < 0)
					 {
						 file_found = false;
					 }
	}
		break;
	case E_M_OPTIONS:
	{
						if (!SE_Engine.isSoundPlaying(SoundList[ST_BGM]))
							SE_Engine.playSound2D(SoundList[ST_BGM], 1);

						static bool mRButtonPressed = false;
						if (Application::IsKeyPressed(VK_RBUTTON) && !mRButtonPressed)
						{
							mRButtonPressed = true;
						}
						if (!Application::IsKeyPressed(VK_RBUTTON) && mRButtonPressed)
						{
							mRButtonPressed = false;
							if (CUR_MENU == MT_PAUSE_MENU)
							{
								assignsave(true);
								SceneManager::Instance()->pop();
							}
							else
							{
								SE_Engine.playSound2D(SoundList[ST_BUTTON_CLICK]);
								PREV_STATE = MENU_STATE;
								MENU_STATE = E_M_MAIN;
								setMenu(MT_MAIN_MENU);
								InitMenu();
							}
						}

						static bool mLButtonPressed = false;
						if (!mLButtonPressed && Application::IsKeyPressed(VK_LBUTTON))
						{
							mLButtonPressed = true;
						}
						if (mLButtonPressed && !Application::IsKeyPressed(VK_LBUTTON))
						{
							mLButtonPressed = false;

							LuaScript nameScript("button");

							if (FetchTB(nameScript.get<std::string>("option.option_1.textbutton_1.text"))->active)
							{
								std::cout << "Easy" << std::endl;
							}
							else if (FetchTB(nameScript.get<std::string>("option.option_1.textbutton_2.text"))->active)
							{
								std::cout << "Medium" << std::endl;
							}
							else if (FetchTB(nameScript.get<std::string>("option.option_1.textbutton_3.text"))->active)
							{
								std::cout << "Hard" << std::endl;
							}
						}
						break;
	}

	case E_M_SONAR: case E_M_ENEMY: case E_M_ZONE:
	{
						if (SE_Engine.isSoundPlaying(SoundList[ST_BGM]))
							SE_Engine.stopAllSounds();

						if (Application::IsKeyPressed(VK_SPACE))
						{
							SceneManager::Instance()->pop();
						}
						break;
	}

	}
}

/******************************************************************************/
/*!
\brief
Updates control display in options menu
*/
/******************************************************************************/
void MenuScene::UpdateControlSettingLabels(unsigned short key, int button)
{
	char temp_char = static_cast<char>(key);
	std::stringstream ss;
	std::string s;
	ss << temp_char;
	ss >> s;
	if (key == 16)
	{
		us_controlCB[button].CONTROLTEXT = "SHIFT";
	}
	else if (key == 17)
	{
		us_controlCB[button].CONTROLTEXT = "CTRL";
	}
	else if (key == 164)
	{
		us_controlCB[button].CONTROLTEXT = "LALT";
	}
	else if (key == 32)
	{
		us_controlCB[button].CONTROLTEXT = "SPACE";
	}
	else if (key == 1)
	{
		us_controlCB[button].CONTROLTEXT = "LMB";
	}
	else if (key == 2)
	{
		us_controlCB[button].CONTROLTEXT = "RMB";
	}
	else if (key == 4)
	{
		us_controlCB[button].CONTROLTEXT = "MMB";
	}
	else if (key == 5)
	{
		us_controlCB[button].CONTROLTEXT = "BUTTON 4";
	}
	else if (key == 6)
	{
		us_controlCB[button].CONTROLTEXT = "BUTTON 5";
	}
	else if (key == 9)
	{
		us_controlCB[button].CONTROLTEXT = "TAB";
	}
	else if (key == 96)
	{
		us_controlCB[button].CONTROLTEXT = "NUM 0";
	}
	else if (key == 97)
	{
		us_controlCB[button].CONTROLTEXT = "NUM 1";
	}
	else if (key == 98)
	{
		us_controlCB[button].CONTROLTEXT = "NUM 2";
	}
	else if (key == 99)
	{
		us_controlCB[button].CONTROLTEXT = "NUM 3";
	}
	else if (key == 100)
	{
		us_controlCB[button].CONTROLTEXT = "NUM 4";
	}
	else if (key == 101)
	{
		us_controlCB[button].CONTROLTEXT = "NUM 5";
	}
	else if (key == 102)
	{
		us_controlCB[button].CONTROLTEXT = "NUM 6";
	}
	else if (key == 103)
	{
		us_controlCB[button].CONTROLTEXT = "NUM 7";
	}
	else if (key == 104)
	{
		us_controlCB[button].CONTROLTEXT = "NUM 8";
	}
	else if (key == 105)
	{
		us_controlCB[button].CONTROLTEXT = "NUM 9";
	}
	else
	{
		us_controlCB[button].CONTROLTEXT = s;
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
void MenuScene::editFOV(float &newFOV)
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
void MenuScene::UpdateFOV()
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
void MenuScene::InitShadersAndLights(void)
{
	LuaScript scriptshader("Shader");

	//Load vertex and fragment shaders
	//u_m_programID = LoadShaders("GameData//Shader//comg.vertexshader", "GameData//Shader//comg.fragmentshader");
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
void MenuScene::RenderMesh(Mesh *mesh, bool enableLight, float Glow, Color GlowColor)
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
void MenuScene::RenderText(Mesh* mesh, std::string text, Color color)
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
void MenuScene::RenderTextCenter(Mesh* mesh, std::string text, Color color)
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
void MenuScene::RenderTextCenterOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
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
void MenuScene::RenderTextOnScreen(Mesh* mesh, std::string text, Color color)
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
void MenuScene::RenderMeshOnScreen(Mesh* mesh, float Glow, Color GlowColor)
{
	glUniform1i(u_m_parameters[U_UNI_GLOW], static_cast<GLint>(Glow));
	glUniform3fv(u_m_parameters[U_UNI_GLOW_COLOR], 1, &GlowColor.r);


	glDisable(GL_DEPTH_TEST);
	glUniform1i(u_m_parameters[E_UNI_LIGHTENABLED], 0);
	glUniform1i(u_m_parameters[E_UNI_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID[0]);
	glUniform1i(u_m_parameters[E_UNI_COLOR_TEXTURE], 0);

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
void MenuScene::RenderTextButtons(void)
{
	for (unsigned i = 0; i < v_textButtonList.size(); ++i)
	{
		TextButton *S_MB = v_textButtonList[i];
		if ((S_MB->gamestate == MENU_STATE || S_MB->gamestate == PREV_STATE) && S_MB->gamestate != E_M_LOADING)
		{
			modelStack.PushMatrix();
			modelStack.Translate(v3_Menupos[S_MB->gamestate]);
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
void MenuScene::RenderButtons(void)
{
	for (unsigned i = 0; i < v_buttonList.size(); ++i)
	{
		Button *m_B = v_buttonList[i];
		if ((m_B->gamestate == MENU_STATE || m_B->gamestate == PREV_STATE) && m_B->gamestate != E_M_LOADING)
		{
			modelStack.PushMatrix();
			modelStack.Translate(v3_Menupos[m_B->gamestate]);
			modelStack.Translate(m_B->Position);
			modelStack.Scale(m_B->Scale);
			RenderMeshOnScreen(m_B->mesh, 10.f, m_B->color);

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
void MenuScene::Render()
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
		v3_MenuCam.x, v3_MenuCam.y, v3_MenuCam.z,
		v3_MenuCam.x, v3_MenuCam.y, -100.f,
		camera.up.x, camera.up.y, camera.up.z
		);

	modelStack.LoadIdentity();

	modelStack.PushMatrix();
	modelStack.Translate(-v3_MenuCam);

	switch (MENU_STATE)
	{
	case E_M_LOADING:
	{
						RenderTextButtons();
						RenderButtons();
						modelStack.PushMatrix();
						modelStack.LoadIdentity();
						modelStack.Translate(static_cast<float>(Application::GetWindowWidth() * 0.5f), static_cast<float>(Application::GetWindowHeight() * 0.5f), 0);
						RenderMeshOnScreen(P_meshArray[E_GEO_LOADING_BACKGROUND]);
						modelStack.PopMatrix();
						break;
	}
	case E_M_SPLASH:
	{
					   RenderTextButtons();
					   RenderButtons();
					   modelStack.PushMatrix();
					   modelStack.LoadIdentity();
					   modelStack.Translate(static_cast<float>(Application::GetWindowWidth() * 0.5f), static_cast<float>(Application::GetWindowHeight() * 0.5f), 0);
					   RenderMeshOnScreen(P_meshArray[E_GEO_SPLASH]);
					   modelStack.PopMatrix();
					   break;
	}
	case E_M_MAIN:
	{
					 modelStack.PushMatrix();
					 modelStack.LoadIdentity();
					 modelStack.Translate(static_cast<float>(Application::GetWindowWidth() * 0.5f), static_cast<float>(Application::GetWindowHeight() * 0.5f), 0);
					 RenderMeshOnScreen(P_meshArray[E_GEO_BACKGROUND_MAIN]);
					 modelStack.PopMatrix();
					 RenderTextButtons();
					 RenderButtons();
					 break;
	}
	case E_M_SELECTION:
	{
						  modelStack.PushMatrix();
						  modelStack.LoadIdentity();
						  modelStack.Translate(static_cast<float>(Application::GetWindowWidth() * 0.5f), static_cast<float>(Application::GetWindowHeight() * 0.5f), 0);
						  RenderMeshOnScreen(P_meshArray[E_GEO_BACKGROUND_MAIN]);
						  modelStack.PopMatrix();
						  RenderTextButtons();
						  RenderButtons();
						  break;
	}

	case E_M_GAMEMODE:
	{
						 modelStack.PushMatrix();
						 modelStack.LoadIdentity();
						 modelStack.Translate(static_cast<float>(Application::GetWindowWidth() * 0.5f), static_cast<float>(Application::GetWindowHeight() * 0.5f), 0);
						 RenderMeshOnScreen(P_meshArray[E_GEO_BACKGROUND_MAIN]);
						 modelStack.PopMatrix();
						 RenderTextButtons();
						 RenderButtons();
						 break;
	}

	case E_M_OPTIONS:
	{
						modelStack.PushMatrix();
						modelStack.LoadIdentity();
						modelStack.Translate(static_cast<float>(Application::GetWindowWidth() * 0.5f), static_cast<float>(Application::GetWindowHeight() * 0.5f), 0);
						RenderMeshOnScreen(P_meshArray[E_GEO_BACKGROUND_OPTION]);
						modelStack.PopMatrix();
						LuaScript buttonScript("button");

						int total_option = buttonScript.get<int>("option.total_option");
						for (int i = 1; i <= total_option; i++)
						{
							std::string buttonName = "option.option_" + std::to_string(static_cast<unsigned long long>(i)) + ".";
							modelStack.PushMatrix();
							modelStack.Translate(v3_Menupos[MENU_STATE]);
							modelStack.Translate(Application::GetWindowWidth()*0.5f + buttonScript.get<float>(buttonName + "posX"), Application::GetWindowHeight()*0.48f + +buttonScript.get<float>(buttonName + "posY"), 0.1f);
							modelStack.Scale(buttonScript.get<float>(buttonName + "scale"), buttonScript.get<float>(buttonName + "scale"), buttonScript.get<float>(buttonName + "scale"));
							RenderTextOnScreen(P_meshArray[E_GEO_TEXT], buttonScript.get<std::string>(buttonName + "text"), UIColor);
							modelStack.PopMatrix();
						}
						RenderTextButtons();
						RenderButtons();
						break;
	}
	case E_M_MAP:
	{
					modelStack.PushMatrix();
					modelStack.LoadIdentity();
					modelStack.Translate(static_cast<float>(Application::GetWindowWidth() * 0.5f), static_cast<float>(Application::GetWindowHeight() * 0.5f), 0);
					RenderMeshOnScreen(P_meshArray[E_GEO_BACKGROUND_MAIN]);
					modelStack.PopMatrix();

					RenderTextButtons();
					RenderButtons();
					break;
	}

	case E_M_END:
	{
					modelStack.PushMatrix();
					modelStack.LoadIdentity();
					modelStack.Translate(static_cast<float>(Application::GetWindowWidth() * 0.5f), static_cast<float>(Application::GetWindowHeight() * 0.5f), 0);
					RenderMeshOnScreen(P_meshArray[E_GEO_BACKGROUND_END]);
					modelStack.PopMatrix();
					for (int i = 0; i < SceneManager::Instance()->end_star; i++)
					{
						modelStack.PushMatrix();
						modelStack.Translate(v3_Menupos[MENU_STATE]);
						modelStack.Translate(Application::GetWindowWidth()*0.65f - ((SceneManager::Instance()->end_star - 1) * 80) + (i * 100), Application::GetWindowHeight() * 0.5f, 0.1);
						modelStack.Scale(40, 40, 40);
						RenderMeshOnScreen(P_meshArray[E_GEO_STAR]);
						modelStack.PopMatrix();
					}
					RenderTextButtons();
					RenderButtons();
					break;
	}

	case E_M_SKIP:
	{
					 modelStack.PushMatrix();
					 modelStack.LoadIdentity();
					 modelStack.Translate(static_cast<float>(Application::GetWindowWidth() * 0.5f), static_cast<float>(Application::GetWindowHeight() * 0.5f), 0);
					 RenderMeshOnScreen(P_meshArray[E_GEO_BACKGROUND_MAIN]);
					 modelStack.PopMatrix();

					 RenderTextButtons();
					 RenderButtons();

					 LuaScript buttonScript("button");

					 std::string buttonName = "main_skip.";
					 modelStack.PushMatrix();
					 modelStack.Translate(v3_Menupos[MENU_STATE]);
					 modelStack.Translate(Application::GetWindowWidth() * buttonScript.get<float>(buttonName + "posX"), Application::GetWindowHeight() * buttonScript.get<float>(buttonName + "posY"), 0.1f);
					 modelStack.Scale(buttonScript.get<float>(buttonName + "scale"), buttonScript.get<float>(buttonName + "scale"), buttonScript.get<float>(buttonName + "scale"));
					 RenderTextOnScreen(P_meshArray[E_GEO_TEXT], buttonScript.get<std::string>(buttonName + "text"), UIColor);
					 modelStack.PopMatrix();
					 break;
	}

	case E_M_FIND:
	{
					 modelStack.PushMatrix();
					 modelStack.LoadIdentity();
					 modelStack.Translate(static_cast<float>(Application::GetWindowWidth() * 0.5f), static_cast<float>(Application::GetWindowHeight() * 0.5f), 0);
					 RenderMeshOnScreen(P_meshArray[E_GEO_BACKGROUND_MAIN]);
					 modelStack.PopMatrix();

					 RenderTextButtons();
					 RenderButtons();

					 modelStack.PushMatrix();
					 modelStack.Translate(v3_Menupos[MENU_STATE]);
					 modelStack.Translate(Application::GetWindowWidth() * 0.5f, Application::GetWindowHeight() * 0.55f, 0);
					 modelStack.Scale(35, 35, 1);
					 RenderTextOnScreen(P_meshArray[E_GEO_TEXT], "Enter Level Name: ", UIColor);
					 modelStack.PopMatrix();

					 std::stringstream ss3;
					 ss3 << temp_total_string;
					 modelStack.PushMatrix();
					 modelStack.Translate(v3_Menupos[MENU_STATE]);
					 modelStack.Translate(Application::GetWindowWidth() * 0.5f, Application::GetWindowHeight() * 0.5f, 0);
					 modelStack.Scale(35, 35, 1);
					 RenderTextOnScreen(P_meshArray[E_GEO_TEXT], ss3.str(), UIColor);
					 modelStack.PopMatrix();

					 if (file_found == true)
					 {
						 modelStack.PushMatrix();
						 modelStack.Translate(v3_Menupos[MENU_STATE]);
						 modelStack.Translate(Application::GetWindowWidth() * 0.5f, Application::GetWindowHeight() * 0.45f, 0);
						 modelStack.Scale(35, 35, 1);
						 RenderTextOnScreen(P_meshArray[E_GEO_TEXT], "File Not Found", UIColor);
						 modelStack.PopMatrix();
					 }
					 break;
	}
	case E_M_SONAR:
	{
					  modelStack.PushMatrix();
					  modelStack.LoadIdentity();
					  modelStack.Translate(static_cast<float>(Application::GetWindowWidth() * 0.5f), static_cast<float>(Application::GetWindowHeight() * 0.5f), 0);
					  RenderMeshOnScreen(P_meshArray[E_GEO_SONAR]);
					  modelStack.PopMatrix();
					  RenderTextButtons();
					  RenderButtons();
					  break;
	}

	case E_M_ENEMY:
	{
					  modelStack.PushMatrix();
					  modelStack.LoadIdentity();
					  modelStack.Translate(static_cast<float>(Application::GetWindowWidth() * 0.5f), static_cast<float>(Application::GetWindowHeight() * 0.5f), 0);
					  RenderMeshOnScreen(P_meshArray[E_GEO_ENEMY]);
					  modelStack.PopMatrix();
					  RenderTextButtons();
					  RenderButtons();
					  break;
	}

	case E_M_ZONE:
	{
					 modelStack.PushMatrix();
					 modelStack.LoadIdentity();
					 modelStack.Translate(static_cast<float>(Application::GetWindowWidth() * 0.5f), static_cast<float>(Application::GetWindowHeight() * 0.5f), 0);
					 RenderMeshOnScreen(P_meshArray[E_GEO_ZONE]);
					 modelStack.PopMatrix();
					 RenderTextButtons();
					 RenderButtons();
					 break;
	}
	}

	modelStack.PopMatrix();
}

/******************************************************************************/
/*!
\brief
Clears memory upon exit
*/
/******************************************************************************/
void MenuScene::Exit()
{
	assignsave(true);

	SE_Engine.Exit();

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
void MenuScene::CleanShaders(void)
{
	glDeleteVertexArrays(1, &u_m_vertexArrayID);
	glDeleteProgram(u_m_programID);
}