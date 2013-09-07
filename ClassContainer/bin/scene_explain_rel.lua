Explain = {
	margin = 20,
	paragraph_interval = 10	
}
setmetatable(Explain, {})
getmetatable(Explain).__call = function(_,conf)
  if conf == nil then conf = {} end
	local self = CompositeItem()
	self.width = screen_width - Explain.margin*2
	self.height = screen_height - Explain.margin*2
	self.x, self.y = screen_width/2, screen_height/2
	root:add(self)
	
  self.title = TextItem("self.title")
	self:add(self.title)
	self.title.rel_hpy = 0
	self.title.y = 0
	self.title.x = self.width / 2
	
	self.content = CompositeItem()
	self.content.rel_hpy = 0
	self.content.rel_y = Explain.paragraph_interval
	self:add(self.content)
	self.content:setLocationOrigin(self.title, 0.5, 1)
	local old_content_add = self.content.add
	self.content.onRequestLayOut = function(_)	-- resize to fit all
		local w, h = 0, 0
		for ch in pairs(self.content.children) do
			if ch.right > w then w = ch.right end
			if ch.bottom > h then h = ch.bottom end
		end
		-- TODO Will call twice because of recursion. Optimize!
		self.content.width, self.content.height = w, h
	end
	
	self.agree_button = Button(TwoStateAnimation(Animation(load_config("Start.anim"))))
	self.agree_button.rel_hpy = 1
	self.agree_button.x = self.width / 2
	self.agree_button.y = self.height
	self:add(self.agree_button)
	self.agree_button.onClick = function()
		if self.onFinish then self:onFinish() end
	end
	
	self.start = function(_)
	end
	
	return self
end
