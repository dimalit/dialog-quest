-------------- system things ----------------

-- arg = base OBJECTS
-- resulting mt will redirect all access to them
-- when accessing luabinded function it will also convert 1-st argument appropriately
-- when writing to inexistant property - will write to ALL base objects!
local function inherit(...)
  local mt = {}
	-- find key in parents
	mt.__index = function(self, key)
		for i=1, #arg do
			if arg[i][key]~=nil then
				-- handle functions!
				-- NOTE: Pure-Lua classes can handle derived instance as self
				-- binded classes - cannot
				if type(arg[i] == "userdata") and
					 type(arg[i][key]) == "function" and
					 getmetatable(arg[i]).__luabind_class~=nil
				then
					local parent = arg[i];
					-- NOTE: Need to remember it before returning function
					-- because parents[i][key] may be changed later!
					local parent_func = parent[key]
					return function(...)
--!!!						assert(arg[1] == self, "1st arg must be self")	doesn't work with 2-deep inheritance (TextButton->PackAsDragDrop)
						-- but we change it to appropriate parent
						return parent_func(parent, unpack(arg, 2))
					end -- func body
				else
					return arg[i][key]
				end -- if func
			end -- if not nil
		end -- for parents
	-- if not found
	return nil
	end

	-- find existing and do assignment
	mt.__newindex = function(self, key, val)
		for i=1, #arg do
			if arg[i][key]~=nil then
				arg[i][key]=val
				return
			end
		end
	-- set to ALL! if not found
		for i=1, #arg do
			arg[i][key] = val
		end
	end
	
	-- tag the mt with __luabinded_base
	-- HACK for now allow multiple inheritance for Item+View
	-- TODO Implement BlaBlaItem directly in C++
	for i,s in ipairs(arg) do
		-- if father
		if getmetatable(s).__luabind_class ~= nil then
			--assert(mt.__luabinded_base == nil)	-- disallow multiple inheritance
			mt.__luabinded_base = s
			break
		end -- if
		-- if grandfather
		if getmetatable(s).__luabinded_base ~= nil then
			--assert(mt.__luabinded_base == nil)	-- also disallow both mt fields
			mt.__luabinded_base = getmetatable(s).__luabinded_base
			break
		end -- if
	end
	
	return mt
end

function wait_for(obj, proc)
	local thread = coroutine.running()
	obj[proc] = function()
		coroutine.resume(thread)
	end
	coroutine.yield()
end

-- for blocked operations in init.lua
-- for some strange reason it doesn't work with built-in dofile
-- HACK!
dofile = function(f)
	local func, err = loadfile(f)
	assert(func~=nil, err)
	return func()
end

------------- Lua-aware wrappers for native classes -----------------

-- CompositeItem with children and parent
-- TODO "children" and "parent" must be write protected!
local old_CompositeItem = CompositeItem
CompositeItem = function(...)
	local self = old_CompositeItem(unpack(arg))
	self.children = {}
	
	local old_add = self.add
	self.add = function(self_or_derived, child)
		assert(self.children[child] == nil)
		self.children[child] = true
		child.parent = self_or_derived
		-- need to go to luabinded base class
		while type(child)~="userdata" do
			child = getmetatable(child).__luabinded_base
		end
		assert(child)
		old_add(self_or_derived, child)
		
		return self_or_derived
	end
	
	local old_remove = self.remove
	self.remove = function(self_or_derived, child)
		assert(self.children[child])
		self.children[child] = nil
		child.parent = nil
		-- need to go to luabinded base class
		while type(child)~="userdata" do
			child = getmetatable(child).__luabinded_base
		end
		assert(child)
		old_remove(self_or_derived, child)
		
		return self_or_derived		
	end	
	
	return self
end

-- override root also to be Lua-CompositeItem!
do
	local old_root = root
	root = CompositeItem()
	root.width, root.height = old_root.width, old_root.height
	root.rel_hpx, root.rel_hpy = 0, 0
	root.x, root.y = 0, 0
	old_root:add(root)
end

-- make adjustSize function in simple Items - specially for Cassowary
ScreenItem.adjustSize = function(_) return true, true end
AnimatedItem.adjustSize = function(_) return true, true end
TextItem.adjustSize = function(_) return false, false end
TextBoxItem.adjustSize = function(self)
	self.width = self.oneLineWidth
	return false, false
end
TextInputItem.adjustSize = function(_) return false, false end
ImageItem.adjustSize = function(_) return true, true end
TextureItem.adjustSize = function(_) return true, true end
-- TODO: add here other Items too!

