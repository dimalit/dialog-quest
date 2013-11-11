scene = Baloons()
root:add(scene)

scene.background.texture = "interface/start2.rttex"

-- px/sec
scene.launch_speed = 50
scene.max_speed = 150

-- how many baloons simultaneusly on screen
scene.onscreen_count = 5

-- scene.launch_location_policy may be "choose" or "random"[]
-- one of these should be uncommented:

-- 1
-- scene.launch_points = {100, 200, 300, 400, 500}
-- scene.launch_location_policy = "choose"

-- 2
scene.left_margin = 100
scene.right_margin = 100
scene.launch_location_policy = "random"

Baloons.mover_image = "interface/flask.rttex"
--Baloons.mover_image = "interface/p_plus.rttex"

-- syntax: add("text", "sound file", "right sound for this baloon")
scene.baloons
	:add{"A", "audio/enter.wav", ""}
	:add{"B", "", ""}
	:add{"enter", "", "audio/enter.wav"}
	:add{"A", "", ""}
	:add{"B", "", ""}
	:add{"C", "audio/click.wav", ""}
	:add{"click", "", "audio/click.wav"}
	:add{"B", "", ""}
	:add{"C", "", ""}
	:add{"A", "", ""}
	:add{"ding", "audio/ding.wav", "audio/ding.wav"}
scene:start()