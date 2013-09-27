Baloons = {
	margin = 20
}

-- TODO: need to create new proxy object and wrap original one
-- can't do it because of need to preserve object identity to pass it to C via luabind :(
function UseProperties(obj)
	local old_mt = getmetatable(obj)
	local mt = {}
	
	local props = {}
	
	mt.__newindex = function(self, key, val)
		local prop = props[prop_name]
		if prop then			-- its property
			local old = prop.value
			prop.value = val
			prop.onChange(obj, key, old)
		else							-- not property
			if rawget(obj, key) ~= nil then rawset(obj, key, val)
			else
				if old_mt.__newindex ~= nil then old_mt.__newindex(obj, key, val)
				else rawset(obj, key, val) end
			end
		end
	end
	
	setmetatable(obj, mt)
	
	obj.property = function(_, prop_name)
		local prop = props[prop_name]
		if prop==nil then
			prop = {value=nil, onChange = nil}
			props[prop_name] = prop
		end
		return prop
	end
	-- not found in mt - look at old_mt!
	setmetatable(mt, {})
	getmetatable(mt).__index = old_mt
	
	return obj
end

setmetatable(Baloons, {})
getmetatable(Baloons).__call = function()
--	local self = UseProperties(CompositeItem())
	local self = CompositeItem()
	
	self.width = screen_width
	self.height = screen_height
	self.rel_hpx, self.rel_hpy = 0, 0
	self.x, self.y = 0, 0

	self.background = TextureItem("", screen_width, screen_height)
	self.background.rel_hpx, self.background.rel_hpy = self.x/self.background.width, self.y/self.background.height
	self:add(self.background)	

	-- self:property("margin").value = 20
	-- self:property("margin").onChange = function() print("MARGIN CHANGED") end	
	
	self.baloons = {}
	self.baloons.add = function(_, b)
		if b[2]==nil then b[2]="" end
		if b[3]==nil then b[3]=b[2] end
		local obj = {text = b[1], sound = b[2], answer = b[3]}
		table.insert(self.baloons, obj)
		return self.baloons
	end
	
	local onscreen_baloons = 0
	local hit_right = {}
	local hit_wrong = {}			-- not currently in use
	local lost = {}
	local speed
	local current_answer = ""
	
	local launch
	
	-- TODO: do not need this func
	local killed = function()
		onscreen_baloons = onscreen_baloons - 1
		if onscreen_baloons==0 and #self.baloons==0 then
			print("Results: ", #hit_right, #lost)
			if self.onFinish then self:onFinish() end
		end
	end
	
	local out = function(b)
		table.insert(lost, b)
		killed()
	end
	
	-- returns true if right
	local hit = function(b)
		-- right
		if b.answer==current_answer then
			table.insert(hit_right, b)
			if self.max_speed ~= nil then
				speed = speed + (self.max_speed - self.launch_speed)*0.1
				if speed > self.max_speed then speed = self.max_speed end
			end
			killed()
			return true
		-- wrong
		else
--			table.insert(hit_wrong, b)
			if self.max_speed ~= nil then
				speed = speed - (self.max_speed - self.launch_speed)*0.1
				if speed < self.launch_speed then speed = self.launch_speed end
			end			
--			killed()
			return false
		end
	end
	
	launch = function()
		if speed==nil then speed = self.launch_speed end
	
		local b = table.remove(self.baloons, 1)		
		local obj = MakeMover(Baloons.Baloon(b.text))
			self:add(obj)
		obj.y = screen_height
		
		-- set x appropriately
		if self.launch_location_policy == "random" then
			obj.x = self.left_margin + (self.width-self.left_margin-self.right_margin-obj.width)*rand()
		elseif self.launch_location_policy == "choose" then
			local i = math.floor((#self.launch_points)*rand()) + 1
			obj.x = self.launch_points[i] - obj.width/2
		else
			error("launch_location_policy should be 'random' or 'choose'")
		end
		
		-- movement
		obj.onFrame = function(_, dt)
			obj.y = obj.y - dt * speed
			-- out
			if obj.bottom <= 0 then
				obj.visible = false
				obj:stop()
				out(b)
			end
		end -- onFrame()
		
		-- sound and correct answer
		-- TODO: Memory management here!!!
		if b.sound~=nil and b.sound~="" then
			local s = SoundEffect(b.sound)
			s.onFinish = function()
				current_answer = b.sound
			end
			s:play()
		end
				
		-- TODO: check if really mouse over
		obj.onDragEnd = function()
			-- process logic
			if hit(b) then
			-- remove
				local x = obj.left + obj.width/2
				local y = obj.top + obj.height/2
				obj:stop()
				obj.visible = false				
				self:remove(obj)				
			
			-- explode
				local explosion = AnimatedItem("explosion.anim")
				explosion.x, explosion.y = x, y
				self:add(explosion)
				explosion.onFinish = function() explosion:stop() explosion.visible=false end
				explosion:play()
			end
		end -- onDragEnd
		
		onscreen_baloons = onscreen_baloons + 1
		obj:start()
		
	end -- launch()
	
	self.start = function(_)
		assert(#self.baloons > 0)
		launch()
		
		local timer = Timer(function(t)
				if #self.baloons>0 then
					launch()
					t:restart(self.height / self.onscreen_count / speed)
				else
					t:cancel()
				end
				
		end, self.height / self.onscreen_count / speed)
	end
	
	return self
end

Baloons.mover_image = "interface/flask.rttex"

Baloons.Baloon = function(text)
	local self = CompositeItem()
	local oimage = ImageItem(Baloons.mover_image)
		self.width, self.height = oimage.width, oimage.height
		oimage.rel_hpx, oimage.rel_hpy = 0, 0
	local otext  = TextItem(text)
		otext.x, otext.y = self.width/2, self.height/2
	self.oimage = oimage
	self.otext = otext
	self:add(oimage):add(otext)
	self.rel_hpx, self.rel_hpy = 0, 0
	return self
end