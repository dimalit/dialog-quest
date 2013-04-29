pics = {}			-- images
drops = {}			-- drop areas near them
take = function(drops, w)
  w.onDrag = function(obj, dx, dy)
    onDrag(drops, w)
  end
  w.onDragEnd = function() onDrop(drops, w) end
  return w
end

-- create images
pics.ball  = ImageItem(300, 60, "ball/normal.jpg")
pics.doll  = ImageItem(500, 60, "doll/normal.jpg")
pics.bike  = ImageItem(700, 60, "bike/normal.jpg")

-- create drop areas
drops.ball_dst = DropArea(300, 160, TwoStateAnimation(Animation("DropArea")))
drops.ball_src = DropArea(300, 260, TwoStateAnimation(Animation("DropArea")))
drops.doll_dst = DropArea(500, 160, TwoStateAnimation(Animation("DropArea")))
drops.doll_src = DropArea(500, 260, TwoStateAnimation(Animation("DropArea")))
drops.bike_dst = DropArea(700, 160, TwoStateAnimation(Animation("DropArea")))
drops.bike_src = DropArea(700, 260, TwoStateAnimation(Animation("DropArea")))

-- create text movers
t_ball = take(drops, Mover(0, 0, Text("ball")))
t_doll = take(drops, Mover(0,0, Text("doll")))
t_bike = take(drops, Mover(0, 0, Text("bike")))

-- put names into drop areas
drops.ball_src:take(t_ball)
drops.doll_src:take(t_doll)
drops.bike_src:take(t_bike)

-- answer checker
local function check_answer()
    if drops.ball_dst.object == t_ball and
       drops.doll_dst.object == t_doll and
       drops.bike_dst.object == t_bike
    then
      print("Right!")
    else
      print("Wrong...")
    end
end

-- create start button
start = ImageWidget("start", 400, 300)
start.click_response = true
start.onDragEnd = function(self)
  self.checked = false
  check_answer()
end