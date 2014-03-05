scene = Baloons()
root:add(scene)

--scene.background.texture = "interface/start2.rttex"

scene.mistake_sound = "audio/ding.wav"

-- fly or not?
scene.fly = false
-- interval between lines of standing objects
Baloons.lines_interval = 200

-- px/sec, not valid if scene.flt==false
scene.launch_speed = 50
scene.max_speed = 150

-- how many baloons simultaneusly on screen
scene.onscreen_count = 5

-- scene.launch_location_policy may be "choose" or "random"[]
-- one of these should be uncommented:

-- 1
scene.launch_points = {100, 200, 300, 400, 500}
scene.launch_location_policy = "choose"

-- 2
-- scene.left_margin = 100
-- scene.right_margin = 100
-- scene.launch_location_policy = "random"

-- uncomment to set default image
--Baloons.mover_image = "interface/flask.rttex"
--Baloons.mover_image = "interface/p_plus.rttex"

scene.baloons
	:add{text="A", sound="audio/enter.wav", image="interface/p_plus.rttex"}
	:add{text="B", image="interface/flask.rttex"}
	:add{text="enter", answer="audio/enter.wav"}
	:add{text="click", answer="audio/click.wav", }
	:add{text="C", sound="audio/click.wav"}
	:add{text="A", image="interface/flask.rttex"}
	:add{image="interface/flask.rttex"}
	:add{text="B"}
	:add{text="ding", sound="audio/ding.wav", answer="audio/ding.wav", image="interface/p_plus.rttex"}
	:add{text="C", image="interface/p_plus.rttex"}
	:add{text="A"}
scene:start()

wait_for(scene, "onFinish")
scene.visible = false
root:remove(scene)