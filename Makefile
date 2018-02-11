
WXDIR ?= $(realpath ../../../src/wxWidgets-3.0.3)

WXDEFINE := -D_FILE_OFFSET_BITS=64 -DWXUSINGDLL -D__WXGTK__
WXINCDIR := -isystem ${WXDIR}/include -isystem ${WXDIR}/lib/wx/include/gtk2-unicode-3.0/ -I/usr/include/gtk-unix-print-2.0 -I/usr/include/gtk-2.0 -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/pango-1.0 -I/usr/lib64/gtk-2.0/include -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include -I/usr/include/harfbuzz -I/usr/include/freetype2 -I/usr/include/pixman-1 -I/usr/include/libpng15 -I/usr/include/libdrm
WXLIBDIR := -L${WXDIR}/lib
WXLIBS := -L${WXDIR}/lib -pthread -Wl,-rpath,${WXDIR}/lib -lwx_gtk2u_xrc-3.0 -lwx_gtk2u_html-3.0 -lwx_gtk2u_qa-3.0 -lwx_gtk2u_adv-3.0 -lwx_gtk2u_core-3.0 -lwx_baseu_xml-3.0 -lwx_baseu_net-3.0 -lwx_baseu-3.0

ROOTDIR := $(realpath .)
BINDIR := ${ROOTDIR}/.bin
LIBDIR := ${ROOTDIR}/.bin
OBJDIR := ${ROOTDIR}/.bin

TOOL_AR ?= ar
TOOL_CXX ?= g++
#/opt/gcc-5.5/bin/g++
#/opt/gcc-7.2/bin/g++

COMMON_CPPFLAGS := -std=c++11 -g
COMMON_ARFLAGS := 
COMMON_LDFLAGS := -std=c++11 -g -pthread
# -static-libgcc -static-libstdc++

ifeq ($(BUILD_MODE),release)
COMMON_CPPFLAGS += -O2 -DNDEBUG
COMMON_LDFLAGS += -O2 -DNDEBUG
else
COMMON_CPPFLAGS += -O0 -DDEBUG
COMMON_LDFLAGS += -O0 -DDEBUG
endif

include BUILD/build.mk
include BUILD/VS.mk


########################################
# jjbase
SRCDIR_jjbase := $(realpath .)
SOURCE_jjbase := string.cpp stream.cpp cmdLine.cpp
CPPFLAGS_jjbase := ${COMMON_CPPFLAGS} ${WXDEFINE} -I$(realpath ${SRCDIR_jjbase}/..)
VSNAME_jjbase := jjBase
VSTYPE_jjbase := lib
VSGUID_jjbase := 9D3B6614-DCEB-419C-A035-BE06BF4D7BEF
$(eval $(call define_static_library,jjbase,libs,clean))
define VSINPUT_jjbase
includedirs:
..
defines:
a=x86|WIN32
m=debug|_DEBUG
m=release|NDEBUG
_LIB
endef
VSHEADER_jjbase := $(shell cd "${SRCDIR_jjbase}" && find * -maxdepth 0 -name '*.h' -o -name '*.hpp')
$(eval $(call define_generate_vsproj,jjbase))

########################################
# jjgui
SRCDIR_jjgui := $(realpath gui)
SOURCE_jjgui := common_wx.cpp application_wx.cpp window_wx.cpp menu_wx.cpp sizer_wx.cpp control_wx.cpp button_wx.cpp textLabel_wx.cpp textInput_wx.cpp comboBox_wx.cpp
CPPFLAGS_jjgui := ${COMMON_CPPFLAGS} ${WXDEFINE} -I$(realpath ${SRCDIR_jjgui}/../..) ${WXINCDIR}
VSNAME_jjgui := jjGUI
VSTYPE_jjgui := lib
VSGUID_jjgui := 0F2CE363-F079-4F3D-A745-F1E6212EEFC7
VSREFS_jjgui := jjbase
define VSINPUT_jjgui
includedirs:
..\..
D:\test\wxWidgets\include
m=debug,r=static,a=x64|D:\test\wxWidgets\lib\vc_x64_lib\mswud
m=release,r=static,a=x64|D:\test\wxWidgets\lib\vc_x64_lib\mswu
m=debug,r=dll,a=x64|D:\test\wxWidgets\lib\vc_x64_dll\mswud
m=release,r=dll,a=x64|D:\test\wxWidgets\lib\vc_x64_dll\mswu
m=debug,r=static,a=x86|D:\test\wxWidgets\lib\vc_lib\mswud
m=release,r=static,a=x86|D:\test\wxWidgets\lib\vc_lib\mswu
m=debug,r=dll,a=x86|D:\test\wxWidgets\lib\vc_dll\mswud
m=release,r=dll,a=x86|D:\test\wxWidgets\lib\vc_dll\mswu
defines:
a=x86|WIN32
m=debug|_DEBUG
m=release|NDEBUG
_LIB
r=dll|WXUSINGDLL
endef
VSHEADER_jjgui := $(shell cd "${SRCDIR_jjgui}" && find * -maxdepth 0 -name '*.h' -o -name '*.hpp')
$(eval $(call define_static_library,jjgui,libs,clean))
$(eval $(call define_generate_vsproj,jjgui))

