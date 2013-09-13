scene = Baloons()

scene.launch_speed = 50
scene.max_speed = 150

scene.onscreen_count = 5

scene.launch_points = {100, 200, 300, 400, 500}
scene.launch_location_policy = "choose"

-- scene.left_margin = 100
-- scene.right_margin = 100
-- scene.launch_location_policy = "random"

scene.baloons
	:add{"A", "audio/enter.wav"}
	:add{"B", "audio/ding.wav"}
	:add{"C", "audio/click.wav"}
	:add{"A", "audio/enter.wav"}
	:add{"B", "audio/ding.wav"}
	:add{"C", "audio/click.wav"}
	:add{"A", "audio/enter.wav"}
	:add{"B", "audio/ding.wav"}
	:add{"C", "audio/click.wav"}
	:add{"A", "audio/enter.wav"}
	:add{"B", "audio/ding.wav"}
	:add{"C", "audio/click.wav"}
	:add{"A", "audio/enter.wav"}
	:add{"B", "audio/ding.wav"}
	:add{"C", "audio/click.wav"}
	:add{"A", "audio/enter.wav"}
	:add{"B", "audio/ding.wav"}
	:add{"C", "audio/click.wav"}
	:add{"A", "audio/enter.wav"}
	:add{"B", "audio/ding.wav"}
	:add{"C", "audio/click.wav"}
	:add{"A", "audio/enter.wav"}
	:add{"B", "audio/ding.wav"}
	:add{"C", "audio/click.wav"}
	:add{"A", "audio/enter.wav"}
	:add{"B", "audio/ding.wav"}
	:add{"C", "audio/click.wav"}
	:add{"A", "audio/enter.wav"}
	:add{"B", "audio/ding.wav"}
	:add{"C", "audio/click.wav"}
	:add{"A", "audio/enter.wav"}
	:add{"B", "audio/ding.wav"}
	:add{"C", "audio/click.wav"}
	:add{"A", "audio/enter.wav"}
	:add{"B", "audio/ding.wav"}
	:add{"C", "audio/click.wav"}
	
scene:start()