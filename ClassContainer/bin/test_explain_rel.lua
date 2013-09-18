Explain.margin = 20
Explain.paragraph_interval = 10

scene = Explain()
root:add(scene)
--scene.content.debugDrawBox = true

scene.title.text = "Unit 2 ���1"

local p1 = FlowLayout(0,50)
p1:addItem(TextBoxItem("����� �� � �������� ����� ������� ��� [?]. ������ ����� � ������� ���� ���. ��� ����� ������� ����� �������� �� � ���. ����� ���������� ���� ���� ���������, ������ ������� ��� ���, ����� ������ ������� ��, � ����� ���. ���� [?] ����� ��������, ��� �� � ���� ������ ����� �����."))
p1:addItem(ImageItem("interface/flask.rttex"))
scene.content:add(p1)
--p1:setLocationOrigin(scene.content, 0.5, 0)
--p1:setWidthOrigin(scene)
p1.rel_hpy = 0
scene.content:link(p1, 0, nil, scene.content, 0, nil)
scene.content:link(p1, 1, nil, scene.content, 1, nil)

p2 = FlowLayout()
p2:addItem(TextBoxItem("����� �� � �������� ����� ������� ��� [?]. ������ ����� � ������� ���� ���. ��� ����� ������� ����� �������� �� � ���. ����� ���������� ���� ���� ���������, ������ ������� ��� ���, ����� ������ ������� ��, � ����� ���. ���� [?] ����� ��������, ��� �� � ���� ������ ����� �����."))
p2:addObstacle(ImageItem("interface/flask.rttex"), 16, 16, "right")
scene.content:add(p2)
--p2:setLocationOrigin(p1, 0.5, 1)
--p2:setWidthOrigin(p1)
--p2.rel_hpy = 0
--p2:rel_y(10)
scene.content:link(p2, 0, 0, p1, 0, 1, 0, Explain.paragraph_interval)
scene.content:link(p2, 1, 0, p1, 1, 1, 0, Explain.paragraph_interval)

local t = TextItem("sample text")
	scene.content:add(t)
	-- t.rel_hpy = 0
	-- t:setLocationOrigin(p2, 0.25, 1)
	-- t:rel_y(20)
	scene.content:link(t, 0.5, 0, p2, 0.25, 1, 0, 50)
local i = ImageItem("interface/flask.rttex")
	scene.content:add(i)
	-- i.rel_hpy = 0
	-- i:setLocationOrigin(p2, 0.75, 1)
	-- i:rel_y(30)
	scene.content:link(i, 0.5, 0, p2, 0.75, 1, 0, 50)
scene:start()