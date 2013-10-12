local old_ScreenItem = ScreenItem.__init
ScreenItem.__init = function(...)
	old_ScreenItem(unpack(arg))
	MakeLayoutAgent(arg[1])
end

local old_SimpleItem = SimpleItem.__init
SimpleItem.__init = function(...)
	old_SimpleItem(unpack(arg))
	MakeLayoutAgent(arg[1])
end

local old_CompositeItem = CompositeItem.__init
CompositeItem.__init = function(...)
	old_CompositeItem(unpack(arg))
	MakeLayoutAgent(arg[1])
end