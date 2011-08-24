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
	if (name == "pressed") then
		draw_pressed = value
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
	if (name == "pressed") then
		return(draw_pressed)
	end
	return(0)
end

function mouse_button(x, y, button)
	if (button == "1") then
		is_pressed = true
		request_redraw();
	end
	if (button ~= "1" and is_pressed) then
		is_pressed = false
		request_redraw();
		send_event("clicked")
	end
end

function draw()
	if (not draw_pressed and not is_pressed) then
		fill(0, 0, width, height, bg_color)
		draw_image(background, 0, 0, width, height)
		write_text(0, 0, text_height, text, fg_color, bg_color, default_font)
	else
		fill(0, 0, width, height, bg_color_pressed)
		draw_image(background_pressed, 0, 0, width, height)
		write_text(0, 0, text_height, text, fg_color_pressed, bg_color_pressed, default_font)
	end
end

text = ""
width = 0
height = 0
text_height = get_theme_attribute("button_text_height", "default_text_height")
background = load_image(get_theme_path() .. "/button.bmp")
background_pressed = load_image(get_theme_path() .. "/button_pressed.bmp")
fg_color = get_theme_attribute("button_foreground", "default_foreground")
bg_color = get_theme_attribute("button_background", "default_background")
fg_color_pressed = get_theme_attribute("button_pressed_foreground")
if (fg_color_pressed == 0) then
	fg_color_pressed = fg_color
end
bg_color_pressed = get_theme_attribute("button_pressed_background")
if (bg_color_pressed == 0) then
	bg_color_pressed = bg_color
end
draw_pressed = false
is_pressed = false
