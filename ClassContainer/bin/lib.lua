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

-- override root also to be CompositeItem!
do
	local old_root = root
	root = CompositeItem()
	root.width, root.height = old_root.width, old_root.height
	root.rel_hpx, root.rel_hpy = 0, 0
	root.x, root.y = 0, 0
	old_root:add(root)
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
	self.id="TextButton"
		
	local image_item = nil
	if image then
		if string.sub(image, -5) == ".anim" then
			image_item = AnimatedItem(load_config(image))
			self:add(image_item)
			image_item:stop()			
		else
			image_item = ImageItem(image)
			image_item.num_frames = 1					-- HACK: better add it to Image?
			self:add(image_item)			
		end
	end
	image_item.id = "image_item"

	local text_item = TextBoxItem(text)
	text_item.id = "text_item"
	self:add(text_item)
	text_item.rel_hpx, text_item.rel_hpy = 0, 0
	
	image_item.debugDrawColor = 0xff0000ff	
	text_item.debugDrawColor = 0x00ff00ff	
	
	local padding = 10
	if arg.padding then padding = arg.padding end
	
	local shrink = arg.shrink
	local scaleFree = arg.scaleFree
	
	-- link dimensions
	image_item.rel_hpx, image_item.rel_hpy = 0, 0
	image_item.x, image_item.y = 0, 0
	self:link(text_item, 0, 0, self, 0, 0, padding, padding)
	self:link(text_item, 1, nil, self, 1, nil, -padding)
	self:link(self, nil, 1, text_item, nil, 1, 0, padding)
	self:link(image_item, 0, 0, self, 0, 0)	

	--  set self width as initial. BAD
	--self.width = 80--text_item.width + padding*2

	local old_onRequestLayOut = self.onRequestLayOut
	self.onRequestLayOut = function(...)
		if old_onRequestLayOut then old_onRequestLayOut(unpack(arg)) end

		-- adjust text w from self
		-- TODO Hangs here!
--		text_item.width = self.width - padding*2
		
		-- adjust image from self
		local required_width = self.width
		local required_height = self.height
		
		--print("image: ", image_item.scaleX, image_item.scaleY, image_item.x, image_item.y, image_item.width, image_item.height)
		
		-- HACK: should eliminate this e-comparison!!!		
		if math.abs(image_item.width - required_width) > 0.01 then		
			image_item.scaleX = required_width / (image_item.frameWidth)
		end
		if math.abs(image_item.height - required_height) > 0.01 then		
			image_item.scaleY = required_height / (image_item.frameHeight)
		end
		
		--print(self.width, self.height, text_item.x, text_item.y, text_item.width, text_item.height)
		--print("image: ", image_item.scaleX, image_item.scaleY, image_item.x, image_item.y, image_item.width, image_item.height)
		return
	
		-- local required_width = text_item.width + padding*2
		-- local required_height = text_item.height + padding*2
		
		-- local kx = required_width / (image_item.width  / image_item.scaleX)
		-- local ky = required_height/ (image_item.height / image_item.scaleY)
		
		-- if scaleFree then
			-- local k = max(kx, ky)
			-- kx = k
			-- ky = k
		-- end
		
		-- -- scale up or scale down if enabled
		-- if kx>1 or shrink then
			-- -- HACK: should eliminate this e-comparison!!!
			-- if math.abs(image_item.width - required_width) > 0.01 then
				-- image_item.scaleX = kx
			-- end
		-- else
			-- image_item.scaleX = 1
		-- end
		
		-- if ky>1 or shrink then
			-- -- HACK: should eliminate this e-comparison!!!
			-- if math.abs(image_item.height - required_height) > 0.01 then
				-- image_item.scaleY = ky
			-- end
		-- else
			-- image_item.scaleY = 1
		-- end		
		
		-- -- set my dimension and positions of children
		-- self.width = image_item.width
		-- self.height = image_item.height
		
		-- text_item.x, text_item.y = image_item.width/2, image_item.height/2
	end -- onRequestLayOut
	
	self.onDragStart = function()
		if image_item.num_frames > 1 then
			image_item.frame = 1
		end
	end

	self.onDragEnd = function()
		image_item.frame = 0
		if self.onClick then self:onClick() end
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
		self:requestLayOut()
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
	self:onRequestLayOut()
  
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
	local solver = Cassowary()
	
	-- make 2.0-strength Stay on them, so inner vaes were prefereble to change
	-- TODO: No id at this moment - so var will have partial name
	-- solver:addExternalStay(self, "width")
	-- solver:addExternalStay(self, "height")
	
	-- add our own stuff
	local links = {}				-- array of link_obj

	-- move patient to needed point right now, before any solving
	local adjust_link = function(patient, px, py, nurse, nx, ny, dx, dy)
		local max_delta = 0

		assert(patient.parent==nurse or nurse.parent==patient or patient.parent==nurse.parent)
			-- both nils or both non-nils!
		assert(((nx==nil) == (px==nil)) and ((ny==nil) == (py==nil)))
		-- TODO: Bad to create two identical parts for x and y...
					
		if nx ~= nil then
		local tx = nurse.left+nurse.width*nx + dx		-- target
			-- if nurse.id=="content" then print("nurse tx", nurse.left, nurse.width, nx, dx) end
			-- if patient.id=="content" then print("patient tx", nurse.left, nurse.width, nx, dx) end
			if patient.parent==nurse then tx=tx-nurse.left end	-- left=0 if i am parent
		-- if 0
		if px==0 then
			assert(nurse.parent~=patient)				-- left edje of parent cannot depend on child
			-- if patient.right - tx >= 0 then
				-- local delta = (patient.right - tx - patient.width)
				-- if math.abs(delta) > max_delta then max_delta = math.abs(delta) end
				-- patient.width = patient.width + delta
				-- if patient.drop=="drop" then print(debug.traceback("", 1)) end
			-- end
			local delta = (tx-patient.left)
			if math.abs(delta) > max_delta then max_delta = math.abs(delta) end
			patient.x = patient.x + delta
		-- if 1
		elseif px==1 then
			if nurse.parent~=patient then
				if tx - patient.left >= 0 then
					local delta = (tx - patient.left - patient.width)
					if math.abs(delta) > max_delta then max_delta = math.abs(delta) end
