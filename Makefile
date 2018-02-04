
BUILD_MODE ?= debug
BUILD_ARCH ?= x86_64

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

.PHONY: all libs tests clean_all clean clean_tests

libs:
all: libs tests
clean_all: clean clean_tests

########################################
# defines

#$(call showhint,"hint")
ifeq ($(SHOW_HINTS),1)
define showhint
	@echo -e $(1)
endef
else
define showhint
endef
endif

ifeq ($(COLOR_HINTS),1)
COLOR_HL ?= \\e[1m
COLOR_0 ?= \\e[0m
COLOR_SUPPORT ?= \\e[34m
COLOR_COMPILE ?= \\e[32m
COLOR_STATLIB ?= \\e[36m
COLOR_PROGRAM ?= \\e[36m
COLOR_CLEAN ?= \\e[31m
COLOR_INFO ?= \\e[1;33m
else
COLOR_HL :=
COLOR_0 :=
COLOR_SUPPORT :=
COLOR_COMPILE :=
COLOR_STATLIB :=
COLOR_PROGRAM :=
COLOR_CLEAN :=
COLOR_INFO :=
endif

ifeq ($(HIDE_COMMANDS),1)
COMMAND_HIDE_PREFIX := @
else
COMMAND_HIDE_PREFIX :=
endif


# Defines all the undefined definitions and rules that are needed in the macros below.
# The first parameter is the name of the library and also the suffix of all defines,
# the second parameter is the parent make rule, third is the parent clean rule.
# The fourth parameter provides additional targets that the make rule depends on.
# Assumes SRCDIR_<name>, SOURCE_<name> and RESULT_<name> to be defined.
# Awaits OBJDIR_<name>, DEPDIR_<name> to be optionally defined, defines them if not found.
# Assumes any <*>FLAGS_<name> (where <*> stands for CPP, LD, AR,...) to be defined where
# necessary; if not defined these will be defined to COMMON_<*>FLAGS, except that
# undefined CPPFLAGS_<name> will be defined to COMMON_CPPFLAGS and INCDIR_<name>,
# undefined LDFLAGS_<name> to COMMON_LDFLAGS and LIBS_<name>
# Defines SRC_<name>, OBJ_<name> and DEP_<name> and all the rules (except the final one that produces the result)
#(call define_common_part,name,makerule,cleanrule,adddeps)
define define_common_part
OBJDIR_$(1) ?= ${OBJDIR}/${BUILD_MODE}.${BUILD_ARCH}/obj/$(1)
DEPDIR_$(1) ?= ${OBJDIR}/${BUILD_MODE}.${BUILD_ARCH}/dep/$(1)

SRC_$(1) := $$(addprefix $${SRCDIR_$(1)}/,$${SOURCE_$(1)})
OBJ_$(1) := $$(addprefix $${OBJDIR_$(1)}/,$$(SOURCE_$(1):.cpp=.o))
DEP_$(1) := $$(addprefix $${DEPDIR_$(1)}/,$$(SOURCE_$(1):.cpp=.d))

CPPFLAGS_$(1) ?= ${COMMON_CPPFLAGS} $${INCDIR_$(1)}
LDFLAGS_$(1) ?= ${COMMON_LDFLAGS} $${LIBS_$(1)}
ARFLAGS_$(1) ?= ${COMMON_ARFLAGS}

-include $${DEP_$(1)}

.PHONY: $(1) $(1)_only clean_$(1) clean_$(1)_only info_$(1)

$(2): $(1)
$(3): clean_$(1)
$(1): $${RESULT_$(1)}
$${RESULT_$(1)}: $(addsuffix },$(addprefix $${RESULT_,$(4)))
clean_$(1): clean_$(1)_only $(addprefix clean_,$(4))

clean_$(1)_only:
	$$(call showhint,"$${COLOR_CLEAN}=== Clean all for $${COLOR_HL}$(1)$${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)rm -f $${RESULT_$(1)} $${OBJ_$(1)} $${DEP_$(1)}

info_$(1):
	@echo -e "SRCDIR_$(1) = [$${COLOR_INFO}$$(SRCDIR_$(1))$${COLOR_0}]"
	@echo -e "SOURCE_$(1) = [$${COLOR_INFO}$$(SOURCE_$(1))$${COLOR_0}]"
	@echo -e "CPPFLAGS_$(1) = [$${COLOR_INFO}$$(CPPFLAGS_$(1))$${COLOR_0}]"
	@echo -e "ARFLAGS_$(1) = [$${COLOR_INFO}$$(ARFLAGS_$(1))$${COLOR_0}]"
	@echo -e "LDFLAGS_$(1) = [$${COLOR_INFO}$$(LDFLAGS_$(1))$${COLOR_0}]"
	@echo -e "OBJDIR_$(1) = [$${COLOR_INFO}$$(OBJDIR_$(1))$${COLOR_0}]"
	@echo -e "DEPDIR_$(1) = [$${COLOR_INFO}$$(DEPDIR_$(1))$${COLOR_0}]"
	@echo -e "SRC_$(1) = [$${COLOR_INFO}$$(SRC_$(1))$${COLOR_0}]"
	@echo -e "OBJ_$(1) = [$${COLOR_INFO}$$(OBJ_$(1))$${COLOR_0}]"
	@echo -e "DEP_$(1) = [$${COLOR_INFO}$$(DEP_$(1))$${COLOR_0}]"
	@echo
	@echo -e "Defines these rules: [$${COLOR_INFO}$(1) $(1)_only$${COLOR_0}] [$${COLOR_INFO}clean_$(1) clean_$(1)_only$${COLOR_0}] [$${COLOR_INFO}info_$(1)$${COLOR_0}]"
	@echo -e "Depends on these rules: [$${COLOR_INFO}$(4)$${COLOR_0}]"
	@echo -e "Is part of these rules: [$${COLOR_INFO}$(2)$${COLOR_0}] [$${COLOR_INFO}$(3)$${COLOR_0}]"

$${DEPDIR_$(1)}/dircreated:
	$$(call showhint,"$${COLOR_SUPPORT}=== Creating directory $${COLOR_HL}$${DEPDIR_$(1)}$${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)mkdir -p $${DEPDIR_$(1)}
	$(COMMAND_HIDE_PREFIX)touch $${DEPDIR_$(1)}/dircreated

$${OBJDIR_$(1)}/dircreated:
	$$(call showhint,"$${COLOR_SUPPORT}=== Creating directory $${COLOR_HL}$${OBJDIR_$(1)}$${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)mkdir -p $${OBJDIR_$(1)}
	$(COMMAND_HIDE_PREFIX)touch $${OBJDIR_$(1)}/dircreated

$${DEPDIR_$(1)}/%.d : ;

$${OBJDIR_$(1)}/%.o : $${SRCDIR_$(1)}/%.cpp $${OBJDIR_$(1)}/dircreated $${DEPDIR_$(1)}/dircreated
	$$(call showhint,"$${COLOR_COMPILE}=== Compiling $${COLOR_HL}$$(subst $$(ROOTDIR)/,,$$<)$${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)${TOOL_CXX} $${CPPFLAGS_$(1)} -MMD -MT $$@ -MF $${DEPDIR_$(1)}/$$(notdir $$(@:.o=.d)) -c -o $$@ $$<
endef

# Defines all the undefined definitions and rules for a library unless already defined
# The first parameter is the name of the library and also the suffix of all defines,
# the second parameter is the parent make rule, third is the parent clean rule.
# The fourth parameter provides additional targets that the make rule depends on.
# Assumes SRCDIR_<name> and SOURCE_<name> to be defined.
# Awaits RESULT_<name>, OBJDIR_<name>, DEPDIR_<name> to be optionally defined, defines if not found
# Defines SRC_<name>, OBJ_<name> and DEP_<name> and all the rules
# See define_common_part for descriptions on the <*>FLAGS_<name>.
#(call define_static_library,name,makerule,cleanrule,adddeps)
define define_static_library
RESULT_$(1) ?= ${LIBDIR}/${BUILD_MODE}.${BUILD_ARCH}/$(1).a

$(call define_common_part,$(1),$(2),$(3),$(4))

$${RESULT_$(1)}: $${OBJ_$(1)}
	$$(call showhint,"$${COLOR_STATLIB}=== Creating static library $${COLOR_HL}$$(subst $$(ROOTDIR)/,,$${RESULT_$(1)})$${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)${TOOL_AR} cr $${ARFLAGS_$(1)} $${RESULT_$(1)} $${OBJ_$(1)}
$(1)_only: $${OBJ_$(1)}
	$$(call showhint,"$${COLOR_STATLIB}=== Creating static library $${COLOR_HL}$$(subst $$(ROOTDIR)/,,$${RESULT_$(1)})$${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)${TOOL_AR} cr $${ARFLAGS_$(1)} $${RESULT_$(1)} $${OBJ_$(1)}
endef

# Defines all the undefined definitions and rules for a program unless already defined
# The first parameter is the name of the program and also the suffix of all defines,
# the second parameter is the parent make rule, third is the parent clean rule.
# The fourth parameter provides additional targets that the make rule depends on.
# Assumes SRCDIR_<name> and SOURCE_<name> to be defined.
# Awaits RESULT_<name>, OBJDIR_<name>, DEPDIR_<name> to be optionally defined, defines if not found
# Defines SRC_<name>, OBJ_<name> and DEP_<name> and all the rules
# See define_common_part for descriptions on the <*>FLAGS_<name>.
#(call define_program,name,makerule,cleanrule,adddeps)
define define_program
RESULT_$(1) ?= ${BINDIR}/${BUILD_MODE}.${BUILD_ARCH}/$(1)

$(call define_common_part,$(1),$(2),$(3),$(4))

$${RESULT_$(1)}: $${OBJ_$(1)}
	$$(call showhint, "$${COLOR_PROGRAM}=== Linking program $${COLOR_HL}$$(subst $$(ROOTDIR)/,,$${RESULT_$(1)})$${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)${TOOL_CXX} $${OBJ_$(1)} $${LDFLAGS_$(1)} -o $${RESULT_$(1)}
$(1)_only: $${OBJ_$(1)}
	$$(call showhint, "$${COLOR_PROGRAM}=== Linking program $${COLOR_HL}$$(subst $$(ROOTDIR)/,,$${RESULT_$(1)})$${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)${TOOL_CXX} $${OBJ_$(1)} $${LDFLAGS_$(1)} -o $${RESULT_$(1)}
endef

#(call define_generate_vssln,name,dir)
define define_generate_vssln
vssln_$(1):
	{ $(foreach proj,${VSSLN_PROJS_$(1)},echo project=$${VSGUID_$(proj)} ; echo path='$${SRCDIR_$(proj)}/$${VSNAME_$(proj)}.vcxproj' ;) } | \
	tools/generate_vc.pl sln $${VSSLN_GUID1_$(1)} "$(realpath $(2))/$(1).sln" $${VSSLN_GUID2_$(1)} > "$(realpath $(2))/$(1).sln"

.PHONY: vssln_$(1)
endef

#(call define_generate_vsproj,name)
define define_generate_vsproj
VSNAME_$(1) ?= $(1)

.ONESHELL: vsproj_$(1)

vsproj_$(1):
	{
		echo '$${VSINPUT_$(1)}'
		echo 'sources:'
		for sf in $${SOURCE_$(1)}
		do
			echo "$$$$sf"
		done
		echo 'includes:'
		for sf in $${VSHEADER_$(1)}
		do
			echo "$$$$sf"
		done
		echo 'references:'
		$(foreach proj,${VSREFS_$(1)},echo $${VSGUID_$(proj)} '$${SRCDIR_$(proj)}/$${VSNAME_$(proj)}.vcxproj' ;)
	} | tools/generate_vc.pl vcproj $${VSGUID_$(1)} "$$(realpath $${SRCDIR_$(1)})/$${VSNAME_$(1)}.vcxproj" $${VSTYPE_$(1)} > "$$(realpath $${SRCDIR_$(1)})/$${VSNAME_$(1)}.vcxproj"
	{
		echo 'sources:'
		for sf in $${SOURCE_$(1)}
		do
			echo "$$$$sf"
		done
		echo 'includes:'
		for sf in $${VSHEADER_$(1)}
		do
			echo "$$$$sf"
		done
	} | tools/generate_vc.pl vcfilter $${VSGUID_$(1)} "$$(realpath $${SRCDIR_$(1)})/$${VSNAME_$(1)}.vcxproj.filters" > "$$(realpath $${SRCDIR_$(1)})/$${VSNAME_$(1)}.vcxproj.filters"

.PHONY: vsproj_$(1)
endef

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
properties:
= BUILD\jj
includedirs:
$$(jjIncDir)
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
properties:
= ..\BUILD\jj
* ..\BUILD\wx
includedirs:
$$(jjIncDir2);$$(wxIncludePath)
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
properties:
= ..\BUILD\jj
includedirs:
$$(jjIncDir2)
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
SOURCE_jjbase-tests := string_tests.cpp cmdLine_tests.cpp cmdLineOptions_tests.cpp
CPPFLAGS_jjbase-tests := ${COMMON_CPPFLAGS} ${WXDEFINE} -I$(realpath ${SRCDIR_jjbase-tests}/../..)
LIBS_jjbase-tests := ${RESULT_jjtest} ${RESULT_jjbase}
VSNAME_jjbase-tests := jjbase.Tests
VSTYPE_jjbase-tests := capp
VSGUID_jjbase-tests := 1A5FD8DC-621C-41EE-BC8A-BC327F0E9A38
VSREFS_jjbase-tests := jjtest
define VSINPUT_jjbase-tests
properties:
= ..\BUILD\jj
includedirs:
$$(jjIncDir2)
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
define VSINPUT_jjtest-tests
properties:
= ..\..\BUILD\jj
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
define VSINPUT_TestApp
properties:
= ..\BUILD\jj
* ..\BUILD\wx
includedirs:
$$(jjIncDir2)
defines:
a=x86|WIN32
m=debug|_DEBUG
m=release|NDEBUG
_WINDOWS
libraries:
$$(wxBaseLibs)
libdirs:
$$(wxLibPath)
endef
$(eval $(call define_program,TestApp,tests,clean_tests,jjbase jjgui))
$(eval $(call define_generate_vsproj,TestApp))

########################################
# test solutions
VSSLN_GUID1_jjTest := 5D226A8D-49CF-4B24-89CB-FD8DBA82C1E5
VSSLN_GUID2_jjTest := 8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942
VSSLN_PROJS_jjTest := TestApp jjbase jjgui jjtest jjbase-tests jjtest-tests
$(eval $(call define_generate_vssln,jjTest,tests))