########################################
# jjtest
SRCDIR_jjtest := $(realpath test)
SOURCE_jjtest := test.cpp
CPPFLAGS_jjtest := ${COMMON_CPPFLAGS} ${WXDEFINE} -I$(realpath ${SRCDIR_jjtest}/../..)
VSTYPE_jjtest := lib
VSGUID_jjtest := AFD50C25-67B4-4BA2-B68B-0AB431A322B5
VSREFS_jjtest := jjbase
define VSINPUT_jjtest
includedirs:
..\..
defines:
a=x86|WIN32
m=debug|_DEBUG
m=release|NDEBUG
_LIB
endef
VSHEADER_jjtest := $(shell cd "${SRCDIR_jjtest}" && find * -maxdepth 0 -name '*.h' -o -name '*.hpp')
$(eval $(call define_static_library,jjtest,libs,clean))
$(eval $(call define_generate_vsproj,jjtest))



########################################
# jjbase-tests
SRCDIR_jjbase-tests := $(realpath tests)
SOURCE_jjbase-tests := string_tests.cpp flagSet_tests.cpp options_tests.cpp cmdLine_tests.cpp cmdLineOptions_tests.cpp
CPPFLAGS_jjbase-tests := ${COMMON_CPPFLAGS} ${WXDEFINE} -I$(realpath ${SRCDIR_jjbase-tests}/../..)
LIBS_jjbase-tests := ${RESULT_jjtest} ${RESULT_jjbase}
VSNAME_jjbase-tests := jjbase.Tests
VSTYPE_jjbase-tests := capp
VSGUID_jjbase-tests := 1A5FD8DC-621C-41EE-BC8A-BC327F0E9A38
VSREFS_jjbase-tests := jjtest
VSFOLDER_jjbase-tests := Tests
define VSINPUT_jjbase-tests
includedirs:
..\..
defines:
a=x86|WIN32
m=debug|_DEBUG
m=release|NDEBUG
_CONSOLE
endef
$(eval $(call define_program,jjbase-tests,tests,clean_tests,jjbase jjtest))
$(eval $(call define_generate_vsproj,jjbase-tests))

########################################
# jjtest-tests
SRCDIR_jjtest-tests := $(realpath tests/test)
SOURCE_jjtest-tests := test_tests.cpp filter_tests.cpp
CPPFLAGS_jjtest-tests := ${COMMON_CPPFLAGS} ${WXDEFINE} -I$(realpath ${SRCDIR_jjtest-tests}/../../..)
LIBS_jjtest-tests := ${RESULT_jjtest} ${RESULT_jjbase}
VSNAME_jjtest-tests := jjtest.Tests
VSTYPE_jjtest-tests := capp
VSGUID_jjtest-tests := 07D954B6-05DD-4B85-9BFA-0473B5591768
VSREFS_jjtest-tests := jjtest
VSFOLDER_jjtest-tests := Tests
define VSINPUT_jjtest-tests
includedirs:
..\..\..
defines:
a=x86|WIN32
m=debug|_DEBUG
m=release|NDEBUG
_CONSOLE
endef
$(eval $(call define_program,jjtest-tests,tests,clean_tests,jjbase jjtest))
$(eval $(call define_generate_vsproj,jjtest-tests))



########################################
# TestApp
SRCDIR_TestApp := $(realpath tests)
SOURCE_TestApp := TestAppMain.cpp
LIBS_TestApp := ${RESULT_jjgui} ${RESULT_jjbase} ${WXLIBS}
INCDIR_TestApp := -I$(realpath ${SRCDIR_TestApp}/../..)
VSNAME_TestApp := jjTestApp
VSTYPE_TestApp := gapp
VSGUID_TestApp := 84C36B8F-8BC6-47FF-9027-F8706D3FA72D
VSREFS_TestApp := jjgui
VSFOLDER_TestApp := Tests
define VSINPUT_TestApp
includedirs:
..\..
defines:
a=x86|WIN32
m=debug|_DEBUG
m=release|NDEBUG
_WINDOWS
libraries:
m=debug|wxbase30ud.lib;wxmsw30ud_core.lib;wxmsw30ud_adv.lib;wxpngd.lib;wxzlibd.lib
m=release|wxbase30u.lib;wxmsw30u_core.lib;wxmsw30u_adv.lib;wxpng.lib;wxzlib.lib
comctl32.lib;Rpcrt4.lib
libdirs:
r=static,a=x64|D:\test\wxWidgets\lib\vc_x64_lib
r=dll,a=x64|D:\test\wxWidgets\lib\vc_x64_dll
r=static,a=x86|D:\test\wxWidgets\lib\vc_lib
r=dll,a=x86|D:\test\wxWidgets\lib\vc_dll
endef
$(eval $(call define_program,TestApp,tests,clean_tests,jjbase jjgui))
$(eval $(call define_generate_vsproj,TestApp))

########################################
# test solutions
VSSLN_GUID1_jjTest := 5D226A8D-49CF-4B24-89CB-FD8DBA82C1E5
VSSLN_GUID2_jjTest := 8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942
VSSLN_PROJS_jjTest := TestApp jjbase jjgui jjtest jjbase-tests jjtest-tests
define VSSLN_FOLDERS_jjTest
Tests|586B014B-D960-4C75-AEB6-F1129F25082E
endef
$(eval $(call define_generate_vssln,jjTest,tests))