-- CompositeItem with Cassowary
local old_CompositeItem = CompositeItem
CompositeItem = function(...)
	local self = old_CompositeItem(unpack(arg))
	local solver = Cassowary()
	
	-- make 2.0-strength Stay on them, so inner vals were prefereble to change
	-- TODO: No id at this moment - so var will have partial name
	-- solver:addExternalStay(self, "width")
	-- solver:addExternalStay(self, "height")
	
	-- add our own stuff

	self.adjustSize = adjustSize
	
	self.link = function(_, patient, px, py, nurse, nx, ny, dx, dy)
		if dx == nil then dx = 0 end
		if dy == nil then dy = 0 end
		
		-- move anchor to top-left for all
		patient.rel_hpx, patient.rel_hpy = 0, 0
		nurse.rel_hpx, nurse.rel_hpy = 0, 0
	
		--bad idea! adjust_link(patient, px, py, nurse, nx, ny, dx, dy)
	
		-- nurse is parent
		if nurse == patient.parent then
		  -- TODO remove other solver functions and do nicer: nurse x*0
			if px ~= nil then	solver:addEquation({patient, "x", 1, "width", px}, {nurse, "x", 0, "width", nx}, dx) end
			if py ~= nil then	solver:addEquation({patient, "y", 1, "height", py}, {nurse, "y", 0, "height", ny}, dy) end
		-- patient is parent
		elseif patient == nurse.parent then
			assert(px ~= 0 and py ~= 0)			-- cannot link top-left of parent to child!
			if px ~= nil then	solver:addEquation({patient, "x", 0, "width", px}, {nurse, "x", 1, "width", nx}, dx) end
			if py ~= nil then	solver:addEquation({patient, "y", 0, "height", py}, {nurse, "y", 1, "height", ny}, dy) end
		-- both inside container
		else
			if px ~= nil then solver:addEquation({patient, "x", 1, "width", px}, {nurse, "x", 1, "width", nx}, dx) end
			if py ~= nil then	solver:addEquation({patient, "y", 1, "height", py}, {nurse, "y", 1, "height", ny}, dy) end
		end -- if
		
		self:requestLayOut()
		return self
	end -- link()
	
	self.restrict = function(_, left, op, right)
		solver:addConstraint(left, op, right)
	end

	self.maximize = function(_, expr)
		solver:maximize(expr)
	end
	self.minimize = function(_, expr)
		solver:minimize(expr)
	end	

	-- TODO: think again about "growing" and "shrinking" containers as in GTK
	
	self.onRequestSize = function()
		print("+onRequestSize", self.id)
		local saved = {}
		
		-- ask
		solver:beginEdit()
		for ch,_ in pairs(self.children) do
			local rigid_w, rigid_h = ch.adjustSize(ch)
			print(ch.id, "wants", rigid_w, rigid_h)
			if not rigid_w then
				solver:suggestValue(ch, "width")
				table.insert(saved, {ch, "width"})
			end
			if not rigid_h then
				solver:suggestValue(ch, "height")
				table.insert(saved, {ch, "height"})
			end
		end
		
		-- adjust
		-- here we can force child for different size - and he may want to change it!
		solver:endEdit()
		
		-- add weak stays
		for _,e in ipairs(saved) do
			solver:addExternalStay(e[1], e[2])
		end
		
		-- ask children again:
		for ch,_ in pairs(self.children) do
			if ch.adjustLayout then
				ch:adjustLayout()
			end
		end		
		
		-- TODO Here we sould do loop - until everything gets constant!!
		solver:getExternalVariables()
		
		-- return booleans
		local rigid_w = self.width == 0
		local rigid_h = self.height == 0
		print("-onRequestSize", self.id, rigid_w, rigid_h)		
		return rigid_w, rigid_h
	end
	
	self.onRequestLayOut = function()
	
		print("+onRequestLayOut", self.id)
	
		solver:beginEdit()

		-- add required stays on width and height if rigid
		-- TODO not sure if ~= 0 will always work: should check what adjustSize returns
		if self.width ~= 0 then
			solver:suggestValue(self, "width")
		end
		if self.height ~= 0 then
			solver:suggestValue(self, "height")
		end

		solver:endEdit()		
		
		-- ask children again:
		for ch,_ in pairs(self.children) do
			if ch.adjustLayout then
				ch:adjustLayout()
			end
		end			
		
		solver:getExternalVariables()		-- accomodate changed height after changing width of some child
--		solver:solve()								-- updates vars in, solve and updates out
-- when solving and changing width, item may change its height
-- will solve again with no difference otherwise
--!!!		self.need_lay_out = false			-- will be raised again by child
--		print("set to false")
	print("-onRequestLayOut", self.id)
		do return end	-- our overloaded child should use editVar/editPoint!
	end
	
	-- HACK: for manual onRequesrLayOut. Generally should be private!
	self.solver = solver
	
	return self
end -- CompositeItem:__init

-- override root again!
do
	local old_root = root
	root = CompositeItem()
	root.rel_hpx, root.rel_hpy = 0, 0
	root.x, root.y = 0, 0
	root.width, root.height = old_root.width, old_root.height
	old_root:add(root)
	root.id = "root"
	root:link(root, 0, 0, old_root, 0, 0)
	root:link(root, 1, 1, old_root, 0, 0, old_root.width, old_root.height)
	
	-- new root will call adjustSize before onRequestLayOut
	local old_onRequestLayOut = root.onRequestLayOut
	root.onRequestLayOut = function(...)
		root:adjustSize()
		if old_onRequestLayOut then old_onRequestLayOut(unpack(arg)) end
		root.need_lay_out = false			-- HACK: beacause of adjustSize we had constant grow and shrink - and endless loop
	end
end

------------- ..Items ----------------

function PhonemicItem(text)
	local self = TextItem("["..text.."]", 3)
	local sound = SoundEffect("audio/p_"..text..".wav")
	if sound then
		self.onDragEnd = function()
			sound:play()
		end
	end
	return self
end

function VoiceTextItem(text)
	local self = TextItem(text)
	local sound = SoundEffect("audio/"..text..".wav")
	if sound then
		self.onDragEnd = function()
			sound:play()
		end
	end
	return self
end

function TextButton(...)
	if type(arg[1])=="table" then arg = arg[1] end				-- allow call with () too
	local text = arg[1]
	local image = arg[2]
	
	local self = CompositeItem()
