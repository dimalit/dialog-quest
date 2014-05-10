page:setLayoutParameter("margin", 20)
page.background.texture = "interface/summary_bg.rttex"

local p1 = TextBoxItem("Буквы b, p, k также похожи на соответствующие русские звуки – «б», «п» и «к», но в английском языке они произносятся с придыханием (аспирацией). То есть при произнесении этих звуков нужно энергично выдохнуть воздух. ")
p1.id = "p1"
page.content:addAtBottom(p1)

local p2 = VerticalLayout()
p2.id = "p2"

p2:add(TextBoxItem("Задание 1. Послушай, сравни и повтори:"))

local r
r = FlowLayout()
r:addItem(TextItem("«б» - ")):addItem(PhonemicItem("b"))
p2:add(r)
r = FlowLayout()
r:addItem(TextItem("«п» - ")):addItem(PhonemicItem("p"))
p2:add(r)
r = FlowLayout()
r:addItem(TextItem("«к» - ")):addItem(PhonemicItem("k"))
p2:add(r)

page.content:addAtBottom(p2, 20)

local p3 = TextBoxItem("Аспирация усиливается в ударных слогах, особенно перед долгими звуками, и ослабляется в безударных слогах. В конце слова этот звук также произносится с аспирацией.")
p3.id = "p3"
page.content:addAtBottom(p3, 20)
	
page.debugDrawBox = true
