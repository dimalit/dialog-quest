-- all "test_XXX.lua" scripts create global variable "scene"
-- and add its contents to root

-- dofile("test_text_button.lua")
local solver = Cassowary()

local button = {}
button.id = "Button"
button.height = 2

local text = {}
text.id = "but_text_item"
text.height = 3

print "Adding constraint"
--solver:addConstraint(Expr(obj, "x")+Expr(obj, "width"), "<=", Expr(obj2, "width")-Expr(1))
solver:addConstraint(Expr(text, "height"), "==", Expr(button, "height"))

print "Solving"
solver:solve()

print "Insist on height=3"
text.height = 3

print "Solving"
solver:solve()

coroutine.yield()

--SoundEffect("audio/enter.wav"):play()
--dofile("test_buttons.lua")

--local se = SoundEffect("audio/enter.wav")
--se:play()
--wait_for(se, "onFinish")

--SoundEffect("audio/enter.wav"):play()
dofile("s_1_1.lua")
SoundEffect("audio/enter.wav"):play()
dofile("test_explain_rel.lua")
dofile("test_baloons.lua")

local se = SoundEffect("audio/enter.wav")
se:play()
wait_for(se, "onFinish")

SoundEffect("audio/enter.wav"):play()
dofile("test_input.lua")
