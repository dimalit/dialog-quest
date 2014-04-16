root.debugDrawBox = true
------------------------------------------------------------------------------

i1 = CompositeItem()
i1.id="i1"
root:add(i1)
root:link(i1, 0, 0, root, 0, 0, 10, 10)

ta11 = TextBoxItem("declare functions for tests some text in button")
ta11.id = "ta11"
i1:add(ta11)
i1:link(ta11, 0, 0, i1, 0, 0, 2, 2)
i1:link(i1, 1, 1, ta11, 1, 1, 2, 2)

-- restrict width or not?
root:restrict(Expr(i1, "width"), "<=", Expr(200))
--root:restrict(Expr(i1, "width"), "<=", Expr(400))
--root:restrict(Expr(i1, "width"), "==", Expr(200))

------------------------------------------------------------------------------

i2 = CompositeItem()
i2.id="i2"
root:add(i2)
root:link(i2, 0, 0, root, 0, 0.5, 10, 10)

ta21 = TextBoxItem("declare functions for tests some text in button")
ta21.id = "ta21"
i2:add(ta21)

ta22 = TextBoxItem("more some stuff and shorter")
ta22.id = "ta22"
i2:add(ta22)

i2:link(ta21, 0, 0, i1, 0, 0, 2, 2)			-- 1st - 0,0 corner
i2:link(ta22, 0, 0, ta21, 0, 1, 0, 2)		-- 2nd - under 1st

i2:link(i2, nil, 1, ta22, nil, 1, 2, 2)			-- parent's bottom
-- TODO: not very convenient! make restrict with right, left?
i2:restrict(Expr(i2, "width"), ">=", Expr(ta21, "width")+Expr(40))			-- width = max
i2:restrict(Expr(i2, "width"), ">=", Expr(ta22, "width")+Expr(40))			-- width = max

-- restrict width or not?
--root:restrict(Expr(i2, "width"), "<=", Expr(200))