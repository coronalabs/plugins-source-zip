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



##### zipFile ~^(required)^~
_[String][api.type.String]._ The the zip file to compress files to

##### zipBaseDir ~^(required)^~
_[Constant][api.type.Constant]._ The directory containing the zip archive

##### srcBaseDir ~^(required)^~
_[Constant][api.type.Constant]._ The directory containing the files to add to the zip archive

##### srcFiles ~^(required)^~
_[Array][api.type.Array]._ A table specifying a set of file names to compress into the archive

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
	srcBaseDir =system.DocumentsDirectory,
	srcFiles = { "space.jpg"},
	listener = listener
}
zip.compress(params);

``````


event.name = "zip"
event.type = "compress", "uncompress", "list"
event.isError = true/false
event.errorMessage = "Error b.c of X" (if isError == false, then this shoudl be nil)

-- compress
event.response = {
	[1] = "space.jpg",
	[2] = "space1.jpg",
}

-- uncompress
event.response = {
	[1] = "space.jpg",
	[2] = "space1.jpg",
}

-- list
event.response = {
	[1] =
	{
	   ratio: 98
	   size: 2188295440
	   file: space.jpg
	},
	[2] =
	{
	   ratio: 98
	   size: 2188295440
	   file: space1.jpg
	},
}
