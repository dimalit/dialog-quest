-------------- general ----------------
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

-------------- classes ----------------

-- arg = base OBJECTS
-- resulting mt will redirect all access to them
-- when accessing lubinded function it will also convert 1-st argument appropriately
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
						assert(arg[1] == self, "1st arg must be self")
						-- but we change it to appropriate parent
						return parent_func(parent, unpack(arg, 2))
					end -- func body
				else
					return arg[i][key]
				end -- if func
			end -- id not nil
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

-------- Lua classes -----------

------------- wrappers for native classes -----------------

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

------------- ..Items ----------------

ImageItem = function(path)
  local item = SimpleItem()
  local image = Image(path)
  item.view = image
  local self = {}
  self.item = item
  setmetatable(self, inherit(item, image))
  return self
end

TextureItem = function(path, width, height)
  local item = SimpleItem()
  local texture = Texture(path, width, height)
  item.view = texture
  local self = {}
  self.item = item
  setmetatable(self, inherit(item, texture))
  return self
end

TextItem = function(text, font)
  local item = SimpleItem()
  local txt
	if font==nil then
		txt = Text(text)
	else
		txt = Text(text, font)
	end
	item.view = txt

  local self = {}
  self.item = item
  setmetatable(self, inherit(item, txt))
  return self
end

TextBoxItem = function(text, w, font)
  if w == nil then w = 0 end
	if font == nil then font = 0 end
  
  local item = SimpleItem()
  local txt = TextBox(text, w, 0, font)
  item.view = txt

  local self = {}
  self.item = item
  setmetatable(self, inherit(item, txt))
  return self
end

function PhonemicItem(text)
	local self = TextItem("["..text.."]", 3)
	local sound = SoundEffect("audio/"..text..".wav")
	if sound then
		self.onDragEnd = function()
			sound:play()
		end
	end
	return self
end

function VoiceTextItem(text)
	local self = TextItem("`1"..text)
	local sound = SoundEffect("audio/voice_"..text..".ogg")
	if sound then
		self.onDragEnd = function()
			sound:play()
		end
	end
	return self
end

