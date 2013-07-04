local take = function(drops, w)
  w.onDrag = function(obj, dx, dy)
    onDrag(drops, w)
  end
  w.onDragEnd = function()
	onDrop(drops, w)
  end
  return w
end

Mosaic = function(conf)
  if conf == nil then conf = {} end
  local self = Scene(conf)
  
  conf.line_length = conf.line_length or 4
  conf.line_interval = conf.line_interval or 1.5
  conf.drops_interval =  conf.drops_interval or 100
  conf.image_y = conf.image_y or 100
  conf.button_y = conf.button_y or screen_height * 3 / 4
  
  self.step = 1
  
  self.drops = {}
  self.movers = {}
  
  self.clear = function(self)
  
    -- clear person
    if self.person then
      self.person:destroy()
    end
    self.preson = nil
  
    -- clear image
    if self.image then
      self.image:destroy()
    end
    self.image = nil
  
    -- clear center
    if self.center_img then
      self.center_img:destroy()
    end
    self.center_img = nil
  
    self.title = ""
  
    -- clear drops
    for i=1,#self.drops
    do
      self.drops[i]:destroy()
    end
    self.drops = {}
    
    -- clear movers
    for i=1,#self.movers
    do
      self.movers[i]:destroy()
    end
    self.movers = {}
    
    -- remove button
    if self.button then self.button:destroy(); self.button = nil; end
 
  end

  local next_step = function(next)
    self.step = next
    -- 0 means end
    if self.step ~= nil and self.step~=0 and self.step <= #conf.table
    then
      self:ask(unpack(conf.table[self.step]))
    elseif self.onFinish
    then
      self:onFinish()
    end
      
  end

  self.ask = function(self, person, corner_img, title, center_img, drops_conf, words, sound, right_goto, wrong_goto, time)
    self:clear()
    
    -- add image/title
    if person then
      self.person = AnimatedItem(0, 0, person)
    end
    if corner_img then
      self.image = ImageItem(0, 0, corner_img)
    end
    if title then  
      self.title = title
    end
    
    -- play sound
--    local snd
--    if sound then
--      snd = SoundEffect(sound)
--      snd:play()
--    end
    
    -- center image
    if center_img then
        self.center_img = ImageItem(screen_width / 2, 0, center_img)
        self.center_img.hpy = 0
        self.center_img.y = conf.image_y
    end
    
    -- add places
    self.drops = {}
    if drops_conf == nil then drops_conf = {} end
    -- make it 2-d
    if type(drops_conf[1]) ~= "table" then drops_conf = {drops_conf} end
--    print_table(drops_conf)
    local y = 0--conf.margin
	local x = 0--conf.margin
    if self.center_img then y = y + self.center_img.bottom end
    for row = 1, #drops_conf
    do
      local x = screen_width / 2 - conf.drops_interval * (#drops_conf[row] - 1) / 2
      for col = 1, #drops_conf[row]
      do
--        print(row, col)
        local d = DropArea(x, y, TwoStateAnimation(Animation(load_config("DropArea.anim"))))
          d.y = d.y + d.height / 2
        table.insert(self.drops, d)
        d.right_text_id = drops_conf[row][col]        -- ->words
        x = x + conf.drops_interval
      end
      -- handle empty case
      if #self.drops > 0 then y = y + self.drops[#self.drops].height * conf.line_interval end
--      print(y)
--      print(d.x, d.y, d.width, d.height)
    end
    
    -- add movers
    if words == nil then words = {} end    
    if #words > 0 then
	  -- TODO make Animation ctrpr with string param
      local tmp = DropArea(0, 0, TwoStateAnimation(Animation(load_config("DropArea.anim"))))
      tmp.visible = false
      y = screen_height - conf.margin - (math.ceil(#words / conf.line_length) - 1)*tmp.height*conf.line_interval - tmp.height/2    -- make margin to bottom
    end
    local x
    for i = 1, #words
    do
        -- handle line end
        if i % conf.line_length == 1 then
          if (#words-i+1) > conf.line_length then x = screen_width / 2 - conf.drops_interval * (conf.line_length-1) / 2
          else x = screen_width / 2 - conf.drops_interval * (#words - i) / 2 end
          
          if i ~= 1 then y = y + self.drops[#self.drops].height * conf.line_interval end
        end
        
        local d = DropArea(x, y, TwoStateAnimation(Animation(load_config("DropArea.anim"))))
        table.insert(self.drops, d)
        --!!! make "bottom" writable? (move)
        local t = take(self.drops, Mover(0, 0, Text(words[i])))
        d:take(t)
        table.insert(self.movers, t)
        x = x + conf.drops_interval
    end  
    
    local function check_answer()
      local wrong_cnt = 0
      for i = 1, #self.drops
      do
        if self.drops[i].right_text_id then
          if self.drops[i].object ~= self.movers[self.drops[i].right_text_id] then wrong_cnt = wrong_cnt + 1 end
        end
      end
      if wrong_cnt > 0 then
        next_step(wrong_goto)
      elseif right_goto then
        next_step(right_goto)
      else
        next_step(self.step + 1)
      end
    end
    
    -- add button
    self.button = Button(screen_width / 2, conf.button_y, TwoStateAnimation(Animation(load_config("Start.anim"))))
    self.button.onClick = function(btn)
      if not time then
        check_answer()
      end
    end
    
    if time then
      self.timer = Timer(function(timer)
        -- wrong_goto may hold next step
        if wrong_goto~=nil then
          next_step(wrong_goto)
        else
          next_step(self.step + 1)
        end
      end, time)
    end
  end -- ask 
  
  self.start = function(self)
    next_step(1)    
  end
  
  self.destroy = function(self)
    self:clear()
  end
  
  return self
end