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
	:addItem(TextBoxItem("����� �� ������ ����� � ��������, ��� ��� ������������. ������� �� �� ��������. �����, ����� ����", 0, 0))
	:addItem(TextBoxItem(" [x] ", 0, 3))
	:addItem(TextBoxItem("��������� ����� ��������.", 0, 0))

scene.columns[1]
	:add{"man", " - �������", "audio/man.ogg"}
	:add{"fat", " - ������", "audio/fat.ogg"}
	:add{"sad", " - ��������", "audio/sad.ogg"}
	
scene.columns[2]
	:add{"m`@a`0p", " - �����", "audio/map.ogg"}
	:add{"r`@a`0t", " - �����", "audio/rat.ogg"}
	:add{"t`@a`0n", " - �����", "audio/tan.ogg"}
	
scene.columns[3]
	:add{"mad", " - ��������������", "audio/mad.ogg"}
	:add{"bat", " - ������� ����", "audio/bat.ogg"}
	:add{"hat", " - �����", "audio/hat.ogg"}

--scene.debugDrawBox = true	