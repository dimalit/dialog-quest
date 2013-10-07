-- set global scene parameters --
Input.margin = 20
Input.num_columns=3
Input.row_interval = 10
Input.drop_frame = "interface/frame"
Input.drop_frame_active = "interface/frame_glow"

scene = Input()
root:add(scene)

scene.background.texture = "interface/menu_bg.rttex"

scene.title.text = "Input"
-- TextBoxItem(text, width, font)
-- width = always 0
-- font: 0=times, 1=trajan, 3=phonetic
-- NOTE: Center alignment doesn't work yet!
scene.description
	:addItem(TextBoxItem("Нажми на каждое слово и послушай, как они произносятся. Повтори их за диктором. Следи, чтобы звук", 0, 0))
	:addItem(TextBoxItem(" [x] ", 0, 3))
	:addItem(TextBoxItem("оставался очень коротким.", 0, 0))
	:addItem(VoiceTextItem("ding"))

scene.columns[1]
	:add{"man", 1}
	:add{"fat", 2}
	:add{"sad", 3}
	
scene.columns[2]
	:add{"m`@a`0p", 2}
	:add{"r`@a`0t", 4}
	:add{"t`@a`0n", 1}
	
scene.columns[3]
	:add{"mad", 2}
	:add{"bat", 1}
	:add{"hat", 7}

scene.columns[1].debugDrawBox = true	