--	if not arg.one_line then rigid_width = true end
	self.id="TextButton"
		
	local image_item = nil
	if image then
		if string.sub(image, -5) == ".anim" then
			image_item = AnimatedItem(load_config(image))
			image_item.id = "button_animation"
			self:add(image_item)
			image_item:stop()			
		else
			image_item = ImageItem(image)
			image_item.id = "button_image"
			image_item.num_frames = 1					-- HACK: better add it to Image?
			self:add(image_item)			
		end
		
		-- link
		image_item.rel_hpx, image_item.rel_hpy = 0, 0
		image_item.x, image_item.y = 0, 0
		self:link(image_item, 0, 0, self, 0, 0)
		self:link(image_item, 1, 1, self, 1, 1)
	end

	local text_item = nil
	if text~=nil and text~="" then
		text_item = TextBoxItem(text)
		if arg.one_line then text_item.oneLineMode = true end	
		text_item.id = "but_text_item"
		text_item.width = 100000					-- max: full line length	
		self:add(text_item)
		text_item.rel_hpx, text_item.rel_hpy = 0, 0
		
		self:link(text_item, 0, 0, self, 0, 0, padding, padding)
		self:link(text_item, 1, nil, self, 1, nil, -padding)		
		self:link(self, nil, 1, text_item, nil, 1, 0, padding)		
	end
	
	local padding = 10
	if arg.padding then padding = arg.padding end
	
	local shrink = arg.shrink
	local scaleFree = arg.scaleFree

	--  set self width as initial. BAD
	--self.width = 80--text_item.width + padding*2
	
	self.onDragStart = function()
		if image_item.num_frames > 1 then
			image_item.frame = 1
		end
	end

	self.onDragEnd = function()
		image_item.frame = 0
		if self.onClick then self:onClick() end
	end
	
	self.onRequestSize = function()
		print("+onRequestSize", self.id)
		if text_item ~= nil then
			self.width = 2*padding + text_item.oneLineWidth
			self.height = 2*padding + text_item.height	-- TODO: better use Cassowary for this!
		elseif image_item ~= nil then
			self.width = image_item.width
			self.height = image_item.height
		else
			assert(false)
		end
		print("-onRequestSize", self.id, self.width, self.height)
		return false, false
	end
	
	--add some properties
	local fake = {}
	setmetatable(fake, {})
	getmetatable(fake).__index = function(_, key)
		if key == "oneLineWidth" then return text_item.oneLineWidth + padding*2
		else return nil
		end
	end
	local res = {}
	setmetatable(res, inherit(self, fake))
	
	return res
end

function FrameItem(...)
	if type(arg[1])=="table" then arg = arg[1] end				-- allow call with () too
	local name = arg[1]
	
  local self = CompositeItem()
	self.width = arg.width or 0
	self.height = arg.height or 0
--	self.rigid_width, self.rigid_height = true, true
  
	
  local tex = {
	c1 = ImageItem(name.."_c1.rttex"),
	c2 = ImageItem(name.."_c2.rttex"),
	c3 = ImageItem(name.."_c3.rttex"),
	c4 = ImageItem(name.."_c4.rttex"),
	bl = TextureItem(name.."_bl.rttex", 4, self.height-8),
	br = TextureItem(name.."_br.rttex", 4, self.height-8),
	bt = TextureItem(name.."_bt.rttex", self.width-8,  4),
	bb = TextureItem(name.."_bb.rttex", self.width-8,  4)
  }
	
  -- TODO: Implement add(array)
  self:add(tex.c1):add(tex.c2):add(tex.c3):add(tex.c4):add(tex.bl):add(tex.br):add(tex.bt):add(tex.bb)	
	self.tex = tex

	-- will be called on resize
	self.onRequestLayOut = function(_)
		tex.bl.height = self.height-8
		tex.br.height = self.height-8
		tex.bt.width  = self.width-8
		tex.bb.width  = self.width-8
	
		tex.c1.rel_hpx, tex.c1.rel_hpy = 0, 0
			tex.c1.x, tex.c1.y = 0, 0
		tex.c2.rel_hpx, tex.c2.rel_hpy = 1, 0
			tex.c2.x, tex.c2.y = self.width, 0
		tex.c3.rel_hpx, tex.c3.rel_hpy = 1, 1
			tex.c3.x, tex.c3.y = self.width, self.height	
		tex.c4.rel_hpx, tex.c4.rel_hpy = 0, 1
			tex.c4.x, tex.c4.y = 0, self.height	
			
		tex.bl.rel_hpx = 0
			tex.bl.x, tex.bl.y = 0, self.height/2	
		tex.br.rel_hpx = 1
			tex.br.x, tex.br.y = self.width, self.height/2	
		tex.bt.rel_hpy = 0
			tex.bt.x, tex.bt.y = self.width/2, 0	
		tex.bb.rel_hpy = 1
			tex.bb.x, tex.bb.y = self.width/2, self.height	
	end
	
  return self
end

---------- Layouts -------------

function FlowLayout(indent)
	if indent==nil then indent = 0 end
  local self = CompositeItem()
