# zip.uncompress()

> --------------------- ------------------------------------------------------------------------------------------
> __Type__              [Function][api.type.Function]
> __Library__           [zip.*][api.library.zip]
> __Return value__      [Object][api.type.Object]
> __Revision__          [REVISION_LABEL](REVISION_URL)
> __Keywords__          asynchronous, http, https, get, post
> __Sample code__       
> __See also__          [zip.uncompress()][api.library.zip.uncompress]
>                       
>                       
>                       
> --------------------- ------------------------------------------------------------------------------------------


## Overview

Extracts all files from a zip archive

## Syntax

	zip.uncompress( params )


##### params ~^(required)^~
_[Table][api.type.Table]._ A table that specifies options for zip

##### zipFile ~^(required)^~
_[String][api.type.String]._ The the zip file to uncompress files from

##### zipBaseDir ~^(required)^~
_[Constant][api.type.Constant]._ The source folder containing the zip archive

##### dstBaseDir ~^(required)^~
_[Constant][api.type.Constant]._ The destination folder to decompress files to (from the zip archive)

##### files ~^(optional)^~
_[Array][api.type.Array]._ A table specifying file names to decompress from the archive. By default all files will be uncompressed

##### listener ~^(required)^~
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
                print ( "event.name: " .. event.name )

                -- example response
				--event.response = {
				--	[1] = "space.jpg",
				--	[2] = "space1.jpg",
				--}
        end
end
 
-- Extraction routine

local params =
{
	zipFile="test.zip",
	zipBaseDir = system.ResourceDirectory,
	dstBaseDir = system.DocumentsDirectory,
	files = { "space.jpg","space1.jpg" }
	listener = listener,
}

zip.uncompress( params )

``````