Explain.paragraph_interval = 10

scene = Explain()

scene.title.text = "Unit 2 ���1"

local p
p = FlowLayout()
p:addItem(TextBoxItem("����� �� � �������� ����� ������� ��� [?]. ������ ����� � ������� ���� ���. ��� ����� ������� ����� �������� �� � ���. ����� ���������� ���� ���� ���������, ������ ������� ��� ���, ����� ������ ������� ��, � ����� ���. ���� [?] ����� ��������, ��� �� � ���� ������ ����� �����."))
scene.paragraphs:add(p)

p = FlowLayout()
p:addItem(TextBoxItem("����� �� � �������� ����� ������� ��� [?]. ������ ����� � ������� ���� ���. ��� ����� ������� ����� �������� �� � ���. ����� ���������� ���� ���� ���������, ������ ������� ��� ���, ����� ������ ������� ��, � ����� ���. ���� [?] ����� ��������, ��� �� � ���� ������ ����� �����."))
scene.paragraphs:add(p)

scene.onFinish = function()
	scene.visible = false
	root:remove(scene)
	print("Finished!")
end

scene:start()