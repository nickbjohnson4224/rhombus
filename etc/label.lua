function setattribute(name, value)
	if (name == "size") then
		size = value
	end
	if (name == "text") then
		text = value
	end
	if (name == "fg") then
		fg = value
	end
	if (name == "bg") then
		bg = value
	end
end

function getattribute(name)
	if (name == "size") then
		return(size)
	end
	if (name == "text") then
		return(text)
	end
	if (name == "fg") then
		return(fg)
	end
	if (name == "bg") then
		return(bg)
	end
	return(0)
end

function draw()
	ret = write_text(0, 0, size, text, fg, bg)
	update()
end

size = 12
text = ""
fg = 0xffffffff
bg = 0xff000000
