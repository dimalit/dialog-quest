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
	self.id="scene"
	self.width = screen_width - Input.margin*2
	self.height = screen_height - Input.margin*2
	self:restrict(Expr(self, "x"), "==", Expr(Input.margin))
	self:restrict(Expr(self, "y"), "==", Expr(Input.margin))
	self:restrict(Expr(self, "width"), "==", Expr(self.width))
	self:restrict(Expr(self, "height"), "==", Expr(self.height))				-- TODO: add special function a-la "keep value"?
	
	self.background = TextureItem("", screen_width, screen_height)
	--self.background.rel_hpx, self.background.rel_hpy = self.x/self.background.width, self.y/self.background.height
	self:add(self.background)
	self:link(self.background, 0, 0, self, 0, 0, -Input.margin, -Input.margin)
	
	self.title = TextItem("self.title")
	self.title.id = "title"
	self:add(self.title)
	self:link(self.title, 0.5, 0, self, 0.5, 0)

	self.description = FlowLayout()
	self.description.id = "desc"
	self:add(self.description)
	self:link(self.description, 0, nil, self, 0, nil)
	self:link(self.description, 1, nil, self, 1, nil)
	self:link(self.description, nil, 0, self.title, nil, 1)		

	local bottom_side = WordsPlacement()
	bottom_side.id = "bottom_side"
		self:add(bottom_side)
	self:link(bottom_side, 0, 1, self, 0, 1)
	self:link(bottom_side, 1, 1, self, 1, 1)	
	-- link height
	self:restrict(Expr(bottom_side, "height"), "==", (Expr(self, "y")+Expr(self, "height")-Expr(self.description, "y")-Expr(self.description, "height"))*Expr(Input.bottom_ratio))
	self:restrict(Expr(bottom_side, "height"), ">=", Expr(80))
	
	self.columns = {}
	local all_drops = {}				-- needed for movers!
	local all_words = {}
	local dropped_movers_cnt = 0
	
	for i=1,Input.num_columns do
		local col = CompositeItem()
		self:add(col)
		col.id = "col"..(#self.columns+1)
		
		-- align top
		self:link(col, nil, 0, self.description, nil, 1, 0, Input.margin)
		-- align edges
		self:link(col, 0, nil, self, 1/Input.num_columns*(i-1), nil)
		self:link(col, 1, nil, self, 1/Input.num_columns*i, nil)
		-- link bottom to bottom side
		--self:link(col, nil, 1, self, nil, 1, 0, -Input.margin)
		self:link(col, nil, 1, bottom_side, nil, 0)
		
		
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
				col:link(child, 0, 0, col, 0, 0, 10, Input.row_interval)
			else
				col:link(child, 0, 0, col.elements[#col.elements-1], 0, 1, 0, Input.row_interval)
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

		--adjust bottom_side: not needed because of Cassowary
		-- local total_h = bottom_side.bottom - self.description.bottom
		-- local bot = total_h * Input.bottom_ratio
		-- if bot < 80 then bot = 80 end
		-- if math.abs(bottom_side.height-bot) >= 0.5 then
			-- bottom_side.height = bot
		-- end
		
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
		if #all_words==0 then return end
	
		local left = bottom_side.left
		local right = bottom_side.right
		local top = bottom_side.top
		local bottom = bottom_side.bottom

--		local placed_movers = {}

		-- local conflicts_with_placed = function(mover)
			-- local res = false
			-- for _, m in ipairs(placed_movers) do
				-- -- TODO Measure DISTANCE - not just overlapping
				-- if intersects(mover, m) then return false end
			-- end
			-- return true
		-- end
		
		local x = left + 20
		local y = top + 20
		
		-- put movers into temp_array and shuffle them
		local temp_array = {}
		local random_array = random_permutation(#all_words)
		for i=1,#random_array do
			local j = random_array[i]
			table.insert(temp_array, all_words[j])
		end		
		
		local lines = {}					-- for later centering
		table.insert(lines, {})
		
		-- put on screen
		for _,mover in ipairs(temp_array)
		do
			if right-left-mover.width > 0 then
				-- repeat
					-- mover.y = top + rand()*(bottom-top-mover.height)
					-- mover.x = left + rand()*(right-left-mover.width)
					-- print("trying", mover.x, mover.y, mover.width, mover.height)
				-- until conflicts_with_placed(mover, placed_movers)
				mover.x, mover.y = x, y
				table.insert(lines[#lines], mover)
				x = x + mover.width + 20
				if x > right then														-- wrap
					x, y = 20, y + 40
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
--				lines[i][j].y = lines[i][j].y + vert_delta
			end
		end
		
	end -- placeWordsRandomly	
	
	return self
end

InputElement = function(phonetic_text, answer)
	local self = CompositeItem()
	self.id="InputElement"
	local input_w = 50
	local drop_w = 80

	-- invent poperties  also
	
	self.text = TextItem("["..phonetic_text.."]", 3)
	self.text.id="text"
	self.input = TextInputItem(input_w)
	self.input.id="input"
	self.drop = DropArea(TwoStateAnimation(
																					FrameItem(Input.drop_frame, drop_w, 40),
																					FrameItem(Input.drop_frame_active, drop_w, 40)
																					)
												)
	self.drop.id="drop"
	self:add(self.text):add(self.input):add(self.drop)

	-- padder to slign texts from different rows
	self.text_padder = ScreenItem()
	self.text_padder.id = "padder"
	self:add(self.text_padder)
	self:link(self.text_padder, 0, 0.5, self.text, 0, 0.5)
	self:restrict(Expr(self.text_padder, "width"), ">=", Expr(self.text, "width"))
	
	-- stay widths
	self:restrict(Expr(self.input, "width"), "==", Expr(input_w))
	self:restrict(Expr(self.drop, "width"), "==", Expr(drop_w))
	self:restrict(Expr(self.drop, "height"), "==", Expr(40))
	
	self:link(self.text, 0, 0.5, self, 0, 0.5)
	self:link(self.input, 0, 0.5, self.text_padder, 1, 0.5, 5, 0)
	self:link(self.drop, 0, 0.5, self.input, 1, 0.5, 5, 0)
	
	self:link(self, 1, 1, self.drop, 1, 1)
	
	return self
end