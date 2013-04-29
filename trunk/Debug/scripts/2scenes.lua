local start = function(file)
  dofile(file)
  scene.onFinish = function()
    scene:destroy()
    next()
  end
end

next = function()
  if step <= #script then script[step](); step = step + 1 end
end

script = {
  function() start("scripts/mosaic.lua") end,
  function() start("scripts/shooter.lua") end
}

step = 1
next()