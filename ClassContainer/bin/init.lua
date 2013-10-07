it = TextInputItem(200,0)
root:add(it)
it.x, it.y = 100, 20

-- all "test_XXX.lua" scripts create global variable "scene"
-- and add its contents to root

-- declare functions for tests
-- first of them will be run at the bottom
local test_input, test_basket, test_buttons, test_explain_rel, test_baloons, test_mosaic

test_input = function()
	dofile("test_input.lua")
	scene.onFinish = function()
		scene.visible = false
		root:remove(scene)
		test_basket()
	end
end

test_basket = function()
	dofile("test_basket.lua")
	scene.onFinish = function()
		scene.visible = false
		root:remove(scene)
		test_buttons()
	end
end

test_buttons = function()
	dofile("test_buttons.lua")
	scene.onFinish = function()
		scene.visible = false
		root:remove(scene)
		test_explain_rel()
	end
end

test_explain_rel = function()
	dofile("test_explain_rel.lua")
	scene.onFinish = function()
		scene.visible = false
		root:remove(scene)
		test_baloons()
	end
end

test_baloons = function()
	dofile("test_baloons.lua")
	scene.onFinish = function()
		scene.visible = false
		root:remove(scene)
		print("The end!")
	end
end

test_mosaic = function()
	dofile("test_mosaic.lua")
	scene.onFinish = function()
		scene.visible = false
		root:remove(scene)
		test_buttons()
	end
end

-- run it!
test_input()
--test_mosaic()