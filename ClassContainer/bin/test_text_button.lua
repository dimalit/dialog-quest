local b = TextButton{"Мне понятно", "btn_rect.anim", shrink = true}
root:add(b)
b.width, b.height = 250, 50
root:link(b, 0.5, 0.5, root, 0.5, 0.5)