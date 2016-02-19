sonar = {
	left_mouse = {
		sonar_radius = 150.0,
		sonar_sides = 120
	},

	right_mouse = {
		sonar_radius = 300.0,
		sonar_sides = 120
	}
}

main = {
	--[[Button_1 = Start Button]]--
	textbutton_1 = {
		text = "Start Game",
		scale = 40.0,
		posX = 400.0,
		posY = -100.0
	},

	--[[Button_2 = Option Button]]--
	textbutton_2 = {
		text = "Option",
		scale = 30.0,
		posX = 400.0,
		posY = -150.0
	},

	--[[Button_3 = Map Editor]]--
	textbutton_3 = {
		text = "Map Editor",
		scale = 30.0,
		posX = 400.0,
		posY = -200.0
	},

	--[[Button_3 = Quit Button]]--
	textbutton_4 = {
		text = "Quit",
		scale = 30.0,
		posX = 400.0,
		posY = -250.0
	},

	total_button = 4
}

main_selection = {
	--[[option_1 = New Game]]--
	option_1 = {
		text = "New Game",
		scale = 35.0,
		posX = 420.0,
		posY = -120.0,
		
		title = {
			text = "Level Selection",
			scale = 40.0,
			posX = 420.0,
			posY = -120.0
		},

		--[[Button_1 = Level 1]]--
		textbutton_1 = {
			text = "Level 1",
			scale = 30.0,
			posX = 435.0,
			posY = -160.0
		},

		--[[Button_2 = Level 2]]--
		textbutton_2 = {
			text = "Level 2",
			scale = 30.0,
			posX = 450.0,
			posY = -200.0
		},

		--[[Button_3 = Level 3]]--
		textbutton_3 = {
			text = "Level 3",
			scale = 30.0,
			posX = 465.0,
			posY = -240.0
		},

		--[[Button_4 = Test Level]]--
		textbutton_4 = {
			text = "Test Your Level",
			scale = 30.0,
			posX = 480.0,
			posY = -280.0
		},

		total_button = 4
	},

	--[[option_2 = Load Game]]--
	option_2 = {
		text = "Load Game",
		scale = 35.0,
		posX = 420.0,
		posY = -170.0,
	},

	total_option = 2
}

option = {
	--[[Option_1 = Difficulty]]--
	option_1 = {
		text = "Difficulty",
		scale = 35.0,
		posX = 420.0,
		posY = -120.0,
		
		--[[Button_1 = Easy Button]]--
		textbutton_1 = {
			text = "Easy",
			scale = 25.0,
			posX = 420.0,
			posY = -160.0
		},
		
		--[[Button_2 = Medium Button]]--
		textbutton_2 = {
			text = "Medium",
			scale = 25.0,
			posX = 420.0,
			posY = -200.0
		},
		
		--[[Button_3 = Hard Button]]--
		textbutton_3 = {
			text = "Hard",
			scale = 25.0,
			posX = 420.0,
			posY = -240.0
		},

		total_button = 3
	},

	total_option = 1		
}