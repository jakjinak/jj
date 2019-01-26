#!/bin/bash

# TODO convert to a Makefile

if [ -z "$1" ]
then
    cat << END_OF_BLOCK
Available builds:
install-centos.x86_64
install-centos.x86@x86_64
install-ubuntu.x86
linux.x86_64
linux.x86_64_static
linux.x86
linux.x86_static
patch-cygwin
cygwin.x86_64
cygwin.x86_64_static
END_OF_BLOCK
    exit
fi

SWITCHES='--with-libjpeg=builtin --with-libpng=builtin --with-libtiff=builtin --with-regex=builtin --with-zlib=builtin --with-expat=builtin'

patchfile()
{ f="$1"
  shift
  sed "$@" < "$f" > "$f.bak" && mv "$f.bak" "$f"
}

while [ "$1" ]
do
    case "$1" in
        install-centos.x86_64)
            yum install gcc-c++ gtk2-devel
            ;;
        install-centos.x86@x86_64)
            yum install glibc-devel.i686 libgcc.i686 libstdc++-devel.i686 atk-devel.i686 glib2-devel.i686 pango-devel.i686 gdk-pixbuf2-devel.i686 gtk2-devel.i686 libX11-devel.i686 libXxf86vm-devel.i686
            ;;
        install-ubuntu.x86)
            sudo apt-get install libpango1.0-dev libgtk2.0-dev libxxf86vm-dev
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
        patch-cygwin)
            # https://trac.wxwidgets.org/ticket/16746
            # https://forums.wxwidgets.org/viewtopic.php?t=33849
            set -x
            set -e
            patchfile src/msw/volume.cpp 's|static long s_cancelSearch = FALSE;|static wxInt32 s_cancelSearch = FALSE;|'
            patchfile src/common/socket.cpp -e 's|struct timeval tv;|TIMEVAL tv;|' -e 's|tv = \*timeout;|{ tv.tv_sec = timeout->tv_sec; tv.tv_usec = timeout->tv_usec; }|'
            patchfile src/msw/sockmsw.cpp 's|timeval tv = { 0, 0 };|TIMEVAL tv = { 0, 0 };|'
            ;;
        cygwin.x86_64)
            # TODO link error
            mkdir -p ../wxout/$1
            CPPFLAGS='-w -fpermissive -D__USE_W32_SOCKETS' LDFLAGS='-L /lib/w32api/' ./configure --prefix=$(realpath ../wxout/$1) $SWITCHES --with-msw
            make
            make install
            make distclean
            ;;
        cygwin.x86_64_static)
            mkdir -p ../wxout/$1
            CPPFLAGS='-w -fpermissive -D__USE_W32_SOCKETS' LDFLAGS='-L /lib/w32api/' ./configure --prefix=$(realpath ../wxout/$1) --disable-shared $SWITCHES --with-msw
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
