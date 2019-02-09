ROOTDIR := $(realpath .)
FRAMEWORKDIR := ${ROOTDIR}/BUILD

include ${FRAMEWORKDIR}/detect.mk

include 3rdparty/wxWidgets.mk

CUSTOM_CXXFLAGS := -std=c++11 -g
CUSTOM_LDFLAGS := -std=c++11 -g -pthread

COMPILER_LIB_LINKAGE ?= shared
ifeq (${COMPILER_LIB_LINKAGE},static)
CUSTOM_CXXFLAGS += -static-libgcc -static-libstdc++
CUSTOM_LDFLAGS += -static-libgcc -static-libstdc++
endif

.PHONY: all libs uiall uilibs uitests tests testsbin clean_all clean clean_tests

libs:
tests: testsbin
all: libs tests
uilibs: ${WXDIR}
uitests: tests
uiall: uilibs uitests
clean_all: clean clean_tests

include ${FRAMEWORKDIR}/build.mk
include ${FRAMEWORKDIR}/VS.mk

.PHONY: infoextratargets infoextravariables

infoextratargets:
	@${TOOL_ECHO} "${COLOR_HL}libs${COLOR_0} ... build all static and shared non-UI libraries"
	@${TOOL_ECHO} "${COLOR_HL}uilibs${COLOR_0} ... build all static and shared libraries (UI inclusive)"
	@${TOOL_ECHO} "${COLOR_HL}clean${COLOR_0} ... clean all static and shared libraries (UI inclusive)"
	@${TOOL_ECHO} "${COLOR_HL}tests${COLOR_0} ... build all test programs (non-UI)"
	@${TOOL_ECHO} "${COLOR_HL}uitest${COLOR_0} ... build all test programs (UI inclusive)"
	@${TOOL_ECHO} "${COLOR_HL}clean_tests${COLOR_0} ... clean all test programs (UI inclusive)"
	@${TOOL_ECHO} "${COLOR_HL}all${COLOR_0} ... build all libs and programs (non-UI)"
	@${TOOL_ECHO} "${COLOR_HL}uiall${COLOR_0} ... build all libs and programs (UI inclusive)"
	@${TOOL_ECHO} "${COLOR_HL}clean_all${COLOR_0} ... clean all libs and programs (UI inclusive)"
	@${TOOL_ECHO} "${COLOR_HL}package${COLOR_0} ... create a .tar.xz containing the built libs and headers (UI inclusive)"
infotargets: infoextratargets

VARNAMES_GLOBAL += COMPILER_LIB_LINKAGE WXSTATIC RELEASE_PACKAGE_ADD_GUI

infoextravariables:
	@${TOOL_ECHO} "Additional variables:"
	@${TOOL_ECHO} "${COLOR_HL}COMPILER_LIB_LINKAGE${COLOR_0} ... defines whether the libgcc and libstdc++ are linked dynamically or statically"
	@${TOOL_ECHO} "         possible values: static or shared [${COLOR_HL}${COMPILER_LIB_LINKAGE}${COLOR_0}]"
	@${TOOL_ECHO} "${COLOR_HL}WXSTATIC${COLOR_0} ... defines whether wxWidgets are linked dynamically or statically, enabled if set to 1 [${COLOR_HL}${WXSTATIC}${COLOR_0}]"
	@${TOOL_ECHO} "${COLOR_HL}RELEASE_PACKAGE_ADD_GUI${COLOR_0} ... set to 1 (default) if target ${COLOR_HL}package${COLOR_0} shall add jjgui into the archive [${COLOR_HL}${RELEASE_PACKAGE_ADD_GUI}${COLOR_0}]"
infovariables: infoextravariables