--	self.rigid_width = true
  
	local old_width = self.width					-- need it when resizing
  local items     = {}		-- array
  local obstacles = {}		-- set
	-- both profiles base on left edge
  local profile = { left = StairsProfile(), right = StairsProfile() }
  
	local rebuild_profiles = function()
		profile.left:clear();
		profile.right:clear();
		
		for obst, side in pairs(obstacles) do
			if side=="left" then
				profile.left:setInterval(obst.top, obst.height, obst.right)
			else
				profile.right:setInterval(obst.top, obst.height, self.width-obst.left)
			end		
		end
	end
	
	-- align all in line
	self.onRequestSize = function()
		local	self_height = 0												-- will find max
		local cur_x = profile.left:at(0,1)+indent		-- will add to the right
		
		for _,item in ipairs(items) do
			-- if item
			if type(item.firstLineDecrement)=="nil" then
				cur_x = cur_x + item.width
				self_height = max(self_height, item.height)
			-- if text
			else
				cur_x = cur_x + item.oneLineWidth
				self_height = max(self_height, item.oneLineHeight)
			end -- select type
		end -- for
		
		-- check also obstacles
		for obst,side in pairs(obstacles) do
			self_height = max(self_height, obst.bottom)
			cur_x = max(cur_x, obst.right)
		end
		
		self.width, self.height = cur_x, self_height
		return false, false
	end
	
  self.onRequestLayOut = function(_)
		if self.width==0 then return end
	
		local dw = self.width-old_width
		if dw ~= 0 then
		-- don't know what's this:
			old_width = self.width				-- prevents recursion		
--			profile.right:add(-dw)
			for obst, side in pairs(obstacles) do
				if side=="right" then
					obst:move(dw, 0)
				end
			end
		-- somebody moved!
		else
			rebuild_profiles()
		end
	
		local cur_x, cur_y = profile.left:at(0,1)+indent, 0
		for _,item in ipairs(items) do
			-- TODO Check how to use == operator to compare references!
			--assert(item.parent == self)
				item.rel_hpx, item.rel_hpy = 0, 0	
			-- if item
			if type(item.firstLineDecrement)=="nil" then
				item.x, item.y = cur_x, cur_y
				cur_x = cur_x + item.width
				while cur_x > self.width - profile.right:at(cur_y, item.height) do
					cur_y = cur_y + 24							-- TODO Must know font line height here!!
					cur_x = profile.left:at(cur_y, item.height)
					item.x, item.y = cur_x, cur_y
					cur_x = cur_x + item.width
					if cur_y > 1000 then print ("W:", self.width) end
					assert(cur_y < 10000)						-- in case of hanging
				end -- while line over	  
			-- if text
			else
				item.firstLineDecrement = cur_x					-- TODO Align baselines here!!
				item.x, item.y = 0, cur_y
				item.width = self.width
				profile.left:shifted(-cur_y)
				item.leftObstacles = profile.left:shifted(-cur_y)
				item.rightObstacles = profile.right:shifted(-cur_y)		
				cur_x, cur_y = item.lastLineEndX, cur_y + item.lastLineEndY
			end -- select type
		end -- for
		local	self_height = cur_y + 24			-- same magic number!
		
		-- check also obstacles
		for obst,_ in pairs(obstacles) do
			if obst.bottom > self_height then
				self_height = obst.bottom
			end
		end
		
		-- we use temporary var here because changing self.height will trigger new lay_out()!
		self.height = self_height
  end -- onRequestLayOut
  
  self.addItem = function(self, item)
		self:add(item)
		table.insert(items, item)
		return self
  end
  
  self.clear = function(self)
		for _,item in ipairs(items) do
			self:remove(item)
			item:destroy()
		end
		items = {}
  end
  
  self.addItems = function(self, added)
		for i,item in ipairs(added) do
			self:add(item)
			table.insert(items, item)
		end
  end  
  
  self.addObstacle = function(self, obst, x, y, side)
		if side==nil then side="left" end
		assert(side=="left" or side=="right")
		obstacles[obst] = side		-- t/f/nil
		self:add(obst)
		
		obst.x, obst.y = x, y
		
		if side=="left" then
			profile["left"]:setInterval(obst.top, obst.height, obst.right)
		else
			obst.x = self.width - obst.x			-- coords from right
			profile["right"]:setInterval(obst.top, obst.height, self.width-obst.left)
		end
		return self
  end
  
  self.clearObstacles = function(self)
    for obst,val in pairs(obstacles) do
			if val then obst:destroy() end
		end
		obstacles = {}
		profile.left = StairsProfile()
		profile.right = StairsProfile()
  end
  
  return self
end -- FlowLayout

function TableLayout(nrows, ncols)
	local self = CompositeItem()
	self.id = "TableLayout"
	self.rows = {}
	self.columns = {}
	
	local equalize_columns, equalize_rows = false, false
	local rows_fixed = {}
	local columns_fixed = {}
	
	self.equalizeRows = function(_, arg)
		if arg==nil then arg = true end
		equalize_rows = arg
		self:requestLayOut()
	end
	self.getEqualizeRows = function()
		return equalize_rows
	end
	self.equalizeColumns = function(_, arg)
		if arg==nil then arg = true end
		equalize_columns = arg
		self:requestLayOut()
	end
	self.getEqualizeColumns = function()
		return equalize_columns
	end
	
	self.fixRow = function(_, row, h)
		rows_fixed[row] = h
		self:requestLayOut()
	end
	self.unfixRow = function(_, row)
		rows_fixed[row] = nil
		self:requestLayOut()
	end
	self.fixColumn = function(_, col, w)
		columns_fixed[col] = w
		self:requestLayOut()
	end
	self.unfixColumn = function(_, col)
		columns_fixed[col] = nil
		self:requestLayOut()
	end	
	
	local elements = {}				-- 2d array of keys + hspan + vspan
														-- spanned cells are ignored when computing widths/heights
	
	-- create rows
	for i=1,nrows do
		local it = ScreenItem()
		it.id = "row_"..i
		it.rel_hpx, it.rel_hpy = 0, 0
		self:add(it)
