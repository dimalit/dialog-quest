-- it = TextButton("`bHello", "btn_rect.anim")
-- it.debugDrawBox = true
-- root:add(it)
-- it.rel_hpx, it.rel_hpy = 0, 0
-- it.x, it.y = 0, 20
-- it.width = 200

-- all "test_XXX.lua" scripts create global variable "scene"
-- and add its contents to root

-- declare functions for tests
-- first of them will be run at the bottom
local test_input, test_basket, test_buttons, test_explain_rel, test_baloons, test_mosaic

test_1_1 = function()
	local se = SoundEffect("audio/enter.wav")
	se:play()
	se.onFinish = function()
		SoundEffect("audio/enter.wav"):play()
	end
	dofile("s_1_1.lua")
	scene.onFinish = function()
		scene.visible = false
		root:remove(scene)
		test_input()
	end
end

test_input = function()
	local se = SoundEffect("audio/enter.wav")
	se:play()
	se.onFinish = function()
		SoundEffect("audio/enter.wav"):play()
	end
	dofile("test_input.lua")
	scene.onFinish = function()
		scene.visible = false
		root:remove(scene)
		test_explain_rel()
	end
end

test_explain_rel = function()
	SoundEffect("audio/enter.wav"):play()
	dofile("test_explain_rel.lua")
	scene.onFinish = function()
		scene.visible = false
		root:remove(scene)
		test_basket()
	end
end

test_basket = function()
	SoundEffect("audio/enter.wav"):play()
	dofile("test_basket.lua")
	scene.onFinish = function()
		scene.visible = false
		root:remove(scene)
		test_buttons()
	end
end

test_buttons = function()
	SoundEffect("audio/enter.wav"):play()
	dofile("test_buttons.lua")
	scene.onFinish = function()
		scene.visible = false
		root:remove(scene)
		test_baloons()
	end
end

test_baloons = function()
	SoundEffect("audio/enter.wav"):play()
	dofile("test_baloons.lua")
	scene.onFinish = function()
		scene.visible = false
		root:remove(scene)
		print("The end!")
	end
end

test_mosaic = function()
	SoundEffect("audio/enter.wav"):play()
	dofile("test_mosaic.lua")
	scene.onFinish = function()
		scene.visible = false
		root:remove(scene)
		test_buttons()
	end
end

-- run it!
--test_buttons()
test_1_1()
--test_explain_rel()
--test_basket();
--test_baloons()
--test_mosaic()
