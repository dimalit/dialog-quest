ti = TextureItem(root, 0, 0, 100, 100, "interface/menu_bg.rttex")

c = FlowLayoutItem(root, 100, 100, 400)
t1 = TextBoxItem(nil, 0, 0, 0, "Here is a long long description of the scene as a whole. It consists of multiple 'tasks' each with its ow words ")
t2 = TextBoxItem(nil, 0, 0, 0, " Short text")
b = Button(nil, 0, 0, TwoStateAnimation(Animation(load_config("Start.anim"))))
c:addItems({t1, b, t2});

im = ImageItem(nil, 20, 0, "interface/flask.rttex")
c:addObstacle(im)
--c:addItem(t1)