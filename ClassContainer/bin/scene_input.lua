local intersects = function(a, b)
	local overlays = function(ax1, ax2, bx1, bx2)
		return min(bx1, bx2) < max(ax1, ax2) and max(bx1, bx2) > min(ax1, ax2)
	end
	return overlays(a.left, a.right, b.left, b.right) and overlays(a.top, a.bottom, b.top, b.bottom)
end

local inside = function(a, b)
	return a.left > b.left and a.right < b.right and a.top > b.top and a.bottom < b.bottom
end

local take = function(drops, w)
  w.onDrag = function(obj, dx, dy)
    onDrag(drops, w)
  end
  w.onDragEnd = function()
		onDrop(drops, w)
  end
  return w
end

Input = {
	margin = 20,
	num_columns = 2,
	row_interval = 20,
	drop_frame = "interface/frame",
	drop_frame_active = "interface/frame_glow",
	mover_bk_image = "interface/rect_bk.rttex",
	bottom_ratio = 0.35	
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
	local all_drops = {}				-- needed for movers!
	local all_words = {}
	local dropped_movers_cnt = 0
	
	for i=1,Input.num_columns do
		local col = CompositeItem()
		self:add(col)
		
		-- align top
		self:link(col, nil, 0, self.description, nil, 1, 0, Input.margin)
		-- align edges
		self:link(col, 0, nil, self, 1/Input.num_columns*(i-1), nil)
		self:link(col, 1, nil, self, 1/Input.num_columns*i, nil)
		-- bottom
		self:link(col, nil, 1, self, nil, 1, 0, -Input.margin)
		
		col.elements = {}
		
		local old_add = col.add
		col.add = function(_, arr)
			-- make good child
			local child = InputElement(arr[1])
			table.insert(col.elements, child)
			-- place it
			old_add(col, child)
			child.rel_hpx, child.rel_hpy = 0, 0

			if #col.elements==1 then
				col:link(child, 0.5, 0, col, 0.5, 0, 10, Input.row_interval)
			else
				col:link(child, 0.5, 0, col.elements[#col.elements-1], 0.5, 1, 0, Input.row_interval)
			end

			-- movers and drops
			table.insert(all_drops, child.drop)
			self:addWords{arr[2]}
			local mover = all_words[#all_words]
			child.drop.right_obj = mover
			
			return col
		end -- add
		
		local old_onRequestLayOut = col.onRequestLayOut
		col.onRequestLayOut = function(...)			
			if old_onRequestLayOut then old_onRequestLayOut(unpack(arg)) end

			-- adjust labels
			local max = 0
			for i, row in ipairs(col.elements) do
				if row.text.width > max then max = row.text.width end
			end -- for
			for i, row in ipairs(col.elements) do
				row.text_padder.width = max
			end -- for			

		end -- LayOut col
		
		table.insert(self.columns, col)
	end -- for columns
	
	local bottom_side = WordsPlacement()
		self:add(bottom_side)
	self:link(bottom_side, 0, 1, self, 0, 1)
	self:link(bottom_side, 1, 1, self, 1, 1)	
	
	local check_finish = function()
		dropped_movers_cnt = 0
		local right_cnt = 0
		for i, drop in ipairs(all_drops) do
			if drop.object then dropped_movers_cnt = dropped_movers_cnt + 1 end
			if drop.object == drop.right_obj then right_cnt = right_cnt + 1 end
		end
		local finished = dropped_movers_cnt == #all_drops		
		if finished and self.onFinish ~= nil then
			self:onFinish()
		end
	end
	
	local old_onRequestLayout = self.onRequestLayOut
	self.onRequestLayOut = function(...)
		if old_onRequestLayout then old_onRequestLayout(unpack(arg)) end
	
		--adjust bottom_side
		local total_h = bottom_side.bottom - self.description.bottom
		local bot = total_h * Input.bottom_ratio
		if math.abs(bottom_side.height-bot) >= 0.5 then
			bottom_side.height = bot
		end
		
		-- user callback
		-- HACK: call only if we didn't begin moving
		if dropped_movers_cnt==0 and self.afterLayOut then
			self:afterLayOut()
		end
	end
	
	-- movers
	self.addWords = function(_, words)
		for i,w in ipairs(words) do
			local item = take(all_drops, Mover(TextButton{w, Input.mover_bk_image, shrink=true, freeScale=true, padding=5}))
			self:add(item)
			local old_handler = item.onDragEnd
			item.onDragEnd = function(...)
				if old_handler then old_handler(unpack(arg)) end
				check_finish()
			end
			table.insert(all_words, item)
		end
	end	 -- add_words
	
	self.placeWordsRandomly = function(_)
		local left = bottom_side.left + 50
		local right = bottom_side.right - 50
		local top = bottom_side.top + 20
		local bottom = bottom_side.bottom - 20
		
		--print(left, right, top, bottom)

		local placed_movers = {}		

		local conflicts_with_placed = function(mover)
			local res = false
			for _, m in ipairs(placed_movers) do
				-- TODO Measure DISTANCE - not just overlapping
				if intersects(mover, m) then return false end
			end
			return true
		end  
		
		for _,mover in pairs(all_words)
		do
			repeat
				mover.y = top + rand()*(bottom-top)
				mover.x = left + rand()*(right-left)
			until conflicts_with_placed(mover, placed_movers)
			table.insert(placed_movers, mover)
		end -- for mover
	end -- placeWordsRandomly	
	
	return self
end

InputElement = function(phonetic_text, answer)
	local self = CompositeItem()
	self.line="line"
	local input_w = 50
	local drop_w = 80

	-- invent poperties  also
	
	self.text = TextItem("["..phonetic_text.."]", 3)
	self.input = TextInputItem(input_w)
	self.drop = DropArea(TwoStateAnimation(
																					FrameItem(Input.drop_frame, drop_w, 40),
																					FrameItem(Input.drop_frame_active, drop_w, 40)
																					)
												)
	self:add(self.text):add(self.input):add(self.drop)

	self.text_padder = SimpleItem()
	self.text_padder.width = self.text.width
	self:add(self.text_padder)
	self:link(self.text_padder, 0, 0.5, self.text, 0, 0.5)
	
	self:link(self.text, 0, 0.5, self, 0, 0.5)
	self:link(self.input, 0, 0.5, self.text_padder, 1, 0.5, 5, 0)
	self:link(self.drop, 0, 0.5, self.input, 1, 0.5, 5, 0)

	self:link(self, 1, 1, self.drop, 1, 1)
	
	return self
end