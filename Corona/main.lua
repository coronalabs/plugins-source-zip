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


local function listener( event )
	print("listener reached")
	
	printTable(event)
end


print("The following test opens a zip file and lists its contents")
--TESTS
local params = {
	zipFile = "test.zip",
	zipBaseDir = system.DocumentsDirectory,
	listener = listener
}

zip.list(params)


print("The following test opens or creates a zip file and adds 2 files to the archive")
local params = 
{ 
	zipFile="test.zip",
	zipBaseDir = system.DocumentsDirectory,
	srcBaseDir=system.ResourceDirectory,
	srcFiles = { "space.jpg","space1.jpg"},
	listener = listener
}
zip.compress(params);

print("The following test uncompresses specific files from the zip archive")
local params =
{
	zipFile="test.zip",
	zipBaseDir = system.ResourceDirectory,
	dstBaseDir = system.DocumentsDirectory,

	files = { "space2.jpg"}, --,"space1.jpg" }
	listener = listener,
}
zip.uncompress(params)

print("The following test lists files again")
local params = {
	zipFile = "test.zip",
	zipBaseDir = system.DocumentsDirectory,
	listener = listener
}
zip.list(params)

print("The following test attempts to open a non-existent zip file for uncompression")
local params =
{
	zipFile="test123.zip",
	zipBaseDir = system.ResourceDirectory,
	dstBaseDir = system.DocumentsDirectory,
	--files = { "space1.jpg"}, --,"space1.jpg" }
	listener = listener,
}

zip.uncompress( params )










