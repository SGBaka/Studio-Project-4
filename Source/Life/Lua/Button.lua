main = {
	--[[Button_1 = Start Button]]--
	button_1 = {
		text = "Start Game",
		scale = 40.0,
		posX = 20.0,
		posY = -120.0
	},

	--[[Button_2 = Option Button]]--
	button_2 = {
		text = "Option",
		scale = 30.0,
		posX = 15.0,
		posY = -170.0
	},

	--[[Button_3 = Quit Button]]--
	button_3 = {
		text = "Quit",
		scale = 30.0,
		posX = 15.0,
		posY = -220.0
	},

	total_button = 3
}

main_selection = {
	--[[option_1 = New Game]]--
	option_1 = {
		text = "New Game",
		scale = 35.0,
		posX = 20.0,
		posY = -120.0,
		
		title = {
			text = "Level Selection:",
			scale = 40.0,
			posX = 20.0,
			posY = -120.0
		},

		--[[Button_1 = Level 1]]--
		button_1 = {
			text = "Level 1",
			scale = 30.0,
			posX = 20.0,
			posY = -160.0
		},

		--[[Button_2 = Level 2]]--
		button_2 = {
			text = "Level 2",
			scale = 30.0,
			posX = 20.0,
			posY = -200.0
		},

		--[[Button_3 = Level 3]]--
		button_3 = {
			text = "Level 3",
			scale = 30.0,
			posX = 20.0,
			posY = -240.0
		},

		--[[Button_4 = Test Level]]--
		button_4 = {
			text = "Test Your Level",
			scale = 30.0,
			posX = 20.0,
			posY = -280.0
		},

		total_button = 4
	},

	--[[option_2 = Load Game]]--
	option_2 = {
		text = "Load Game",
		scale = 35.0,
		posX = 20.0,
		posY = -170.0,

		total_button = 0
	},

	--[[option_3 = Map Editor]]--
	option_3 = {
		text = "Map Editor",
		scale = 35.0,
		posX = 20.0,
		posY = -220.0,

		total_button = 0
	},

	total_option = 3
}

option = {
	--[[Option_1 = Difficulty]]--
	option_1 = {
		text = "Difficulty",
		scale = 35.0,
		posX = 20.0,
		posY = -120.0,
		
		--[[Button_1 = Easy Button]]--
		button_1 = {
			text = "Easy",
			scale = 25.0,
			posX = 15.0,
			posY = -160.0
		},
		
		--[[Button_2 = Medium Button]]--
		button_2 = {
			text = "Medium",
			scale = 25.0,
			posX = 15.0,
			posY = -200.0
		},
		
		--[[Button_3 = Hard Button]]--
		button_3 = {
			text = "Hard",
			scale = 25.0,
			posX = 15.0,
			posY = -240.0
		},

		total_button = 3
	},

	total_option = 1		
}