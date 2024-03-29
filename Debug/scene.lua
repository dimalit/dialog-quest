Scene = function(conf)
  if conf == nil then conf = {} end
  
  -- change it for visual tuning
  conf.margin = conf.margin or 10

  local self = {}
  self.margin = conf.margin       -- used in scoreboard

  local image = nil
  local title = TextItem(screen_width/2, 0, "", "font1.fnt")
  local person = nil
  local subtitle = TextItem(screen_width/2, 0, "", "font1.fnt")
    
  title.y = title.height / 2 + conf.margin
  subtitle.y = screen_height - subtitle.height / 2 - conf.margin

  setmetatable(self, {
    __index = function(self, key)
      if key == "title" then
        return title.text
      elseif key == "subtitle" then
        return subtitle.text
      end
    end,
    __newindex = function(self, key, val)
      if key=="image"
      then
        if image ~= nil then image:destroy() end
        image = val
        if val ~= nil then
          val.x = val.width  / 2 + conf.margin
          val.y = val.height / 2 + conf.margin
        end
      elseif key=="title" then
        if val == nil then val = "" end
        title.view.text = val
        title.hpx = title.view.width / 2
        title.hpy = title.view.height / 2
      elseif key=="subtitle" then
        if val == nil then val = "" end
        subtitle.view.text = val
        subtitle.hpx = subtitle.view.width / 2
        subtitle.hpy = subtitle.view.height / 2        
      elseif key=="person" then
        if person ~= nil then person:destroy() end      
        person = val
        if val ~= nil then
          val.x = screen_width - val.width / 2 - conf.margin
          val.y = val.height / 2 + conf.margin
        end
      -- add if not found
      else
        rawset(self, key, val)
      end
    end
  })

  return self
end

dofile("scene_shooter.lua")
dofile("scene_mosaic.lua")
