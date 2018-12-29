
include BUILD/detect.mk

WXSTATIC ?= 0
ifeq (${WXSTATIC},1)
WXDIR ?= $(abspath .bin/3rdparty/wxWidgets/${BUILD_OS}.${BUILD_ARCH}_static)
else
WXDIR ?= $(abspath .bin/3rdparty/wxWidgets/${BUILD_OS}.${BUILD_ARCH})
endif

ifeq (${WXSTATIC},1)
WXDEFINE := -D_FILE_OFFSET_BITS=64 -D__WXGTK__
WXSETUPINCDIR := lib/wx/include/gtk2-unicode-static-3.0
WXLIBDIR := -L${WXDIR}/lib
WXEXTRALIBS := -lwxpng-3.0 -lwxzlib-3.0 -lpangocairo-1.0 -lpango-1.0 -lcairo -ldl -lgtk-x11-2.0 -lgdk_pixbuf-2.0 -lgdk-x11-2.0 -lgobject-2.0 -lglib-2.0 -lX11 -lXxf86vm
else
WXDEFINE := -D_FILE_OFFSET_BITS=64 -DWXUSINGDLL -D__WXGTK__
WXSETUPINCDIR := lib/wx/include/gtk2-unicode-3.0
WXLIBDIR := -L${WXDIR}/lib -Wl,-rpath,${WXDIR}/lib
WXEXTRALIBS :=
endif
WXINCDIR := -isystem ${WXDIR}/include/wx-3.0 -isystem ${WXDIR}/${WXSETUPINCDIR} -I/usr/include/gtk-unix-print-2.0 -I/usr/include/gtk-2.0 -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/pango-1.0 -I/usr/lib64/gtk-2.0/include -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include -I/usr/include/harfbuzz -I/usr/include/freetype2 -I/usr/include/pixman-1 -I/usr/include/libpng15 -I/usr/include/libdrm
WXLIBS := -pthread -lwx_gtk2u_xrc-3.0 -lwx_gtk2u_html-3.0 -lwx_gtk2u_qa-3.0 -lwx_gtk2u_adv-3.0 -lwx_gtk2u_core-3.0 -lwx_baseu_xml-3.0 -lwx_baseu_net-3.0 -lwx_baseu-3.0 ${WXEXTRALIBS}

ROOTDIR := $(realpath .)

CUSTOM_CXXFLAGS := -std=c++11 -g
CUSTOM_LDFLAGS := -std=c++11 -g -pthread
# -static-libgcc -static-libstdc++

.PHONY: all libs uiall uilibs uitests tests clean_all clean clean_tests

libs:
all: libs tests
uilibs: ${WXDIR}
uitests: tests
uiall: uilibs uitests
clean_all: clean clean_tests

include BUILD/build.mk
include BUILD/VS.mk


########################################
# jjbase
SRCDIR_jjbase := $(realpath .)
SOURCE_jjbase := string.cpp stringLiterals.cpp stream.cpp cmdLine.cpp configurationStorage.cpp
CXXFLAGS_jjbase := ${COMMON_CXXFLAGS} -I$(realpath ${SRCDIR_jjbase}/..)
VSNAME_jjbase := jjBase
VSTYPE_jjbase := lib
VSGUID_jjbase := 9D3B6614-DCEB-419C-A035-BE06BF4D7BEF
$(eval $(call define_static_library,jjbase,libs,clean))
VSINCDIRS_jjbase := ..
VSDEFINES_jjbase := \
	a=x86|WIN32 \
	m=debug|_DEBUG \
	m=release|NDEBUG \
	_LIB
VSHEADER_jjbase := $(shell cd "${SRCDIR_jjbase}" && find * -maxdepth 0 -name '*.h' -o -name '*.hpp')
$(eval $(call define_generate_vsproj,jjbase))

