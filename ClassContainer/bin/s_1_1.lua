-- set global scene parameters --
Buttons.margin = 20
Buttons.num_columns=3
Buttons.row_interval = 10
button_anim = "btn_rect.anim"

scene = Buttons()
root:add(scene)

scene.background.texture = "interface/menu_bg.rttex"

scene.title.text = "`b 1.1"
-- TextBoxItem(text, font)
-- width = always 0
-- font: 0=times, 1=trajan, 3=phonetic
-- NOTE: Center alignment doesn't work yet!

local tb = TextBoxItem("`b Послушайте диалог-знакомство. Проговаривайте за дикторами каждую фразу, подражая интонации. ", 0)
tb.id = "TextBox"
scene.description
	:addItem(tb)
	
scene.columns[1]
	:add{"`b -Hello", "", "audio/ding.wav"}
	:add{"`b -	Pleased to meet you, Anna.", "", "audio/ding.wav"}
	:add{"`b -	Nice to meet you too.", "", "audio/ding.wav"}
	scene.columns[1].id = "col1"

scene.columns[2]
	:add{"", " [хелоу]", "audio/ding.wav"}
	:add{"", " [пли:зд ту ми:ч ю, анна]", "audio/ding.wav"}
	:add{"", " [найс ту ми:ч ю, ту:]", "audio/ding.wav"}
	scene.columns[2].id = "col2"
	
scene.columns[3]
	:add{"", "- здравствуй", "audio/ding.wav"}
	:add{"", "- Приятно познакомиться, Анна.", "audio/ding.wav"}
	:add{"", "- Мне тоже приятно познакомиться.", "audio/ding.wav"}


scene.debugDrawBox = true	

wait_for(scene, "onFinish")
scene.visible = false
root:remove(scene)