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
	onDrop(drops, w)
  end
  return w
end

Mosaic = function(conf)
  -------- general vars --------
  if conf == nil then conf = {} end
  local self = {}
  
  -- set supplied values or defaults
  conf.margin = conf.margin or 20
  conf.line_interval = conf.line_interval or 1.5
--  conf.description_interval =  conf.description_interval or conf.margin
  
  -- copy everything to self
  for k, v in pairs(conf) do self[k] = v end
  conf = nil
  
  -- show general description
  local title = TextItem("self.title", screen_width/2, 0)
	root:add(title)
  title.y = self.margin + title.height
  local description = FlowLayout(screen_width-self.margin*2, screen_width/2, 0);
	root:add(description)
  description.hpy_relative = 0
  description.y = title.y + title.height/2	
  self.current_task = 1
  
  -------- specific vars --------
  local assignment = TextItem("assignment", screen_width/2, 0)
	root:add(assignment)
  --description.y + description.height
  assignment.hpy_relative = 0
  
  local labels = {}  
  local drops = {}
  local buttons = {}
  local movers = {}
  
--  local src_drops = {}
  local dst_drops = {}
  local sounds = {} 
  
  self.right_cnt = 0
  self.wrong_cnt = 0
  self.hint_cnt  = 0
  -------- private functions --------
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
	if right_cnt+wrong_cnt == #dst_drops then
	  self.right_cnt = self.right_cnt + right_cnt
	  self.wrong_cnt = self.wrong_cnt + wrong_cnt
	  self:next_task()
	end
  end

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
  
  local ask = function(task)
	assignment.text = task.assignment
	local max_mover_width = 0

	-- generate movers
	local permut = random_permutation(#task.lines)
	for i = 1, #permut do
	  local line = task.lines[permut[i]]
	  local mover = take(drops, PackAsDragDrop(Text(line[3])), r0, 0)
		root:add(mover)
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
	
	-- geneate labels, buttons and places
	local y = assignment.y + assignment.height + self.margin
	local dy = assignment.height * self.line_interval
	for k, line in pairs(task.lines)
	do
	  local button = PackAsButton(TwoStateAnimation(Animation(load_config("Start.anim"))), screen_width / 2, y)
		root:add(button)
		if k==1 then			-- first button is aligned by top
			button.y = button.y + button.height/2
			y = y + button.height/2
		end
	  local twostate = TwoStateAnimation(
		FrameItem("interface/frame"),--, max_mover_width + self.margin/2, 30),
	    FrameItem("interface/frame_glow")--, max_mover_width + self.margin / 2, 30)
	  )	  
	  local drop_dst = DropArea(twostate)
		root:add(drop_dst)
		drop_dst.x = button.x - button.width/2 - self.margin - drop_dst.width/2
		drop_dst.y = y
	  local label = TextItem(line[1], -100, y)
		root:add(label)
		label.x = drop_dst.x - drop_dst.width/2 - self.margin - label.width/2
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
	  
	  y = y + dy
	end -- for buttons
	
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
  end
  
  -------- public functions --------
  self.show_results = function(dummy)
	self:clear()
	title.text = "Results"
	
	-- !!! putting to self exclusively not to be garbage-collected
	local dy = title.height*self.line_interval
	local y = title.y + title.height + self.margin
	local x = screen_width/2 - 150
	self.completed = TextItem("Completed tasks        "..(self.right_cnt+self.wrong_cnt), x, y)
		root:add(self.completed)
		self.completed.hpx_relative = 0
		y = y + dy
	self.right = TextItem("Completed right       "..(self.right_cnt), x, y)
		root:add(self.right)
		self.right.hpx_relative = 0
		y = y + dy
	self.wrong = TextItem("Completed wrong     "..(self.wrong_cnt), x, y)
		root:add(self.wrong)
		self.wrong.hpx_relative = 0
		y = y + dy		
	self.hints = TextItem("Hints used                   "..(self.hint_cnt), x, y)
		root:add(self.hints)
		self.hints.hpx_relative = 0
		y = y + dy		
  end
  
  self.next_task = function(dummy)
	self.current_task = self.current_task + 1
	if self.current_task > #self.tasks then
		self:show_results()
		return
	end
	
	destroy_items_array(labels)
	destroy_items_array(drops)
	destroy_items_array(movers)
	destroy_items_array(buttons)
	destroy_items_array(dst_drops)
	
	ask(self.tasks[self.current_task])
  end
  
  self.clear = function(self)
    -- TODO better would be: scene.clear(self)
	title.text=""
	description:clear()
	description:clearObstacles()
	assignment.text=""
	destroy_items_array(labels)
	destroy_items_array(drops)
	destroy_items_array(movers)
	destroy_items_array(buttons)
	destroy_items_array(dst_drops)
  end
  
  self.start = function(self)
    title.text = self.title
	if self.description then
		description:addItems(self.description)
	end
	if self.description.obstacles then
		for _,obst in pairs(self.description.obstacles) do
			local align = "left"
			if type(obst)=="table" and #obst==2 then
				align = obst[2]
				obst = obst[1]
			end -- if align supplied
			-- recompute relative coords
			if(align=="left") then
				obst.x = -description.hpx + obst.x
				obst.y = -description.hpy + obst.y
			else
				obst.x = description.width - obst.x - description.hpx
				obst.y = -description.hpy + obst.y
			end			
			description:addObstacle(obst, align)
		end -- for obstacles
	end -- if obstacles
	assignment.y = description.y + description.height + self.margin
	
	ask(self.tasks[self.current_task])
  end
  
  self.destroy = function(self)
    self:clear()
  end
  
  return self
end