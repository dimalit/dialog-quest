Baloons = {
	margin = 20,
	lines_interval = 80
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
		--local obj = {text = b[1], sound = b[2], answer = b[3]}
		table.insert(self.baloons, b)
		return self.baloons
	end
	
	local onscreen_baloons = 0
	local hit_right = {}
	local hit_wrong = {}			-- not currently in use
	local lost = {}
	local speed
	local current_answer = ""
	
	local launch
	local placeRandomly
	
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
		local obj = MakeMover(Baloons.Baloon(b.text, b.image))
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
			-- play
			SoundEffect(current_answer):play()			
			
			-- remove
				local x = obj.left + obj.width/2
				local y = obj.top + obj.height/2
				obj:stop()
				obj.visible = false				
				self:remove(obj)				
			
			-- explode
				local explosion = AnimatedItem(load_config("explosion.anim"))
				explosion.x, explosion.y = x, y
				self:add(explosion)
				explosion.onFinish = function() explosion:stop() explosion.visible=false end
				explosion:play()
			else
				if self.mistake_sound ~= nil then SoundEffect(self.mistake_sound):play() end			
			end
		end -- onDragEnd
		
		onscreen_baloons = onscreen_baloons + 1
		obj:start()
		
	end -- launch()
	
	local launch_all_stay = function()
	
		-- TODO speed needed by hit() but it's better to remove this dependency!
		if speed==nil then speed = self.launch_speed end
	
		local function next_sound()
			local b
			while #self.baloons>0 do
				b = table.remove(self.baloons, 1)
				if b.answer ~= nil and b.answer ~= "" then
					current_answer = b.answer
					SoundEffect(current_answer):play()
					return true
				end -- found
			end -- while
			
			-- not found
			return false
		end			
	
		local baloons = {}
		for _,b in ipairs(self.baloons) do
			local obj = MakeMover(Baloons.Baloon(b.text, b.image))
			self:add(obj)
			table.insert(baloons, obj)
			
			-- TODO There is the same function above!!
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
					local explosion = AnimatedItem(load_config("explosion.anim"))
					explosion.x, explosion.y = x, y
					self:add(explosion)
					explosion.onFinish = function() explosion:stop() explosion.visible=false end
					explosion:play()
				end -- if hit
				
				if not next_sound() then
					-- TODO merge with kill()
					print("Results: ", #hit_right, #lost)
					if self.onFinish then self:onFinish() end
				end
				
			end -- click!
		end -- for
				
		placeRandomly(baloons)
		
		next_sound()
	end
	
	self.start = function(_)
		assert(#self.baloons > 0)
		
		if self.fly then
			launch()
			Timer(function(t)
				if #self.baloons>0 then
					launch()
					t:restart(self.height / self.onscreen_count / speed)
				else
					t:cancel()
				end				
			end, self.height / self.onscreen_count / speed)
		else	--	not fly
			launch_all_stay()
		end
	end

	-- TODO Take the same function from scene_input and make a universl one
	placeRandomly = function(baloons)
		if #baloons==0 then return end
	
		local left = Baloons.margin
		local right = self.width - Baloons.margin
		local top = Baloons.margin
		local bottom = self.bottom - Baloons.margin

		local x = left + 20
		local y = top + 20
		
		-- put movers into temp_array and shuffle them
		local temp_array = {}
		local random_array = random_permutation(#baloons)
		for i=1,#random_array do
			local j = random_array[i]
			table.insert(temp_array, baloons[j])
		end		
		
		local lines = {}					-- for later centering
		table.insert(lines, {})
		
		-- put on screen
		for _,mover in pairs(temp_array)
		do
			if right-left-mover.width > 0 then
				mover.x, mover.y = x, y
				table.insert(lines[#lines], mover)
				x = x + mover.width + 20
				if x > right then														-- wrap
					x, y = 20, y + Baloons.lines_interval
					table.insert(lines, {})
				end
				if mover.right > right-20 then							-- place again
					mover.x, mover.y = x, y
					table.remove(lines[#lines-1])
					table.insert(lines[#lines], mover)
					x = x + mover.width + 20
				end
				--table.insert(placed_movers, mover)
			end -- if not too wide
		end -- for mover
		
		if #lines[1]==0 then return end					-- too narrow		
		
		-- center them
		local t = lines[1][1].top
		local b
			if #lines[#lines] > 0 then
				b = lines[#lines][1].bottom
			else
				b = lines[#lines-1][#lines[#lines-1]]
			end
		local vert_delta = (t-top + bottom-b)/2 - (t-top)
		
		for i=1,#lines do
		
			if #lines[i]==0 then break end
			
			local l = lines[i][1].left - left
			local r = right - lines[i][#lines[i]].right
			local delta = (l+r)/2 - (l-left)
			for j=1, #lines[i] do
				-- TODO: Why move() doesn't wor here!?
				--lines[i][j]:move(delta, vert_delta)
				lines[i][j].x = lines[i][j].x + delta
				lines[i][j].y = lines[i][j].y + vert_delta
			end
		end
		
	end -- placeRandomly	
	
	return self
end

-- no defaults!
--Baloons.mover_image = "interface/flask.rttex"

Baloons.Baloon = function(text, image)
	local self = CompositeItem()
	
	local oimage = image or Baloons.mover_image or nil
	if oimage then
		oimage = ImageItem(oimage)
		self.width, self.height = oimage.width, oimage.height
		oimage.rel_hpx, oimage.rel_hpy = 0, 0
		oimage.x, oimage.y = 0, 0
		self:add(oimage)
	end
	
	local otext
	if text and text~="" then
		otext  = TextItem(text)
		if oimage==nil then
			self.width, self.height = otext.width, otext.height
		end
		otext.x, otext.y = self.width/2, self.height/2
		self:add(otext)
	end
	
	self.oimage = oimage
	self.otext = otext
	self.rel_hpx, self.rel_hpy = 0, 0
	return self
end