--		it.debugDrawBox = true
		table.insert(self.rows, it)
		table.insert(elements, {})			-- add rows to matrix
		for j=1,ncols do table.insert(elements[i], {}) end
	end
	
	-- create cols
	for i=1,ncols do
		local it = ScreenItem()
		it.id = "col_"..i
		it.rel_hpx, it.rel_hpy = 0, 0		
		self:add(it)
--		it.debugDrawBox = true
		table.insert(self.columns, it)
	end	
	
	local old_add = self.add
	self.add = function(_, item, row, col, vspan, hspan)
		assert(item ~= nil)
		assert(row>=1 and row<=nrows)
		assert(col>=1 and col<=ncols)
		if hspan==nil then hspan = 1 end
		if vspan==nil then vspan = 1 end
		old_add(self, item)
		item.id = "cell_"..row.."_"..col
		elements[row][col][item] = true
		elements[row][col].hspan = hspan
		elements[row][col].vspan = vspan
	end

	self.getSpans = function(_, r, c)
		return elements[r][c].vspan, elements[r][c].hspan
	end
	
	local padding = 10	
	
	self.onRequestSize = function(...)
	
		-- adjust sizes of children before
		for i=1,nrows do
			for j=1,ncols do
				for k,_ in pairs(elements[i][j]) do
					if type(k)~="string" and k~=nil then
						-- store rigidness for later
						k.rigid_width, k.rigid_height = k:adjustSize()
						print("cell:", k.rigid_width, k.rigid_height)
					end
				end -- for elements
			end -- cols
		end -- rows		
		
		local max_max_h = 0
		local max_max_w = 0
		
		self.rows[1].y = 0
		
		-- for rows
		for i=1,nrows do
			local max_h = 0
			for j=1,ncols do
				for k,_ in pairs(elements[i][j]) do
					if elements[i][j].vspan~=1 then break end			-- ignore spanned				
					if type(k)~="string" and k.height+2*padding > max_h then max_h = k.height+2*padding end	-- string for hspan and vspan
				end -- for elements
			end -- cols

			-- if fixed
			if rows_fixed[i] ~= nil then
				max_h = rows_fixed[i]
			end
			
			-- apply
			if not equalize_rows then
				self.rows[i].height = max_h
				if i~=1 then self.rows[i].y = self.rows[i-1].bottom end
			else
				max_max_h = max(max_h, max_max_h)
			end
		end -- rows
		
		self.columns[1].x = 0
		
		-- for cols
		for j=1,ncols do
			local max_w = 0
			for i=1,nrows do
				for k,_ in pairs(elements[i][j]) do
					if elements[i][j].hspan~=1 then break end			-- ignore spanned
					if type(k)~="string" and k.width+2*padding > max_w then max_w = k.width+2*padding end
				end -- for elements
			end -- rows
			
			-- if fixed
			if columns_fixed[j] ~= nil then
				max_w = columns_fixed[j]
			end
			
			-- apply
			if not equalize_columns then
				self.columns[j].width = max_w
				if j~=1 then self.columns[j].x = self.columns[j-1].right end				
			else
				max_max_w = max(max_w, max_max_w)
			end
		end -- cols

		if equalize_columns then
			for j=1,ncols do
				self.columns[j].width = max_max_w
				if j~=1 then self.columns[j].x = self.columns[j-1].right end
			end
		end

		if equalize_rows then
			for i=1,nrows do
				self.rows[i].height = max_max_h
				if i~=1 then self.rows[i].y = self.rows[i-1].bottom end				
			end
		end		
		
		self.height = self.rows[nrows].bottom;		
		self.width = self.columns[ncols].right;
		
		-- make grid
		for i=1,nrows do
			self.rows[i].x = 0
			self.rows[i].width = self.width
		end -- cols		
		for j=1,ncols do
			self.columns[j].y = 0
			self.columns[j].height = self.height
		end

		-- move elements
		for i=1,nrows do
			for j=1,ncols do
				for k,_ in pairs(elements[i][j]) do
					if type(k)~="string" then					-- TODO Remove "hspan" and "vspan" strings from here
						k.x = padding + self.columns[j].left + k.hpx
						k.y = padding + self.rows[i].top + k.hpy
					end
				end -- for elements
			end -- cols
		end -- rows

