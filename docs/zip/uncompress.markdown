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

##### zipFilename ~^(required)^~
_[String][api.type.String]._ The the zip file to uncompress files from

##### zipBaseDir ~^(required)^~
_[String][api.type.String]._ The source folder containing the zip archive

##### destBaseDir ~^(required)^~
_[String][api.type.String]._ The folder to decompress files to (from the zip archive)

##### filenames ~^(optional)^~
_[Table][api.type.Table]._ A table specifying file names to decompress from the archive. By default all files will be uncompressed


##### listener ~^(required)^~
_[Listener][api.type.Listener]._ The listener function invoked at the end of the zip operation. It is passed a [zip][api.event.uncompress] event.


### Zip Uncompress
The following sample code attempts to uncompress all files from test.zip to the destination folder with optional parameters

``````lua
local function zipListener( event )
        if ( event.isError ) then
                print( "Unzip Error")
        else
                print ( "RESPONSE: " .. event.response )
        end
end
 
-- Extraction routine

local params =
{
	zipFilename="test.zip",
	zipBaseDir = system.ResourceDirectory,
	destBaseDir = system.DocumentsDirectory,
	--flattenPath = false,
	--progress = true,
	filenames = { "space.jpg"}, --,"space1.jpg" }
	listener = listener,
}

zip.uncompress( params )

``````