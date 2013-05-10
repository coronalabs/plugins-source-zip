--Zip support

local zip = require( "plugin.zip" )


-- Prints all contents of a Lua table to the log.
local function printTable(table, stringPrefix)
	if not stringPrefix then
		stringPrefix = "### "
	end
	if type(table) == "table" then
		for key, value in pairs(table) do
			if type(value) == "table" then
				print(stringPrefix .. tostring(key))
				print(stringPrefix .. "{")
				printTable(value, stringPrefix .. "   ")
				print(stringPrefix .. "}")
			else
				print(stringPrefix .. tostring(key) .. ": " .. tostring(value))
			end
		end
	end
end


local table = { a="1",b="2"}


local function listener( event )
	print("listener reached")
	
	printTable(event)
end
-- Prints all contents of a Lua table to the log.
local function printTable(table, stringPrefix)
	if not stringPrefix then
		stringPrefix = "### "
	end
	if type(table) == "table" then
		for key, value in pairs(table) do
			if type(value) == "table" then
				print(stringPrefix .. tostring(key))
				print(stringPrefix .. "{")
				printTable(value, stringPrefix .. "   ")
				print(stringPrefix .. "}")
			else
				print(stringPrefix .. tostring(key) .. ": " .. tostring(value))
			end
		end
	end
end




--TESTS



local params = {
	zipFile = "test.zip",
	zipBaseDir = system.DocumentsDirectory,
	listener = listener
}
zip.list(params)


local params = 
{ 
	zipFile="test.zip",
	zipBaseDir = system.DocumentsDirectory,
	srcBaseDir=system.ResourceDirectory,
	srcFiles = { "space.jpg","space1.jpg"},
	listener = listener
}
zip.compress(params);

local params =
{
	zipFile="test.zip",
	zipBaseDir = system.ResourceDirectory,
	dstBaseDir = system.DocumentsDirectory,
	--flattenPath = false,
	--progress = true,
	
	files = { "space2.jpg"}, --,"space1.jpg" }
	listener = listener,
}
zip.uncompress(params)

local params = {
	zipFile = "test.zip",
	zipBaseDir = system.DocumentsDirectory,
	listener = listener
}
zip.list(params)


local params =
{
	zipFile="test123.zip",
	zipBaseDir = system.ResourceDirectory,
	dstBaseDir = system.DocumentsDirectory,
	--flattenPath = false,
	--progress = true,
	--files = { "space1.jpg"}, --,"space1.jpg" }
	listener = listener,
}

zip.uncompress( params )