--		print("RETURNING1", false, false)		
		return false, false
	end	
	
	self.onRequestLayOut = function(...)
		print("+onRequestLayOut", self.id, self.width, self.height)
	
		local max_max_h = 0
		local max_max_w = 0
		
		self.rows[1].y = 0
		
		-- for rows
		for i=1,nrows do
			local max_h = 0
			for j=1,ncols do
				for k,_ in pairs(elements[i][j]) do
					if elements[i][j].vspan~=1 then break end			-- ignore spanned				
					if type(k)~="string" and not elements[i][j].rigid_height and k.height+2*padding > max_h then	-- string for hspan and vspan
						max_h = k.height+2*padding
					end
				end -- for elements
			end -- cols

			-- if fixed
			if rows_fixed[i] ~= nil then
				max_h = rows_fixed[i]
			end
			
			-- apply
			if not equalize_rows then
				self.rows[i].height = max_h						-- will be 0 if "rigid"
			else
				max_max_h = max(max_h, max_max_h)
			end
		end -- rows
		
		self.columns[1].x = 0
		
		-- for cols
		for j=1,ncols do
			local max_w = 0
			for i=1,nrows do
				for k,_ in pairs(elements[i][j]) do
					if elements[i][j].hspan~=1 then break end			-- ignore spanned
					if type(k)~="string" and not elements[i][j].rigid_width and k.width+2*padding > max_w then
						max_w = k.width+2*padding
					end
				end -- for elements
			end -- rows
			
			-- if fixed
			if columns_fixed[j] ~= nil then
				max_w = columns_fixed[j]
			end
			
			-- apply
			if not equalize_columns then
				self.columns[j].width = max_w
			else
				max_max_w = max(max_w, max_max_w)
			end
		end -- cols

		-- make rigid rows and cols equal
		do		
			local free_height = self.height
			local n_rigid_rows = 0
			for i=1,nrows do
				free_height = free_height - self.rows[i].height
				if self.rows[i].height == 0 then n_rigid_rows = n_rigid_rows + 1 end
			end
			
			local free_width = self.width
			local n_rigid_cols = 0
			for j=1,ncols do
				free_width = free_width - self.columns[j].width
				if self.columns[j].width == 0 then n_rigid_cols = n_rigid_cols + 1 end
			end
			
			print("layout:", free_width, free_height, n_rigid_cols, n_rigid_rows)
			
			-- set them
			
			for i=1,nrows do
				if self.rows[i].height == 0 then self.rows[i].height = free_height / n_rigid_rows end
			end
			
			for j=1,ncols do
				if self.columns[j].width == 0 then self.columns[j].width = free_width / n_rigid_cols end
			end
			
		end -- make rigid rows and cols equal
		
		for j=1,ncols do
			if equalize_columns then self.columns[j].width = max_max_w end
			if j~=1 then self.columns[j].x = self.columns[j-1].right end
		end

		for i=1,nrows do
			if equalize_rows then	self.rows[i].height = max_max_h end
			if i~=1 then self.rows[i].y = self.rows[i-1].bottom end
		end
		
		self.height = self.rows[nrows].bottom;		
		self.width = self.columns[ncols].right;
		
		-- make grid
		for i=1,nrows do
			self.rows[i].x = 0
			self.rows[i].width = self.width
		end -- cols		
		for j=1,ncols do
			self.columns[j].y = 0
			self.columns[j].height = self.height
		end

		-- move elements
		for i=1,nrows do
			for j=1,ncols do
				for k,_ in pairs(elements[i][j]) do
					if type(k)~="string" then					-- TODO Remove "hspan" and "vspan" strings from here
						k.x = padding + self.columns[j].left + k.hpx
						k.y = padding + self.rows[i].top + k.hpy
						if elements[i][j].rigid_width then k.width = self.columns[j].width end
						if elements[i][j].rigid_height then k.height = self.rows[i].height end
					end
				end -- for elements
			end -- cols
		end -- rows	
		
		print("-onRequestLayOut", self.id, self.width, self.height)			
	end -- onRequestLayOut
	
	return self
end

function Table(rows, cols, data)
	local self = CompositeItem()
	self.id = "Table"
	
	local lay = TableLayout(rows, cols)
	lay.rel_hpx, lay.rel_hpy = 0, 0
	lay.x, lay.y = 0, 0	
	self:add(lay)
	
	local frame
	local cell_frames = {}
	local cell_frames_path
	
	-- local funcs
	local create_cell_frames
	local resize_cell_frames
	
	self.setFrame = function(_, path)
		if path ~= nil then
			frame = FrameItem(path)
			self:add(frame)
			frame.rel_hpx, frame.rel_hpy = 0, 0
			frame.x, frame.y = 0, 0
		elseif frame ~= nil then
			self:remove(frame)
			frame = nil
		end
		
		self:requestLayOut()
	end
	
	self.setCellFrames = function(_, path)
		cell_frames_path = path
		create_cell_frames()
	end
	
	create_cell_frames = function()
	
		for i=1,#cell_frames do
			for j,v in pairs(cell_frames[i]) do
				self:remove(v)
			end -- for j
		end -- for i

		cell_frames = {}
		if cell_frames_path == nil then return end
		
		for i=1,#lay.rows do
			table.insert(cell_frames, {})
			for j=1,#lay.columns do
				if data[i][j] ~= nil then
					local f = FrameItem(cell_frames_path)
					f.id="frame_"..i.."_"..j
					f.rel_hpx, f.rel_hpy = 0, 0
					self:add(f)
					cell_frames[i][j] = f
				end -- if
			end -- for j
		end -- for i
		
		resize_cell_frames()
	end	
	
	resize_cell_frames = function()
		for i=1,#cell_frames do
			for j,v in pairs(cell_frames[i]) do
				if data[i][j] ~= nil then
					local vspan, hspan = lay:getSpans(i, j)
					local h = 0
					for d=1,vspan do h = h + lay.rows[i-1+d].height end
					local w = 0
					for d=1,hspan do w = w + lay.columns[j-1+d].width end
					
					v.x, v.width = lay.columns[j].left, w
					v.y, v.height = lay.rows[i].top, h
				end -- if
			end -- for j
		end -- for i
	end	

	if data~=nil then
		for i=1,rows do
			for j=1,cols do
				local item = data[i][j]
				if type(item)~="userdata" and type(item)~="nil" then item = TextItem(tostring(item)) end
				
				-- find spans
				local vspan=1
				for d=1,#data do if i+d>rows or data[i+d][j]~=nil then vspan=d; break; end; end
				local hspan=1
				for d=1,#data[i] do if j+d>cols or data[i][j+d]~=nil then hspan=d; break; end; end
				
				if item~=nil then
					lay:add(item, i, j, vspan, hspan)
				end
			end -- cols
			table.insert(cell_frames, {})
		end -- rows	
		
		create_cell_frames()
	end -- if data
	
	self.onRequestSize = function(_)
		local rigid_width, rigid_height = lay:adjustSize()
		
		self.width = lay.width
		self.height = lay.height
		
		if frame ~= nil then
			frame.width = self.width
			frame.height = self.height
		end
		
		resize_cell_frames()
		
