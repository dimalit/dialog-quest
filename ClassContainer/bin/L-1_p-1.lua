Explain.margin = 20
Explain.paragraph_interval = 10

scene = Explain()
root:add(scene)

scene.background.texture = "interface/summary_bg.rttex"
scene.title.text = "Урок 1"

local p1 = TextBoxItem("В этом уроке вы узнаете")
scene.content:add(p1)
scene.content:link(p1, 0, nil, scene.content, 0, nil)
scene.content:link(p1, 1, nil, scene.content, 1, nil)
scene.content:link(p1, nil, 0, scene.content, nil, 0)

local tab = Table(2, 2, 
	{
		{"Чтение",		TextBoxItem("I.	Как читаются буквы m, n, l, f, v, b, p, k, d, t, e, буквосочетание ее Как произносятся звуки:  согласные  ")},
		{"Грамматика",	"мясо"}
	}
)

--tab:setFrame("interface/frame_glow")
--tab:setCellFrames("interface/frame")

tab:equalizeRows(false)
tab:equalizeColumns(false)
--tab:fixRow(1, 40)
--tab:fixColumn(2, 120)

scene.content:add(tab)
--scene.content:link(tab, 0, nil, scene.content, 0, nil)
--scene.content:link(tab, 1, nil, scene.content, 1, nil)
scene.content:link(tab, 0.5, nil, scene.content, 0.5, nil)
scene.content:restrict(Expr(tab, "width"), "<=", Expr(scene.content, "width"))
scene.content:link(tab, nil, 0, p1, nil, 1, nil, Explain.paragraph_interval)
	
scene.debugDrawBox = true
	
scene:start()

wait_for(scene, "onFinish")
scene.visible = false
root:remove(scene)