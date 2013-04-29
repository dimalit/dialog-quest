function global_char_callback(ch)
  s = string.char(ch)
  local o = MakeMover(TextWidget(s, 20, 768, "courier20b"))
  o.vx = 300 + (rand() - 0.5)*200
  o.vy = -1000
  o.onFrame = function(s, dt)
    s.x  = s.x  + s.vx * dt
    s.y  = s.y  + s.vy * dt
    s.vy = s.vy + 1000 * dt
    s:rotate(10 * dt)

    -- kill out of screen
    local lim = 2048
    if(s.x > lim or s.x < -100 or s.y > lim or s.y < -100)
    then
      s.onFrame = nil
      print("killed", s.x, s.y)
    end
  end
  o:start()
end