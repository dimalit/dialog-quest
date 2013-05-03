normal = {
	width=32,
	height=32,
	tex_x=0,
	tex_y=0,
	file="normal.png"
}

over=_G.copy_table(normal) over.file="over.png"

checked = {
	width=32,
	height=32,
	tex_x=0,
	tex_y=0,
	file="transparent_32.png"
}
checked_over=_G.copy_table(normal) checked_over.file="checked_over.png"

disabled=_G.copy_table(normal) disabled.file="disabled.png"

normal2checked = {
	width=32,
	height=32,
	tex_x=0,
	tex_y=0,
	file="normal2checked.png",
	nframes=4,
	fps=16
}