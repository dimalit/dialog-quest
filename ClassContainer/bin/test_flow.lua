ti = TextureItem("interface/menu_bg.rttex", 400, 400, 0, 0)
	ti.hpx_relative, ti.hpy_relative = 0, 0
	root:add(ti)

c = FlowLayout(400, 200, 0)
	c.hpy_relative=0
--c = CompositeItem(200, 200)
--	c.width, c.height = 400, 400
	root:add(c)
t1 = TextBoxItem("Here is a long long description of the scene as a whole. It consists of multiple 'tasks' each with its own words ", 400, -200, 0)
	t1.hpx_relative, t1.hpy_relative = 0, 0	
t2 = TextBoxItem(" Short text")
b1 = Button(TwoStateAnimation(Animation(load_config("Start.anim"))))
b2 = Button(TwoStateAnimation(Animation(load_config("Start.anim"))))
	c:addItems({t1, b1, t2, b2});

im = ImageItem("interface/flask.rttex", 0, 0)
	im.hpx_relative, im.hpy_relative = 0, 0
--	c:addObstacle(im)