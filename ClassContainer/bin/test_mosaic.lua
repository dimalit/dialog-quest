scene = Mosaic{
  margin = 20,
  line_interval = 1.5,
--  description_interval = 20
}

scene.title = "Title"
--scene.description = "Here is a long long description of the scene as a whole. It consists of multiple 'tasks' each with its ow words."
scene.description = {
	TextBoxItem("Here is a long long description of the scene as a whole. It consists of multiple 'tasks' each with its ow words "),
	Button(TwoStateAnimation(Animation(load_config("Start.anim")))),
	TextBoxItem(" Short text"),
	Button(TwoStateAnimation(Animation(load_config("Start.anim"))))
};

scene.description.obstacles = {
	TextureItem("interface/menu_bg.rttex", 40, 40, 20, 20),
	{ImageItem("interface/flask.rttex", 20, 20), "right"}
}

scene.tasks = {
  {
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
  }, --task
  {
    assignment = "Who produces what",
	lines = {
		{"General Motors", "audio/click.wav", "cars"},
		{"Scott", "audio/click.wav", "bycicles"},
		{"Mivina", "audio/click.wav", "food"},
		{"Q&Q", "audio/click.wav", "watches"},
		{"IBM", "audio/click.wav", "computers"}
	}, --lines
	movers_placement = {"random vertical", 300}
  }, --task
  {
    assignment = "Match the parts of speech",
	lines = {
		{"Walk", "audio/click.wav", "verb"},
		{"Home", "audio/click.wav", "noun"},
		{"Eight", "audio/click.wav", "number"},
		{"Nice", "audio/click.wav", "adjective"},
		{"Simply", "audio/click.wav", "adverb"}
	}, --lines
	movers_placement = "default"
  } --task  
} --tasks

scene.onFinish = function()
  scene:destroy()
end

scene:start()