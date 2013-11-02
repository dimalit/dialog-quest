-- set global scene parameters --
Buttons.margin = 20
Buttons.num_columns=2
Buttons.row_interval = 60
button_anim = "btn_rect.anim"

scene = Buttons()
root:add(scene)

scene.background.texture = "interface/menu_bg.rttex"

scene.title.text = "`b 1.1"
-- TextBoxItem(text, width, font)
-- width = always 0
-- font: 0=times, 1=trajan, 3=phonetic
-- NOTE: Center alignment doesn't work yet!
scene.description
	:addItem(TextBoxItem("`b Послушайте диалог-знакомство. Проговаривайте за дикторами каждую фразу, подражая интонации. ", 0, 0))
	

 scene.columns[1]
	:add{"`b -Hello", "", "audio/ding.wav"}
	:add{"`b -	Pleased to meet you, Anna.", "", "audio/ding.wav"}
	:add{"`b -	Nice to meet you too.", "", "audio/ding.wav"}

scene.columns[2]
	:add{"", "`b [хелоу]", "audio/ding.wav"}
	:add{"", "`b [пли:зд ту ми:ч ю, анна]", "audio/ding.wav"}
	:add{"", "`b [найс ту ми:ч ю, ту:]", "audio/ding.wav"}

-- scene.columns[3]
	-- :add{"", "`b- здравствуй", "audio/ding.wav"}
	-- :add{"", "`b- Приятно познакомиться, Анна.", "audio/ding.wav"}
	-- :add{"", "`b- Мне тоже приятно познакомиться.", "audio/ding.wav"}


--scene.debugDrawBox = true	