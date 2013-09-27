Buttons = {
	margin = 20,
	num_columns = 2,
	row_interval = 20
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
	self:add(self.background)	
	
	self.title = TextItem("self.title")
	self:add(self.title)
	self.title.rel_hpy = 0
	self.title.y = 0
	self.title.x = self.width / 2	
	
	self.description = CompositeItem()
	self:add(self.description)
	self:link(self.description, 0, nil, self, 0, nil)
	self:link(self.description, 1, nil, self, 1, nil)
	self:link(self.description, nil, 0, self.title, nil, 1, 0, Buttons.margin)		

	self.columns = {}		
	
	for i=1,Buttons.num_columns do
		local it = CompositeItem()
		self:add(it)
		
		-- align top
		self:link(it, nil, 0, self.title, nil, 1, 0, Buttons.margin)
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
			self:requestLayOut(it)
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
	
	local check_finish = function()
		if #left_side.items + #right_side.items == #all_words and
			 self.onFinish
		then
			 self:onFinish()
		end
	end
	
	return self
end

class 'ButtonsElement'(CompositeItem)

ButtonsElement.__init = function(self, button_text, label_text, sound)
	CompositeItem.__init(self)

	local button = TextItem(button_text)
	local label = TextItem(label_text)
	local sound = SoundEffect(sound)
	self:add(button):add(label)
	label.rel_hpx, label.rel_hpy = 0, 0
	button.rel_hpx, button.rel_hpy = 0, 0
	
	button.onDragEnd = function()
		sound:play()
	end

	-- HACK
	label.x = button.right
	self.width = label.right
	self.height = max(button.height, label.height)
	print(self.width, self.height)

	-- BUG: doesn't get called!!!
	local old_onRequestLayout = self.onRequestLayout
	self.onRequestLayout = function(_, child)
		print("YES!")
		if old_onRequestLayout then old_onRequestLayout(_, child) end
		label.x = button.right
		self.width = label.right
		self.height = max(button.height, label.height)
	end
	self:onRequestLayOut(self)

	return self
end

-- THINK abou layou requests!