########################################
# jjgui
SRCDIR_jjgui := $(realpath gui)
SOURCE_jjgui := common_wx.cpp application_wx.cpp window_wx.cpp menu_wx.cpp sizer_wx.cpp control_wx.cpp button_wx.cpp textLabel_wx.cpp textInput_wx.cpp comboBox_wx.cpp
CXXFLAGS_jjgui := ${COMMON_CXXFLAGS} ${WXDEFINE} -I$(realpath ${SRCDIR_jjgui}/../..) ${WXINCDIR}
VSNAME_jjgui := jjGUI
VSTYPE_jjgui := lib
VSGUID_jjgui := 0F2CE363-F079-4F3D-A745-F1E6212EEFC7
VSREFS_jjgui := jjbase
VSINCDIRS_jjgui := ..\.. \
	D:\test\wxWidgets\include \
	m=debug,r=static,a=x64|D:\test\wxWidgets\lib\vc_x64_lib\mswud \
	m=release,r=static,a=x64|D:\test\wxWidgets\lib\vc_x64_lib\mswu \
	m=debug,r=dll,a=x64|D:\test\wxWidgets\lib\vc_x64_dll\mswud \
	m=release,r=dll,a=x64|D:\test\wxWidgets\lib\vc_x64_dll\mswu \
	m=debug,r=static,a=x86|D:\test\wxWidgets\lib\vc_lib\mswud \
	m=release,r=static,a=x86|D:\test\wxWidgets\lib\vc_lib\mswu \
	m=debug,r=dll,a=x86|D:\test\wxWidgets\lib\vc_dll\mswud \
	m=release,r=dll,a=x86|D:\test\wxWidgets\lib\vc_dll\mswu
VSDEFINES_jjgui := \
	a=x86|WIN32 \
	m=debug|_DEBUG \
	m=release|NDEBUG \
	_LIB \
	r=dll|WXUSINGDLL
VSHEADER_jjgui := $(shell cd "${SRCDIR_jjgui}" && find * -maxdepth 0 -name '*.h' -o -name '*.hpp')
$(eval $(call define_static_library,jjgui,uilibs,clean))
$(eval $(call define_generate_vsproj,jjgui))

########################################
# jjtest
SRCDIR_jjtest := $(realpath test)
SOURCE_jjtest := test.cpp
CXXFLAGS_jjtest := ${COMMON_CXXFLAGS} ${WXDEFINE} -I$(realpath ${SRCDIR_jjtest}/../..)
VSTYPE_jjtest := lib
VSGUID_jjtest := AFD50C25-67B4-4BA2-B68B-0AB431A322B5
VSREFS_jjtest := jjbase
VSINCDIRS_jjtest := ..\..
VSDEFINES_jjtest := \
	a=x86|WIN32 \
	m=debug|_DEBUG \
	m=release|NDEBUG \
	_LIB
VSHEADER_jjtest := $(shell cd "${SRCDIR_jjtest}" && find * -maxdepth 0 -name '*.h' -o -name '*.hpp')
$(eval $(call define_static_library,jjtest,libs,clean))
$(eval $(call define_generate_vsproj,jjtest))



########################################
# jjbase-tests
SRCDIR_jjbase-tests := $(realpath tests)
SOURCE_jjbase-tests := string_tests.cpp flagSet_tests.cpp options_tests.cpp cmdLine_tests.cpp cmdLineOptions_tests.cpp configuration_tests.cpp
CXXFLAGS_jjbase-tests := ${COMMON_CXXFLAGS} ${WXDEFINE} -I$(realpath ${SRCDIR_jjbase-tests}/../..)
LIBS_jjbase-tests := ${RESULT_jjtest} ${RESULT_jjbase}
VSNAME_jjbase-tests := jjbase.Tests
VSTYPE_jjbase-tests := capp
VSGUID_jjbase-tests := 1A5FD8DC-621C-41EE-BC8A-BC327F0E9A38
VSREFS_jjbase-tests := jjtest
VSFOLDER_jjbase-tests := Tests
VSINCDIRS_jjbase-tests := ..\..
VSDEFINES_jjbase-tests := \
	a=x86|WIN32 \
	m=debug|_DEBUG \
	m=release|NDEBUG \
	_CONSOLE
