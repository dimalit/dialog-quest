local b = TextButton{"��� �������", "btn_rect.anim", shrink = true}
root:add(b)
b.width, b.height = 150, 30
root:link(b, 0.5, 0.5, root, 0.5, 0.5)