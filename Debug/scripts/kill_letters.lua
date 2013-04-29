local letters = {}

-- kills typed letter if it exists
function global_char_callback(code)
  local ch = string.char(code)
  -- kill letter with that code
  if letters[ch] then
    letters[ch].onFrame = nil;
    letters[ch].visible = false
    letters[ch] = nil
    print("Killed: "..ch)
  end
end

-- called each frame to move a letter a bit
local moveLetter = function(s, dt)
  s.x  = s.x  + s.vx * dt
  s.y  = s.y  + s.vy * dt
  s.vy = s.vy + 250 * dt

  -- kill out of screen
  local lim = 2048
  if(s.x > lim or s.x < -100 or s.y > lim or s.y < -100)
  then
    s.onFrame = nil
    letters[s.text] = nil
  end
end

-- each 1 sec we generate new letter
timer_1sec = Timer(function(t)

  -- make new letter, repeat if already exists
  local ch
  repeat
    local n = rand() * 25
    ch = string.char(string.byte('a') + n)
  until letters[ch] == nil

  print("Generated: "..ch)
  local o = MakeMover(TextWidget(ch, 20, 768, "courier20b"))
  o.vx = 150 + (rand() - 0.5)*50
  o.vy = -500
  o.onFrame = moveLetter
  letters[ch] = o
  o:start()
  t:restart(1)
end)

timer_1sec:start()

