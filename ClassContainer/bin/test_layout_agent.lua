f1 = ScreenItem()--FrameItem("interface/frame_glow", 100, 30)
f1.width, f1.height = 100, 30
f1.x, f1.y = 0, 0
f1.hpx_relative, f1.hpy_relative = 0, 0
root:add(f1)

f2 = FrameItem("interface/frame", 200, 60)
f2.hpx_relative, f2.hpy_relative = 0, 0
f2.x, f2.y = 0, 0

f1 = MakeLayoutAgent(f1)
f2 = MakeLayoutAgent(f2)
root:add(f2)

f2:setLocationOrigin(f1, 0.5, 0.5)
f1:move(10, 10)
f1.width = 150

f2:setWidthOrigin(f1)

f1.onDrag = function(self, dx, dy)
	self:move(dx, dy)
	self.width = self.width + dx / 2
end