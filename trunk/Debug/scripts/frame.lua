item = ScreenItem(10, 10)
-- add frame and sting
def_w = 100
def_h = 100
frame = Frame(def_w, def_h)

item.view = frame
item.hpx = 0
item.hpy = 0
item.text = TextItem(item.x, item.y, ">", "courier20b/checked_over.fnt")
item.text.hpx = 0
item.text.hpy = 0

item:takeCharFocus()
item.onChar = function(dummy, code)
  -- backspace
  if code == 8 then
    item.text.text = string.sub(item.text.text, 1, -2)
  else
    ch = string.char(code)
    if code == 13 then ch = "\n" end
    item.text.text = item.text.text..ch
  end
  
  if item.text.width > frame.width then frame.width = item.text.width end
  if item.text.height > frame.height then frame.height = item.text.height end
end

