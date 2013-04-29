Shooter = function(conf)
  local launch_plate
  local next_step
  local on_right_input
  local on_wrong_input
  local char_callback

  if conf == nil then conf = {} end
  
  conf.plate       = conf.plate or "bullet.png"
  conf.break_ani   = conf.break_ani or "explosion"
  conf.shelf_plate = conf.shelf_plate or "bullet.png"
  
  if conf.subtitle == nil then conf.subtitle = true end
  
  if conf.start_x == nil then conf.start_x = 0 end
  if conf.start_y == nil then conf.start_y = 500 end
  if conf.vx      == nil then conf.vx = 400 end
  if conf.vy      == nil then conf.vy = -400 end
  if conf.gravity == nil then conf.gravity = 300 end

  local self = Scene(conf)
  self.table = conf.table

  -- class for points
  local Scoreboard = function()
    local scene = self      -- save it
    local self = {}         -- self = board
    self.max = #scene.table
    self.__value = self.max
    
    -- make column
    self.plates = {}
    local plate_file = conf.shelf_plate
    local temp = ImageItem(0, 0, plate_file)
    local x = temp.width / 2 + scene.margin
    temp:destroy()
    local height = self.max * (temp.height + 2)
    local y = (screen_height - height) / 2
    for i = 1, self.max
    do
      table.insert(self.plates, ImageItem(x, y, plate_file))
      y  = y + height / self.max
    end

    self.destroy = function(self)
      for k, v in pairs(self.plates)
      do
        if self.plates[k] then
          self.plates[k]:destroy()
        end
      end
    end

    setmetatable(self, {
      __index = function(self, key)
        if key == "value" then return self.__value end
      end,
      __newindex = function(self, key, val)
        if(key == "value")
        then
          self.__value = val
          for i = 1, self.max
          do
            self.plates[i].visible = (i > (self.max - self.__value))
          end
        end
      end
    })

    return self
  end -- Scoreboard

  -- private
  local target_letters = ""
  local target_pos = 1
  local block_input = true
  local scoreboard = Scoreboard()
  local snd   -- dummy object for sound

  local launch_plate = function(dummy)
    local step_conf = self.table[self.step]

    self.title = step_conf.show
    self.image = ImageItem(0, 0, step_conf.image)

    target_letters = step_conf.check
    target_pos = 1
    self.subtitle = ""

    snd = SoundEffect(step_conf.snd)
    snd:play()
    snd.onFinish = function()

      -- Вылетает тарелочка
      plate = PlateTarget{x=conf.start_x, y=conf.start_y, vx=conf.vx, vy=conf.vy, g=conf.gravity, image=conf.plate, break_ani=conf.break_ani}
      plate.onOut = function(obj)
        next_step("wrong")
      end

      -- разрешить стрелять
      plate:start()
      block_input = false
    end
  end -- launch_plate

  -- переходит к следующему шагу
  -- шаг зависит от значения ans
  next_step = function(ans)
    if ans=="right" then
      self.step = self.table[self.step].right_goto
    else
      self.step = self.table[self.step].wrong_goto
    end

    scoreboard.value = #self.table - self.step + 1

    if self.step > 0 and self.step <= #self.table then
      launch_plate(self)
    elseif self.onFinish then
      self:onFinish()
    end  
  end

  -- 3) Пока летит тарелочка, при нажатии на  заранее определенную букву
  --    (или определенную последовательность букв) тарелочка разлетается
  --    на куски (gif анимация файл);
  -- При этом нужно дать возможность только одного "выстрела";
  on_right_input = function(self)
    block_input = true
    plate:hit()

    -- очки
    self.shot_cnt = self.shot_cnt + 1
    self.hit_cnt = self.hit_cnt + 1

    -- звук "правильно"
    snd = SoundEffect(self.table[self.step].right_snd)
    snd:play();
    snd.onFinish = function()
      next_step("right")
    end
  end

  on_wrong_input = function(self)
    block_input = true
    self.shot_cnt = self.shot_cnt + 1
  --  print_score()

    -- звук "неправильно"
    snd = SoundEffect(self.table[self.step].wrong_snd)
    snd:play();
  --  self.__snd.onFinish = function()
  --    next_step(self, "wrong")
  --  end
  end

  -- проверить ответ
  -- вызвать on_right_input при совпадении
  -- on_wrong_input при ошибке
  local char_callback = function(self, code)
    if block_input then return end

    local ch = string.char(code)

    -- update substitle
    if conf.subtitle then
      self.subtitle = self.subtitle..string.upper(ch)
    end

    -- if right
    if target_letters:sub(target_pos, target_pos) == ch
    then
      target_pos = target_pos + 1
      -- if finish
      if target_pos == string.len(target_letters) + 1
      then
        on_right_input(self)
      end -- finish
    -- if wrong
    else
      on_wrong_input(self)
    end -- right
  end

  -- public
  self.table = conf.table
  self.shot_cnt = 0
  self.hit_cnt  = 0
  self.step     = 1

  self.start = function(self)
    self.step = 1
    global_char_callback = function(code)
      char_callback(self, code)
    end
    launch_plate(self)
  end

  self.destroy = function(self)
    
    if self.image then
      self.image:destroy()
    end
    self.image = nil
    
    if self.title then
      self.title = ""
    end
    self.title = nil

    if self.person then
      self.person:destroy()
    end    
    self.person = nil
    
    if scoreboard then
      scoreboard:destroy()
    end
    scoreboard = nil
    
    self.subtitle = ""

  end

  return self
end
