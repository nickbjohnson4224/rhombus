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
		send_event("clicked")
	end
end

function draw()
	write_text(0, 0, 12, text, 0xffffffff, 0xff000000)
end

text = ""
width = 0
height = 0
