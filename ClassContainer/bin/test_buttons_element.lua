i = CompositeItem()
i.id="i"

---------------------------------------------------

line = CompositeItem()
line.id="ButtonsElement"

t1 = TextBoxItem("declare functions for tests", 200)
t1.id="right_label"
line:add(t1)
line:link(t1, 0, 0, line, 0, 0, 2, 2)
line:link(t1, 1, nil, line, 1, nil, -2)
line:link(line, nil, 1, t1, nil, 1, 0, 5)

i:add(line)
i:link(line, nil, 0, i, nil, 0, 0, 2)
i:link(line, 0, nil, i, 0, nil, 2, 2)
i:link(line, 1, nil, i, 1, nil, -2, 0)
---------------------
-- line = ButtonsElement("", "declare functions for tests", "audio/ding.wav")
-- i:add(line)
-- i:link(line, 0, 0, i, 0, 0, 2, 2)
-- i:link(line, 1, nil, i, 1, nil, -2, 0)

-----------------------------------------------------

t2 = TextBoxItem("declare functions for tests -- first of them will be run at the bottom")
t2.id="t2"
i:add(t2)
i:link(t2, 0, 0, line, 0, 1, 0, 10)
i:link(t2, 1, nil, i, 1, nil, -2)

root:add(i)
root:link(i, 0, 0, root, 0, 0, 2, 2)
root:link(i, 1, 1, root, 0.25, 1, 0, -2)
root.debugDrawBox = true