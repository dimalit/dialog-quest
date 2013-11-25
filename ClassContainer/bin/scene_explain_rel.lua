Explain = {
	margin = 20,
	paragraph_interval = 10	
}
setmetatable(Explain, {})
getmetatable(Explain).__call = function(_,conf)
  if conf == nil then conf = {} end
	local self = CompositeItem()
	self.id = "scene"
	self.width = screen_width - Explain.margin*2
	self.height = screen_height - Explain.margin*2
	-- TODO: bad to restrict own x, y
	self:restrict(Expr(self, "x"), "==", Expr(Explain.margin))
	self:restrict(Expr(self, "y"), "==", Expr(Explain.margin))
	self:restrict(Expr(self, "width"), "==", Expr(self.width))
	self:restrict(Expr(self, "height"), "==", Expr(self.height))				-- TODO: add special function a-la "keep value"?

	self.background = TextureItem("", screen_width, screen_height)
	self.background.rel_hpx, self.background.rel_hpy = self.x/self.background.width, self.y/self.background.height
	self:add(self.background)
	
  self.title = TextItem("self.title")
	self:add(self.title)
	self:link(self.title, 0.5, 0, self, 0.5, 0)
	
	self.content = CompositeItem()
	-- HACK: 600 prevents FlowLayout inside to overflow
	-- should remove it nicely!
	self.content.width = 600
	self.content.id = "content"
--	self.content.rel_hpy = 0
--	self.content:rel_y(Explain.paragraph_interval)
	self:add(self.content)
--	self.content:setLocationOrigin(self.title, 0.5, 1)
	self:link(self.content, 0, nil, self, 0, nil)
	self:link(self.content, 1, nil, self, 1, nil)
	self:link(self.content, nil, 0, self.title, nil, 1, 0, Explain.paragraph_interval)
	
	-- old_request = self.content.onRequestLayOut
	-- self.content.onRequestLayOut = function(...)	-- resize to fit all
		-- if old_request then old_request(unpack(arg)) end
		-- local w, h = 0, 0
		-- for ch in pairs(self.content.children) do
			-- if ch.right > w then w = ch.right end
			-- if ch.bottom > h then h = ch.bottom end
		-- end
		---- TODO Will call twice because of recursion. Optimize!
		-- self.content.width, self.content.height = w, h
	-- end
	
	self.agree_button = TextButton{"", "Start.anim"}
--	Button(TwoStateAnimation(Animation(load_config("Start.anim"))))
	self:add(self.agree_button)
	self:link(self.agree_button, 0.5, 1, self, 0.5, 1)
	self.agree_button.onClick = function()
		if self.onFinish then self:onFinish() end
	end
	
	self.start = function(_)
	end
	
	return self
end