########################################
# jjbase
SRCDIR_jjbase := $(realpath .)
SOURCE_jjbase := $(call locate_files,${SRCDIR_jjbase},*.cpp,1)
CXXFLAGS_jjbase := ${COMMON_CXXFLAGS} -I$(realpath ${SRCDIR_jjbase}/..)
SO_CXXFLAGS_jjbase := ${SO_COMMON_CXXFLAGS} -I$(realpath ${SRCDIR_jjbase}/..)
VSNAME_jjbase := jjBase
VSTYPE_jjbase := lib
VSGUID_jjbase := 9D3B6614-DCEB-419C-A035-BE06BF4D7BEF
$(eval $(call define_static_and_shared_library,jjbase,libs,clean))
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
SOURCE_jjgui := $(call locate_files,${SRCDIR_jjgui},*.cpp)
CXXFLAGS_jjgui := ${COMMON_CXXFLAGS} ${WXDEFINE} -I$(realpath ${SRCDIR_jjgui}/../..) ${WXINCDIR}
VSNAME_jjgui := jjGUI
VSTYPE_jjgui := lib
VSGUID_jjgui := 0F2CE363-F079-4F3D-A745-F1E6212EEFC7
VSREFS_jjgui := jjbase
VSINCDIRS_jjgui := ..\.. \
	${WXDIR_VS}\include \
	m=debug,r=static,a=x64|${WXDIR_VS}\lib\vc_x64_lib\mswud \
	m=release,r=static,a=x64|${WXDIR_VS}\lib\vc_x64_lib\mswu \
	m=debug,r=dll,a=x64|${WXDIR_VS}\lib\vc_x64_dll\mswud \
	m=release,r=dll,a=x64|${WXDIR_VS}\lib\vc_x64_dll\mswu \
	m=debug,r=static,a=x86|${WXDIR_VS}\lib\vc_lib\mswud \
	m=release,r=static,a=x86|${WXDIR_VS}\lib\vc_lib\mswu \
	m=debug,r=dll,a=x86|${WXDIR_VS}\lib\vc_dll\mswud \
	m=release,r=dll,a=x86|${WXDIR_VS}\lib\vc_dll\mswu
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
CXXFLAGS_jjtest := ${COMMON_CXXFLAGS} -I$(realpath ${SRCDIR_jjtest}/../..)
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
SOURCE_jjbase-tests := $(filter-out TestAppMain.cpp,$(call locate_files,${SRCDIR_jjbase-tests},*.cpp,1))
CXXFLAGS_jjbase-tests := ${COMMON_CXXFLAGS} -I$(realpath ${SRCDIR_jjbase-tests}/../..)
LIBS_jjbase-tests := ${RESULT_jjtest} ${SO_RESULT_jjbase}
VSNAME_jjbase-tests := jjbase-tests
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
$(eval $(call define_program,jjbase-tests,testsbin,clean_tests,jjbase_so jjtest))
$(eval $(call define_testrun,jjbase-tests,tests,jjbase-tests))
$(eval $(call define_generate_vsproj,jjbase-tests))

########################################
# jjtest-tests
SRCDIR_jjtest-tests := $(realpath tests/test)
SOURCE_jjtest-tests := test_tests.cpp filter_tests.cpp
CXXFLAGS_jjtest-tests := ${COMMON_CXXFLAGS} -I$(realpath ${SRCDIR_jjtest-tests}/../../..)
LIBS_jjtest-tests := ${RESULT_jjtest} ${RESULT_jjbase}
VSNAME_jjtest-tests := jjtest-tests
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
$(eval $(call define_program,jjtest-tests,testsbin,clean_tests,jjbase jjtest))
TESTRUN_NAME_jjtest-tests := tests/test/runner.sh
TESTRUN_PARS_jjtest-tests := ${RESULT_jjtest-tests}${PLATFORM_BINARY_SUFFIX}
TESTRUN_VARS_jjtest-tests := VERBOSITY_commands=$(VERBOSITY_commands) VERBOSITY_colors=$(VERBOSITY_colors) VERBOSITY_tests=$(VERBOSITY_tests)
$(eval $(call define_testrun,jjtest-tests,tests,jjtest-tests))
TESTRUN_NAME_jjtest-tests2 := ${RESULT_jjtest-tests}${PLATFORM_BINARY_SUFFIX}
TESTRUN_PARS_jjtest-tests2 := +t filterTests_t/
$(eval $(call define_testrun,jjtest-tests2,tests,jjtest-tests))
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
	r=static,a=x64|${WXDIR_VS}\lib\vc_x64_lib \
	r=dll,a=x64|${WXDIR_VS}\lib\vc_x64_dll \
	r=static,a=x86|${WXDIR_VS}\lib\vc_lib \
	r=dll,a=x86|${WXDIR_VS}\lib\vc_dll
