-- таблица действий
local tasks = {
  [1] = {image = "doll/normal.jpg",  snd="ding.wav", show="A",  check="a",  right_snd = "explosion.wav", wrong_snd = "miss.wav", right_goto=2, wrong_goto = 1},
  [2] = {image = "ball/normal.jpg",  snd="ding.wav", show="J",  check="j",  right_snd = "explosion.wav", wrong_snd = "miss.wav", right_goto=3, wrong_goto = 2},
  [3] = {image = "flask/normal.png", snd="ding.wav", show="AJ", check="aj", right_snd = "explosion.wav", wrong_snd = "miss.wav", right_goto=4, wrong_goto = 1},
  [4] = {image = "book/normal.png",  snd="ding.wav", show="JA", check="ja", right_snd = "explosion.wav", wrong_snd = "miss.wav", right_goto=5, wrong_goto = 1},
  [5] = {image = "bike/normal.jpg",  snd="ding.wav", show="F",  check="f",  right_snd = "explosion.wav", wrong_snd = "miss.wav", right_goto=6, wrong_goto = 5},
  [6] = {image = "book/normal.png",  snd="ding.wav", show="AF", check="af", right_snd = "explosion.wav", wrong_snd = "miss.wav", right_goto=0, wrong_goto = 5},
}

scene = Shooter{
  table=tasks,
  margin = 10,
  plate="bullet.png", break_ani = "explosion",
  shelf_plate = "flask/normal.png",
  subtitle = true,
  start_x = 0, start_y = 500,
  vx = 400, vy = -400,
  gravity = 300
}

scene.onFinish = function(self)
  print("onFinish")
  self:destroy()
end

scene:start()