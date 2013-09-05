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
local function inherit(...)
  return {
    -- find key in parents
    __index = function(self, key)
      for i=1, #arg do
        if arg[i][key]~=nil then
          -- handle functions!
          if type(arg[i][key]) == "function"
          then
            local par = arg;
            return function(...) return par[i][key](par[i], unpack(arg, 2)) end
          else
            return arg[i][key]
          end -- if func
        end -- id not nil
      end -- for parents
	  -- if not found
	  return nil
    end,

    -- find existing and do assignment
    __newindex = function(self, key, val)
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
  }
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
  local txt = Text(text)
	item.view = txt

  local self = {}
  self.item = item
  setmetatable(self, inherit(item, txt))
  return self
end

TextBoxItem = function(text, w)
  if w == nil then w = 0 end
  
  local item = SimpleItem()
  local txt = TextBox(text, w, 0)
  item.view = txt

  local self = {}
  self.item = item
  setmetatable(self, inherit(item, txt))
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
				item.hpx_relative, item.hpy_relative = 0, 0	
			-- if item
			if type(item.firstLineDecrement)=="nil" then
				item.x, item.y = cur_x, cur_y
				cur_x = cur_x + item.width
				while cur_x > self.width - profile.right:at(cur_y, item.height) do
					cur_y = cur_y + 24							-- TODO Must know font line height here!!
					cur_x = profile.left:at(cur_y, item.height)
					item.x, item.y = cur_x, cur_y
					cur_x = cur_x + item.width
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
	
		tex.c1.hpx_relative, tex.c1.hpy_relative = 0, 0
			tex.c1.x, tex.c1.y = 0, 0
		tex.c2.hpx_relative, tex.c2.hpy_relative = 1, 0
			tex.c2.x, tex.c2.y = self.width, 0
		tex.c3.hpx_relative, tex.c3.hpy_relative = 1, 1
			tex.c3.x, tex.c3.y = self.width, self.height	
		tex.c4.hpx_relative, tex.c4.hpy_relative = 0, 1
			tex.c4.x, tex.c4.y = 0, self.height	
			
		tex.bl.hpx_relative = 0
			tex.bl.x, tex.bl.y = 0, self.height/2	
		tex.br.hpx_relative = 1
			tex.br.x, tex.br.y = self.width, self.height/2	
		tex.bt.hpy_relative = 0
			tex.bt.x, tex.bt.y = self.width/2, 0	
		tex.bb.hpy_relative = 1
			tex.bb.x, tex.bb.y = self.width/2, self.height	
	end
	self:onRequestLayOut(self)
  
  return self
end

---------- Layouts -------------
function MakeLayoutAgent(obj)
	if obj.rel_x ~= nil then
		print("Warning: attempt of repeated MakeLayoutAgent")
		return obj
	end
	local self = {}
	self.item = obj
	
	local cord = {
		rel_x = obj.x,
		rel_y = obj.y
	}
	
	local pos_origin, pos_origin_xr, pos_origin_yr
	local width_origin
	local height_origin
	
	local dependents = {}	

	local old_onmove = obj.onMove
	obj.onMove = function(_)
		if old_onmove then old_onmove(obj) end
		for dep,_ in pairs(dependents) do
			dep:updateLocation(self)
		end
	end
	
	self.updateLocation = function(_, origin)
		-- update position
		if origin==pos_origin then
			obj.gx = origin.gx - origin.hpx + origin.width*pos_origin_xr  + cord.rel_x
			obj.gy = origin.gy - origin.hpy + origin.height*pos_origin_yr + cord.rel_y
		end
		-- update width
		if origin==width_origin then
			obj.width = origin.width
		end
		-- update height
		if origin==height_origin then
			obj.height = origin.height
		end
	end
	
	self.setLocationOrigin = function(_, ref_obj, xr, yr)
		if pos_origin then pos_origin:removeDependent(self) end
		pos_origin, pos_origin_xr, pos_origin_yr = ref_obj, xr, yr
		if pos_origin then
			pos_origin:addDependent(self)
			self:updateLocation(pos_origin)
		else
			obj.x, obj.y = cord.rel_x, cord.rel_y	-- if nil
		end		
	end
	self.setWidthOrigin = function(_, ref_obj)
		if width_origin then width_origin:removeDependent(self) end
		width_origin = ref_obj
		if width_origin then
			width_origin:addDependent(self)
			self:updateLocation(width_origin)
		end
	end
	self.setHeightOrigin = function(_, ref_obj)
		if height_origin then height_origin:removeDependent(self) end
		height_origin = ref_obj
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
	
	-- NOTE: Cannot use direct indexing here because functions need correct 1st arg!
	local obj_mt = inherit(obj)
	-- hook access to x and y to make movement relative
	setmetatable(self, {})
	getmetatable(self).__newindex = function(_, key, val)
	-- handle x/y
		if key=='rel_x' or key=='rel_y' then
			cord[key] = val
			if pos_origin~=nil then
				self:updateLocation(pos_origin)
			elseif pos_origin==nil then
				obj[key] = val
			end
		else
	-- handle anything else
			obj_mt.__newindex(_, key, val)		
		end -- if not x, y
	end	-- newindex	
	getmetatable(self).__index = function(_, key)
		if key=='rel_x' or key=='rel_y' then
			return cord[key]
		else
			return obj_mt.__index(_, key)
		end -- if not x, y
	end	-- newindex	
	
	return self
end -- MakeLayoutAgent

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
			obj.hpx_relative, obj.hpy_relative = 0, 0
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
			obj.hpx_relative, obj.hpy_relative = 0, 0
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

function Mover(view)
  local item = MakeMover(SimpleItem())    -- put it into closure!
  item.view = view

  local self = {
    item = item,
    view = view
  }
  
  local ox = x		-- origin x
  local oy = y		-- origin y
  local drop = nil

  item.onFrame = function(dummy, dt)
--    print(self.x, self.y, self.ox, self.oy, dt)
    local norm = dist(ox, oy, self.x, self.y)
    if norm > self.prev_norm or norm < 50 then	-- if passed over!
      self.x = ox; self.y = oy
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
    ox=item.x oy=item.y
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
	local norm = dist(ox, oy, self.x, self.y)
	self.vx = (ox - self.x) / norm * 2000
    self.vy = (oy - self.y) / norm * 2000
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
dofile("make_layout_agents.lua")
dofile("scene.lua")