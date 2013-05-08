# zip.compress()

> --------------------- ------------------------------------------------------------------------------------------
> __Type__              [Function][api.type.Function]
> __Library__           [zip.*][api.library. zip]
> __Return value__      [Object][api.type.Object]
> __Revision__          [REVISION_LABEL](REVISION_URL)
> __Keywords__          
> __Sample code__       
> __See also__          
>                       
>                       
>                       
> --------------------- ------------------------------------------------------------------------------------------


## Overview

Adds to zip

## Syntax

	zip.compress( params )


##### params ~^(required)^~
_[Table][api.type.Table]._ A table that specifies options for zip

##### zipFilename ~^(required)^~
_[String][api.type.String]._ The the zip file to compress files to

##### zipBaseDir ~^(required)^~
_[String][api.type.String]._ The directory containing the zip archive

##### srcFilesBaseDir ~^(required)^~
_[String][api.type.String]._ The directory containing the files to add to the zip archive

##### srcFilenames ~^(required)^~
_[Table][api.type.Table]._ A table specifying a set of file names to compress into the archive

##### listener ~^(required)^~
_[Listener][api.type.Listener]._ The listener function invoked at the end of the zip operation. It is passed a [zip][api.event.extractFromZip] event.


### Zip Compress
The following sample code attempts to compress files from the filenames param to test.zip with a parameter table

``````lua
local function listener( event )
        if ( event.isError ) then
                print( "Unzip Error")
        else
                print ( "RESPONSE: " .. event.response )
        end
end
 

local params = 
{ 
	zipFilename="test.zip",
	zipBaseDir = system.ResourceDirectory,
	srcFilesBaseDir =system.DocumentsDirectory,
	srcFilenames = { "space.jpg"},
	listener = listener
}
zip.compress(params);

``````