
#include "SceneManager.h"
//#include <vld.h>

SceneManager* SceneManager::m_sInstance = NULL;

/******************************************************************************/
/*!
\brief
Default constructor
*/
/******************************************************************************/
SceneManager::SceneManager()
{
	end_star = -1;
	winner = -1;
	difficulty = 1;
}

/******************************************************************************/
/*!
\brief
Default destructor
*/
/******************************************************************************/
SceneManager::~SceneManager()
{

}

/******************************************************************************/
/*!
\brief
Initialises the scenemanager
\param firstScene
the first scene to load
*/
/******************************************************************************/
void SceneManager::Init(SCENES firstScene)
{
	Math::InitRNG();
	push(firstScene);
}

/******************************************************************************/
/*!
\brief
Initialises the scenemanager
\param dt
time since previous frame
*/
/******************************************************************************/
void SceneManager::Update(double dt)
{
	if (v_SceneVector.size() > 0)
		v_SceneVector.back()->Update(dt);
}

/******************************************************************************/
/*!
\brief
Renders the current scene
*/
/******************************************************************************/
void SceneManager::Render(void)
{
	if (v_SceneVector.size() > 0)
		v_SceneVector.back()->Render();
}

/******************************************************************************/
/*!
\brief
cleans up the scenemanager
*/
/******************************************************************************/
void SceneManager::exit(void)
{
	while (v_SceneVector.size() > 0)
	{
		pop(false);
	}
}

/******************************************************************************/
/*!
\brief
Pushes a new scene on top of the current one
\param newScene
the scene to load
*/
/******************************************************************************/
void SceneManager::push(SCENES newScene)
{
	if (v_SceneVector.size() > 0)
	{
		v_SceneVector.back()->CleanShaders();
	}

	Scene *scene;
	switch (newScene)
	{
	case SceneManager::S_MAIN_MENU_SPLASH:
	{
		scene = new MenuScene();
		MenuScene *ms_tmp = dynamic_cast<MenuScene*>(scene);
		ms_tmp->setMenu(MenuScene::MT_MAIN_MENU_SPLASH);
		break;
	}
	case SceneManager::S_MAIN_MENU:
	{
		scene = new MenuScene();
		MenuScene *ms_tmp = dynamic_cast<MenuScene*>(scene);
		ms_tmp->setMenu(MenuScene::MT_MAIN_MENU);
		break;
	}
	case SceneManager::S_END_MENU:
	{
		scene = new MenuScene();
		MenuScene *ms_tmp = dynamic_cast<MenuScene*>(scene);
		ms_tmp->setMenu(MenuScene::MT_END_MENU);
		break;
	}
	case SceneManager::S_END_MENU_MULT:
	{
		scene = new MenuScene();
		MenuScene *ms_tmp = dynamic_cast<MenuScene*>(scene);
		ms_tmp->setMenu(MenuScene::MT_END_MENU_MULT);
		break;
	}
	case SceneManager::S_GAME:
		scene = MainScene::GetInstance();
		break;
	case SceneManager::S_MULT:
		scene = MultScene::GetInstance();
		break;
	case SceneManager::S_PAUSE_MENU:
	{
		scene = new MenuScene();
		MenuScene *ms_tmp = dynamic_cast<MenuScene*>(scene);
		ms_tmp->setMenu(MenuScene::MT_PAUSE_MENU);
		break;
	}
	case SceneManager::S_EDITOR_NEW:
	{
		scene = MapScene::GetInstance();
		MapScene *ms_tmp = dynamic_cast<MapScene*>(scene);
		ms_tmp->setState(MapScene::MT_CREATE);
		break;
	}
	case SceneManager::S_EDITOR_EDIT:
	{
		scene = MapScene::GetInstance();
		MapScene *ms_tmp = dynamic_cast<MapScene*>(scene);
		ms_tmp->setState(MapScene::MT_EDIT);
		break;
	}
	case SceneManager::S_SONAR:
	{
		scene = new MenuScene();
		MenuScene *ms_tmp = dynamic_cast<MenuScene*>(scene);
		ms_tmp->setMenu(MenuScene::MT_SONAR);
		break;
	}
	case SceneManager::S_ENEMY:
	{
								  scene = new MenuScene();
								  MenuScene *ms_tmp = dynamic_cast<MenuScene*>(scene);
								  ms_tmp->setMenu(MenuScene::MT_ENEMY);
								  break;
	}
	case SceneManager::S_ZONE:
	{
								  scene = new MenuScene();
								  MenuScene *ms_tmp = dynamic_cast<MenuScene*>(scene);
								  ms_tmp->setMenu(MenuScene::MT_ZONE);
								  break;
	}
	default:
		scene = new MenuScene();
		break;
	}

	scene->Init();
	scene->InitShaders();
	v_SceneVector.push_back(scene);
}

/******************************************************************************/
/*!
\brief
Replaces the current scene with a new one
\param newScene
the scene to load
*/
/******************************************************************************/
void SceneManager::replace(SCENES newScene)
{
	pop();
	push(newScene);
}

/******************************************************************************/
/*!
\brief
Removes the current scene and reopens the previous one
\param reloadscene
determines whether to load the scene shaders of the current scene after popping
*/
/******************************************************************************/
void SceneManager::pop(bool reloadscene)
{
	if (v_SceneVector.size() > 0)
	{
		Scene *scene = v_SceneVector.back();
		if (scene != NULL)
		{
			scene->Exit();
			scene->CleanShaders();

			if (scene == MainScene::GetInstance())
			{
				MainScene::Destroy();
				scene = NULL;
			}
			if (scene == MapScene::GetInstance())
			{
				MapScene::Destroy();
				scene = NULL;
			}
			if (scene == MultScene::GetInstance())
			{
				MultScene::Destroy();
				scene = NULL;
			}
			else
			{
				delete scene;
				scene = NULL;
			}			
		}
		v_SceneVector.pop_back();

		if (reloadscene)
		{
			if (v_SceneVector.size() > 0)
			{
				v_SceneVector.back()->InitShaders();
			}
		}
	}
}

/******************************************************************************/
/*!
\brief
gets the current scene at the back of the vector
\return
returns a scene pointer of the running scene
*/
/******************************************************************************/
Scene* SceneManager::getCurScene()
{
	return v_SceneVector.back();
}

/******************************************************************************/
/*!
\brief
Selfdestruct
*/
/******************************************************************************/
void SceneManager::destroy()
{
	if (m_sInstance != NULL)
	{
		m_sInstance->exit();
		delete m_sInstance;
		m_sInstance = NULL;
	}
}

/******************************************************************************/
/*!
\brief
gets an instance of itself. Generate a new instance if it doesn't exist yet
\return
returns an instance of itself
*/
/******************************************************************************/
SceneManager* SceneManager::Instance()
{
	if (m_sInstance == NULL)
	{
		m_sInstance = new SceneManager();
	}
	return m_sInstance;
}