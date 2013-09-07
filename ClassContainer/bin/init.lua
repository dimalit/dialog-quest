dofile("test_basket.lua")
--dofile("test_explain_rel.lua")
--dofile("test_mosaic.lua")
--dofile("test_layout_agent.lua")
--dofile("test_flow.lua")
--dofile("test_box.lua")

-- s = CompositeItem()
-- s.height  = 16

t = TextItem("hello")
t.x, t.y = 100, 100
root:add(t)

m = Mover(t)