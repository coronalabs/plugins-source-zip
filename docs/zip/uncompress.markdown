# zip.uncompress()

> --------------------- ------------------------------------------------------------------------------------------
> __Type__              [Function][api.type.Function]
> __Library__           [zip.*][plugin.zip]
> __Return value__      NONE
> __Revision__          [REVISION_LABEL](REVISION_URL)
> __Keywords__          
> __Sample code__       
> __See also__          [zip.compress()][plugin.zip.compress]
>                       
>                       
>                       
> --------------------- ------------------------------------------------------------------------------------------


## Overview

Extracts all files from a zip archive

## Syntax

	zip.uncompress( options )

## Required Settings
``````lua
	plugins =
	{
		-- key is the name passed to Lua's 'require()'
		["plugin.zip"] =
		{
			-- required
			publisherId = "com.coronalabs",
		},
	},
``````

##### options.zipFile ~^(required)^~
_[String][api.type.String]._ The the zip file to uncompress files from

##### options.zipBaseDir ~^(required)^~
_[Constant][api.type.Constant]._ The source folder containing the zip archive

##### options.dstBaseDir ~^(required)^~
_[Constant][api.type.Constant]._ The destination folder to decompress files to (from the zip archive)

##### options.files ~^(optional)^~
_[Array][api.type.Array]._ A table specifying file names to decompress from the archive. By default all files will be uncompressed

##### options.listener ~^(required)^~
_[Listener][api.type.Listener]._ The listener function invoked at the end of the zip operation. It is passed a [zip][api.event.uncompress] event.


### Zip Uncompress
The following sample code attempts to uncompress all files from test.zip to the destination folder with optional parameters

``````lua
local function zipListener( event )
        if ( event.isError ) then
                print( "Unzip Error")
        else
                print ( "event.name: " .. event.name )
                print ( "event.type: " .. event.type )

                -- example response
				--event.response = {
				--	[1] = "space.jpg",
				--	[2] = "space1.jpg",
				--}
        end
end
 
-- Extraction routine

local options =
{
	zipFile="test.zip",
	zipBaseDir = system.ResourceDirectory,
	dstBaseDir = system.DocumentsDirectory,
	files = { "space.jpg","space1.jpg" }
	listener = zipListener,
}

zip.uncompress( options )

``````