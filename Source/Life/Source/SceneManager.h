
#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "Scene.h"
#include "MenuScene.h"
#include "MainScene.h"
#include "MapScene.h"
#include "MultScene.h"

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
		S_FINAL_MENU,
		S_END_MENU_MULT,
		S_GAME,
		S_GAME_MODE,
		S_MULT,
		S_EDITOR_NEW,
		S_EDITOR_EDIT,
		S_SONAR,
		S_ENEMY,
		S_ZONE,
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
	
	int end_star;
	int winner;
	int difficulty;
private:
	SceneManager();
	~SceneManager();

	void exit(void);

	std::vector<Scene*> v_SceneVector;

	static SceneManager* m_sInstance;
};
#endif