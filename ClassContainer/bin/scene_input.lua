Input = {
	margin = 20,
	num_columns = 2,
	row_interval = 20,
	drop_frame = "interface/frame",
	drop_frame_active = "interface/frame_glow"
}

setmetatable(Input, {})
getmetatable(Input).__call = function(_,conf)
	if conf == nil then conf = {} end
	
	local self = CompositeItem()
	self.width = screen_width - Input.margin*2
	self.height = screen_height - Input.margin*2
	self.rel_hpx, self.rel_hpy = 0, 0
	self.x, self.y = Input.margin, Input.margin

	self.background = TextureItem("", screen_width, screen_height)
	self.background.rel_hpx, self.background.rel_hpy = self.x/self.background.width, self.y/self.background.height
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
	
	for i=1,Input.num_columns do
		local col = CompositeItem()
		self:add(col)
		
		-- align top
		self:link(col, nil, 0, self.description, nil, 1, 0, Input.margin)
		-- align edges
		self:link(col, 0, nil, self, 1/Input.num_columns*(i-1), nil)
		self:link(col, 1, nil, self, 1/Input.num_columns*i, nil)
		
		col.elements = {}
		
		local old_add = col.add
		col.add = function(_, arr)
			-- make good child
			local child = InputElement(arr[1], arr[2])
			table.insert(col.elements, child)			
			-- place it
			old_add(col, child)
			child.rel_hpx, child.rel_hpy = 0, 0
			local top = 0
			if #col.elements>1 then	top = col.elements[#col.elements-1].bottom end
			child.y = top + Input.row_interval
			self:requestLayOut(col)
			return col
		end -- add
		
		col.height = 500			-- just to see it
		table.insert(self.columns, col)
	end
	
	local old_onRequestLayout = self.onRequestLayOut
	self.onRequestLayOut = function(...)
		if old_onRequestLayout then old_onRequestLayout(unpack(arg)) end
	
		-- adjust columns
		for i,col in pairs(self.columns) do
			-- find max
			local max = 0
			for _,it in ipairs(col.elements) do
				if it.width > max then max = it.width end
			end
			-- set pos
			for _,it in ipairs(col.elements) do
				it.x = col.width / 2 - max / 2
			end			
		end -- for cols
	end
	
	return self
end

InputElement = function(phonetic_text, answer)
	local self = CompositeItem()
	local input_w = 50
	local drop_w = 100

	local text = TextItem("["..phonetic_text.."]")
	local input = TextInputItem(input_w)
	local drop = DropArea(TwoStateAnimation(
																					FrameItem(Input.drop_frame, drop_w, input.height),
																					FrameItem(Input.drop_frame_active, drop_w, input.height)
																					)
												)
	self:add(text):add(input):add(drop)
	
	self:link(text, 0, 0.5, self, 0, 0.5)
	self:link(input, 0, 0.5, text, 1, 0.5, 5, 0)
	self:link(drop, 0, 0.5, input, 1, 0.5, 5, 0)

	
	-- TODO: Think about solving linear equations iteratively to move objects!
	print("link")
	--self:link(self, 1, nil, drop, 1, nil)
	print("after link")
	self.width = 220
	self.height = 40
	
	return self
end