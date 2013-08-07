ti = TextureItem(50, 50, 100, 100, "interface/menu_bg.rttex")
	root:add(ti)

c = FlowLayoutItem(300, 100, 400)
	c.hpx_relative, c.hpy_relative = 0, 0
	root:add(c)
t1 = TextBoxItem(0, 0, 0, "Here is a long long description of the scene as a whole. It consists of multiple 'tasks' each with its ow words ")
t2 = TextBoxItem(0, 0, 0, " Short text")
b = Button(0, 0, TwoStateAnimation(Animation(load_config("Start.anim"))))
	c:addItems({t1, b, t2});

im = ImageItem(0, 0, "interface/flask.rttex")
	im.hpx_relative, im.hpy_relative = 0, 0
	c:addObstacle(im)