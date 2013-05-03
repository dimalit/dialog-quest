local ding = SoundEffect("ding.wav")
ding:play()
ding.onFinish = function()
  print("ding")
end

add_layer("floor")
add_layer("objects")
add_layer("dragging")

function global_char_callback(ch)
  print(string.char(ch))
end

pics = {}			-- images
drops = {}			-- drop areas near them
take = function(self, w)
  w.over_response = true
  w.drag_response = true
  w.onDrag = function() onDrag(self, w) end
  w.onDragEnd = function() onDrop(self, w) end
  return w
end

set_layer("objects")

-- create images
pics.flask = ImageWidget("flask", 300, 20)
pics.book  = ImageWidget("book", 500, 20)

set_layer("floor")

-- create drop areas
drops.flask_dst = DropArea(300, 60);
drops.flask_src = DropArea(300, 160);
drops.book_dst = DropArea(500, 60);
drops.book_src = DropArea(500, 160);

set_layer("dragging")

-- create text movers
t_flask = take(drops, TextMover("FLASK\nFLASK"))
t_book = take(drops, TextMover("BOOK"))

-- put names into drop areas
drops.flask_src:take(t_flask)
drops.book_src:take(t_book)

-- answer checker
local function check_answer()
    if drops.flask_dst.object == t_flask and
       drops.book_dst.object  == t_book
    then
      print("Right!")
    else
      print("Wrong...")
    end
end

--sound!
local click = SoundEffect("menu.wav")

set_layer("floor")

-- create start button
start = ImageWidget("start", 400, 300)
start.click_response = true
start.onDragStart = function(self)
  click:play(20, 100)	
end

start.onDragEnd = function(self)
  click:play()
  self.checked = false
  check_answer()
end

