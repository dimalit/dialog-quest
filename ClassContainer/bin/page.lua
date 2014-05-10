Page = function()
	local self = CompositeItem()
	self.id = "page"
	
	self:addLayoutParameter("margin", 10)
	
	self.background = TextureItem("", screen_width, screen_height)
	self:add(self.background)
	self:link(self.background, 0, 0, self, 0, 0, 0, 0)	
	
	self.content = CompositeItem()
	self.content.id = "content"
	self:add(self.content)

	--self:link(self.content, 0, nil, self, 0, nil, self.margin, 0)
	--self:link(self.content, 1, nil, self, 1, nil, -self.margin, 0)
	--self:link(self.content, nil, 0, self, nil, 0, 0, self.margin)
	
	self:restrict(Expr(self.content, "x"), "==", Expr(self, "margin"))
	self:restrict(Expr(self.content, "x")+Expr(self.content, "width"), "==", Expr(self, "width")-Expr(self, "margin"))
	self:restrict(Expr(self.content, "y"), "==", Expr(self, "margin"))
	
	self.agree_button = TextButton{"Понятно, дальше", Buttons.button_anim, shrink=true, one_line=true}
	self:add(self.agree_button)
	self:link(self.agree_button, 0.5, nil, self, 0.5, nil)
	self:restrict(Expr(self.agree_button, "y")+Expr(self.agree_button, "height"), "==", Expr(self, "height")-Expr(self, "margin"))
		
	-- make content tall
	self:restrict(Expr(self.content, "y")+Expr(self.content, "height"), "==", Expr(self.agree_button, "y")-Expr(self, "margin"))
		
	self.agree_button.onClick = function()
		if self.onFinish then self:onFinish() end
	end
	
	self.run = function(_)
	end
	
	return self
end
