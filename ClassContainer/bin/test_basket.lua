-- set global scene parameters --
--Basket.smth = smth
WordsPlacement.top_margin = 34
WordsPlacement.spacing = 5

scene = Basket()
root:add(scene)

scene.background.texture = "interface/menu_bg.rttex"

scene.title.text = "Hello, Basket"
scene.left_title.text = "Greek letters"
scene.right_title.text = "Latin letters"

scene:addRightWords{"Alpha", "Beta", "Gamma"}
scene:addLeftWords{"A", "B", "C", "D"}
--scene:addDummyWords{"Á", "Ö"}

scene:placeWordsRandomly()
