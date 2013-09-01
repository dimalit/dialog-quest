Explain = {
	margin = 20,
	paragraph_interval = 10	
}
setmetatable(Explain, {})
getmetatable(Explain).__call = function(_,conf)
  if conf == nil then conf = {} end
	local self = MakeLayoutAgent(CompositeItem())
	self.width = screen_width - Explain.margin*2
	self.height = screen_height - Explain.margin*2
	self.x, self.y = screen_width/2, screen_height/2
	root:add(self)
	
  self.title = MakeLayoutAgent(TextItem("self.title"))
	self:add(self.title)
	self.title.hpy_relative = 0
	self.title.y = 0
	self.title.x = self.width / 2
	
	self.paragraphs = MakeLayoutAgent(VBox(self.width))
	self.paragraphs.spacing = Explain.paragraph_interval
	self.paragraphs.hpy_relative = 0
	self.paragraphs.y = Explain.paragraph_interval
	self:add(self.paragraphs)
	self.paragraphs:setLocationOrigin(self.title, 0.5, 1)
	
	self.agree_button = Button(TwoStateAnimation(Animation(load_config("Start.anim"))))
	self.agree_button.hpy_relative = 1
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
