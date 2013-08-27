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
	self.title.hpy_relative = 0
	self.title.y = 0
	self.title.x = self.width / 2
	
	self.paragraphs = {}
	self.paragraphs.add = function(_, p)
		if p.width==0 then p.width = self.width end	-- if not set
		p.hpy_relative = 0
		p.hpx_relative = 0.5
		local y = self.title.bottom + Explain.margin
		if #self.paragraphs > 0 then
			y = self.paragraphs[1].y + self.paragraphs[1].height + Explain.paragraph_interval
		end
		p.y = y
		p.x = self.width / 2
		table.insert(self.paragraphs, p)
		self:add(p)
	end
	
	self.agree_button = Button(TwoStateAnimation(Animation(load_config("Start.anim"))))
	self.agree_button.hpy_relative = 1
	self.agree_button.x = self.width / 2
	self.agree_button.y = self.height
	self:add(self.agree_button)
	self.agree_button.onClick = function()
		if self.onFinish then self:onFinish() end
	end
	
	self.onRequestLayOut = function(_, child)
		local y = self.title.bottom + Explain.margin
		for _, p in ipairs(self.paragraphs) do
			p.y = y
			y = y + p.height + Explain.paragraph_interval
		end
	end
	
	self.start = function(_)
	end
	
	return self
end
