/****************************************************************************/
/*!
\file MainScene.cpp
\author Gabriel Wong Choon Jieh
\par email: AuraTigital\@gmail.com
\brief
Main menu for the openGL framework
!*/
/****************************************************************************/
#ifndef MAIN_SCENE_H
#define MAIN_SCENE_H

#include "Scene.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Light.h"
#include "LoadTGA.h"
#include "AzimuthAltitudeCamera.h"
#include "TextButton.h"
#include "Button.h"
#include "GabrielDLC.h"
#include "SoundEngine.h"
#include "GameObject.h"
#include "ItemObject.h"
#include "CharacterObject.h"
#include "SpecialObject.h"
#include "maploader.h"
#include <string>
#include <iostream>
#include <fstream>

using namespace::std;

class CharacterObject;
class Player;

/******************************************************************************/
/*!
Class Assignment3:
\brief	Define geometries, light sources
*/
/******************************************************************************/
class MainScene : public Scene
{
public:
	/******************************************************************************/
	/*!
	enum E_GEO_TYPE:
	\brief	Lists the types of Meshes
	*/
	/******************************************************************************/
	enum E_GEO_TYPE
	{
		E_GEO_AXES,
		// Text
		E_GEO_TEXT,
		E_GEO_BUTTON_LEFT,
		E_GEO_BUTTON_RIGHT,
		E_GEO_BUTTON_REFRESH,

		//World
		E_GEO_FLOOR_1,
		E_GEO_FLOOR_2,
		E_GEO_WALL_1,
		E_GEO_PLAYER,
		E_GEO_ENEMY,
		E_GEO_DANGER,
		E_GEO_EXIT,
		E_GEO_STAR,
		//Projectile
		E_GEO_LINE,

		E_GEO_TOTAL,
	};
private:
	/******************************************************************************/
	/*!
	enum E_UNI_TYPE:
	\brief	List the types of uniform parameters
	*/
	/******************************************************************************/
	enum E_UNI_TYPE
	{
		E_UNI_MVP = 0,
		E_UNI_MODELVIEW,
		E_UNI_MODELVIEW_INVERSE_TRANSPOSE,
		E_UNI_MATERIAL_AMBIENT,
		E_UNI_MATERIAL_DIFFUSE,
		E_UNI_MATERIAL_SPECULAR,
		E_UNI_MATERIAL_SHININESS,
		E_UNI_LIGHTENABLED,
		// Light[0]---------------------------
		E_UNI_LIGHT0_POSITION,
		E_UNI_LIGHT0_COLOR,
		E_UNI_LIGHT0_POWER,
		E_UNI_LIGHT0_KC,
		E_UNI_LIGHT0_KL,
		E_UNI_LIGHT0_KQ,
		E_UNI_LIGHT0_TYPE,
		E_UNI_LIGHT0_SPOTDIRECTION,
		E_UNI_LIGHT0_COSCUTOFF,
		E_UNI_LIGHT0_COSINNER,
		E_UNI_LIGHT0_EXPONENT,

		E_UNI_NUMLIGHTS,
		// Texture----------------------------
		E_UNI_COLOR_TEXTURE_ENABLED,
		E_UNI_COLOR_TEXTURE,
		// Text-------------------------------
		E_UNI_TEXT_ENABLED,
		E_UNI_TEXT_COLOR,
		//------------------------------------
		U_UNI_GLOW,
		U_UNI_GLOW_COLOR,

		E_UNI_TOTAL,
	};

	enum SOUND_TYPE
	{
		ST_BUTTON_CLICK,
		ST_BUTTON_CLICK_2,
		ST_TOTAL
	};

	enum E_BUTTON_ID
	{
		BI_BACK,
		BI_PREV_MAP,
		BI_NEXT_MAP,
		BI_REFRESH,
	};

public:

	MainScene();
	~MainScene();

	virtual void Init();
	virtual void InitShaders();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
	virtual void CleanShaders();

	/******************************************************************************/
	/*!
	Mesh* P_meshArray[E_GEO_TOTAL]:
	\brief	Stores pointers to meshes
	*/
	/******************************************************************************/
	Mesh* P_meshArray[E_GEO_TOTAL];

	//Runtime
	float f_timer;
	int i_SimulationSpeed;

	//Objectstuff
	std::vector<GameObject*> GO_List;
	MapLoader ML_map;

	void CO_attach(CharacterObject *CO, GameObject *GO);
	void CO_drop(CharacterObject *CO);
	bool InitSimulation(void);

	Vector3 calTilePos(Vector3 Worldpos);
	Vector3 calWorldPos(Vector3 Tilepos);
	Player *player_ptr;

	bool onDanger, onExit, toggleVisible;
	bool shownSonar, shownEnemy, shownZone;
	int LEVEL;
private:
	

	// Static Constants
	/******************************************************************************/
	/*!
	static const unsigned int ui_NUM_LIGHT_PARAMS:
	\brief	Stores the number of uniform light parameters
	*/
	/******************************************************************************/
	static const unsigned int ui_NUM_LIGHT_PARAMS;

