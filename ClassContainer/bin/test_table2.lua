local tab = Table(2, 2, 
	{
		{"Чтение",		TextBoxItem("I.	Как читаются буквы m, n, l, f, v, b, p, k, d, t, e, буквосочетание ее Как")},-- произносятся звуки:  согласные  ")},
		{"Грамматика",	"мясо"}
	}
)

--tab:setFrame("interface/frame_glow")
--tab:setCellFrames("interface/frame")

tab:equalizeRows(false)
tab:equalizeColumns(false)
--tab:fixRow(1, 40)
--tab:fixColumn(2, 120)

root:add(tab)
--scene.content:link(tab, 0, nil, scene.content, 0, nil)
--scene.content:link(tab, 1, nil, scene.content, 1, nil)
root:link(tab, 0.5, nil, root, 0.5, nil)
root:restrict(Expr(tab, "width"), "<=", Expr(root, "width"))
root:link(tab, nil, 0, root, nil, 0, nil, 20)

root.debugDrawBox = true