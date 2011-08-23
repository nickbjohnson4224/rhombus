function set_attribute(name, value)
	if (name == "width") then
		width = value
		request_redraw()
	end
	if (name == "height") then
		height = value
		request_redraw()
	end
	if (name == "size") then
		size = value
		request_redraw()
	end
	if (name == "text") then
		text = value
		request_redraw()
	end
	if (name == "foreground") then
		foreground = value
		request_redraw()
	end
	if (name == "background") then
		background = value
		request_redraw()
	end
	if (name == "font") then
		if (font) then
			free_font(font)
		end
		font = load_font(value)
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
	if (name == "size") then
		return(size)
	end
	if (name == "text") then
		return(text)
	end
	if (name == "foreground") then
		return(foreground)
	end
	if (name == "background") then
		return(background)
	end
	return(0)
end

function draw()
	fill(0, 0, width, height, background)
	if (font) then
		write_text(0, 0, size, text, foreground, background, font)
	else
		write_text(0, 0, size, text, foreground, background, default_font)
	end
end

size = get_theme_attribute("label_text_height", "default_text_height")
text = ""
foreground = get_theme_attribute("label_foreground", "default_foreground")
background = get_theme_attribute("label_background", "default_background")
width = 0
height = 0
font = nil
