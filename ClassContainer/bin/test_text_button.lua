local box = CompositeItem()
box.id = "box"
root:add(box)

root:link(box, 0, 0, root, 0, 0)
root:link(box, 1, 0, root, 1, 0)

local b = TextButton{"Мне понятно", "btn_rect.anim", shrink = true}
box:add(b)

box:link(b, 0.5, 0, box, 0.5, 0)
box:restrict(Expr(box, "height"), ">=", Expr(b, "height"))

local dummy = TextItem("Hello")
dummy.id="Hello"
root:add(dummy)
root:link(dummy, 0, 0, box, 0, 1)	-- to use box's height

root.debugDrawBox = true