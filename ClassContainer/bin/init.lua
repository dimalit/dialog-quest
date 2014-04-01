-- all "test_XXX.lua" scripts create global variable "scene"
-- and add its contents to root

--dofile("test_input_element.lua")
--wait_for(scene, "onFinish")

SoundEffect("audio/enter.wav"):play()
dofile("test_input.lua")

SoundEffect("audio/enter.wav"):play()
dofile("test_buttons.lua")

local se = SoundEffect("audio/enter.wav")
se:play()
wait_for(se, "onFinish")

SoundEffect("audio/enter.wav"):play()
dofile("s_1_1.lua")
SoundEffect("audio/enter.wav"):play()
dofile("test_explain_rel.lua")
dofile("test_baloons.lua")

local se = SoundEffect("audio/enter.wav")
se:play()
wait_for(se, "onFinish")

