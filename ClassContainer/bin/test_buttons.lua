-- set global scene parameters --
Buttons.margin = 20
Buttons.num_columns=3
Buttons.row_interval = 10
button_anim = "btn_rect.anim"

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

scene.columns[1]
	:add{"man", " - мужчина", "audio/man.ogg"}
	:add{"fat", " - жирный", "audio/fat.ogg"}
	:add{"sad", " - грустный", "audio/sad.ogg"}
	
scene.columns[2]
	:add{"m`@a`0p", " - карта", "audio/map.ogg"}
	:add{"r`@a`0t", " - крыса", "audio/rat.ogg"}
	:add{"t`@a`0n", " - загар", "audio/tan.ogg"}
	
scene.columns[3]
	:add{"mad", " - рассвирепевший", "audio/mad.ogg"}
	:add{"bat", " - летучая мышь", "audio/bat.ogg"}
	:add{"hat", " - шляпа", "audio/hat.ogg"}

--scene.debugDrawBox = true	