-- make playing animation item
AnimatedItem = function(name)
  local item = SimpleItem()
  local anim = Animation(load_config(name))
  item.view = anim
 
  local self = {}
  self.item = item
  setmetatable(self, inherit(item, anim))

  -- moved this after self.view=anim because of Components
  anim.loop = true
  anim:play()
 
  -- methods are not inherited :(
  -- TODO find why methods do not inherit
  self.play = function()
    self.view:play()
  end
--  self.pause = function()
--    self.view:pause()
--  end
  
  return self
end

function FlowLayout(w, indent)
	if w == nil then w = 0 end
	if indent==nil then indent = 0 end
  local self = CompositeItem()
  self.width = w
  
	local old_width = w					-- need it when resizing
  local items     = {}		-- array
  local obstacles = {}		-- set
	-- both profiles base on left edge
  local profile = { left = StairsProfile(), right = StairsProfile() }
  
  self.onRequestLayOut = function(_)
		if self.width==0 then return self end
	
		local dw = self.width-old_width
		if dw ~= 0 then
			old_width = self.width				-- prevents recursion		
			profile.right:add(-dw)
			for obst, side in pairs(obstacles) do
				if side=="right" then
					obst:move(dw, 0)
				end
			end
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
		
		-- we use tempprary var here because changing self.height will trigger new lay_out()!
		self.height = self_height
  end -- onRequestLayOut
  
  self.addItem = function(self, item)
		self:add(item)
		table.insert(items, item)
		-- HACK: this sould be called automatically:
		-- BUG:
		self:requestLayOut(item)
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
end -- FlowLayoutItem

function FrameItem(name, w, h)
  local self = CompositeItem()
  self.width, self.height = w, h
  
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
	self.onRequestLayOut = function(_, child)
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
	self:onRequestLayOut(self)
  
  return self
end

---------- Layouts -------------
-- NOTE We need Luabind-inheritance from this class
-- so we must re-use obj and not make new self
-- NOTE It is not vail anymore
function MakeLayoutAgent(self)

	if self.rel_x ~= nil then
		print("Warning: attempt of repeated MakeLayoutAgent")
		return self
	end
	
	local cord = {
		rel_x = self.x,
		rel_y = self.y
	}
	
	local pos_origin, pos_origin_xr, pos_origin_yr
	local width_origin, width_ratio
	local height_origin, height_ratio
	
	local dependents = {}	

	local old_onmove = self.onMove
	self.onMove = function(_)
		if old_onmove then old_onmove(self) end
		for dep,_ in pairs(dependents) do
			dep:updateLocation(self)
		end
	end
	
	self.updateLocation = function(_, origin)
		-- print("------------------------------")
		-- print(type(pos_origin), class_info(pos_origin).name)
		-- print_table(class_info(pos_origin).methods)
		-- print(class_info(origin).name)
		-- print_table(class_info(origin).methods)
		
		-- update position
		if origin==pos_origin then
			self.gx = origin.gx - origin.hpx + origin.width*pos_origin_xr  + cord.rel_x
			self.gy = origin.gy - origin.hpy + origin.height*pos_origin_yr + cord.rel_y
		end
		-- update width
		if origin==width_origin then
			self.width = origin.width * width_ratio
		end
		-- update height
		if origin==height_origin then
			self.height = origin.height * height_ratio
		end
	end
	
	self.setLocationOrigin = function(_, ref_obj, xr, yr)
		if pos_origin then pos_origin:removeDependent(self) end
		pos_origin, pos_origin_xr, pos_origin_yr = ref_obj, xr, yr
		if pos_origin then
			pos_origin:addDependent(self)
			self:updateLocation(pos_origin)
		else
			self.x, self.y = cord.rel_x, cord.rel_y	-- if nil
		end		
	end
	self.setWidthOrigin = function(_, ref_obj, ratio)
		if width_origin then width_origin:removeDependent(self) end
		if ratio==nil then ratio = 1 end
		width_origin = ref_obj
		width_ratio = ratio
		if width_origin then
			width_origin:addDependent(self)
			self:updateLocation(width_origin)
		end
	end
	self.setHeightOrigin = function(_, ref_obj, ratio)
		if height_origin then height_origin:removeDependent(self) end
		if ratio==nil then ratio = 1 end
		height_origin = ref_obj
		height_ratio = ratio		
		if height_origin then
			height_origin:addDependent(self)
			self:updateLocation(height_origin)
		end	
	end
	
	self.addDependent = function(_, dep)
		dependents[dep] = true
	end
	self.removeDependent = function(_, dep)
		dependents[dep] = nil
	end

	self.rel_x = function(_, x)
		if x~=nil then
			cord.rel_x = x
			if pos_origin~=nil then
				self:updateLocation(pos_origin)
			else
				self.x = x
			end
		else
			return cord.rel_x
		end
	end

	self.rel_y = function(_, y)
		if y~=nil then
			cord.rel_y = y
			if pos_origin~=nil then
				self:updateLocation(pos_origin)
			else
				self.y = y
			end
		else
			return cord.rel_y
		end
	end	
	-- no return - we just change self!
end -- MakeLayoutAgent

local old_CompositeItem = CompositeItem
CompositeItem = function(...)
	local self = old_CompositeItem(unpack(arg))
	
	-- add our own stuff
	local links = {}				-- key=nurse, val=array of link_obj
	local depends_cnt = {}	-- key=child if it has nurse, used to find independent nurses
	
	self.link = function(_, patient, px, py, nurse, nx, ny, dx, dy)
		if dx == nil then dx = 0 end
		if dy == nil then dy = 0 end
		link_obj = {
			patient = patient,
			px = px,
			py = py,
			nx = nx,
			ny = ny,
			dx = dx,
			dy = dy
		}
		if links[nurse] == nil then links[nurse]={} end
		table.insert(links[nurse],link_obj)
		if depends_cnt[patient]==nil then depends_cnt[patient]=0 end
		depends_cnt[patient] = depends_cnt[patient] + 1
		self:requestLayOut(self)
	end
	
	local adjust_dependents			-- for recursion
	adjust_dependents = function(nurse)
		if links[nurse]==nil then return end

		for _,link in ipairs(links[nurse]) do
			local patient = link.patient
			assert(patient.parent==nurse or nurse.parent==patient or patient.parent==nurse.parent)
				-- both nils or both non-nils!
			assert(((link.nx==nil) == (link.px==nil)) and ((link.ny==nil) == (link.py==nil)))
			-- TODO: Bad to create two identical parts for x and y...
						
			if link.nx ~= nil then
			local tx = nurse.left+nurse.width*link.nx + link.dx		-- target
				if patient.parent==nurse then tx=tx-nurse.left end	-- left=0 if i am parent
			-- if 0
			if link.px==0 then
				assert(nurse.parent~=patient)				-- left edje of parent cannot depend on child
				if patient.right - tx >= 0 then patient.width = patient.right - tx end
				patient.x = patient.x + (tx-patient.left)
			-- if 1
			elseif link.px==1 then
				if nurse.parent~=patient then
					if tx - patient.left >= 0 then patient.width = tx - patient.left end
					patient.x = patient.x + (tx-patient.right)
				else
					if tx >=0 then patient.width = tx end
				end
			-- else just move
			else
				assert(nurse.parent~=patient)												-- parent pos cannot depend on child
				local sx = patient.left+patient.width*link.px				-- source			
				patient.x = patient.x + (tx-sx)
			end
			end -- if nx ~= nil
			
			if link.ny ~= nil then
			local ty = nurse.top+nurse.height*link.ny + link.dy		-- target
				if patient.parent==nurse then ty=ty-nurse.top end	-- left=0 if i am parent
			-- if 0
			if link.py==0 then
				assert(nurse.parent~=patient)				-- left edje of parent cannot depend on child
				if patient.bottom - ty >= 0 then patient.height = patient.bottom - ty end
				patient.y = patient.y + (ty-patient.top)
			-- if 1
			elseif link.py==1 then
				if nurse.parent~=patient then
					if ty - patient.top >= 0 then patient.height = ty - patient.top end
					patient.y = patient.y + (ty-patient.bottom)
				else
					if ty >=0 then patient.height = ty end
				end
			-- else just move
			else
				assert(nurse.parent~=patient)												-- parent pos cannot depend on child
				local sy = patient.top+patient.height*link.py				-- source			
				patient.y = patient.y + (ty-sy)
			end
			end -- if link.ny ~= nil
			
			-- recurse!
			adjust_dependents(patient)
		end -- for
	end -- adjust_dependents
	
	self.onRequestLayOut = function()
		-- adjust those who explicitly depend on me
		adjust_dependents(self)
		-- adjust those who just have x,y
		local children = self.children
		for ch in pairs(children) do
			if ch.text~=nil then
			end
			if depends_cnt[ch]==nil or depends_cnt[ch]==0 then adjust_dependents(ch) end
		end -- for
	end
	
	return self
end -- CompositeItem:__init

-- TODO: spacing is not dynamic - if you change it later on - nothing happens!
-- height can be nil which means automatic
VBox = function(width, use_height)
	local self = ScreenItem()
	self.width = width
	if use_height~=nil then self.height=use_height end
	self.spacing = 0
	
	local items = {}
	
	-- resize and move all children
	self.onMove = function(_)
		local width = self.width
			local item_height = nil
			if use_height~=nil and #items>0 then item_height = (self.height-self.spacing*(#items-1)) / #items end
		-- re-align!
		local y = self.top
		for i, obj in ipairs(items) do
			obj.rel_hpx, obj.rel_hpy = 0, 0
			obj.width = width
			if item_height~=nil then obj.height = item_height end
			obj.x = self.left
			obj.y = y
			y = y + obj.height + self.spacing
		end
		if use_height==nil then
			local h = y-self.top-self.spacing
			if h < 0 then h = 0 end
			self.height = h
		end
	end
	
	self.add = function(_, obj, pos)
		if pos==nil then
			table.insert(items, obj)
		else
			table.insert(items, pos, obj)
		end
		self.parent:add(obj)
		self:onMove()
		return self
	end
	
	self.remove = function(_, obj)
		local pos = 0
		for pos=1,#items do
			if items[pos]==obj then
				table.remove(items, pos)
				self.parent:remove(obj)
				self:onMove()
				return
			end
		end
	end
	
	return self
end

-- width can be nil!
HBox = function(use_width, height)
	-- height is mandatory!
	if height==nil then
		height=use_width
		use_width=nil
	end

	local self = ScreenItem()
	self.height = height
	if use_width~=nil then self.width=use_width end
	self.spacing = 0
	
	local items = {}
	
	-- resize and move all children
	self.onMove = function(_)
		local height = self.height
			local item_width = nil
			if use_width~=nil and #items>0 then item_width=(self.width-self.spacing*(#items-1))/#items end
		-- re-align!
		local x = self.left
		for i, obj in ipairs(items) do
			obj.rel_hpx, obj.rel_hpy = 0, 0
			obj.height = height
			if item_width~=nil then obj.width=item_width end
			obj.x = x
			obj.y = self.top
			x = x + obj.width + self.spacing
		end		
		if use_width==nil then
			local w = x-self.left-self.spacing
			if w < 0 then w = 0 end
			self.width = w
		end
	end
	
	self.add = function(_, obj, pos)
		if pos==nil then
			table.insert(items, obj)
		else
			table.insert(items, pos, obj)
		end
		self.parent:add(obj)
		self:onMove()
		return self
	end
	
	self.remove = function(_, obj)
		local pos = 0
		for pos=1,#items do
			if items[pos]==obj then
				table.remove(items, pos)
				self.parent:remove(obj)
				self:onMove()
				return
			end
		end
	end
	
	return self
end

---------- MakeMover -----------

function MakeMover(self)
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

----------- DropArea & Mover ----------

function TwoStateAnimation(a, b, c, d)
	if b==nil then
		return TwoStateAnimation1(a)
	else
		return TwoStateAnimation4(a, b, c, d)
	end
end

function TwoStateAnimation1(anim)
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

function TwoStateAnimation4(i1, i2)
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
		self.width, self.height = i1.width, i1.height
		i1.x, i1.y = i1.hpx, i1.hpy
    else
      return i2.visible
    end
  end
  return self
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
    return overlays(r.left, r.right, item.left, item.right) and overlays(r.top, r.bottom, item.top, item.bottom)
  end
  local dist = function(dummy, r)
      return dist(item.x, item.y, r.x, r.y)
  end
  local take = function(self, obj)
    -- remove it
    if obj.drop then
      obj.drop.object = nil
    end
    -- move to new x,y
    obj.x = item.x; obj.y = item.y
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

find_drop = function(drops, obj)
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

onDrag = function(drops, obj)
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

onDrop = function(drops, obj)
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
function Mover(item)
  item = MakeMover(item)

  local self = {
    item = item,
		-- can be set outside!
		ox = x,		-- origin x
		oy = y		-- origin y		
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
end -- Mover

------------- button ------------------
Button = function(view)
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
--dofile("make_layout_agents.lua")
dofile("scene.lua")