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
        end
      end
    end,

    -- find existing and do assignment
    __newindex = function(self, key, val)
      for i=1, table.getn(arg) do
        if arg[i][key]~=nil then
          arg[i][key]=val
          return
        end
      end
	    -- set to first if not found
      arg[1][key] = val      
    end
  }
end

local item_properties = {
  x = true,
  y = true,
  width = true,
  height = true,
  rot = true,
  top = true,
  bottom = true,
  left = true,
  right = true,
  visible = true,
  destroy = true
}

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

ImageItem = function(parent, x, y, path)
  local item = SimpleItem(parent, x, y)
  local image = Image(path)
  local self = {}
  setmetatable(self, inherit(item, image))
  self.item = item
  self.view = image
  return self
end

TextItem = function(parent, x, y, text, font)
  local item = SimpleItem(parent, x, y)
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

TextBoxItem = function(parent, x, y, w, text)
  local item = SimpleItem(parent, x, y)
  local txt
  txt = TextBox(text, w, 0)

  local self = {}
  setmetatable(self, inherit(item, txt))
  self.item = item
  self.view = txt
  return self
end

-- make playing animation item
AnimatedItem = function(parent, x, y, name)
  local item = SimpleItem(parent, x, y)
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

function TwoStateAnimation(anim)
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

function DropArea(parent, x, y, view)
  local item = SimpleItem(parent, x, y)
  item.view = view

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
    obj.ox = item.x obj.oy = item.y
    -- attach new objects to each other
    self.object = obj
    obj.drop = self
  end

  local over = function(dummy, arg)
    return view:over(arg)
  end

  local self = {
    intersects = intersects,
    dist = dist,
    take = take,
    over = over
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

function Mover(parent, x, y, view)
  if x == nil then x = 0 end
  if y == nil then y = 0 end

  local item = MakeMover(SimpleItem(parent, x, y))    -- put it into closure!
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
Button = function(parent, x, y, view)
  local item = SimpleItem(parent, x, y)
  local self = {}
  setmetatable(self, inherit(item, view))
  self.view = view
  
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