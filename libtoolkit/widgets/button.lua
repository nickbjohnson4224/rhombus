function set_attribute(name, value)
	if (name == "width") then
		width = value
		request_redraw()
	end
	if (name == "height") then
		height = value
		request_redraw()
	end
	if (name == "text") then
		text = value
		request_redraw()
	end
end

function get_attribute(name)
	if (name == "width") then
		return(width)
	end
	if (name == "height") then
		return(height)
	end
	if (name == "text") then
		return(text)
	end
	return(0)
end

function mouse_button(x, y, button)
	if (button == "1") then
		pressed = true
		request_redraw();
	end
	if (button ~= "1" and pressed) then
		pressed = false
		request_redraw();
		send_event("clicked")
	end
end

function draw()
	fill(0, 0, width, height, bg_color)
	if (not pressed) then
		draw_image(background, 0, 0, width, height)
	else
		draw_image(background_pressed, 0, 0, width, height)
	end
	write_text(0, 0, 12, text, fg_color, bg_color, default_font)
end

text = ""
width = 0
height = 0
background = load_image(get_theme_path() .. "/button.bmp")
background_pressed = load_image(get_theme_path() .. "/button_pressed.bmp")
fg_color = get_color("button_foreground", "default_foreground")
bg_color = get_color("button_background", "default_background")
pressed = false
