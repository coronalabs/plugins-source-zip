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

	zip.compress( options )


##### options.zipFile ~^(required)^~
_[String][api.type.String]._ The the zip file to compress files to

##### options.zipBaseDir ~^(required)^~
_[Constant][api.type.Constant]._ The directory containing the zip archive

##### options.srcBaseDir ~^(required)^~
_[Constant][api.type.Constant]._ The directory containing the files to add to the zip archive

##### options.srcFiles ~^(required)^~
_[Array][api.type.Array]._ A table specifying a set of file names to compress into the archive

##### options.listener ~^(required)^~
_[Listener][api.type.Listener]._ The listener function invoked at the end of the zip operation. It is passed a [zip][api.event.extractFromZip] event.


### Zip Compress
The following sample code attempts to compress files from the filenames param to test.zip with a parameter table

``````lua
local function listener( event )
        if ( event.isError ) then
                print( "Unzip Error")
        else
		--Example response
                print ( "event.name: " .. event.name )
                print ( "event.type: " .. event.type )
                print ( "event.name: " .. event.name )

		--event.response = {
		--	[1] = "space.jpg",
		--	[2] = "space1.jpg",
		--}
        end
end
 

local options = 
{ 
	zipFilename="test.zip",
	zipBaseDir = system.ResourceDirectory,
	srcBaseDir =system.DocumentsDirectory,
	srcFiles = { "space.jpg", "space1.jpg",  "space2.jpg"},
	listener = listener
}
zip.compress(options);

``````