$(eval $(call define_program,TestApp,uitests,clean_tests,jjbase jjgui))
$(eval $(call define_generate_vsproj,TestApp))

########################################
# test solutions
VSSLN_GUID1_jj := 5D226A8D-49CF-4B24-89CB-FD8DBA82C1E5
VSSLN_GUID2_jj := 8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942
VSSLN_PROJS_jj := TestApp jjbase jjgui jjtest jjbase-tests jjtest-tests
VSSLN_FOLDERS_jj := Tests
VSSLN_FOLDERDEFS_NAME_jj_Tests := Tests
VSSLN_FOLDERDEFS_GUID_jj_Tests := 586B014B-D960-4C75-AEB6-F1129F25082E
$(eval $(call define_generate_vssln,jj,.))


########################################
# release package creation
RELEASE_VERSION := 0.1.1
RELEASE_PACKAGE := ${BINDIR}/jj_${BUILD_MODE}-${BUILD_OS}.${BUILD_ARCH}_${RELEASE_VERSION}.tar.gz

.PHONY: package clean_package

package: ${RELEASE_PACKAGE}

RELEASE_PACKAGE_ADD_GUI ?= 1

ifneq (${BUILD_OS},windows)
STUFF_TO_PACKAGE := ${RESULT_jjbase} ${SO_RESULT_jjbase} ${RESULT_jjtest}
ifeq (${RELEASE_PACKAGE_ADD_GUI},1)
STUFF_TO_PACKAGE += ${RESULT_jjgui}
endif
else
STUFF_TO_PACKAGE := ${BINDIR}/jjBase.lib ${BINDIR}/jjtest.lib
ifeq (${RELEASE_PACKAGE_ADD_GUI},1)
STUFF_TO_PACKAGE += ${BINDIR}/jjGUI.lib
endif
endif

${RELEASE_PACKAGE} : ${STUFF_TO_PACKAGE} LICENSE
	$(call showlabel,"${COLOR_INFO}=== Creating package ${COLOR_HL}$@${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)${TOOL_RMR} ${TMPDIR}/package ${RELEASE_PACKAGE}
	$(COMMAND_HIDE_PREFIX)${TOOL_MKDIR} ${TMPDIR}/package/lib
	$(COMMAND_HIDE_PREFIX)${TOOL_CP} ${STUFF_TO_PACKAGE} ${TMPDIR}/package/lib
	$(COMMAND_HIDE_PREFIX)${TOOL_CP} LICENSE ${TMPDIR}/package
	$(COMMAND_HIDE_PREFIX)${TOOL_MKDIR} ${TMPDIR}/package/include
	$(COMMAND_HIDE_PREFIX)${TOOL_CP} --parents $$(${TOOL_FIND} . -maxdepth 1 -name '*.h' ) ${TMPDIR}/package/include
	$(COMMAND_HIDE_PREFIX)[ "${RELEASE_PACKAGE_ADD_GUI}" != 1 ] || ${TOOL_CP} --parents $$(${TOOL_FIND} gui -maxdepth 1 -name '*.h' -a \! -name '*wx*' ) ${TMPDIR}/package/include
	$(call ARCHIVE_TAR_XZ,${RELEASE_PACKAGE},${TMPDIR}/package,*)
	$(COMMAND_HIDE_PREFIX)${TOOL_RMR} ${TMPDIR}/package
	$(call showlabel,"${COLOR_INFO}=== Created ${COLOR_HL}$(notdir $@)${COLOR_0}${COLOR_INFO} in ${COLOR_HL}$(dir $@)${COLOR_0}")

clean_package:
	$(call showlabel,"${COLOR_CLEAN}=== Cleaning package ${COLOR_HL}${RELEASE_PACKAGE}${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)${TOOL_RMR} ${TMPDIR}/package ${RELEASE_PACKAGE}
