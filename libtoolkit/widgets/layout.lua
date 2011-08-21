function update_layout()
	if (horizontal == false) then
		local size = height / table.getn(children)
		for i,child in pairs(children) do
			set_child_attribute(child, "height", size)
			set_child_attribute(child, "y", (i - 1) * size)
		end
	else
		local size = width / table.getn(children)
		for i,child in pairs(children) do
			set_child_attribute(child, "width", size)
			set_child_attribute(child, "x", (i - 1) * size)
		end
	end
end

function set_attribute(name, value)
	if (name == "width") then
		width = value
		if (horizontal == false) then
			for _,child in pairs(children) do
				set_child_attribute(child, name, value)
			end
		else
			update_layout()
		end
	end
	if (name == "height") then
		height = value
		if (horizontal == false) then
			update_layout()
		else
			for _,child in pairs(children) do
				set_child_attribute(child, name, value)
			end
		end
	end
	if (name == "horizontal") then
		horizontal = value
		update_layout()
	end
end

function get_attribute(name)
	if (name == "width") then
		return(width)
	end
	if (name == "height") then
		return(height)
	end
	if (name == "horizontal") then
		return(horizontal)
	end
	return (0)
end

function add_widget(widgettype)
	widget = add_child(widgettype, 0, 0, width, height)
	table.insert(children, widget)
	update_layout()
end

width = 0
height = 0
children = {}
horizontal = false
