local table = {
--  [1] = {nil, "interface/test.bmp", "Hello, bla-bla-bla", "doll/normal.jpg", {{1,2}, {5,3}}, {"I am", "he is", "she was", "France", "UK"}, "ding.wav", nil, 7},
  [1] = {nil, nil, "Welcome! Wait for 5 sec...", "interface/test.bmp", nil, nil, "ding.wav", nil, nil, 5},
  [2] = {nil, "interface/test.bmp", "Hello, 22-22-222-2", "interface/test.bmp", {1,2,3}, {"I am", "he is", "she was", "France"}, nil, 3, 4},
  [3] = {nil, "interface/test.bmp", "Right!", nil, nil, nil, "ding.wav", nil, 5, 2},
  [4] = {nil, "interface/test.bmp", "Wrong", nil, nil, nil, "miss.wav", nil, nil, 2},
  [5] = {nil, "interface/test.bmp", "Hello, bla-bla-bla", "doll/normal.jpg", {{1,2}, {5,3}}, {"I am", "he is", "she was", "France", "UK"}, "ding.wav", nil, 7},
  [6] = {nil, nil, "Right!", "interface/test.bmp", nil, nil, "ding.wav", nil, 0, 2},
  [7] = {nil, nil, "Wrong", "interface/test.bmp", nil, nil, "miss.wav", nil, nil, 2}
}

scene = Mosaic{
  margin = 20,
  line_length = 4,
  line_interval = 1.5,
  drops_interval = 100,
  image_y = 100,
  button_y = screen_height * 2 / 3,
  table = table
}

scene.onFinish = function()
  scene:destroy()
end

scene:start()