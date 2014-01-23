Explain.margin = 20
Explain.paragraph_interval = 10

scene = Explain()
root:add(scene)

scene.background.texture = "interface/summary_bg.rttex"
scene.title.text = "Unit 2 шаг1"

local p1 = FlowLayout(0,50)
p1.id = "p1"
-- default width = 0 and font = 0
-- for phonetic we use width=0, font=3 => TextBoxItem("[x]", 0, 3) (Phonetic TM)
local i1 = TextBoxItem("Буква «а» в закрытом слоге читается как ", 0)
i1.debugDrawColor=0xff0000ff
i1.id = "item1"
p1:addItem(i1):addItem(VoiceTextItem("ding")):addItem(TextBoxItem(". Такого звука в русском языке нет. Это нечто среднее между русскими «а» и «э». Чтобы произнести этот звук правильно, широко раскрой рот так, будто хочешь сказать «а», и скажи «э». Звук ", 0)):addItem(PhonemicItem("x"))
local im = ImageItem("interface/flask.rttex")
im.scaleX = 1.0
p1:addItem(im)
p1:addItem(TextBoxItem(" очень короткий, его ни в коем случае нельзя тянуть.", 0))
scene.content:add(p1)
scene.content:link(p1, 0, nil, scene.content, 0, nil, -1, -1)
scene.content:link(p1, 1, nil, scene.content, 1, nil, -2, -2)
scene.content:link(p1, nil, 0, scene.content, nil, 0, -3, -3)

t = TableLayout(4,3)
	t:add(TextItem("once"), 1, 1)
	t:add(TextItem("upon"), 1, 2)
	t:add(TextItem("a"), 1, 3)
	t:add(TextItem("time"), 2, 1)
	t:add(TextItem("there"), 2, 2)
	t:add(TextItem("was"), 2, 3)
	t:add(TextItem("a"), 3, 1)
	t:add(TextItem("little"), 3, 2)
	t:add(TextItem("but"), 3, 3)
	t:add(TextItem("proud"), 4, 1)
	t:add(TextItem("kingdom"), 4, 2)
	t:add(TextItem("et cetera"), 4, 3)
scene.content:add(t)
scene.content:link(t, 0.5, nil, scene.content, 0.5, nil)
scene.content:link(t, nil, 0, p1, nil, 1, nil, Explain.paragraph_interval)
	
p2 = FlowLayout()
p2:addItem(TextBoxItem("Буква «а» в закрытом слоге читаетс как [?]. Такого звука в русском зыке нет. Это нечто среднее между русскими «а» и «э». Чтобы произнести этот звук правильно, широко раскрой рот так, будто хочешь сказать «а», и скажи «э». Звук [?] очень короткий, его ни в коем случае нельз тнуть.", 0))
local mo = Mover(ImageItem("interface/flask.rttex"))
mo.onMove = function() mo.parent:requestLayOut() end
p2:addObstacle(mo, 16, 16, "right")
scene.content:add(p2)
scene.content:link(p2, nil, 0, t, nil, 1, 0, Explain.paragraph_interval)
scene.content:link(p2, 0, nil, p1, 0, nil)
scene.content:link(p2, 1, nil, p1, 1, nil)

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