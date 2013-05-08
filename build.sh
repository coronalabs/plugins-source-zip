#!/bin/bash

path=`dirname $0`

xcodebuild -project "$path"/../../platform/mac/lua.xcodeproj -alltargets -configuration Release

cd $path/ios
./build.sh ../../build/zip/ios/
cd -

#cd $path/mac
#./build.sh ../../build/zip/mac/
#cd -

#cd $path/android
#./build.sh
#cd -

# echo "Succeeded in building: iOS, Mac, Android"
# echo "You must build windows separately"
