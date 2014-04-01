-- set global scene parameters --
Input.margin = 20
Input.num_columns=2
Input.row_interval = 10
Input.drop_frame = "interface/frame"
Input.drop_frame_active = "interface/frame_glow"
Input.mover_bk_image = "interface/rect_bk.rttex"

scene = Input()
root:add(scene)

scene.background.texture = "interface/menu_bg.rttex"

scene.title.text = "Input"
scene.title.scale = 1.5
-- TextBoxItem(text font)
-- width = always 0
-- font: 0=times, 1=trajan, 3=phonetic
-- NOTE: Center alignment doesn't work yet!
local t2 = TextBoxItem("��������� ����� ��������.", 0)
t2.scale = 2.0
scene.description
	:addItem(TextBoxItem("����� �� ������ ����� � ��������, ��� ��� ������������. ������� �� �� ��������. �����, ����� ����", 0))
	:addItem(TextBoxItem(" [x] ", 3))
	:addItem(t2)
	:addItem(VoiceTextItem("ding"))

scene.columns[1]
	:add{"MAN", "alpha"}
	:add{"FAT", "beta"}
	:add{"SxD", "gamma"}
	
scene.columns[2]
	:add{"m`@a`0p", "delta"}
	:add{"r`@a`0t", "epsilon"}
	:add{"t`@a`0n", "dzeta"}
	
scene:addWords{"dummy", "omega"}

scene.afterLayOut = function(self)
	scene:placeWordsRandomly()
end

--------------------------------------
dummy = TextItem("Hello")
root:add(dummy)
dummy.x, dummy.y = 200, 200
dummy.onDragEnd = function()
	dummy.x = dummy.x + 1
end
--------------------------------------
scene.debugDrawBox = true

wait_for(scene, "onFinish")
scene.visible = false
root:remove(scene)