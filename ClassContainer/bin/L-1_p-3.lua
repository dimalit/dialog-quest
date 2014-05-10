page:setLayoutParameter("margin", 20)
page.background.texture = "interface/summary_bg.rttex"

local p1 = TextBoxItem("����� b, p, k ����� ������ �� ��������������� ������� ����� � ��, �� � ��, �� � ���������� ����� ��� ������������ � ����������� (����������). �� ���� ��� ������������ ���� ������ ����� ��������� ��������� ������. ")
p1.id = "p1"
page.content:addAtBottom(p1)

local p2 = VerticalLayout()
p2.id = "p2"

p2:add(TextBoxItem("������� 1. ��������, ������ � �������:"))

local r
r = FlowLayout()
r:addItem(TextItem("�� - ")):addItem(PhonemicItem("b"))
p2:add(r)
r = FlowLayout()
r:addItem(TextItem("�� - ")):addItem(PhonemicItem("p"))
p2:add(r)
r = FlowLayout()
r:addItem(TextItem("�� - ")):addItem(PhonemicItem("k"))
p2:add(r)

page.content:addAtBottom(p2, 20)

local p3 = TextBoxItem("��������� ����������� � ������� ������, �������� ����� ������� �������, � ����������� � ���������� ������. � ����� ����� ���� ���� ����� ������������ � ����������.")
p3.id = "p3"
page.content:addAtBottom(p3, 20)
	
page.debugDrawBox = true
