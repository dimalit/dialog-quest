Explain.margin = 20
Explain.paragraph_interval = 10

scene = Explain()

scene.title.text = "Unit 2 шаг1"

local p

p = FlowLayout(0,50)
p:addItem(TextBoxItem("Буква «а» в закрытом слоге читаетс как [?]. Такого звука в русском зыке нет. Это нечто среднее между русскими «а» и «э». Чтобы произнести этот звук правильно, широко раскрой рот так, будто хочешь сказать «а», и скажи «э». Звук [?] очень короткий, его ни в коем случае нельз тнуть."))
p:addItem(ImageItem("interface/flask.rttex"))
scene.paragraphs:add(p)

p = FlowLayout()
p:addItem(TextBoxItem("Буква «а» в закрытом слоге читаетс как [?]. Такого звука в русском зыке нет. Это нечто среднее между русскими «а» и «э». Чтобы произнести этот звук правильно, широко раскрой рот так, будто хочешь сказать «а», и скажи «э». Звук [?] очень короткий, его ни в коем случае нельз тнуть."))
p:addObstacle(ImageItem("interface/flask.rttex"), 16, 16, "right")
scene.paragraphs:add(p)

p = HBox(200, 50)
scene.paragraphs:add(p)
local t = MakeLayoutAgent(TextItem("sample text"))
	local ts = MakeLayoutAgent(ScreenItem())
	t:setLocationOrigin(ts, 0.5, 0.5)
	scene:add(t)
local i = MakeLayoutAgent(ImageItem("interface/flask.rttex"))
	local is = MakeLayoutAgent(ScreenItem())
	scene:add(i)
	i:setLocationOrigin(is, 0.5, 0.5)
	scene:add(i)
p:add(ts):add(is)


scene.onFinish = function()
	scene.visible = false
	root:remove(scene)
	print("Finished!")
end

scene:start()