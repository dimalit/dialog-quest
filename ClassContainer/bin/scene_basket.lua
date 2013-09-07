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
	self.x, self.y = screen_width/2, screen_height/2
	root:add(self)
	
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
	
	bottom_side:setWidthOrigin(self)
	bottom_side.rel_hpy = 1
	bottom_side:setLocationOrigin(self, 0.5, 1)
	
	left_side.rel_hpx, left_side.rel_hpy = 1, 0
	left_side:setLocationOrigin(self.title, 0.5, 1)
	left_side.rel_y = Basket.margin
	left_side:setWidthOrigin(self, 0.5)

	right_side.rel_hpx, right_side.rel_hpy = 0, 0
	right_side:setLocationOrigin(self.title, 0.5, 1)
	right_side.rel_y = Basket.margin	
	right_side:setWidthOrigin(self, 0.5)
	
	self.onRequestLayOut = function(_)
		local total_h = bottom_side.bottom - left_side.top
		local bot = total_h * Basket.bottom_ratio
		bottom_side.height = bot
		left_side.height = total_h - bot
		right_side.height = total_h - bot
	end
	
	local words = {
		right = {},
		left = {},
		dummy = {}
	}
	local all_words = {}
	
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

WordsPlacement = {
	top_margin = 20,
	spacing = 10
}
setmetatable(WordsPlacement, {})
getmetatable(WordsPlacement).__call = function(_)
	local self = ScreenItem()
	
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
	
	return self
end

-- THINK: When comtainer changes size, what will be first: onRequestLayOut or children get their signals?

-- THINK: What layout relations between elements we need? (see title/side etc..)

-- TODO: Link not element positions but their borders and corners!?