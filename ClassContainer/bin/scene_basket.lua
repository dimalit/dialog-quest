local intersects = function(a, b)
	local overlays = function(ax1, ax2, bx1, bx2)
		return min(bx1, bx2) < max(ax1, ax2) and max(bx1, bx2) > min(ax1, ax2)
	end
	return overlays(a.left, a.right, b.left, b.right) and overlays(a.top, a.bottom, b.top, b.bottom)
end

local inside = function(a, b)
	return a.left > b.left and a.right < b.right and a.top > b.top and a.bottom < b.bottom
end

Basket = {
	margin = 20,
	-- TODO: Styles? Percent height + min/max height?
	bottom_ratio = 0.35
}

setmetatable(Basket, {})
getmetatable(Basket).__call = function(_,conf)
	if conf == nil then conf = {} end
	
	local self = CompositeItem()
	self.width = screen_width - Basket.margin*2
	self.height = screen_height - Basket.margin*2
	self.rel_hpx, self.rel_hpy = 0, 0
	self.x, self.y = Basket.margin, Basket.margin

	self.background = TextureItem("", screen_width, screen_height)
	self.background.rel_hpx, self.background.rel_hpy = self.x/self.background.width, self.y/self.background.height
	self:add(self.background)	
	
	self.title = TextItem("self.title")
	self:add(self.title)
	self.title.rel_hpy = 0
	self.title.y = 0
	self.title.x = self.width / 2	
	
	local left_side = WordsPlacement()
		self:add(left_side)
	local right_side = WordsPlacement()
		self:add(right_side)
	local bottom_side = WordsPlacement()
		self:add(bottom_side)
	
	-- bottom_side:setWidthOrigin(self)
	self:link(bottom_side, 0, 1, self, 0, 1)
	self:link(bottom_side, 1, 1, self, 1, 1)
	-- bottom_side.rel_hpy = 1
	-- bottom_side:setLocationOrigin(self, 0.5, 1)
	
	-- left_side.rel_hpx, left_side.rel_hpy = 1, 0
	-- left_side:setLocationOrigin(self.title, 0.5, 1)
	-- left_side.rel_y = Basket.margin
	-- left_side:setWidthOrigin(self, 0.5)
	self:link(left_side, 0, nil, self, 0, nil)
	self:link(left_side, 1, 0, self.title, 0.5, 1, 0, Basket.margin)
	
	-- right_side.rel_hpx, right_side.rel_hpy = 0, 0
	-- right_side:setLocationOrigin(self.title, 0.5, 1)
	-- right_side.rel_y = Basket.margin	
	-- right_side:setWidthOrigin(self, 0.5)
	self:link(right_side, 1, nil, self, 1, nil)
	self:link(right_side, 0, 0, self.title, 0.5, 1, 0, Basket.margin)
	
	self.left_title = TextItem("left_title")
	self:add(self.left_title)
	-- self.left_title.rel_hpy = 0
	-- self.left_title:setLocationOrigin(left_side, 0.5, 0)
	self:link(self.left_title, 0.5, 0, left_side, 0.5, 0)
	
	self.right_title = TextItem("left_title")
	self:add(self.right_title)
	-- self.right_title.rel_hpy = 0
	-- self.right_title:setLocationOrigin(right_side, 0.5, 0)
	self:link(self.right_title, 0.5, 0, right_side, 0.5, 0)	
	
	-- TODO TextureItem sometimes needs to have flexible size!!
	self.vert_bar = TextureItem("interface/frame_glow_bl.rttex", 4, 4)
	self:add(self.vert_bar)
	-- self.vert_bar:setHeightOrigin(left_side)
	-- self.vert_bar:setLocationOrigin(left_side, 1, 0.5)
	self:link(self.vert_bar, 0.5, 0, left_side, 1, 0)
	self:link(self.vert_bar, 0.5, 1, left_side, 1, 1)
	
	self.horz_bar = TextureItem("interface/frame_glow_bt.rttex", 200, 4)
	self:add(self.horz_bar)
	self.horz_bar.name="horz"
	-- self.horz_bar.rel_hpy = 1
	-- self.horz_bar:setWidthOrigin(bottom_side)
	-- self.horz_bar:setLocationOrigin(bottom_side, 0.5, 0)
	self:link(self.horz_bar, 0, 1, bottom_side, 0, 0)
	self:link(self.horz_bar, 1, 1, bottom_side, 1, 0)
	self:link(self.horz_bar, nil, 0, bottom_side, nil, 0, 0, -4)
	
	
	local old_onRequestLayout = self.onRequestLayOut
	self.onRequestLayOut = function(...)
		if old_onRequestLayout then old_onRequestLayout(unpack(arg)) end
	
		local total_h = bottom_side.bottom - left_side.top
		local bot = total_h * Basket.bottom_ratio
		-- HACK: if not check this - hangs :(
		if math.abs(bottom_side.height - bot) >= 0.5 then
			bottom_side.height = bot
			left_side.height = total_h - bot
			right_side.height = total_h - bot
		end
		
		if self.afterLayOut and #left_side.items==0 and #right_side.items==0 then
			self:afterLayOut()
		end
	end
	
	local words = {
		right = {},
		left = {},
		dummy = {}
	}
	local all_words = {}
	
	local check_finish = function()
		if #left_side.items + #right_side.items == #all_words and
			 self.onFinish
		then
			 self:onFinish()
		end
	end
	
	local add_words = function(words_, side)
		for i,w in ipairs(words_) do
			local item = Mover(TextItem(w))
			self:add(item)
			table.insert(words[side], item)
			table.insert(all_words, item)
			item.onDragEnd = function()
				if inside(item, left_side) then
					right_side:removeIfExists(item)
					left_side:take(item)
				elseif inside(item, right_side) then
					left_side:removeIfExists(item)
					right_side:take(item)
				else
					item:goHome()
				end
				check_finish()
			end
		end
	end
	
	self.addRightWords = function(_, words)
		add_words(words, "right")
	end
	self.addLeftWords = function(_, words)
		add_words(words, "left")
	end
	self.addDummyWords = function(_, words)
		add_words(words, "dummy")
	end	
	
	self.placeWordsRandomly = function(_)
		local left = bottom_side.left + 50
		local right = bottom_side.right - 50
		local top = bottom_side.top + 20
		local bottom = bottom_side.bottom - 20

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
	end
	
	return self
end

class 'WordsPlacement'(ScreenItem)

-- TODO Here we need link to left/right_title!
-- maybe put title inside "side"?
WordsPlacement.top_margin = 0
WordsPlacement.spacing = 0

WordsPlacement.__init = function(self)
	ScreenItem.__init(self)
	
	local items = {}
	
	self.take = function(_, item)
		local y = self.top + WordsPlacement.top_margin
		if #items > 0 then y = items[#items].bottom + WordsPlacement.spacing end
		item.ox = self.left + self.width / 2 
		item.oy = y + item.hpy
		item:goHome()
		table.insert(items, item)
	end
	
	self.removeIfExists = function(_, item)
		for i=1,#items do
			if items[i] == item then
				table.remove(items, i)
				self:onRequestLayOut()
			end
		end
	end
	
	self.onRequestLayOut = function()
		local y = self.top + WordsPlacement.top_margin
		for i, item in ipairs(items) do
			item.x = self.left + self.width / 2
			item.y = y + item.hpy
			y = y + item.height + WordsPlacement.spacing
		end
	end
	
	self.items = items
	
	return self
end
