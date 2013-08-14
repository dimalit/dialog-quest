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
            return function(...) par[i][key](par[i], unpack(arg, 2)) end
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
      for i=1, table.getn(arg) do
        if arg[i][key]~=nil then
          arg[i][key]=val
          return
        end
      end
	  -- set to ALL! if not found
      for i=1, table.getn(arg) do
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

ImageItem = function(path, x, y)
  if x == nil then x = 0 end
  if y == nil then y = 0 end
  
  local item = SimpleItem(x, y)
  local image = Image(path)
  local self = {}
  setmetatable(self, inherit(item, image))
  self.item = item
  self.view = image
  return self
end

TextureItem = function(path, width, height, x, y)
  if x == nil then x = 0 end
  if y == nil then y = 0 end

  local item = SimpleItem(x, y)
  local texture = Texture(path, width, height)
  local self = {}
  setmetatable(self, inherit(item, texture))
  self.item = item
  self.view = texture
  return self
end

TextItem = function(text, font_or_x, x_or_y, y_or_nil)
  local font, x, y
  if type(font_or_x)=="number" then
	x = font_or_x
	y = x_or_y
  else
	font = font_or_x
	x = x_or_y
	y = y_or_nil
  end
  local item = SimpleItem(x, y)
  local txt
--  if font ~= nil then
--    txt = Text(text, font)
--   else
    txt = Text(text)
--  end

  local self = {}
  setmetatable(self, inherit(item, txt))
  self.item = item
  self.view = txt
  return self
end

TextBoxItem = function(text, w, x, y)
  if w == nil then w = 0 end
  if x == nil then x = 0 end
  if y == nil then y = 0 end
  
  local item = SimpleItem(x, y)
  local txt
  txt = TextBox(text, w, 0)

  local self = {}
  setmetatable(self, inherit(item, txt))
  self.item = item
  item.view = txt	-- TODO Which of these and why second doesn't work?
--  self.view = txt 
  return self
end

-- make playing animation item
AnimatedItem = function(name, x, y)
  if x == nil then x = 0 end
  if y == nil then y = 0 end

  local item = SimpleItem(x, y)
  local anim = Animation(load_config(name))
 
  local self = {}
  setmetatable(self, inherit(item, anim))
  self.item = item
  self.view = anim

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

function FlowLayout(w, x, y)
  if x == nil then x = 0 end
  if y == nil then y = 0 end

  local self = CompositeItem(x, y)
  self.width = w
  
  local items     = {}		-- array
  local obstacles = {}		-- set
  local profile = { left = StairsProfile(), right = StairsProfile() }
  
  local lay_out = function()
	local cur_x, cur_y = -self.hpx+profile.left:at(0,1), -self.hpy		-- from hotspot!
	for _,item in ipairs(items) do
		-- TODO Check how to use == operator to compare references!
		--assert(item.parent == self)
	    item.hpx_relative, item.hpy_relative = 0, 0	
	  -- if item
	  if type(item.firstLineDecrement)=="nil" then
		item.x, item.y = cur_x, cur_y
		cur_x = cur_x + item.width
		while cur_x > self.width-profile.right:at(cur_y, item.height) do
			cur_y = cur_y + 24							-- TODO Must know font line height here!!
			cur_x = profile.left:at(cur_y, item.height)
			item.x, item.y = cur_x, cur_y
			cur_x = cur_x + item.width
			assert(cur_y < 10000)						-- in case of hanging
		end -- while line over	  
	  -- if text
	  else
		item.firstLineDecrement = cur_x+self.hpx					-- TODO Align baselines here!!
		item.x, item.y = -self.hpx, cur_y
		item.width = self.width
		profile.left:shifted(-cur_y)
		item.leftObstacles = profile.left:shifted(-cur_y)
		item.rightObstacles = profile.right:shifted(-cur_y)		
		cur_x, cur_y = -self.hpx + item.lastLineEndX, cur_y + item.lastLineEndY
	  end -- select type
	end -- for
	self.height = cur_y + 24			-- same magic number!
	
	-- check also obstacles
	for obst,_ in pairs(obstacles) do
		if obst.bottom+self.hpy > self.height then self.height = obst.bottom+self.hpy end
	end
  end -- lay_out()
  
  self.addItem = function(self, item)
	self:add(item)
	table.insert(items, item)
	lay_out()
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
	lay_out()
  end  
  
  self.addObstacle = function(self, obst, side)
	if side==nil then side="left" end
	assert(side=="left" or side=="right")
	obstacles[obst] = true
	self:add(obst)
	if side=="left" then
		profile["left"]:setInterval(obst.top, obst.height, obst.right+self.hpx)
	else
		profile["right"]:setInterval(obst.top, obst.height, self.width - (obst.left+self.hpx))
	end
	lay_out()
  end
  
  self.clearObstacles = function(self)
    for obst,val in pairs(obstacles) do
		if val then obst:destroy() end
	end
	obstacles = {}
	profile.left = StairsProfile()
	profile.right = StairsProfile()
	lay_out()
  end
  
  return self
