local table = {
  [1] = {nil, nil, "Welcome! Wait for 5 sec...", "doll/normal.jpg", nil, nil, "ding.wav", nil, nil, 5},
  [2] = {nil, "doll/normal.jpg", "Hello, 22-22-222-2", "bike/normal.jpg", {1,2,3}, {"I am", "he is", "she was", "France"}, nil, 3, 4},
  [3] = {nil, "bike/normal.jpg", "Right!", nil, nil, nil, "ding.wav", nil, 5, 2},
  [4] = {nil, "bike/normal.jpg", "Wrong", nil, nil, nil, "miss.wav", nil, nil, 2},
  [5] = {nil, "ball/normal.jpg", "Hello, bla-bla-bla", "doll/normal.jpg", {{1,2}, {5,3}}, {"I am", "he is", "she was", "France", "UK"}, "ding.wav", nil, 7},
  [6] = {nil, nil, "Right!", "doll/normal.jpg", nil, nil, "ding.wav", nil, 0, 2},
  [7] = {nil, nil, "Wrong", "doll/normal.jpg", nil, nil, "miss.wav", nil, nil, 2}
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