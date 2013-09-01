button = Button(TwoStateAnimation(Animation(load_config("Start.anim"))))
frame = FrameItem("interface/frame", 15, 15)

box = VBox(100, 100)
box.hpx_relative, box.hpy_relative = 0, 0

root:add(box)
box:add(button):add(frame)

box.width = 200

my_root = MakeLayoutAgent(ScreenItem())
my_root.width = screen_width
my_root.height = screen_height
my_root.hpx_relative, my_root.hpy_relative = 0,0
root:add(my_root)

box = MakeLayoutAgent(box)
box:setWidthOrigin(my_root)
box:setLocationOrigin(my_root, 0.5, 0)