# zip.list()

> --------------------- ------------------------------------------------------------------------------------------
> __Type__              [Function][api.type.Function]
> __Library__           [zip.*][api.library.zip]
> __Return value__      [Object][api.type.Object]
> __Revision__          [REVISION_LABEL](REVISION_URL)
> __Keywords__          zip
> __Sample code__       
> __See also__          [zip.list()][api.library.zip.list]
>                       
>                       
>                       
> --------------------- ------------------------------------------------------------------------------------------

## Overview

Lists all files in a zip archive

## Syntax

	zip.list( params )

##### params ~^(required)^~
_[Table][api.type.Table]._ A table that specifies options for zip

##### zipFile ~^(required)^~
_[String][api.type.String]._ The filename of the zip archive

##### zipBaseDir ~^(required)^~
_[String][api.type.String]._ The source folder containing the zip archive

##### listener ~^(required)^~
_[Listener][api.type.Listener]._ The listener function invoked at various phases of the zip operation. It is passed a [Listener][api.event.list] event. 

The listener function can receive events of the following phases:


<!--

#Coming Soon
* `"began"` - The first notification, indicates that extraction began.>
* `"ended"` - The final notification, when the extraction is finished.>

By default, the listener will only receive `"ended"` events.  If params.progress (described below) is set to true, then the listener will also receive `"began"` and `"progress"` events.

-->

## Examples

### Zip List
The following sample code lists all files from test.zip with additional file info

``````lua
local function listener( event )
        if ( event.isError ) then
                print( "Unzip Error")
        else
    
          -- Example list response
          --event.response = {
          --[1] =
          --{
          --  ratio: 98
          --  size: 2188295440
          --  file: space.jpg
          --},
          --[2] =
          --{
          --  ratio: 98
          --  size: 2188295440
          --  file: space1.jpg
          --},
		
        end
end
 
-- Extraction routine

local params = {
        zipFile = "test.zip",
        zipBaseDir = system.DocumentsDirectory,
        listener = listener
}

zip.list( params )

``````



  

