main = {
	--[[Button_1 = Start Button]]--
	textbutton_1 = {
		text = "Start Game",
		scale = 35.0,
		posX = 0.5,
		posY = 0.5
	},

	--[[Button_2 = Map Editor]]--
	textbutton_2 = {
		text = "Map Editor",
		scale = 30.0,
		posX = 0.5,
		posY = 0.45
	},

	--[[Button_3 = Option Button]]--
	textbutton_3 = {
		text = "Option",
		scale = 30.0,
		posX = 0.5,
		posY = 0.40
	},

	--[[Button_4 = Quit Button]]--
	textbutton_4 = {
		text = "Quit",
		scale = 30.0,
		posX = 0.5,
		posY = 0.35
	},

	total_button = 4
}

main_gamemode = {
	--[[option_1 = Single Player]]--
	option_1 = {
		text = "Single Player",
		scale = 30.0,
		posX = 0.5,
		posY = 0.5
	},

	--[[option_2 = Multi-Player]]--
	option_2 = {
		text = "Multi-Player",
		scale = 30.0,
		posX = 0.5,
		posY = 0.45
	},

	--[[option_2 = Play Selected Level]]--
	option_3 = {
		text = "Find Your Level",
		scale = 30.0,
		posX = 0.5,
		posY = 0.4
	},

	total_button = 3
}

main_skip = {
	text = "Skip Tutorial?",
	scale = 35.0,
	posX = 0.5,
	posY = 0.55,

	--[[option_1 = Skip]]--
	option_1 = {
		text = "Yes",
		scale = 30.0,
		posX = 0.5,
		posY = 0.5
	},

	--[[option_2 = No]]--
	option_2 = {
		text = "No",
		scale = 30.0,
		posX = 0.5,
		posY = 0.45
	},

	total_button = 2
}

main_selection = {
	--[[option_1 = New Game]]--
	option_1 = {
		text = "New Game",
		scale = 30.0,
		posX = 0.5,
		posY = 0.5
	},

	--[[option_2 = Load Game]]--
	option_2 = {
		text = "Continue Game",
		scale = 30.0,
		posX = 0.5,
		posY = 0.45
	},

	total_button = 2
}

option = {
	--[[Option_1 = Difficulty]]--
	option_1 = {
		text = "Difficulty",
		scale = 35.0,
		posX = 0,
		posY = 0,
		
		--[[Button_1 = Easy Button]]--
		textbutton_1 = {
			text = "Easy",
			scale = 30.0,
			posX = 0.5,
			posY = 0.40
		},
		
		--[[Button_2 = Medium Button]]--
		textbutton_2 = {
			text = "Medium",
			scale = 30.0,
			posX = 0.5,
			posY = 0.35
		},
		
		--[[Button_3 = Hard Button]]--
		textbutton_3 = {
			text = "Hard",
			scale = 30.0,
			posX = 0.5,
			posY = 0.30	
		},

		total_button = 3
	},

	total_option = 1		
}

editor_replace = {
	--[[Button_1 = Replace Map]]--
	textbutton_1 = {
		text = "Yes",
		scale = 30.0,
		posX = 0.44,
		posY = 0.44
	},

	--[[Button_2 = Save as New Map]]--
	textbutton_2 = {
		text = "No",
		scale = 30.0,
		posX = 0.54,
		posY = 0.44
	},

	--[[Button_3 = Go Back]]--
	textbutton_3 = {
		text = "Back",
		scale = 25.0,
		posX = 0.65,
		posY = 0.25
	},

	total_button = 3
}

editor_difficulty = {
	text = "Difficulty of Level",
	scale = 30.0,
	posX = 0.37,
	posY = 0.67,
		
	--[[Button_1 = Easy Button]]--
	textbutton_1 = {
		text = "Easy",
		scale = 25.0,
		posX = 0.48,
		posY = 0.55
	},
		
	--[[Button_2 = Medium Button]]--
	textbutton_2 = {
		text = "Medium",
		scale = 25.0,
		posX = 0.47,
		posY = 0.45
	},
	
	--[[Button_3 = Hard Button]]--
	textbutton_3 = {
		text = "Hard",
		scale = 25.0,
		posX = 0.48,
		posY = 0.35
	},

	--[[Button_4 = Back Button]]--
	textbutton_4 = {
		text = "Back",
		scale = 25.0,
		posX = 0.73,
		posY = 0.2
	},

	total_button = 4
}

editor_load = {
	text = "Level Name:",
	scale = 35.0,
	posX = 0.35,
	posY = 0.75,

	text_failed = {
		text = "Failed to Load",
		scale = 35.0,
		posX = 0.35,
		posY = 0.58
	},

	--[[Button_1 = Confirm Button]]--
	textbutton_1 = {
		text = "Confirm",
		scale = 25.0,
		posX = 0.35,
		posY = 0.20
	},
		
	--[[Button_2 = Back Button]]--
	textbutton_2 = {
		text = "Back",
		scale = 25.0,
		posX = 0.55,
		posY = 0.20
	},

	total_button = 2
}

editor_button = {
	button_7 = {
		text = "Player"
	},
	
	button_8 = {
		text = "Enemy"
	},

	button_5 = {
		text = "Wall"
	},

	button_6 = {
		text = "Floor"
	},

	button_9 = {
		text = "Danger"
	},

	button_10 = {
		text = "Win"
	},

	button_0 = {
		text = "Main Menu"
	},

	button_1 = {
		text = "Restart"
	},

	button_2 = {
		text = "Save Map"
	},

	button_3 = {
		text = "Load Map"
	},

	button_4 = {
		text = "Delete",

		text_onscreen = "Delete [",
		text_onscreen2 = "]?",
		scale = 35.0,
		posX = 0.35,
		posY = 0.60,
		
		text_failed = {
			text = "Failed to Delete",
			scale = 35.0,
			posX = 0.37,
			posY = 0.70
		},

		--[[Option_ 1 = Yes to Delete]]--		
		option_1 = {
			text = "Yes",
			scale = 30.0,
			posX = 0.43,
			posY = 0.30
		},

		--[[Option_ 1 = Cancel Delete]]--		
		option_2 = {
			text = "No",
			scale = 30.0,
			posX = 0.55,
			posY = 0.30
		},
		
		total_option = 2
	}
}

end_screen = {
	--[[Button_1 = Back]]--
	textbutton_1 = {
		text = "Back",
		scale = 30.0,
		posX = 0.45,
		posY = 0.20
	},

	--[[Button_2 = Retry]]--
	textbutton_2 = {
		text = "Retry",
		scale = 30.0,
		posX = 0.6,
		posY = 0.20
	},

	--[[Button_3 = Next Level]]--
	textbutton_3 = {
		text = "Next Level",
		scale = 30.0,
		posX = 0.75,
		posY = 0.20
	},

	total_button = 3
}

end_screen_mult = {
	--[[Button_1 = Back]]--
	textbutton_1 = {
		text = "Back",
		scale = 30.0,
		posX = 0.45,
		posY = 0.20
	},

	total_button = 1
}