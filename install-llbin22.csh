#!/bin/csh -f

set app=llbin22
# xcodebuild -list -project $app.xcodeproj

# rm -rf DerivedData/
xcodebuild -scheme $app -configuration Release clean build
# xcodebuild -configuration Release -alltargets clean

find ./DerivedData -type f -name $app -perm +111 -ls
set src=./DerivedData/Build/Products/Release/$app

echo "File $src"
ls -al ${src}
cp $src ~/opt/bin/llbin22
ls -al ~/opt/bin/llbin22