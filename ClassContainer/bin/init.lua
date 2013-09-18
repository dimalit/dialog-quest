-- all "test_XXX.lua" scripts create global variable "scene"
-- and add its contents to root

-- declare functions for tests
-- first of them will be run at the bottom
local test_explain_rel, tes_basket, test_baloons

test_explain_rel = function()
	dofile("test_explain_rel.lua")
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

-- run it!
test_explain_rel()