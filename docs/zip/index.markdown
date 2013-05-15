# zip.*

> --------------------- ------------------------------------------------------------------------------------------
> __Type__              [Library][api.type.Library]
> __Revision__          [REVISION_LABEL](REVISION_URL)
> __Keywords__          zip
> __Sample code__       
> __See also__          [Marketplace](http://www.coronalabs.com/store/plugin)
> __Availability__      Pro, Enterprise
> --------------------- ------------------------------------------------------------------------------------------

## Overview

The zip plugin enables basic zip archive support in Corona SDK.

## Platforms

The following platforms are supported:

* Android
* iOS
* Mac
* Windows

## Syntax

	local zip = require "plugin.zip"


## Functions

#### [zip.compress()][plugin.zip.compress]

#### [zip.uncompress()][plugin.zip.uncompress]

#### [zip.list()][plugin.zip.list]


## Project Settings

### SDK

When you build using the Corona Simulator, the server automatically takes care of integrating the plugin into your project. 

All you need to do is add an entry into a `plugins` table of your `build.settings`. The following is an example of a minimal `build.settings` file:

``````
settings =
{
	plugins =
	{
		-- key is the name passed to Lua's 'require()'
		["plugin.zip"] =
		{
			-- required
			publisherId = "com.coronalabs",
		},
	},		
}
``````

### Enterprise

TBD

## Support

More support is available from the Corona Labs team:

* [E-mail](mailto://sean@Pcoronalabs.com)
* [Forum](http://forum.coronalabs.com/plugin/zip)
* [Plugin Publisher](http://www.coronalabs.com)


