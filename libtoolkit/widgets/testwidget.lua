function set_attribute(name, value)
	if (name == "width") then
		width = value
	end
	if (name == "height") then
		height = value
	end
end

function get_attribute(name)
	if (name == "width") then
		return(width)
	end
	if (name == "height") then
		return(height)
	end
end

function draw()
	write_text(100, 0, 10, "hello:", 0xffffffff, 0);
	write_text(50, 0, 10, "hello:", 0xffffffff, 0);
end

function mouse_button(absx, absy, button)
	print(absx .. ";" .. absy .. ": " .. button)
end

width = 0
height = 0
label1 = add_child("label", 0, 15, 100, 15)
label2 = add_child("label", 0, 35, 100, 15)
set_child_attribute(label1, "text", "World!")
text = get_child_attribute(label1, "text")
set_child_attribute(label2, "text", text)
set_child_attribute(label1, "text", "Hello,")
set_child_attribute(label2, "x", 20);
