#!/bin/bash

if [ -z "$1" ]
then
    cat << END_OF_BLOCK
Available builds:
install-centos.x86_64
install-centos.x86@x86_64
linux.x86_64
linux.x86_64_static
linux.x86
linux.x86_static
END_OF_BLOCK
    exit
fi

SWITCHES='--with-libjpeg=builtin --with-libpng=builtin --with-libtiff=builtin --with-regex=builtin --with-zlib=builtin --with-expat=builtin'

while [ "$1" ]
do
    case "$1" in
        install-centos.x86_64)
            yum install gcc-c++ gtk2-devel
            ;;
        install-centos.x86@x86_64)
            yum install glibc-devel.i686 libgcc.i686 libstdc++-devel.i686 atk-devel.i686 glib2-devel.i686 pango-devel.i686 gdk-pixbuf2-devel.i686 gtk2-devel.i686
            ln /usr/lib/libX11.so.6 /usr/lib/libX11.so
            ln /usr/lib/libXxf86vm.so.1.0.0 /usr/lib/libXxf86vm.so
            ;;
        linux.x86_64)
            mkdir -p ../wxout/linux.x86_64
            ./configure --prefix=$(realpath ../wxout/linux.x86_64) $SWITCHES
            make
            make install
            make distclean
            ;;
        linux.x86_64_static)
            mkdir -p ../wxout/linux.x86_64_static
            ./configure --prefix=$(realpath ../wxout/linux.x86_64_static) --disable-shared $SWITCHES
            make
            make install
            make distclean
            ;;
        linux.x86)
            mkdir -p ../wxout/linux.x86
            ./configure --prefix=$(realpath ../wxout/linux.x86) $SWITCHES CFLAGS='-m32' CXXFLAGS='-m32' CPPFLAGS='-m32' LDFLAGS='-m32'
            make
            make install
            make distclean
            ;;
        linux.x86_static)
            mkdir -p ../wxout/linux.x86_static
            ./configure --prefix=$(realpath ../wxout/linux.x86_static) --disable-shared $SWITCHES CFLAGS='-m32' CXXFLAGS='-m32' CPPFLAGS='-m32' LDFLAGS='-m32'
            make
            make install
            make distclean
            ;;
        *)
            echo "Ignoring '$1'".
            ;;
    esac
    shift
done
