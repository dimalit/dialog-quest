-- set global scene parameters --
Buttons.margin = 20
Buttons.num_columns=3
Buttons.row_interval = 10

scene = Buttons()
root:add(scene)

scene.background.texture = "interface/menu_bg.rttex"

scene.title.text = "Buttons"

scene.columns[1]
	:add{"A", " - A", "a.wav"}
	:add{"B", " - A", "a.wav"}
	
scene.columns[2]
	:add{"A", " - A", "a.wav"}
	:add{"B", " - A", "a.wav"}
	
scene.columns[3]
	:add{"A", " - A", "a.wav"}
	:add{"B", " - A", "a.wav"}
	:add{"A", " - A", "a.wav"}
	:add{"B", " - A", "a.wav"}

scene.debugDrawBox = true	