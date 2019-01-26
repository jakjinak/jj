WXVERSION ?= 3.0
WXSTATIC ?= 0
ifeq (${WXSTATIC},1)
    WXDIR ?= $(abspath .bin/3rdparty/wxWidgets/${BUILD_OS}.${BUILD_ARCH}_static)
    WXSTEM_DIR_STATIC := -static
    WXSTEM_DEF_SHARED :=
    WXLINKRPATH :=
else
    WXDIR ?= $(abspath .bin/3rdparty/wxWidgets/${BUILD_OS}.${BUILD_ARCH})
    WXSTEM_DIR_STATIC :=
    WXSTEM_DEF_SHARED := -DWXUSINGDLL
    WXLINKRPATH := -Wl,-rpath,${WXDIR}/lib
endif
WXDIR_VS ?= D:\test\wxWidgets

ifeq (${BUILD_OS},cygwin)
    WXSTEM_STR := mswu
    WXSTEM_DIR := msw-unicode
    WXSTEM_DEF := __WXMSW__
    WXSYSINCDIR :=
ifeq (${WXSTATIC},1)
        WXSYSLIBS := -lrpcrt4 -loleaut32 -lole32 -luuid -lwinspool -lwinmm -lshell32 -lcomctl32 -lcomdlg32 -ladvapi32 -lwsock32 -lgdi32 -lkernel32 -luser32
        WXSYSLINKDIR := -L/lib/w32api
else
        WXSYSLIBS :=
        WXSYSLINKDIR :=
endif

else
# linux
    WXSTEM_STR := gtk2u
    WXSTEM_DIR := gtk2-unicode
    WXSTEM_DEF := __WXGTK__
    WXSYSINCDIR := -isystem /usr/include/gtk-unix-print-2.0 -isystem /usr/include/gtk-2.0 -isystem /usr/include/atk-1.0 -isystem /usr/include/cairo -isystem /usr/include/gdk-pixbuf-2.0 -isystem /usr/include/pango-1.0 -isystem /usr/lib64/gtk-2.0/include -isystem /usr/include/glib-2.0 -isystem /usr/lib64/glib-2.0/include -isystem /usr/include/harfbuzz -isystem /usr/include/freetype2 -isystem /usr/include/pixman-1 -isystem /usr/include/libpng15 -isystem /usr/include/libdrm
ifeq (${WXSTATIC},1)
        WXSYSLIBS := -lpangocairo-1.0 -lpango-1.0 -lcairo -ldl -lgtk-x11-2.0 -lgdk_pixbuf-2.0 -lgdk-x11-2.0 -lgobject-2.0 -lglib-2.0 -lX11 -lXxf86vm
        WXSYSLINKDIR :=
else
        WXSYSLIBS :=
        WXSYSLINKDIR :=
endif
endif

ifeq (${WXSTATIC},1)
    WXBASELIBS := -lwxpng-${WXVERSION} -lwxzlib-${WXVERSION}
else
    WXBASELIBS :=
endif

WXDEFINE := -D_FILE_OFFSET_BITS=64 ${WXSTEM_DEF_SHARED} -D${WXSTEM_DEF}
WXSETUPINCDIR := lib/wx/include/${WXSTEM_DIR}${WXSTEM_DIR_STATIC}-${WXVERSION}
WXINCDIR := -isystem ${WXDIR}/include/wx-${WXVERSION} -isystem ${WXDIR}/${WXSETUPINCDIR} ${WXSYSINCDIR}
WXLIBDIR := -L${WXDIR}/lib ${WXLINKRPATH} ${WXSYSLINKDIR}
WXLIBS := -pthread -lwx_${WXSTEM_STR}_xrc-${WXVERSION} -lwx_${WXSTEM_STR}_html-${WXVERSION} -lwx_${WXSTEM_STR}_qa-${WXVERSION} -lwx_${WXSTEM_STR}_adv-${WXVERSION} -lwx_${WXSTEM_STR}_core-${WXVERSION} \
          -lwx_baseu_xml-${WXVERSION} -lwx_baseu_net-${WXVERSION} -lwx_baseu-${WXVERSION} ${WXBASELIBS} ${WXSYSLIBS}