--print("return", max_delta)			
					patient.width = patient.width + delta
--print("return", max_delta)								
				end
				local delta = (tx-patient.right)
				if math.abs(delta) > max_delta then max_delta = math.abs(delta) end				
				patient.x = patient.x + delta
			else
				if tx >=0 then
					local delta = (tx-patient.width)
					if math.abs(delta) > max_delta then max_delta = math.abs(delta) end
					patient.width = patient.width + delta
				end
			end
		-- else just move
		else
			assert(nurse.parent~=patient)												-- parent pos cannot depend on child
			local sx = patient.left+patient.width*px				-- source			
			local delta = (tx-sx)
			if math.abs(delta) > max_delta then max_delta = math.abs(delta) end
			patient.x = patient.x + delta
		end
		end -- if nx ~= nil
		
		if ny ~= nil then
		local ty = nurse.top+nurse.height*ny + dy		-- target
			if patient.parent==nurse then ty=ty-nurse.top end	-- left=0 if i am parent
		-- if 0
		if py==0 then
			assert(nurse.parent~=patient)				-- left edje of parent cannot depend on child
			-- if patient.bottom - ty >= 0 then
				-- local delta = (patient.bottom - ty - patient.height)
				-- if math.abs(delta) > max_delta then max_delta = math.abs(delta) end
				-- patient.height = patient.height + delta
			-- end
			local delta = (ty-patient.top)
			if math.abs(delta) > max_delta then max_delta = math.abs(delta) end
			patient.y = patient.y + delta
		-- if 1
		elseif py==1 then
			if nurse.parent~=patient then
				if ty - patient.top >= 0 then
					local delta = (ty - patient.top - patient.height)
					if math.abs(delta) > max_delta then max_delta = math.abs(delta) end
					patient.height = patient.height + delta
				end
				local delta = (ty-patient.bottom)
				if math.abs(delta) > max_delta then max_delta = math.abs(delta) end				
				patient.y = patient.y + delta
			else
				if ty >=0 then
					local delta = (ty-patient.height)
					if math.abs(delta) > max_delta then max_delta = math.abs(delta) end
					patient.height = patient.height + delta
				end
			end
		-- else just move
		else
			assert(nurse.parent~=patient)												-- parent pos cannot depend on child
			local sy = patient.top+patient.height*py				-- source			
			local delta = (ty-sy)
			if math.abs(delta) > max_delta then max_delta = math.abs(delta) end			
			patient.y = patient.y + delta
		end
		end -- if ny ~= nil

		return max_delta
	end -- adjust_link
	
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
	
	self.old_link = function(_, patient, px, py, nurse, nx, ny, dx, dy)
		if dx == nil then dx = 0 end
		if dy == nil then dy = 0 end
		link_obj = {
			nurse = nurse,
			patient = patient,
			px = px,
			py = py,
			nx = nx,
			ny = ny,
			dx = dx,
			dy = dy
		}
		table.insert(links,link_obj)
		self:requestLayOut()
	end
	
	-- TODO: think again about "growing" and "shrinking" containers as in GTK
	
	self.onRequestLayOut = function()
		solver:solve()								-- updates vars in, solve and updates out
-- when solving and changing width, item man change its height
-- will solve again with no difference otherwise
--!!!		self.need_lay_out = false			-- will be raised again by child
--		print("set to false")
		do return end	-- our overloaded child should use editVar/editPoint!
--		print ("laying out", self.id)
		local max_delta
		local cnt = 0
		repeat
			cnt = cnt + 1
			max_delta = 0
			for i,link in ipairs(links) do
				delta = adjust_link(link)
				if delta > max_delta then max_delta = delta end
			end
--			print(max_delta)
--			if max_delta >= 0.5 then
--				self.need_lay_out = true
--			end
		until max_delta < 0.5
		-- needed if later "manual" lay-outer moves something 
		self.need_lay_out = false
--		print("Iterations:", cnt)
	end
	
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
end

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