--		print("Table returns", rigid_width, rigid_height)
		return rigid_width, rigid_height
	end
	
	self.onRequestLayOut = function(_)
		
		lay.width, lay.height = self.width, self.height
		
		lay:onRequestLayOut()
		
		self.width = lay.width
		self.height = lay.height
		
		if frame ~= nil then
			frame.width = self.width
			frame.height = self.height
		end
		
		resize_cell_frames()
		
	end
	
	-- composition!
	self.equalizeRows = function(_, arg) lay.equalizeRows(lay, arg) end
	self.getEqualizeRows = function() return lay.getEqualizeRows(lay) end
	self.equalizeColumns = function(_, arg) lay.equalizeColumns(lay, arg)	end
	self.getEqualizeColumns = function() return lay.getEqualizeColumns(lay)	end
	self.fixRow = function(_, row, h) lay.fixRow(lay, row, h)	end
	self.unfixRow = function(_, row) lay.unfixRow(lay, row)	end
	self.fixColumn = function(_, col, w) lay.fixColumn(lay, col, w) end
	self.unfixColumn = function(_, col) lay.unfixColumn(lay, col) end		

	return self
end

----------- added behavior ----------

function PackAsButton(view)
  local item = SimpleItem()
  item.view = view
  local self = {}
  self.item = item
  setmetatable(self, inherit(item, view))
  
  item.onDragStart = function(item)
    view:over(true)
  end
  item.onDragEnd = function(item)
    view:over(false)
    if self.onClick then self:onClick() end
  end
  
  return self
end

function MakeFrameFlyer(self)
  self.timer = Timer(function(t)
    local dt = t.elapsed
    -- check screen out
    if self.x < -screen_width or self.x > 2*screen_width or
       self.y < -screen_height or self.y > 2*screen_height
    then
      self:stop()
      return
    end

    -- else call user function and restart
    -- but if user function wants to stop?!!!
    t:restart(0)      
    self:onFrame(dt)
  end)
  self.start = function(self)
		self.flying = true
		self.timer:start()
  end
  self.stop  = function(self)
		self.flying = false
--    self.onFrame = nil
    if self.timer then self.timer:cancel() end
--    self.timer = nil
  end
  return self
end

local function TwoStateAnimation1(anim)
  assert(anim.num_frames >= 2)
  -- THINK can't call stop before attaching to Entity:
  --anim:stop()
  anim.over = function(self, arg)
    if arg then
      self.frame = 1
    elseif arg==false
    then
      self.frame = 0
    else
      return (self.frame > 0)
    end
  end
  return anim
end

local function TwoStateAnimation4(i1, i2)
  local self = CompositeItem()
  self:add(i1):add(i2)
  
  i1.visible, i2.visible = true, false
  -- TODO Mouse pointer checking will work wrong if we have x, y != 0
  self.width, self.height = i1.width, i1.height
  i1.x, i1.y = i1.hpx, i1.hpy
  
  self.over = function(self, arg)
    if arg then
		i1.visible, i2.visible = false, true
		self.width, self.height = i2.width, i2.height
		i2.x, i2.y = i2.hpx, i2.hpy
    elseif arg==false
    then
		i1.visible, i2.visible = true, false
		print (self.width, self.height, i2.width, i2.height,	i2.x, i2.y, i2.hpx, i2.hpy)		
		self.width, self.height = i1.width, i1.height
		i1.x, i1.y = i1.hpx, i1.hpy
		print (self.width, self.height, i2.width, i2.height,	i2.x, i2.y, i2.hpx, i2.hpy)		
    else
      return i2.visible
    end
  end
  return self
end

function TwoStateAnimation(a, b, c, d)
	if b==nil then
		return TwoStateAnimation1(a)
	else
		return TwoStateAnimation4(a, b, c, d)
	end
end

function DropArea(item)
--  if x == nil then x = 0 end
--  if y == nil then y = 0 end

  -- TODO Maybe change view everywhere to Item?
  -- TODO Adjust CompositeItem's size in according to contents. Margins?
--  local item = CompositeItem(x, y)
--  item:add(view)
--  item.width, item.height = view.width, view.height

  local intersects = function(dummy, r)
    local overlays = function(ax1, ax2, bx1, bx2)
      return min(bx1, bx2) < max(ax1, ax2) and max(bx1, bx2) > min(ax1, ax2)
    end
    return overlays(r.gleft, r.gright, item.gleft, item.gright) and overlays(r.gtop, r.gbottom, item.gtop, item.gbottom)
  end
  local dist = function(dummy, r)
      return dist(item.x, item.y, r.x, r.y)
  end
  local take = function(self, obj)
    -- remove it
    if obj.drop then
      obj.drop.object = nil
    end
		if obj.parent then obj.parent:remove(obj) end
		item.parent:add(obj)
    -- move to new x,y
    obj.x = item.x - item.hpx + item.width/2 + obj.hpx - obj.width/2
		obj.y = item.y - item.hpy + item.height/2 + obj.hpy - obj.height/2
    -- attach new objects to each other
    self.object = obj
    obj.drop = self
  end

  local over = function(dummy, arg)
    return item:over(arg)
  end

  local self = {
    intersects = intersects,
    dist = dist,
    take = take,
    over = over,
	item = item
  }
  setmetatable(self, inherit(item))
  return self
