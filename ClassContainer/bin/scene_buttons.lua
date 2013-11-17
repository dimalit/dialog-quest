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
	self.rel_hpx, self.rel_hpy = 0, 0
	self.x, self.y = Buttons.margin, Buttons.margin

	self.background = TextureItem("", screen_width, screen_height)
	self.background.rel_hpx, self.background.rel_hpy = self.x/self.background.width, self.y/self.background.height
	self.background.x, self.background.y = 0, 0
	self:add(self.background)	
	
	self.title = TextItem("self.title")
	self.title.id = "title"
	self:add(self.title)
	self.title.rel_hpy = 0
	self.title.y = 0
	self.title.x = self.width / 2	

	self.description = FlowLayout()
	self.description.id = "desc"
	self:add(self.description)
	self:link(self.description, 0, nil, self, 0, nil)
	self:link(self.description, 1, nil, self, 1, nil)
	self:link(self.description, nil, 0, self.title, nil, 1)		

	self.columns = {}		
	
	for i=1,Buttons.num_columns do
		local it = CompositeItem()
		it.id = "col_"..i
		self:add(it)
		it.debugDrawColor = 0xff0000ff
		
		-- align top
		self:link(it, nil, 0, self.description, nil, 1, 0, Buttons.margin)
		-- align edges
		self:link(it, 0, nil, self, 1/Buttons.num_columns*(i-1), nil)
		self:link(it, 1, nil, self, 1/Buttons.num_columns*i, nil)
		
		it.buttons = {}
		
		local old_add = it.add
		it.add = function(_, arr)
			-- make good child
			local child = ButtonsElement(arr[1], arr[2], arr[3])
			table.insert(it.buttons, child)			
			-- place it
			old_add(it, child)
			child.rel_hpx, child.rel_hpy = 0, 0
			local top = 0
			if #it.buttons>1 then	top = it.buttons[#it.buttons-1].top end
			--child.y = top + Buttons.row_interval
			if #it.buttons==1 then	-- just me
				it:link(child, nil, 0, it, nil, 0, 0, 0)		-- link to parent
			else
				it:link(child, nil, 0, it.buttons[#it.buttons-1], nil, 1, 0, Buttons.row_interval)		-- link to prev
			end
			it:link(child, 0, nil, it, 0, nil);
			it:link(child, 1, nil, it, 1, nil);
			return it
		end -- add
		
		it.height = 200			-- just to see it
		table.insert(self.columns, it)
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
	self.agree_button = TextButton{"Мне понятно", Buttons.button_anim, shrink=true}
	self.agree_button.width, self.agree_button.height = 200, 25
		--self.agree_button.x = self.width / 2
	 --self.agree_button.y = self.height - self.agree_button.height/2
	self:add(self.agree_button)
	
	--self:link(self.agree_button, 0.5, 1, self, 0.5, 1)
	
--	self:link(self.agree_button, nil, 0, self, nil, 1, 0, -25)
	
	--self:link(self.agree_button, nil, 1, self, nil, 1)
	-- TODO: With mistake: == y + height it will fail assertion on 3 iterations. How do diagnose it?
	self:restrict(Expr(self.agree_button, "y") + Expr(self.agree_button, "height"), "==", Expr(self, "height"))
	self:link(self.agree_button, 0, nil, self, 0.5, nil, -80, 0)
	self:link(self.agree_button, 1, nil, self, 0.5, nil, 80, 0)
		
	self.agree_button.onClick = function()
		if self.onFinish then self:onFinish() end
	end
	return self
end

ButtonsElement = function(button_text, label_text, sound)
	local self = CompositeItem()
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
		right_label = TextBoxItem(label_text, 200)
		right_label.id="right_label"	
		
		self:add(button)
		self:add(right_label)
		
		self:link(button, 0, 0, self, 0, 0)
		self:link(self, nil, 1, button, nil, 1)
		self:link(button, 1, nil, self, 0.5, nil)	-- button - half width
		self:link(right_label, 0, nil, button, 1, nil, 5,0)
		self:link(right_label, nil, 0.5, self, nil, 0.5)		
		-- TODO: This should work!
--		self:link(right_label, 1, nil, self, 1, nil)		
	-- place only button
	elseif button_present then
			button = TextButton{button_text, Buttons.button_anim,  shrink=true, padding=5, freeScale=true}		
	
			self:add(button)
			self:link(button, 0, 0, self, 0, 0)
			self:link(self, nil, 1, button, nil, 1)
			self:link(button, 1, nil, self, 1, nil)
	-- place only label
	elseif label_present then
		right_label = TextBoxItem(label_text, 200)
		right_label.id="right_label"		
	
		self:add(right_label)
		self:link(right_label, 0, 0, self, 0, 0)
		self:link(right_label, 1, nil, self, 1, nil)		
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
--		print("ButtonElement begin")
		if old_onRequestLayOut then old_onRequestLayOut(unpack(arg)) end
--		print("ButtonElement end")
--		print(self.left, self.top, self.width, self.height)
		-- button.x, button.y = button.hpx, button.hpy
	-- --	left_label.x, left_label.y = button.x, button.y
		-- right_label.y = button.y
		-- right_label.x = button.right + right_label.hpx
		-- self.width = right_label.right
		-- self.height = button.height
	end

	return self
end

-- THINK abou layou requests!