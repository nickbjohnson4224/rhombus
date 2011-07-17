function update_layout()
	local size = height / table.getn(children)
	for i,child in pairs(children) do
		set_child_attribute(child, "height", size)
		set_child_attribute(child, "y", (i - 1) * size)
	end
end

function set_attribute(name, value)
	if (name == "width") then
		width = value
		for _,child in pairs(children) do
			set_child_attribute(child, name, value)
		end
	end
	if (name == "height") then
		local old = height
		height = value
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
	return (0)
end

function add_widget(widgettype)
	widget = add_child(widgettype, 0, 0, width, 100)
	table.insert(children, widget)
	update_layout()
end

width = 0
height = 0
children = {}