end

local find_drop = function(drops, obj)
  -- find intersections and min dist
  local min_d, min_drop
  for name,drop in pairs(drops)
  do
    -- find checked
    if not drop.object and drop:intersects(obj) and (min_drop==nil or min_d < drop:dist(obj))
    then
      min_drop = drop
      min_d = drop:dist(obj)
    end
  end
  return min_drop
end

highlightDropOnDrag = function(drops, obj)
  for name,drop in pairs(drops)
  do
    -- uncheck all
    drop:over(false);
  end
  -- check it!
  local over = find_drop(drops, obj)
  if over then
    over:over(true)
  end
end

takeOnDrop = function(drops, obj)
  local over = find_drop(drops, obj)
  if over then
    over:take(obj)
    over:over(false)
  else
    obj:goHome()
  end
end

-- NOTE: We make composition with item to not to exhibit
-- its onDrag etc. Instead we use our "outer" handlers.
function PackAsDragDrop(item)
  item = MakeFrameFlyer(item)

  local self = {
    item = item,
		-- can be set outside!
		ox = x,		-- origin x
		oy = y,		-- origin y		
		id="DragDrop"
  }
  
  local drop = nil

  item.onFrame = function(dummy, dt)
--    print(self.x, self.y, self.ox, self.oy, dt)
    local norm = dist(self.ox, self.oy, self.x, self.y)
    if norm > self.prev_norm or norm < 50 then	-- if passed over!
      self.x = self.ox; self.y = self.oy
      item:stop()
    else
      local dx = self.vx * dt
      local dy = self.vy * dt
      item:move(dx, dy)
    end
	self.prev_norm = norm
  end

  local empty = function(o) end
  self.onDragStart = empty
  self.onDragEnd = empty
  self.onDrag = empty

  item.onDragStart = function(dummy)
		if self.flying then return end
    self.ox=item.x self.oy=item.y
    if self.onDragStart then self:onDragStart() end
  end
  item.onDrag = function (item, dx, dy)
    item:move(dx, dy)
    if self.onDrag then self:onDrag(dx, dy) end
  end
  item.onDragEnd = function(dummy)
    if self.onDragEnd then self:onDragEnd() end
  end

  self.goHome = function(self)
    self.prev_norm = 1e+9
	local norm = dist(self.ox, self.oy, self.x, self.y)
	self.vx = (self.ox - self.x) / norm * 2000
    self.vy = (self.oy - self.y) / norm * 2000
    item:start()
  end -- goHome

  -- redirect some keys to parent
  setmetatable(self, inherit(item))
  return self
end -- PackAsDragDrop

-------------- useful stuff ----------------

function max(a, b)
  if a > b then return a
  else return b end
end

function min(a, b)
  if a > b then return b
  else return a end
end

function copy_table(t)
  local u = { }
  for k, v in pairs(t) do u[k] = v end
  return setmetatable(u, getmetatable(t))
end

function load_config(path)
  local cfg = {_G = _G}
  local f = loadfile(path)
  setfenv(f, cfg)
  f()
  cfg._G = nil
  return cfg
end

-------------- geometry ---------------

function dist(x1,y1,x2,y2)
	return math.sqrt(math.pow(x2-x1,2)+math.pow(y2-y1,2))
end

-- test point x,y against line equation
function line_sign(p1, p2, p)
  return (p2.x - p1.x)*(p.y - p1.y) - (p.x - p1.x)*(p2.y - p1.y)
end

-- for every edge this point must give the same sign as own point
function polygon_test_point(pol, poi)
  -- check sign of own point
  local inner_sign = line_sign( p1[0], p1[1], p1[2] )
  
  -- check other edges
  for i = 1, #pol
  do
	local next = i + 1
	if next > #pol then next = 1 end
	if line_sign(pol[i], pol[next], poi) ~= inner_sign then return false end
  end
  return true
end

-- check convex polygons intersection
-- if at least one point is inside - then true
function polygon_intersection(p1, p2)
  -- check other's sign
  for i = 1, #p2
  do
	if polygon_test_point(p1, p2[i]) then return true end
  end
  return false
end

-------- debug functions ----------

function print_table(t, shift)
  if not t then print("nil") return end

  if shift == nil then shift = 0 end
  local shift_str = string.rep("\t", shift)

  for k,v in pairs(t) do
		if(type(v) ~= "number" and type(v)~="string") then v = type(v) end
    local k2 = k
		if(type(k2) ~= "number" and type(k2)~="string") then k2 = type(k2) end
    print(shift_str, k2, " => ", v)
    if type(t[k]) == "table" and shift < 2 then print_table(t[k], shift+1) end
  end
end

function getupvalues(f)
  local i = 1
  local res = {}
  local name, val
  name, val = debug.getupvalue(f, i)
  while name ~= nil
  do
    -- recurse!
    if type(val) == "function" then
      table.insert(res, getupvalues(val))
    else
      table.insert(res, name)
    end

    i = i + 1
    name, val = debug.getupvalue(f, i)
  end
  return res
end

----------- initialization ------------
dofile("scene.lua")