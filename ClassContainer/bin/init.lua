-- all "test_XXX.lua" scripts create global variable "scene"
-- and add its contents to root

run_page("L-1_p-1.lua")
run_page("L-1_p-3.lua")
--dofile("init_test_scenes.lua")

--dofile("test_input_element.lua")
--dofile("test_table2.lua")

--coroutine.yield()
print("END")