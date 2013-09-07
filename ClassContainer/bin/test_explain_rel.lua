Explain.margin = 20
Explain.paragraph_interval = 10

scene = Explain()

scene.title.text = "Unit 2 шаг1"

local p1 = FlowLayout(0,50)
p1:addItem(TextBoxItem("Буква «а» в закрытом слоге читаетс как [?]. Такого звука в русском зыке нет. Это нечто среднее между русскими «а» и «э». Чтобы произнести этот звук правильно, широко раскрой рот так, будто хочешь сказать «а», и скажи «э». Звук [?] очень короткий, его ни в коем случае нельз тнуть."))
p1:addItem(ImageItem("interface/flask.rttex"))
scene.content:add(p1)
p1:setLocationOrigin(scene.content, 0.5, 0)
p1:setWidthOrigin(scene)
p1.rel_hpy = 0

p2 = FlowLayout()
p2:addItem(TextBoxItem("Буква «а» в закрытом слоге читаетс как [?]. Такого звука в русском зыке нет. Это нечто среднее между русскими «а» и «э». Чтобы произнести этот звук правильно, широко раскрой рот так, будто хочешь сказать «а», и скажи «э». Звук [?] очень короткий, его ни в коем случае нельз тнуть."))
p2:addObstacle(ImageItem("interface/flask.rttex"), 16, 16, "right")
scene.content:add(p2)
p2:setLocationOrigin(p1, 0.5, 1)
p2:setWidthOrigin(p1)
p2.rel_hpy = 0
p2.rel_y = 10

local t = TextItem("sample text")
	scene.content:add(t)
	t.rel_hpy = 0
	t:setLocationOrigin(p2, 0.25, 1)
	t.rel_y = 20
local i = ImageItem("interface/flask.rttex")
	scene.content:add(i)
	i.rel_hpy = 0
	i:setLocationOrigin(p2, 0.75, 1)
	i.rel_y = 30

scene.onFinish = function()
	scene.visible = false
	root:remove(scene)
	print("Finished!")
end

scene:start()