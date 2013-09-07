-- set global scene parameters --
--Basket.smth = smth

scene = Basket()
scene.title.text = "Hello, Basket"

scene:addRightWords{"Alpha", "Beta", "Gamma"}
scene:addLeftWords{"A", "B", "C", "D"}
scene:addDummyWords{"Á", "Ö"}

scene:placeWordsRandomly()