#!/bin/sh
qmake -macx
make
mkdir mine.app/Contents/Resources
cp -r icons mine.app/Contents/Resources
cp mine.icns mine.app/Contents/Resources
cp -f Info.plist mine.app/Contents
cp *.qm mine.app/Contents/Resources
mkdir mine.app/Contents/Resources/zh_CN.lproj
cp InfoPlist.strings mine.app/Contents/Resources/zh_CN.lproj
mkdir mine.app/Contents/Resources/English.lproj
cp InfoPlist.English mine.app/Contents/Resources/English.lproj/InfoPlist.strings
mkdir mine.app/Contents/Frameworks
/bin/cp $QTDIR/lib/libqt-mt.3.dylib mine.app/Contents/Frameworks
install_name_tool -change libqt-mt.3.dylib @executable_path/../Frameworks/libqt-mt.3.dylib mine.app/Contents/MacOS/mine
