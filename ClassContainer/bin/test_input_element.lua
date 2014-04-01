ie = InputElement("Text")
root:add(ie)
root:link(ie, 0, 0, root, 0, 0, 20, 20)

dummy = TextItem("Hello")
root:add(dummy)
dummy.x, dummy.y = 200, 200

dummy.onDragEnd = function()
	dummy.x = dummy.x + 1
end

root.debugDrawBox = true