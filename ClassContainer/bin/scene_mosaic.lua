local take = function(drops, w)
  w.onDrag = function(obj, dx, dy)
    onDrag(drops, w)
  end
  w.onDragEnd = function()
		print("drag end")
		onDrop(drops, w)
  end
  return w
end

Mosaic = {
	margin = 20,
	line_interval = 1.5
}
setmetatable(Mosaic, {})
getmetatable(Mosaic).__call = function(_,conf)
  -------- general vars --------
  if conf == nil then conf = {} end
  local self = CompositeItem()
	self.x, self.y = screen_width/2, 0
  self.width, self.height = screen_width, screen_height
  self.rel_hpy = 0
	root:add(self)
	
  -- copy everything to self
  for k, v in pairs(conf) do self[k] = v end
  conf = nil
  
  -- show general description
  self.title = TextItem("self.title")
	self:add(self.title)
  self.description = FlowLayout(self.width-Mosaic.margin*2);
	self:add(self.description)
  self.description.rel_hpy = 0

	-- NOTE First arg is mandatory because such way inherit(...) works!!!
  local onTaskFinish = function(_, right_cnt, wrong_cnt, hint_cnt)
		self.right_cnt = self.right_cnt + right_cnt
		self.wrong_cnt = self.wrong_cnt + wrong_cnt
		self.hint_cnt = self.hint_cnt + hint_cnt
		self:next_task()
  end    
  
  self.tasks = {}
  self.tasks.add = function(_, t)
		t.visible = false
		t.rel_hpy = 0
		t.onFinish = onTaskFinish
		table.insert(self.tasks, t)
		self:add(t)
  end
  
  self.current_task = 1
  
  self.right_cnt = 0
  self.wrong_cnt = 0
  self.hint_cnt  = 0  

  self.onRequestLayOut = function(_, child)
		self.title.x = self.width/2
	  self.title.y = Mosaic.margin + self.title.height
		self.description.x = self.width/2
		self.description.y = self.title.y + self.title.height/2
		-- TODO Function add() is also in this table - so when doing pairs() it also appears :(
		for _,t in ipairs(self.tasks) do
			t.x = self.width / 2
			t.y = self.description.y + self.description.height + Mosaic.margin
		end
	end
	self:onRequestLayOut()	
	
  -------- public functions --------
  self.show_results = function(dummy)
		self:clear()
		self.title.text = "Results"
		
		-- !!! putting to self exclusively not to be garbage-collected
		local dy = self.title.height*Mosaic.line_interval
		local y = self.title.y + self.title.height + Mosaic.margin
		local x = screen_width/2 - 150
		self.completed = TextItem("Completed tasks        "..(self.right_cnt+self.wrong_cnt))
			self.completed.x, self.completed.y = x, y
			self:add(self.completed)
			self.completed.rel_hpx = 0
			y = y + dy
		self.c_right = TextItem("Completed right       "..(self.right_cnt))
			self.c_right.x, self.c_right.y = x, y
			self:add(self.c_right)
			self.c_right.rel_hpx = 0
			y = y + dy
		self.wrong = TextItem("Completed wrong     "..(self.wrong_cnt))
			self.wrong.x, self.wrong.y = x, y
			self:add(self.wrong)
			self.wrong.rel_hpx = 0
			y = y + dy		
		self.hints = TextItem("Hints used                   "..(self.hint_cnt))
			self.hints.x, self.hints.y = x, y
			self:add(self.hints)
			self.hints.rel_hpx = 0
			y = y + dy		
	end
		
	self.next_task = function(dummy)
		self.tasks[self.current_task].visible = false
		
		self.current_task = self.current_task + 1
		if self.current_task > #self.tasks then
			self:show_results()
			return
		end
		
		self.tasks[self.current_task].visible = true
  end
  
  self.clear = function(self)
		for i=1,#self.tasks do
			self.tasks[i].visible = false
		end
		self.title.text=""
		self.description:clear()
		self.description.clearObstacles()
  end
  
  self.start = function(self)
		-- TODO Zero everything and make tasks invisible
		self.tasks[1].visible = true
  end
  
  self.destroy = function(self)
    self:clear()
  end
  
  return self
end -- Mosaic()

Mosaic.Task = {}
setmetatable(Mosaic.Task, {})
getmetatable(Mosaic.Task).__call = function(_, task)
  local self = CompositeItem()
  self.width = screen_width			-- HACK Need some logic behind CompositeItem resize!
  
  -------- specific vars --------
  self.assignment = TextItem("assignment")
	self.assignment.x, self.assignment.y = screen_width/2, 0
	self:add(self.assignment)
  self.assignment.rel_hpy = 0
  self.hint_cnt = 0  
  
  local labels = {}  
  local drops = {}
  local buttons = {}
  local movers = {}
  
--  local src_drops = {}
  local dst_drops = {}
  local sounds = {}

  -------- private functions --------  
  local intersects = function(a, b)
    local overlays = function(ax1, ax2, bx1, bx2)
      return min(bx1, bx2) < max(ax1, ax2) and max(bx1, bx2) > min(ax1, ax2)
    end
    return overlays(a.left, a.right, b.left, b.right) and overlays(a.top, a.bottom, b.top, b.bottom)
  end
  
  local conflicts_with_placed = function(mover, placed_movers)
	local res = false
	for _, m in ipairs(placed_movers) do
		-- TODO Measure DISTANCE - not just overlapping
		if intersects(mover, m) then return false end
	end
	return true
  end  
  
  local placeMoversRandomly = function(height)
	local left = drops[1].width/2
	local right = screen_height - drops[1].width/2
	local top = buttons[1].y
	local bottom = screen_height - Mosaic.margin - movers[1].hpy
	local vert_middle = buttons[#buttons-1].bottom + Mosaic.margin + movers[1].hpy
	local horz_middle = buttons[1].right + Mosaic.margin + movers[1].hpx
	if height ~= nil then bottom = top + height - buttons[1].height end
	
	local placed_movers = {}
	for _,mover in pairs(movers)
	do
		repeat
			mover.y = top + rand()*(bottom-top)
			if height ~= nil then
				mover.x = buttons[1].right + Mosaic.margin + mover.hpx
			elseif mover.y > vert_middle then
				mover.x = left + rand()*(right-left)
			else
				mover.x = horz_middle + rand()*(right-horz_middle)
			end			
		until conflicts_with_placed(mover, placed_movers)
		table.insert(placed_movers, mover)
	end -- for mover
  end
  
  local check_task_finish = function()
		local right_cnt = 0
		local wrong_cnt = 0
		for i = 1, #dst_drops do
			if dst_drops[i].object and i == dst_drops[i].object.right_drop_id then
			right_cnt = right_cnt + 1
			elseif dst_drops[i].object then		-- only if has object!
			wrong_cnt = wrong_cnt + 1
			end --if
		end -- for
	--	print(right_cnt, wrong_cnt)
		
		-- if finished
		local finished = right_cnt+wrong_cnt == #dst_drops
		if finished and self.onFinish ~= nil then
			self:onFinish(right_cnt, wrong_cnt, self.hint_cnt)
		end
  end	
  
--self.ask = function(self, task)
  do
		self.assignment.text = task.assignment
		local max_mover_width = 0

		-- generate movers
		local permut = random_permutation(#task.lines)
		for i = 1, #permut do
			local line = task.lines[permut[i]]
			local mover = take(drops, PackAsDragDrop(TextItem(line[3])))
			self:add(mover)
			table.insert(movers, mover)
			
			-- check for max
			if mover.width > max_mover_width then
				max_mover_width = mover.width
			end
			
			-- remember my number
			mover.right_drop_id = permut[i]
			
			-- handle drop event
			local old_handler = mover.onDragEnd
			mover.onDragEnd = function(dummy)
				if old_handler~=nil then old_handler(mover) end
				if mover.drop == dst_drops[mover.right_drop_id] and mover.drop.is_dst then			-- if wrong
					-- sound
					sounds[permut[i]]:play()
				end
				check_task_finish()
			end -- onDragEnd
		end --for 
		
		-- generate labels, buttons and places
		for k, line in pairs(task.lines)
		do
			local button = PackAsButton(TwoStateAnimation(Animation(load_config("Start.anim"))))
			button.x, button.y = screen_width / 2, 0
			self:add(button)
			local twostate = TwoStateAnimation(
			  -- TODO: Here 10 was Mosaic.margin. How to use it here?
				FrameItem("interface/frame"),--, max_mover_width + 10, 30),
				FrameItem("interface/frame_glow")--, max_mover_width + 10, 30)
			)	  
			local drop_dst = DropArea(twostate)
			self:add(drop_dst)
			local label = TextItem(line[1])
			self:add(label)
			local snd = SoundEffect(line[2])
			
			drop_dst.is_dst = true			-- needed in checking
				button.onClick = function(dummy)
			if not button.pressed_before then
				self.hint_cnt = self.hint_cnt + 1
			end
			button_pressed_before = true
			snd:play()
				end
		
			table.insert(labels, label)
			table.insert(drops, drop_dst)
			table.insert(dst_drops, drop_dst)
			table.insert(buttons, button)
			table.insert(sounds, snd)
		end -- for buttons
	end -- do
	
	self.onRequestLayOut = function(_)
		local y = self.assignment.y + self.assignment.height + Mosaic.margin
		local dy = self.assignment.height * Mosaic.line_interval
		
		for k=1,#labels do
		
			buttons[k].y = y
		
			if k==1 then			-- first button is aligned by top
				buttons[k].y = buttons[k].y + buttons[k].height/2
				y = y + buttons[k].height/2
			end		
			
			dst_drops[k].x = buttons[k].x - buttons[k].width/2 - Mosaic.margin - dst_drops[k].width/2
			dst_drops[k].y = y		
			
			labels[k].x = dst_drops[k].x - dst_drops[k].width/2 - Mosaic.margin - labels[k].width/2
			labels[k].y = y
			y = y + dy
		end
		-- adjust mover position
		for i=1, #movers do
			movers[i].x, movers[i].y = buttons[i].right + Mosaic.margin + movers[i].width/2, buttons[i].y
		end -- for movers
		
		if task.movers_placement=="random" then
			placeMoversRandomly()
		elseif type(task.movers_placement)=="table" and task.movers_placement[1]=="random vertical" then
			placeMoversRandomly(task.movers_placement[2])
		elseif task.movers_placement=="default" or task.movers_placement==nil then
		else
			error("Wrong movers_placement!");
		end -- if movers_placement
  end -- lay_out
	self:onRequestLayOut()
  
  return self
end -- Mosaic.Task()