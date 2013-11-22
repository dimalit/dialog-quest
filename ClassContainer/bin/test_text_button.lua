local b = TextButton{"Мне понятно", "btn_rect.anim", shrink = true}
root:add(b)
b.width, b.height = 500, 500
--root:link(b, 0.5, 0.5, root, 0.5, 0.5)
root:link(b, 0, 0, root, 0, 0)