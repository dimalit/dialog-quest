Explain.paragraph_interval = 10

scene = Explain()

scene.title.text = "Unit 2 шаг1"

local p
p = FlowLayout()
p:addItem(TextBoxItem("Буква «а» в закрытом слоге читаетс как [?]. Такого звука в русском зыке нет. Это нечто среднее между русскими «а» и «э». Чтобы произнести этот звук правильно, широко раскрой рот так, будто хочешь сказать «а», и скажи «э». Звук [?] очень короткий, его ни в коем случае нельз тнуть."))
scene.paragraphs:add(p)

p = FlowLayout()
p:addItem(TextBoxItem("Буква «а» в закрытом слоге читаетс как [?]. Такого звука в русском зыке нет. Это нечто среднее между русскими «а» и «э». Чтобы произнести этот звук правильно, широко раскрой рот так, будто хочешь сказать «а», и скажи «э». Звук [?] очень короткий, его ни в коем случае нельз тнуть."))
scene.paragraphs:add(p)

scene.onFinish = function()
	scene.visible = false
	root:remove(scene)
	print("Finished!")
end

scene:start()