end -- FlowLayoutItem

function FrameItem(name, w, h, x, y)
  if x == nil then x = 0 end
  if y == nil then y = 0 end
  
  local self = CompositeItem(x, y)
  self.width, self.height = w, h
  
  local left, right, top, bottom = self.left-self.x, self.right-self.x, self.top-self.y, self.bottom-self.y
  
  local tex = {
	c1 = ImageItem(name.."_c1.rttex", left, top),
	c2 = ImageItem(name.."_c2.rttex", right, top),
	c3 = ImageItem(name.."_c3.rttex", right, bottom),
	c4 = ImageItem(name.."_c4.rttex", left, bottom),
	bl = TextureItem(name.."_bl.rttex", 4, self.height-8, left, 0),
	br = TextureItem(name.."_br.rttex", 4, self.height-8, right, 0),
	bt = TextureItem(name.."_bt.rttex", self.width-8, 4,  0, top),
	bb = TextureItem(name.."_bb.rttex", self.width-8, 4,  0, bottom)
  }
  tex.c1.hpx_relative, tex.c1.hpy_relative = 0, 0
  tex.c2.hpx_relative, tex.c2.hpy_relative = 1, 0
  tex.c3.hpx_relative, tex.c3.hpy_relative = 1, 1
  tex.c4.hpx_relative, tex.c4.hpy_relative = 0, 1
  tex.bl.hpx_relative = 0
  tex.br.hpx_relative = 1
  tex.bt.hpy_relative = 0
  tex.bb.hpy_relative = 1
  
  -- TODO: Implement add(array)
  self:add(tex.c1):add(tex.c2):add(tex.c3):add(tex.c4):add(tex.bl):add(tex.br):add(tex.bt):add(tex.bb)
  
  self.tex = tex
  
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

function TwoStateAnimation4(i1, i2, x, y)
  if x == nil then x = 0 end
  if y == nil then y = 0 end

  local self = CompositeItem(x, y)
  self:add(i1):add(i2)
  
  i1.visible, i2.visible = true, false
  -- TODO Mouse pointer checking will work wrong if we have x, y != 0
  self.width, self.height = i1.width, i1.height
  
  self.over = function(self, arg)
    if arg then
		i1.visible, i2.visible = false, true
		self.width, self.height = i2.width, i2.height
    elseif arg==false
    then
		i1.visible, i2.visible = true, false
		self.width, self.height = i1.width, i1.height
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

function Mover(view, x, y)
  if x == nil then x = 0 end
  if y == nil then y = 0 end

  local item = MakeMover(SimpleItem(x, y))    -- put it into closure!
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
Button = function(view, x, y)
  if x == nil then x = 0 end
  if y == nil then y = 0 end
  
  local item = SimpleItem(x, y)
  local self = {}
  setmetatable(self, inherit(item, view))
  self.view = view
  self.item = item
  
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
    print(shift_str, k, " => ", v)
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
--add_layer("default")
dofile("scene.lua")