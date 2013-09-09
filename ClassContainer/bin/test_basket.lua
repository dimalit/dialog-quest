-- set global scene parameters --
--Basket.smth = smth
WordsPlacement.top_margin = 34
WordsPlacement.spacing = 5

scene = Basket()
scene.title.text = "Hello, Basket"
scene.left_title.text = "Greek letters"
scene.right_title.text = "Latin letters"

scene:addRightWords{"Alpha", "Beta", "Gamma"}
scene:addLeftWords{"A", "B", "C", "D"}
--scene:addDummyWords{"Á", "Ö"}

scene:placeWordsRandomly()

h = scene.horz_bar

scene.onFinish = function()
	scene.visible = false
	dofile("test_mosaic.lua")
end