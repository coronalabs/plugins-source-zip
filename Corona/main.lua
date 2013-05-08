--Zip support

local zip = require( "zip" )


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
	zipFilename = "test.zip",
	zipBaseDir = system.DocumentsDirectory,
	listener = listener
}
zip.list(params)

local params = 
{ 
	zipFilename="test.zip",
	zipBaseDir = system.DocumentsDirectory,
	srcFilesBaseDir=system.ResourceDirectory,
	srcFilenames = { "space.jpg","space1.jpg"},
	listener = listener
}
zip.compress(params);


local params =
{
	zipFilename="test.zip",
	zipBaseDir = system.ResourceDirectory,
	dstPath = system.DocumentsDirectory,
	--flattenPath = false,
	--progress = true,
	filenames = { "space.jpg"}, --,"space1.jpg" }
	listener = listener,
}
zip.uncompress(params)


local params = {
	zipFilename = "test.zip",
	zipBaseDir = system.DocumentsDirectory,
	listener = listener
}
zip.list(params)


local params =
{
	zipFilename="test123.zip",
	zipBaseDir = system.ResourceDirectory,
	destBaseDir = system.DocumentsDirectory,
	--flattenPath = false,
	--progress = true,
	--filenames = { "space1.jpg"}, --,"space1.jpg" }
	listener = listener,
}

zip.uncompress( params )








