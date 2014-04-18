Buttons = {
	margin = 20,
	num_columns = 2,
	row_interval = 10,
	button_anim = "btn_rect.anim",
}

setmetatable(Buttons, {})
getmetatable(Buttons).__call = function(_,conf)
	if conf == nil then conf = {} end
	
	local self = CompositeItem()
	self.id = "scene"
	self.width = screen_width - Buttons.margin*2
	self.height = screen_height - Buttons.margin*2
	-- self.x, self.y = Buttons.margin, Buttons.margin
	-- TODO: bad to restrict own x, y	
	self:restrict(Expr(self, "x"), "==", Expr(Buttons.margin))
	self:restrict(Expr(self, "y"), "==", Expr(Buttons.margin))
	self:restrict(Expr(self, "width"), "==", Expr(self.width))
	self:restrict(Expr(self, "height"), "==", Expr(self.height))				-- TODO: add special function a-la "keep value"?
	
	self.background = TextureItem("", screen_width, screen_height)
	self.background.id = "back"
	-- self.background.rel_hpx, self.background.rel_hpy = Buttons.margin/self.background.width, Buttons.margin/self.background.height
	-- self.background.x, self.background.y = 0, 0
	self:add(self.background)
	--TODO: For some misterious reason this would fail on absence of __eq:
	--self:link(self.background, 0, 0, self, 0, 0, -Buttons.margin, -Buttons.margin)
	self:link(self.background, 0, 0, self, 0, 0, -Buttons.margin, -Buttons.margin)
	
	self.title = TextItem("self.title")
	self.title.id = "title"
	self:add(self.title)
	-- self.title.rel_hpy = 0
	-- self.title.y = 0
	-- self.title.x = self.width / 2	
	self:link(self.title, 0.5, 0, self, 0.5, 0)

	self.description = FlowLayout()
	self.description.id = "desc"
	self:add(self.description)
	self:link(self.description, 0, nil, self, 0, nil)
	self:link(self.description, 1, nil, self, 1, nil)
	self:link(self.description, nil, 0, self.title, nil, 1)		

	self.columns = {}
	self.rows = {}					-- dummy items for alignment
	
	for i=1,Buttons.num_columns do
		local it = CompositeItem()
		it.id = "col_"..i
		it.rigid_width, it.rigid_height = false, true
		self:add(it)
		it.debugDrawColor = 0xff0000ff
		
		-- align top
		self:link(it, nil, 0, self.description, nil, 1, 0, Buttons.margin)
		-- align edges
		--self:link(it, 0, nil, self, 1/Buttons.num_columns*(i-1), nil)
		----self:link(it, 1, nil, self, 1/Buttons.num_columns*i, nil)
		self:restrict(Expr(it, "width"), "<=", Expr(self, "width")/Expr(Buttons.num_columns))								-- not more then W/n (and not less then max  child's width)
		
		it.buttons = {}
		
		local old_add = it.add
		it.add = function(_, arr)
			-- make good child
			local child = ButtonsElement(arr[1], arr[2], arr[3])
			table.insert(it.buttons, child)			
			-- place it on scene!
			self:add(child)
			child.rel_hpx, child.rel_hpy = 0, 0
			local top = 0
			if #it.buttons>1 then	top = it.buttons[#it.buttons-1].top end
			--child.y = top + Buttons.row_interval
			if #it.buttons==1 then	-- just me
				--self:link(child, nil, 0, it, nil, 0, 0, 0)		-- link to col object but inside scene!
				self:restrict(Expr(child, "y"), "==", Expr(it, "y"))		-- see TODO below
			else
				--self:link(child, nil, 0, it.buttons[#it.buttons-1], nil, 1, 0, Buttons.row_interval)		-- link to prev
				self:restrict(Expr(child, "y"), ">=", Expr(it.buttons[#it.buttons-1], "y")+Expr(it.buttons[#it.buttons-1], "height")+Expr(Buttons.row_interval))
			end
			self:link(child, 0, nil, it, 0, nil);
			--self:link(child, 1, nil, it, 1, nil);
			self:restrict(Expr(child, "width"), "<=", Expr(it, "width"))
			
			-- adjust row
				-- create
			local row
			if self.rows[#it.buttons] == nil then
				row = ScreenItem()
				row.id = "row_"..#it.buttons
				self:add(row)
				self:link(row, 0, nil, self, 0, nil)
				self:link(row, 1, nil, self, 1, nil)
				self.rows[#it.buttons] = row
			else
				row = self.rows[#it.buttons]
			end
				-- resize
				-- TODO: When we use here 1-st row - title and description disappear! Resolve it!!
				-- see also linking of first buttons to the top (above)
			if #it.buttons > 1	then self:link(row, nil, 0, child, nil, 0, 0) end			-- do not use row for the top
			
			return it
		end -- add
		
		-- equalize the size and center them
		if #self.columns > 0 then
			-- all are equal to 1st and greater then their maximums
			self:restrict(Expr(it, "width"), "==", Expr(self.columns[1], "width"))
			-- distribute evenly: my distance to prev equals to 1st distance to 0
			-- see also below for last element
			local prev = self.columns[#self.columns]
			self:restrict(Expr(it, "x")-Expr(prev, "x")-Expr(prev, "width"), "==", Expr(self.columns[1], "x"))
		end
		
		it.height = 200			-- just to see it
		table.insert(self.columns, it)
	end

	-- space at right edge equals to one at left edge
	if #self.columns > 0 then
		local last = self.columns[#self.columns]
		self:restrict(Expr(self, "width")-Expr(last, "x")-Expr(last, "width"), "==", Expr(self.columns[1], "x"))
		self:maximize(Expr(self.columns[1], "x"));
	end
	
	local old_onRequestLayOut = self.onRequestLayOut
	self.onRequestLayOut = function(...)	
		if old_onRequestLayOut then old_onRequestLayOut(unpack(arg)) end
	end
	
	-- local old_onRequestLayout = self.onRequestLayOut
	-- self.onRequestLayOut = function(...)
		-- if old_onRequestLayout then old_onRequestLayout(unpack(arg)) end
	
		-- -- adjust columns
		-- for i,col in pairs(self.columns) do
			-- -- find max
			-- local max = 0
			-- for _,it in ipairs(col.buttons) do
				-- if it.width > max then max = it.width end
			-- end
			-- -- set pos
			-- for _,it in ipairs(col.buttons) do
				-- it.x = col.width / 2 - max / 2
			-- end			
		-- end -- for cols
	-- end
	
--	self.agree_button_label = TextItem("Мне понятно")
	
	-- self.agree_button = TwoStateAnimation(
		-- FrameItem(Buttons.button_up_frame, self.agree_button_label.width+10, self.agree_button_label.height+10),
		-- FrameItem(Buttons.button_down_frame, self.agree_button_label.width+10, self.agree_button_label.height+10)
	-- )	
	self.agree_button = TextButton{"Мне понятно", Buttons.button_anim, shrink=true, one_line=true}
	--self.agree_button.width, self.agree_button.height = 163.333, 30
		--self.agree_button.x = self.width / 2
	 --self.agree_button.y = self.height - self.agree_button.height/2
	self:add(self.agree_button)
	
	--self:link(self.agree_button, 0.5, 1, self, 0.5, 1)
	
--	self:link(self.agree_button, nil, 0, self, nil, 1, 0, -25)
	
	--self:link(self.agree_button, nil, 1, self, nil, 1)
	-- TODO: With mistake: == y + height it will fail assertion on 3 iterations. How do diagnose it?

--	self:restrict(Expr(self.agree_button, "y") + Expr(self.agree_button, "height"), "==", Expr(self, "height"))
	self:link(self.agree_button, 0.5, 1, self, 0.5, 1)
--	self:link(self.agree_button, 1, nil, self, 0.5, nil, 80, 0)
		
	self.agree_button.onClick = function()
		if self.onFinish then self:onFinish() end
	end
	return self
end

ButtonsElement = function(button_text, label_text, sound)
	local self = CompositeItem()
	self.rigid_width = true			-- should obey outer commands but can also influence them!
	self.debugDrawColor = 0x00ffffff
	self.id = "ButtonsElement"

--	local left_label = TextItem(button_text)
	local right_label	= nil
	local button			= nil
	local sound = SoundEffect(sound)

	local button_present = button_text~=nil and button_text~=""
	local label_present   = label_text~=nil and label_text~=""

	-- place both
	if button_present and label_present then
		button = TextButton{button_text, Buttons.button_anim,  shrink=true, padding=5, freeScale=true}	
		
		right_label = TextBoxItem(label_text)
		right_label.id="right_label"	
		
		self:add(button)
		self:add(right_label)
		
		self:link(button, 0, 0, self, 0, 0)
		self:link(right_label, 0, nil, button, 1, nil, 5,0)
		self:link(right_label, nil, 0, self, nil, 0)		
		
		-- restrict with
		--self:restrict(Expr(right_label, "x")+Expr(right_label, "width"), "<=", Expr(self, "width"))
		
		-- height
		--self:link(self, nil, 1, button, nil, 1)
		self:restrict(Expr(self, "height"), ">=", Expr(button, "y")+Expr(button, "height"))
		self:restrict(Expr(self, "height"), ">=", Expr(right_label, "y")+Expr(right_label, "height"))

--		self:link(button, 1, nil, self, 0.5, nil)	-- button - half width
--		self:link(self, 1, nil, right_label, 1, nil)	-- self - full width
	
		-- TODO: This should work!
--		self:link(right_label, 1, nil, self, 1, nil)		
	-- place only button
	elseif button_present then
			button = TextButton{button_text, Buttons.button_anim,  shrink=true, padding=5, freeScale=true}		
			self:add(button)
			self:link(button, 0, 0, self, 0, 0)
			self:link(self, nil, 1, button, nil, 1)
--			self:link(button, 1, nil, self, 1, nil)
	-- place only label
	elseif label_present then
		right_label = TextBoxItem(label_text)
		right_label.id="right_label"		
	
		self:add(right_label)
		self:link(right_label, 0, 0, self, 0, 0)
--		self:link(right_label, 1, nil, self, 1, nil)		
		self:link(self, nil, 1, right_label, nil, 1)
	else
		error("Create either button or label!")
	end
	
	if button_present then
		button.onClick = function()
			sound:play()
		end
	end

	local old_onRequestLayOut = self.onRequestLayOut
	self.onRequestLayOut = function(...)
		-- use auto lay-out
		if old_onRequestLayOut then old_onRequestLayOut(unpack(arg)) end

		print "Correcting:"
--		self.solver:beginEdit()		
		-- correct it
		if button~=nil and right_label~=nil then
			local w1 = button.oneLineWidth
			local w2 = right_label.oneLineWidth
			
			if(w1 < self.width) then
				button.width = w1
				if(w1+5+w2 <= self.width) then
					right_label.width = w2
				else
					right_label.width = self.width - w1 - 5
				end
--				self.solver:suggestValue(right_label, "width")
			else
				button.width = self.width / 2
				right_label.x = button.right + 5
				right_label.width = self.width - right_label.left
--				self.solver:suggestValue(button, "width")
--				self.solver:suggestValue(right_label, "x")
--				self.solver:suggestValue(right_label, "width")
			end
		elseif button~=nil then
				if(button.oneLineWidth <= self.width) then
					button.width = button.oneLineWidth
				else
					button.width = self.width
				end
--				self.solver:suggestValue(button, "width")
		elseif right_label~=nil then
				if(right_label.oneLineWidth <= self.width) then
					right_label.width = right_label.oneLineWidth
				else
					right_label.width = self.width
				end
--				self.solver:suggestValue(right_label, "width")
		end
		
		-- shrink if needed
		local right = 0
		if button~=nil then right = button.right end
		if right_label~=nil then right = right_label.right end
		if self.width > right then
			self.width = right
--			self.solver:suggestValue(self, "width")
		end
		
--		print(button.width, right_label.width)		
--		self.solver:endEdit()
		self.solver:getExternalVariables()
--		if old_onRequestLayOut then old_onRequestLayOut(unpack(arg)) end
		
		-- print(self.left, self.top, self.width, self.height)
		-- button.x, button.y = button.hpx, button.hpy
	-- --	left_label.x, left_label.y = button.x, button.y
		-- right_label.y = button.y
		-- right_label.x = button.right + right_label.hpx
		-- self.width = right_label.right
		-- self.height = button.height
	end

	local old_adjustSize = self.adjustSize
	self.adjustSize = function(...)
		old_adjustSize(unpack(arg))
		local w1, w2 = 0, 0
		if button ~= nil then w1 = button.oneLineWidth end
		if right_label ~= nil then w2 = right_label.oneLineWidth end
		self.width = w1+5+w2						-- TODO: because right label layout is computed manually!!
		print("--adjustSize", self.id, self.width)
		return false, false
	end
	
	return self
end

-- THINK about layout requests!