function make_flask(x, y)
  local flask
  flask = ImageItem(x, y, "flask/normal.png")
  flask.onChar = function(self, code)
    print(string.char(code))
    if string.char(code)=='g' then self:giveCharFocus() end
  end
  flask.onDragEnd = function(self)
    self:takeCharFocus()
    self:move(0, -10)
  end
  flask.onFocusLose = function(self) self:move(0, 10) end
  return flask
end

f1 = make_flask(screen_width/2, 100)
f2 = make_flask(screen_width/2-50, 200)
f3 = make_flask(screen_width/2+50, 200)

print_table(f1.quad)