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
	:addItem(TextBoxItem("����� �� ������ ����� � ��������, ��� ��� ������������. ������� �� �� ��������. �����, ����� ����", 0, 0))
	:addItem(TextBoxItem(" [x] ", 0, 3))
	:addItem(TextBoxItem("��������� ����� ��������.", 0, 0))
	:addItem(VoiceTextItem("ding"))

scene.columns[1]
	:add{"man", " - �������", "audio/ding.wav"}
	:add{"fat", " - ������", "audio/click.wav"}
	:add{"sad", " - ��������", "audio/click.wav"}
	
scene.columns[2]
	:add{"m`@a`0p", " - �����", "audio/ding.wav"}
	:add{"r`@a`0t", " - �����", "audio/click.wav"}
	:add{"t`@a`0n", " - �����", "audio/click.wav"}
	
scene.columns[3]
	:add{"mad", " - ��������������", "audio/ding.wav"}
	:add{"bat", " - ������� ����", "audio/click.wav"}
	:add{"hat", " - �����", "audio/click.wav"}

--scene.debugDrawBox = true	