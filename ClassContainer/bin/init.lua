dofile("test_basket.lua")
--dofile("test_explain_rel.lua")
--dofile("test_mosaic.lua")

t = TextItem("hello")
t.x, t.y = 100, 100
root:add(t)
m = Mover(t)