	/******************************************************************************/
	/*!
	unsigned u_m_vertexArrayID:
	\brief	Vertex array ID
	*/
	/******************************************************************************/
	unsigned u_m_vertexArrayID;

	// GLFW/GLEW
	/******************************************************************************/
	/*!
	unsigned u_m_programID:
	\brief	Program ID
	*/
	/******************************************************************************/
	unsigned u_m_programID;

	/******************************************************************************/
	/*!
	unsigned u_m_parameters[E_UNI_TOTAL]:
	\brief	An array to store all the uniform parameters specified in the UNIFORM_TYPE enumeration
	*/
	/******************************************************************************/
	unsigned u_m_parameters[E_UNI_TOTAL];

	// Cameras
	/******************************************************************************/
	/*!
	FirstPersonCamera noClipCamera:
	\brief
	*/
	/******************************************************************************/
	Camera camera;

	// Controls
	/******************************************************************************/
	/*!
	float f_mouseSensitivity:
	\brief	Stores and controls mouse sensitivity
	*/
	/******************************************************************************/
	float f_mouseSensitivity;

	/******************************************************************************/
	/*!
	float f_fov:
	\brief	Controls the field of view of the game
	*/
	/******************************************************************************/
	float f_fov;
	float f_fov_target;

	//Lighting
	/******************************************************************************/
	/*!
	const unsigned ui_NUM_LIGHTS:
	\brief	Controls the number of lights rendered in the scene
	*/
	/******************************************************************************/
	const unsigned ui_NUM_LIGHTS;

	/******************************************************************************/
	/*!
	Light* P_lights:
	\brief	Pointers to an array of lights
	*/
	/******************************************************************************/
	Light P_lightsArray[1];

	/******************************************************************************/
	/*!
	unsigned short *us_ControlChange:
	\brief	pointer to point to the control to change
	*/
	/******************************************************************************/
	unsigned short *us_ControlChange;

	Color UIColor, UIColorPressed, UIColorServant;
	std::vector<TextButton*> v_textButtonList;
	TextButton* FetchTB(std::string name);
	void UpdateTextButtons(void);
	void RenderTextButtons(void);

	std::vector<Button*> v_buttonList;
	Button* FetchBUTTON(int ID);
	void UpdateButtons(void);
	void RenderButtons(void);


	// Init Functions
	void InitMeshList(void);
	void InitShadersAndLights(void);
	void InitMenu(void);

	bool InitLevel(int level);

	
	int MENU_STATE;

	// Update/Control Functions
	float MousePosX, MousePosY;
	void editFOV(float &newFOV);
	void UpdateFOV();

	// Render 
	void RenderText(Mesh* mesh, std::string text, Color color);
	void RenderTextCenter(Mesh* mesh, std::string text, Color color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color);
	void RenderTextCenterOnScreen(Mesh* mesh, std::string text, Color color, float size = 1.f, float x = 0.f, float y = 0.f);
	void RenderMesh(Mesh *mesh, bool enableLight, float Glow = 0, Color GlowColor = Color(1.f, 0.f, 0.f));
	void RenderMeshOnScreen(Mesh* mesh, float Glow = 0, Color GlowColor = Color(1.f, 0.f, 0.f));
	void RenderUI();
	void RenderGO();

	//Collision
	bool checkForCollision(Vector3 position_start, Vector3 position_end, Vector3 top_left, Vector3 bottom_right, string *side = &std::string(), Vector3 &Hit = Vector3(0, 0, 0));
	bool LineIntersectsRect(Vector3 p1, Vector3 p2, Vector3 topLeft, Vector3 botRight, Vector3 topRight, Vector3 botLeft, string *side);
	bool LineIntersectsLine(Vector3 l1p1, Vector3 l1p2, Vector3 l2p1, Vector3 l2p2);

	//Other
	Vector3 v3_2DCam;

	/******************************************************************************/
	/*!
	MS modelStack:
	\brief	A stack of transformation matrices with reference to the models
	*/
	/******************************************************************************/
	MS modelStack;

	/******************************************************************************/
	/*!
	MS viewStack:
	\brief	A stack of transformation matrices with reference to the viewer
	*/
	/******************************************************************************/
	MS viewStack;

	/******************************************************************************/
	/*!
	MS projectionStack:
	\brief	A stack of transformation matrices with reference to the projection of the view
	*/
	/******************************************************************************/
	MS projectionStack;

	SoundEngine SE_Engine;
	ISoundSource *SoundList[ST_TOTAL];

private:
	static MainScene* Instance;

	////Pathfinding variables
	//int n;								 // horizontal size of the map
	//int m;								 // vertical size size of the map
	//static int **Map;
	//static int **closed_nodes_map;		 // map of closed (tried-out) nodes
	//static int **open_nodes_map;			 // map of open (not-yet-tried) nodes
	//static int **dir_map;				 // map of directions
	//const int dx[4] = { 1, 0, -1, 0 };
	//const int dy[4] = { 0, 1, 0, -1 };

public:
	static MainScene* GetInstance();
	static void Destroy();
};

#endif
