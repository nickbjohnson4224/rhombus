function set_attribute(name, value)
	if (name == "width") then
		width = value
	end
	if (name == "height") then
		height = value
	end
	if (name == "size") then
		size = value
	end
	if (name == "text") then
		text = value
	end
	if (name == "foreground") then
		foreground = value
	end
	if (name == "background") then
		background = value
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
	write_text(0, 0, size, text, foreground, background)
end

size = 12
text = ""
foreground = 0xffffffff
background = 0xff000000
width = 0
height = 0