VSHEADER_jjbase-tests := $(shell cd "${SRCDIR_jjbase-tests}" && find * -maxdepth 0 -name '*.h' -o -name '*.hpp')
$(eval $(call define_program,jjbase-tests,tests,clean_tests,jjbase jjtest))
$(eval $(call define_generate_vsproj,jjbase-tests))

########################################
# jjtest-tests
SRCDIR_jjtest-tests := $(realpath tests/test)
SOURCE_jjtest-tests := test_tests.cpp filter_tests.cpp
CXXFLAGS_jjtest-tests := ${COMMON_CXXFLAGS} ${WXDEFINE} -I$(realpath ${SRCDIR_jjtest-tests}/../../..)
LIBS_jjtest-tests := ${RESULT_jjtest} ${RESULT_jjbase}
VSNAME_jjtest-tests := jjtest.Tests
VSTYPE_jjtest-tests := capp
VSGUID_jjtest-tests := 07D954B6-05DD-4B85-9BFA-0473B5591768
VSREFS_jjtest-tests := jjtest
VSFOLDER_jjtest-tests := Tests
VSINCDIRS_jjtest-tests := ..\..\..
VSDEFINES_jjtest-tests := \
	a=x86|WIN32 \
	m=debug|_DEBUG \
	m=release|NDEBUG \
	_CONSOLE
$(eval $(call define_program,jjtest-tests,tests,clean_tests,jjbase jjtest))
$(eval $(call define_generate_vsproj,jjtest-tests))



########################################
# TestApp
SRCDIR_TestApp := $(realpath tests)
SOURCE_TestApp := TestAppMain.cpp
LIBS_TestApp := ${RESULT_jjgui} ${RESULT_jjbase} ${WXLIBDIR} ${WXLIBS}
INCDIR_TestApp := -I$(realpath ${SRCDIR_TestApp}/../..)
VSNAME_TestApp := jjTestApp
VSTYPE_TestApp := gapp
VSGUID_TestApp := 84C36B8F-8BC6-47FF-9027-F8706D3FA72D
VSREFS_TestApp := jjgui
VSFOLDER_TestApp := Tests
VSINCDIRS_TestApp := ..\..
VSDEFINES_TestApp := \
	a=x86|WIN32 \
	m=debug|_DEBUG \
	m=release|NDEBUG \
	_WINDOWS
VSLIBS_TestApp := \
	m=debug|wxbase30ud.lib;wxmsw30ud_core.lib;wxmsw30ud_adv.lib;wxpngd.lib;wxzlibd.lib \
	m=release|wxbase30u.lib;wxmsw30u_core.lib;wxmsw30u_adv.lib;wxpng.lib;wxzlib.lib \
	comctl32.lib;Rpcrt4.lib
VSLIBDIRS_TestApp := \
	r=static,a=x64|D:\test\wxWidgets\lib\vc_x64_lib \
	r=dll,a=x64|D:\test\wxWidgets\lib\vc_x64_dll \
	r=static,a=x86|D:\test\wxWidgets\lib\vc_lib \
	r=dll,a=x86|D:\test\wxWidgets\lib\vc_dll
$(eval $(call define_program,TestApp,uitests,clean_tests,jjbase jjgui))
$(eval $(call define_generate_vsproj,TestApp))

########################################
# test solutions
VSSLN_GUID1_jjTest := 5D226A8D-49CF-4B24-89CB-FD8DBA82C1E5
VSSLN_GUID2_jjTest := 8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942
VSSLN_PROJS_jjTest := TestApp jjbase jjgui jjtest jjbase-tests jjtest-tests
VSSLN_FOLDERS_jjTest := Tests
VSSLN_FOLDERDEFS_NAME_jjTest_Tests := Tests
VSSLN_FOLDERDEFS_GUID_jjTest_Tests := 586B014B-D960-4C75-AEB6-F1129F25082E
$(eval $(call define_generate_vssln,jjTest,tests))
