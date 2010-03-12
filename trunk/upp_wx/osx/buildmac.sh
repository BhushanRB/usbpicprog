#!/bin/bash

OUTPUTPATH=$(pwd)

#cp -R osx/* .

cp Makefile.in.osx Makefile.in

make clean

rm -rf src/usbpicprog.app
mkdir -p src/usbpicprog.app
mkdir -p src/usbpicprog.app/Contents
mkdir -p src/usbpicprog.app/Contents/MacOS
mkdir -p src/usbpicprog.app/Contents/Resources
mkdir -p src/usbpicprog.app/Contents/SharedSupport
mkdir -p src/usbpicprog.app/Contents/MacOS/output

#./configure CC=gcc-4.0 CXX=g++-4.0 LD=g++-4.0 --prefix=${OUTPUTPATH}/src/usbpicprog.app/Contents/MacOS/output
#./configure --prefix=${OUTPUTPATH}/src/usbpicprog.app/Contents/MacOS/output

##############################################################
# Edited from usbpicprog source to allow for universal build #
##############################################################

# Copy in the universal library libusb
cp /opt/local/lib/libusb-1.0.dylib libs

# Load our settings
source settings

# Export path so that we use the correct version of wxWidgets
export PATH="$PREFIX_ppc"/bin:"$PREFIX_ppc"/sbin:$PATH
arch_flags="-arch ppc"
./configure CFLAGS="$arch_flags" CXXFLAGS="$arch_flags" CPPFLAGS="$arch_flags" LDFLAGS="$arch_flags" OBJCFLAGS="$arch_flags" OBJCXXFLAGS="$arch_flags" --prefix=${OUTPUTPATH}/src/usbpicprog.app/Contents/MacOS/output

# Build and install for ppc
make -j 2
make install

# Copy the ppc version to the target dir.
cp src/usbpicprog.app/Contents/MacOS/output/bin/usbpicprog "$PREFIX_app"/usbpicprog_ppc

# Cleanup before building again
make clean

# Export path so that we use the correct version of wxWidgets
export PATH="$PREFIX_i386"/bin:"$PREFIX_i386"/sbin:$PATH
arch_flags="-arch i386"
./configure CFLAGS="$arch_flags" CXXFLAGS="$arch_flags" CPPFLAGS="$arch_flags" LDFLAGS="$arch_flags" OBJCFLAGS="$arch_flags" OBJCXXFLAGS="$arch_flags" --prefix=${OUTPUTPATH}/src/usbpicprog.app/Contents/MacOS/output

# Build and install for ppc
make -j 2
make install

# Copy the ppc version to the target dir.
cp src/usbpicprog.app/Contents/MacOS/output/bin/usbpicprog "$PREFIX_app"/usbpicprog_i386

# Create the universal binary, everything else is "universal" anyways...
lipo "$PREFIX_app"/usbpicprog_* -create -output src/usbpicprog.app/Contents/MacOS/usbpicprog

##############################################################
#                        Done editing!                       #
##############################################################


cp -r src/usbpicprog.app/Contents/MacOS/output/lib/locale src/usbpicprog.app/po
cp src/Info.plist src/usbpicprog.app/Contents
cp icons/usbpicprog.icns src/usbpicprog.app/Contents/Resources
echo -n "APPL????" > src/usbpicprog.app/Contents/PkgInfo
cp -R xml_data src/usbpicprog.app/xml_data
cp index.xml src/usbpicprog.app/xml_data

rm -rf src/usbpicprog.app/Contents/MacOS/output

EXECFILE="src/usbpicprog.app/Contents/MacOS/usbpicprog"
LIBPATH="libs"
NEWLIBPATH="@executable_path/../SharedSupport"

# space separated list of libraries
TARGETS="libusb-1.0.dylib "
#\
#"libwx_osx_carbonu-2.9.dylib"
for TARGET in ${TARGETS} ; do
LIBFILE=${LIBPATH}/${TARGET}
cp ${LIBFILE} src/usbpicprog.app/Contents/SharedSupport
LIBFILE=src/usbpicprog.app/Contents/SharedSupport/${TARGET}
TARGETID=`otool -DX ${LIBPATH}/$TARGET`
NEWTARGETID=${NEWLIBPATH}/${TARGET}
install_name_tool -id ${NEWTARGETID} ${LIBFILE}
install_name_tool -change ${TARGETID} ${NEWTARGETID} ${EXECFILE}
done
