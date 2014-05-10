page:setLayoutParameter("margin", 20)
page.background.texture = "interface/summary_bg.rttex"

local p1 = TextBoxItem("� ���� ����� �� �������")
p1.id = "p1"
page.content:addAtBottom(p1, 0, "left")

local r12 = VerticalLayout{
	TextBoxItem("I.	��� �������� ����� m, n, l, f, v, b, p, k, d, t, e, �������������� ��"),
	TextBoxItem("��� ������������ �����:"),
	TextBoxItem("���������  [m], [n], [l], [f], [v], [b], [p], [k], [d], [t]"),
	TextBoxItem("������� [e], [J]"),
	TextBoxItem("II.	��� ����� ������������ � ����� ��� �����"),
	TextBoxItem("III.	��� ����� �������� � �������� ���� � ������ ����� �� ���������")
} -- r12

local tab = Table({0, 0}, {0, 1}, 
	{
		{"������",		r12},
		{"����������",	TextBoxItem("IV.	��� ���� ������� (������������� ����������)")}
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
