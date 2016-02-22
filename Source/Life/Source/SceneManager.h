/****************************************************************************/
/*!
\file SceneManager.h
\author Gabriel Wong Choon Jieh
\par email: AuraTigital\@gmail.com
\brief
A class to handle scene switching and cleaning
!*/
/****************************************************************************/
#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "Scene.h"
#include "MenuScene.h"
#include "MainScene.h"
#include "MapScene.h"

#include <iostream>
#include <string>
#include <vector>

class SceneManager
{
public:
	enum SCENES
	{
		S_MAIN_MENU_SPLASH,
		S_MAIN_MENU,
		S_PAUSE_MENU,
		S_END_MENU,
		S_GAME,
		S_EDITOR,
	};
	
	void Init(SCENES firstScene = S_MAIN_MENU);
	void Update(double dt);
	void Render(void);
	
	void push(SCENES newScene);
	void replace(SCENES newScene);
	void pop(bool reloadscene = true);

	Scene* getCurScene();

	static SceneManager* Instance();
	static void destroy();
				
private:
	SceneManager();
	~SceneManager();

	void exit(void);

	std::vector<Scene*> v_SceneVector;

	static SceneManager* m_sInstance;
};
#endif