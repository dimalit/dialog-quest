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
p1:addItem(TextBoxItem("Буква «а» в закрытом слоге читается как ", 0, 0)):addItem(VoiceTextItem("man")):addItem(TextBoxItem(". Такого звука в русском языке нет. Это нечто среднее между русскими «а» и «э». Чтобы произнести этот звук правильно, широко раскрой рот так, будто хочешь сказать «а», и скажи «э». Звук ", 0, 0)):addItem(PhonemicItem("x")):addItem(TextBoxItem(" очень короткий, его ни в коем случае нельзя тянуть."))
local im = ImageItem("interface/flask.rttex")
im.scaleX = 1.5
p1:addItem(im)
scene.content:add(p1)
--p1:setLocationOrigin(scene.content, 0.5, 0)
--p1:setWidthOrigin(scene)
p1.rel_hpy = 0
scene.content:link(p1, 0, nil, scene.content, 0, nil, -1, -1)
scene.content:link(p1, 1, nil, scene.content, 1, nil, -2, -2)
scene.content:link(p1, nil, 0, scene.content, nil, 0, -3, -3)

p2 = FlowLayout()
p2:addItem(TextBoxItem("Буква «а» в закрытом слоге читаетс как [?]. Такого звука в русском зыке нет. Это нечто среднее между русскими «а» и «э». Чтобы произнести этот звук правильно, широко раскрой рот так, будто хочешь сказать «а», и скажи «э». Звук [?] очень короткий, его ни в коем случае нельз тнуть."))
p2:addObstacle(ImageItem("interface/flask.rttex"), 16, 16, "right")
scene.content:add(p2)
--p2:setLocationOrigin(p1, 0.5, 1)
--p2:setWidthOrigin(p1)
--p2.rel_hpy = 0
--p2:rel_y(10)
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
scene:start()