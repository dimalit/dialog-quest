page:setLayoutParameter("margin", 20)
page.background.texture = "interface/summary_bg.rttex"

local p1 = TextBoxItem("В этом уроке вы узнаете")
p1.id = "p1"
page.content:addAtBottom(p1, 0, "left")

local r12 = VerticalLayout{
	TextBoxItem("I.	Как читаются буквы m, n, l, f, v, b, p, k, d, t, e, буквосочетание ее"),
	TextBoxItem("Как произносятся звуки:"),
	TextBoxItem("согласные  [m], [n], [l], [f], [v], [b], [p], [k], [d], [t]"),
	TextBoxItem("гласные [e], [J]"),
	TextBoxItem("II.	Что такое транскрипция и зачем она нужна"),
	TextBoxItem("III.	Что такое открытый и закрытый слог и почему нужно их различать")
} -- r12

local tab = Table({0, 0}, {0, 1}, 
	{
		{"Чтение",		r12},
		{"Грамматика",	TextBoxItem("IV.	Как дать команду (повелительное наклонение)")}
	}
)

tab:setFrame("interface/frame_glow")
tab:setCellFrames("interface/frame")

tab:equalizeRows(false)
tab:equalizeColumns(false)
--tab:fixRow(1, 40)
--tab:fixColumn(2, 120)

page.content:addAtBottom(tab, 20, "center")
	
page.debugDrawBox = true
