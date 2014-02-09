t = TableLayout(3, 3)
root:add(t)
t.rel_hpx, t.rel_hpy = 0, 0
root:link(t, 0, 0, root, 0, 0, 10, 20)
-- root:link(t, 1, 1, root, 0.5, 0.5)

local ti;

ti = TextItem("once")
ti.id = "once"
t:add(ti, 1, 1)

ti = TextItem("upon")
ti.id = "upon"
t:add(ti, 1, 2)

ti = TextItem("a")
ti.id = "a1"
t:add(ti, 1, 3)

ti = TextItem("time")
ti.id = "time"
t:add(ti, 2, 1)

ti = TextItem("there")
ti.id = "there"
t:add(ti, 2, 2)

ti = TextItem("was")
ti.id = "was"
t:add(ti, 2, 3)

ti = TextItem("a")
ti.id = "a2"
t:add(ti, 3, 1)

ti = TextItem("little")
ti.id = "little"
t:add(ti, 3, 2)

ti = TextItem("but")
ti.id = "but"
t:add(ti, 3, 3)
-- t:add(TextItem("proud"), 4, 1)
-- t:add(TextItem("kingdom"), 4, 2)
-- t:add(TextItem("et cetera"), 4, 3)

-- local f = FrameItem("interface/frame_glow", 10, 10)
-- t:add( f , 2, 2)
-- t:link(f, nil, 1, t.rows[2], nil, 1)
-- t:link(f, 1, nil, t.columns[2], 1, nil)

-- high-level table
local data = {
	{"",       "Продукты", nil,    "Хозтовары", nil},
	{"",       "мясо",     "рыба", "большие",   "маленькие"},
	{"январь", 1.1,        0.7,    0.4,         0.3},
	{"февраль",1.2,        0.6,    0.4,         0.3},
	{"всего",  2.3,        1.2,    0.8,         0.6},
	{nil,      1.8,        nil,    1.4,         nil},
}
--tt = TableLayout(1,2)
--tt = Table(3, 3, data)

-- tt.equalizeRows = true
-- tt.equalizeColumns = false
-- tt.fixRow(1, 30)
-- tt.fixColumn(3, 80)

-- root:add(tt)
-- root:link(tt, 0, 0, root, 0.25, 0.25)
-- root:link(tt, 1, 1, root, 0.5, 0.5)