-- set global scene parameters --
Buttons.margin = 20
Buttons.num_columns=3
Buttons.row_interval = 10
Buttons.button_up_frame = "interface/frame"
Buttons.button_down_frame = "interface/frame_glow"

scene = Buttons()
root:add(scene)

scene.background.texture = "interface/menu_bg.rttex"

scene.title.text = "Buttons"
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
	:add{"man", " - мужчина", "audio/ding.wav"}
	:add{"fat", " - жирный", "audio/click.wav"}
	:add{"sad", " - грустный", "audio/click.wav"}
	
scene.columns[2]
	:add{"m`@a`0p", " - карта", "audio/ding.wav"}
	:add{"r`@a`0t", " - крыса", "audio/click.wav"}
	:add{"t`@a`0n", " - загар", "audio/click.wav"}
	
scene.columns[3]
	:add{"mad", " - рассвирепевший", "audio/ding.wav"}
	:add{"bat", " - летучая мышь", "audio/click.wav"}
	:add{"hat", " - шляпа", "audio/click.wav"}

--scene.debugDrawBox = true	