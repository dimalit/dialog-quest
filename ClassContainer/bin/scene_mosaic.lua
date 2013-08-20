local destroy_items_array = function(arr)
	local i = table.remove(arr)
    while i do
      i:destroy()
	  i = table.remove(arr)
    end
end

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

Mosaic = {}
setmetatable(Mosaic, {})
getmetatable(Mosaic).__call = function(_,conf)
  -------- general vars --------
  if conf == nil then conf = {} end
  local self = CompositeItem(screen_width/2, 0)
  self.width, self.height = screen_width, screen_height
  self.hpy_relative = 0
	root:add(self)
  
  -- set supplied values or defaults
  conf.margin = conf.margin or 20
  conf.line_interval = conf.line_interval or 1.5
  conf.tasks_count = conf.tasks_count or 0
--  conf.description_interval =  conf.description_interval or conf.margin
  
  -- copy everything to self
  for k, v in pairs(conf) do self[k] = v end
  conf = nil
  
  -- show general description
  self.title = TextItem("self.title", screen_width/2, 0)
	self:add(self.title)
  self.title.y = self.margin + self.title.height
  self.description = FlowLayout(self.width-self.margin*2, self.width/2, 0);
	self:add(self.description)
  self.description.hpy_relative = 0
  self.description.y = self.title.y + self.title.height/2	

  local onTaskFinish = function(right_cnt, wrong_cnt, hint_cnt)
		self.right_cnt = self.right_cnt + right_cnt
		self.wrong_cnt = self.wrong_cnt + wrong_cnt
		self.hint_cnt = self.hint_cnt + hint_cnt
		self:next_task()
  end    
  
  -- HACK Really we need lay_out() and onSmthChange()
  self.description.height = 100
  
  self.tasks = {}
  self.tasks.add = function(_, t)
		t.visible = false
		t.hpy_relative = 0
		t.x = screen_width / 2
		t.y = self.description.y + self.description.height + self.margin
		t.onFinish = onTaskFinish
		t.margin = self.margin			-- temporary solution!!!
		t.line_interval = self.line_interval
		t:lay_out()
		table.insert(self.tasks, t)
		self:add(t)
  end
  
  self.current_task = 1
  
  self.right_cnt = 0
  self.wrong_cnt = 0
  self.hint_cnt  = 0  

  -------- public functions --------
  self.show_results = function(dummy)
	self:clear()
	self.title.text = "Results"
	
	-- !!! putting to self exclusively not to be garbage-collected
	local dy = self.title.height*self.line_interval
	local y = self.title.y + self.title.height + self.margin
	local x = screen_width/2 - 150
	self.completed = TextItem("Completed tasks        "..(self.right_cnt+self.wrong_cnt), x, y)
		self:add(self.completed)
		self.completed.hpx_relative = 0
		y = y + dy
	self.c_right = TextItem("Completed right       "..(self.right_cnt), x, y)
		self:add(self.c_right)
		self.c_right.hpx_relative = 0
		y = y + dy
	self.wrong = TextItem("Completed wrong     "..(self.wrong_cnt), x, y)
		self:add(self.wrong)
		self.wrong.hpx_relative = 0
		y = y + dy		
	self.hints = TextItem("Hints used                   "..(self.hint_cnt), x, y)
		self:add(self.hints)
		self.hints.hpx_relative = 0
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
--	if self.description.obstacles then
--		for _,obst in pairs(self.description.obstacles) do
--			local align = "left"
--			if type(obst)=="table" and #obst==2 then
--				align = obst[2]
--				obst = obst[1]
--			end -- if align supplied
--			-- recompute relative coords
--			if(align=="left") then
--				obst.x = -description.hpx + obst.x
--				obst.y = -description.hpy + obst.y
--			else
--				obst.x = description.width - obst.x - description.hpx
--				obst.y = -description.hpy + obst.y
--			end			
--			description:addObstacle(obst, align)
--		end -- for obstacles
--	end -- if obstacles
--	assignment.y = description.y + description.height + self.margin
	
--	ask(self.tasks[self.current_task])
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
  self.assignment = TextItem("assignment", screen_width/2, 0)
	self:add(self.assignment)
  self.assignment.hpy_relative = 0
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
	local bottom = screen_height - self.margin - movers[1].hpy
	local vert_middle = buttons[#buttons-1].bottom + self.margin + movers[1].hpy
	local horz_middle = buttons[1].right + self.margin + movers[1].hpx
	if height ~= nil then bottom = top + height - buttons[1].height end
	
	local placed_movers = {}
	for _,mover in pairs(movers)
	do
		repeat
			mover.y = top + rand()*(bottom-top)
			if height ~= nil then
				mover.x = buttons[1].right + self.margin + mover.hpx
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
		self.onFinish(right_cnt, wrong_cnt, self.hint_cnt)
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
			local mover = take(drops, Mover(Text(line[3])), r0, 0)
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
			end
		end --for 
		
		-- generate labels, buttons and places
		for k, line in pairs(task.lines)
		do
			local button = Button(TwoStateAnimation(Animation(load_config("Start.anim"))), screen_width / 2, 0)
			self:add(button)
			local twostate = TwoStateAnimation(
			  -- TODO: Here 10 was self.margin. How to use it here?
				FrameItem("interface/frame", max_mover_width + 10, 30),
				FrameItem("interface/frame_glow", max_mover_width + 10 / 2, 30)
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
	
	self.lay_out = function(_)
		local y = self.assignment.y + self.assignment.height + self.margin
		local dy = self.assignment.height * self.line_interval
		
		for k=1,#labels do
		
			buttons[k].y = y
		
			if k==1 then			-- first button is aligned by top
				buttons[k].y = buttons[k].y + buttons[k].height/2
				y = y + buttons[k].height/2
			end		
			
			dst_drops[k].x = buttons[k].x - buttons[k].width/2 - self.margin - dst_drops[k].width/2
			dst_drops[k].y = y		
			
			labels[k].x = dst_drops[k].x - dst_drops[k].width/2 - self.margin - labels[k].width/2
			labels[k].y = y
			y = y + dy
		end
		-- adjust mover position
		for i=1, #movers do
			movers[i].x, movers[i].y = buttons[i].right + self.margin + movers[i].width/2, buttons[i].y
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
  
  return self
end -- Mosaic.Task()