Explain.margin = 20
Explain.paragraph_interval = 10

scene = Explain()
root:add(scene)

scene.background.texture = "interface/summary_bg.rttex"
scene.title.text = "Unit 2 ���1"

local p1 = FlowLayout(0,50)
p1.id = "p1"
-- default width = 0 and font = 0
-- for phonetic we use width=0, font=3 => TextBoxItem("[x]", 0, 3) (Phonetic TM)
local i1 = TextBoxItem("����� �� � �������� ����� �������� ��� ", 0)
i1.debugDrawColor=0xff0000ff
i1.id = "item1"
p1:addItem(i1):addItem(VoiceTextItem("ding")):addItem(TextBoxItem(". ������ ����� � ������� ����� ���. ��� ����� ������� ����� �������� �� � ���. ����� ���������� ���� ���� ���������, ������ ������� ��� ���, ����� ������ ������� ��, � ����� ���. ���� ", 0)):addItem(PhonemicItem("x"))
local im = ImageItem("interface/flask.rttex")
im.scaleX = 1.0
p1:addItem(im)
p1:addItem(TextBoxItem(" ����� ��������, ��� �� � ���� ������ ������ ������.", 0))
scene.content:add(p1)
scene.content:link(p1, 0, nil, scene.content, 0, nil, -1, -1)
scene.content:link(p1, 1, nil, scene.content, 1, nil, -2, -2)
scene.content:link(p1, nil, 0, scene.content, nil, 0, -3, -3)

p2 = FlowLayout()
p2:addItem(TextBoxItem("����� �� � �������� ����� ������� ��� [?]. ������ ����� � ������� ���� ���. ��� ����� ������� ����� �������� �� � ���. ����� ���������� ���� ���� ���������, ������ ������� ��� ���, ����� ������ ������� ��, � ����� ���. ���� [?] ����� ��������, ��� �� � ���� ������ ����� �����.", 0))
local mo = Mover(ImageItem("interface/flask.rttex"))
mo.onMove = function() mo.parent:requestLayOut() end
p2:addObstacle(mo, 16, 16, "right")
scene.content:add(p2)
scene.content:link(p2, 0, 0, p1, 0, 1, 0, Explain.paragraph_interval)
scene.content:link(p2, 1, 0, p1, 1, 1, 0, Explain.paragraph_interval)

-- local t = TextItem("sample text")
	-- scene.content:add(t)
	-- -- t.rel_hpy = 0
	-- -- t:setLocationOrigin(p2, 0.25, 1)
	-- -- t:rel_y(20)
	-- scene.content:link(t, 0.5, 0, p2, 0.25, 1, 0, 50)
-- local i = ImageItem("interface/flask.rttex")
	-- scene.content:add(i)
	-- -- i.rel_hpy = 0
	-- -- i:setLocationOrigin(p2, 0.75, 1)
	-- -- i:rel_y(30)
	-- scene.content:link(i, 0.5, 0, p2, 0.75, 1, 0, 50)
	
scene.debugDrawBox = true
	
scene:start()