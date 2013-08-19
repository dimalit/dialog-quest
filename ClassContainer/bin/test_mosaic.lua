scene = Mosaic.new{
  margin = 20,
  line_interval = 1.5,
  tasks_count = 3		-- TODO: Very bad to remember to change it!
--  description_interval = 20
}

scene.title.text = "Title"

scene.description:addItems({
	TextBoxItem("Here is a long long description of the scene as a whole. It consists of multiple 'tasks' each with its own words "),
	Button(TwoStateAnimation(Animation(load_config("Start.anim")))),
	TextBoxItem(" Short text"),
	Button(TwoStateAnimation(Animation(load_config("Start.anim"))))
});

scene.description
	:addObstacle(TextureItem("interface/menu_bg.rttex", 40, 40, 20, 20))
	:addObstacle(ImageItem("interface/flask.rttex", scene.description.width - 20, 20), "right")

scene.tasks[1]:ask({
	assignment = "Match the capitals",
	lines = {
		{"Great Britain", "audio/click.wav", "London"},
		{"France", "audio/click.wav", "Paris"},
		{"Italy", "audio/click.wav", "Rome"},
		{"Japan", "audio/click.wav", "Tokyo"},
	}, -- lines
	fake_answers = {
		"Moscow", "Rio"
	}, -- fake answers
--	movers_placement = "random"
  })
scene.tasks[2]:ask({
    assignment = "Who produces what",
	lines = {
		{"General Motors", "audio/click.wav", "cars"},
		{"Scott", "audio/click.wav", "bycicles"},
		{"Mivina", "audio/click.wav", "food"},
		{"Q&Q", "audio/click.wav", "watches"},
		{"IBM", "audio/click.wav", "computers"}
	}, --lines
	movers_placement = {"random vertical", 300}
  }) --task
scene.tasks[3]:ask({
    assignment = "Match the parts of speech",
	lines = {
		{"Walk", "audio/click.wav", "verb"},
		{"Home", "audio/click.wav", "noun"},
		{"Eight", "audio/click.wav", "number"},
		{"Nice", "audio/click.wav", "adjective"},
		{"Simply", "audio/click.wav", "adverb"}
	}, --lines
	movers_placement = "default"
  }) --task  

scene.onFinish = function()
  scene:destroy()
end

scene:start()