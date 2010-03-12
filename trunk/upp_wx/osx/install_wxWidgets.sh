#!/opt/local/bin/bash
# This file install wxWidgets 2.9.0 for both ppc and i386
# This will allow the building of usbpicprog as a universal application.
#
# All build files assumes that MacPorts is installed. You can change
# settings below if necessary.
#
# Created by Gustav Johansson 2010-03-11
#

# Load the settings file. Change anything you want in that file ONLY!
source settings

# Create all directories needed.
if [ -d "$PREFIX_ppc" ]; then
	echo "$PREFIX_ppc exists!"
else 	
	echo "Creating $PREFIX_ppc"
	mkdir -p "$PREFIX_ppc"
fi
if [ -d "$PREFIX_i386" ]; then
	echo "$PREFIX_i386 exists!"
else 	
	echo "Creating $PREFIX_i386"
	mkdir -p "$PREFIX_i386"
fi
if [ -d "$PREFIX_app" ]; then
	echo "$PREFIX_app exists!"
else 	
	echo "Creating $PREFIX_app"
	mkdir -p "$PREFIX_app"
fi
if [ -d "$WORKDIR" ]; then
	echo "$WORKDIR exists!"
else 	
	echo "Creating $WORKDIR"
	mkdir -p "$WORKDIR"
fi

# Download wxWidgets
cd "$WORKDIR"
wget http://prdownloads.sourceforge.net/wxwindows/wxWidgets-2.9.0.tar.gz
# Unpack it
tar xzf wxWidgets-2.9.0.tar.gz
# Get in there and create a builddir
cd wxWidgets-2.9.0
mkdir build-mac
cd build-mac
# Configure for ppc  
# NOTE: This is the magic line to make it work:
# --with-libiconv-prefix=/opt/local/
# This forces wxWidgets to use the MacPorts installation of libiconv and
# not the one bundled with Apple. THIS IS NECESSARY OR usbpicprog WILL FAIL
# WHEN BUILDING!
arch_flags="-arch ppc"
../configure CFLAGS="$arch_flags" CXXFLAGS="$arch_flags" CPPFLAGS="$arch_flags" LDFLAGS="$arch_flags" OBJCFLAGS="$arch_flags" OBJCXXFLAGS="$arch_flags" --disable-shared --with-libiconv-prefix=/opt/local/ --prefix="$PREFIX_ppc"
make 
make install
make clean
# Configure for i386 
# NOTE: This is the magic line to make it work:
# --with-libiconv-prefix=/opt/local/
# This forces wxWidgets to use the MacPorts installation of libiconv and
# not the one bundled with Apple. THIS IS NECESSARY OR usbpicprog WILL FAIL
# WHEN BUILDING!
arch_flags="-arch i386"
../configure CFLAGS="$arch_flags" CXXFLAGS="$arch_flags" CPPFLAGS="$arch_flags" LDFLAGS="$arch_flags" OBJCFLAGS="$arch_flags" OBJCXXFLAGS="$arch_flags" --disable-shared --with-libiconv-prefix=/opt/local/ --prefix="$PREFIX_i386"
make
make install
make clean
