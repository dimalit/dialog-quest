Buttons = {
	margin = 20,
	num_columns = 2,
	row_interval = 20,
	button_anim = "btn_rect.anim",
}

setmetatable(Buttons, {})
getmetatable(Buttons).__call = function(_,conf)
	if conf == nil then conf = {} end
	
	local self = CompositeItem()
	self.width = screen_width - Buttons.margin*2
	self.height = screen_height - Buttons.margin*2
	self.rel_hpx, self.rel_hpy = 0, 0
	self.x, self.y = Buttons.margin, Buttons.margin

	self.background = TextureItem("", screen_width, screen_height)
	self.background.rel_hpx, self.background.rel_hpy = self.x/self.background.width, self.y/self.background.height
	self.background.x, self.background.y = 0, 0
	self:add(self.background)	
	
	self.title = TextItem("self.title")
	self:add(self.title)
	self.title.rel_hpy = 0
	self.title.y = 0
	self.title.x = self.width / 2	

	self.description = FlowLayout()
	self:add(self.description)
	self:link(self.description, 0, nil, self, 0, nil)
	self:link(self.description, 1, nil, self, 1, nil)
	self:link(self.description, nil, 0, self.title, nil, 1)		

	self.columns = {}		
	
	for i=1,Buttons.num_columns do
		local it = CompositeItem()
		self:add(it)
		
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
			if #it.buttons>1 then	top = it.buttons[#it.buttons-1].bottom end
			child.y = top + Buttons.row_interval
			return it
		end -- add
		
		it.height = 200			-- just to see it
		table.insert(self.columns, it)
	end
	
	local old_onRequestLayout = self.onRequestLayOut
	self.onRequestLayOut = function(...)
		if old_onRequestLayout then old_onRequestLayout(unpack(arg)) end
	
		-- adjust columns
		for i,col in pairs(self.columns) do
			-- find max
			local max = 0
			for _,it in ipairs(col.buttons) do
				if it.width > max then max = it.width end
			end
			-- set pos
			for _,it in ipairs(col.buttons) do
				it.x = col.width / 2 - max / 2
			end			
		end -- for cols
	end
	
--	self.agree_button_label = TextItem("��� �������")
	
	-- self.agree_button = TwoStateAnimation(
		-- FrameItem(Buttons.button_up_frame, self.agree_button_label.width+10, self.agree_button_label.height+10),
		-- FrameItem(Buttons.button_down_frame, self.agree_button_label.width+10, self.agree_button_label.height+10)
	-- )	
	self.agree_button = TextButton{"��� �������", Buttons.button_anim, shrink=true}
	-- self.agree_button.x = self.width / 2
	-- self.agree_button.y = self.height - self.agree_button.height/2
	self:add(self.agree_button)
	self:link(self.agree_button, 0.5, 1, self, 0.5, 1)
	
--	self.agree_button_label.x, self.agree_button_label.y = self.agree_button.x, self.agree_button.y
--	self:add(self.agree_button_label)
	
	-- self.agree_button.onDragStart = function()
		-- self.agree_button:over(true)
	-- end
	
	self.agree_button.onClick = function()
		if self.onFinish then self:onFinish() end
	end
	
	return self
end

ButtonsElement = function(button_text, label_text, sound)
	local self = CompositeItem()

--	local left_label = TextItem(button_text)
	local right_label = TextItem(label_text)
	local button = TextButton{button_text, Buttons.button_anim,  shrink=true, padding=5, freeScale=true}
	-- local button = TwoStateAnimation(	FrameItem(Buttons.button_up_frame, left_label.width+10, left_label.height+10),
																		-- FrameItem(Buttons.button_down_frame, left_label.width+10, left_label.height+10))
	local sound = SoundEffect(sound)
--	self:add(button):add(left_label):add(right_label)
	self:add(button):add(right_label)
	
	button.onClick = function()
		sound:play()
	end

	local old_onRequestLayOut = self.onRequestLayOut
	self.onRequestLayOut = function(...)
		if old_onRequestLayout then old_onRequestLayout(unpack(arg)) end
		button.x, button.y = button.hpx, button.hpy
	--	left_label.x, left_label.y = button.x, button.y
		right_label.y = button.y
		right_label.x = button.right + right_label.hpx
		self.width = right_label.right
		self.height = button.height
	end

	return self
end

-- THINK abou layou requests!