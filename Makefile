
BUILD_MODE ?= debug
BUILD_ARCH ?= x86_64

WXDIR := $(realpath ../../../wxwidgets/wxWidgets-3.0.3)

WXDEFINE := -D_FILE_OFFSET_BITS=64 -DWXUSINGDLL -D__WXGTK__
WXINCDIR := -isystem ${WXDIR}/include -isystem ${WXDIR}/lib/wx/include/gtk2-unicode-3.0/ -I/usr/include/gtk-unix-print-2.0 -I/usr/include/gtk-2.0 -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/pango-1.0 -I/usr/lib64/gtk-2.0/include -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include -I/usr/include/harfbuzz -I/usr/include/freetype2 -I/usr/include/pixman-1 -I/usr/include/libpng15 -I/usr/include/libdrm
WXLIBDIR := -L${WXDIR}/lib
WXLIBS := -L/home/jakjinak/wxwidgets/wxWidgets-3.0.3/lib -pthread -Wl,-rpath,/home/jakjinak/wxwidgets/wxWidgets-3.0.3/lib -lwx_gtk2u_xrc-3.0 -lwx_gtk2u_html-3.0 -lwx_gtk2u_qa-3.0 -lwx_gtk2u_adv-3.0 -lwx_gtk2u_core-3.0 -lwx_baseu_xml-3.0 -lwx_baseu_net-3.0 -lwx_baseu-3.0

ROOTDIR := $(realpath .)
BINDIR := ${ROOTDIR}/.bin
LIBDIR := ${ROOTDIR}/.bin
OBJDIR := ${ROOTDIR}/.bin

COMMON_CPPFLAGS := -std=c++11 -g
COMMON_ARFLAGS := 
COMMON_LDFLAGS := -std=c++11 -g
ifeq ($(BUILD_MODE),release)
COMMON_CPPFLAGS += -O2 -DNDEBUG
COMMON_LDFLAGS += -O2 -DNDEBUG
else
COMMON_CPPFLAGS += -O0 -DDEBUG
COMMON_LDFLAGS += -O0 -DDEBUG
endif

.PHONY: all libs tests clean_all clean clean_tests

all: libs tests
clean_all: clean clean_tests

########################################
# defines

#$(call showhint,"hint")
ifeq ($(SHOW_HINTS),1)
define showhint
	@echo $(1)
endef
else
define showhint
endef
endif

ifeq ($(HIDE_COMMANDS),1)
COMMAND_HIDE_PREFIX := @
else
COMMAND_HIDE_PREFIX :=
endif


# Defines all the undefined definitions and rules that are needed in the macros below.
# The first parameter is the name of the library and also the suffix of all defines,
# the second parameter is the parent make rule, third is the parent clean rule.
# Assumes SRCDIR_<name>, SOURCE_<name> DEFINE_<name> and INCDIR_<name>, RESULT_<name> to be defined
# Awaits OBJDIR_<name>, DEPDIR_<name> to be optionally defined, defines them if not found
# Defines SRC_<name>, OBJ_<name> and DEP_<name> and all the rules (except the final one that produces the result)
#(call define_common_part,name,makerule,cleanrule)
define define_common_part
OBJDIR_$(1) ?= ${OBJDIR}/${BUILD_MODE}.${BUILD_ARCH}/obj/$(1)
DEPDIR_$(1) ?= ${OBJDIR}/${BUILD_MODE}.${BUILD_ARCH}/dep/$(1)

SRC_$(1) := $$(addprefix $${SRCDIR_$(1)}/,$${SOURCE_$(1)})
OBJ_$(1) := $$(addprefix $${OBJDIR_$(1)}/,$$(SOURCE_$(1):.cpp=.o))
DEP_$(1) := $$(addprefix $${DEPDIR_$(1)}/,$$(SOURCE_$(1):.cpp=.d))

.PHONY: $(1) clean_$(1) info_$(1)

$(2): $(1)
$(3): clean_$(1)
$(1): $${RESULT_$(1)}
clean_$(1):
	$$(call showhint,"Clean all for $(1)")
	$(COMMAND_HIDE_PREFIX)rm -f $${RESULT_$(1)} $${OBJ_$(1)} $${DEP_$(1)}

info_$(1):
	@echo "SRCDIR_$(1) = [$$(SRCDIR_$(1))]"
	@echo "SOURCE_$(1) = [$$(SOURCE_$(1))]"
	@echo "DEFINE_$(1) = [$$(DEFINE_$(1))]"
	@echo "INCDIR_$(1) = [$$(INCDIR_$(1))]"
	@echo "OBJDIR_$(1) = [$$(OBJDIR_$(1))]"
	@echo "DEPDIR_$(1) = [$$(DEPDIR_$(1))]"
	@echo "SRC_$(1) = [$$(SRC_$(1))]"
	@echo "OBJ_$(1) = [$$(OBJ_$(1))]"
	@echo "DEP_$(1) = [$$(DEP_$(1))]"
	@echo
	@echo "Defines these rules: [$(1)] [clean_$(1)] [info_$(1)]"
	@echo "Is part of these rules: [$(2)] [$(3)]"

$${DEPDIR_$(1)}/dircreated:
	$$(call showhint,"Creating directory $${DEPDIR_$(1)}")
	$(COMMAND_HIDE_PREFIX)mkdir -p $${DEPDIR_$(1)}
	$(COMMAND_HIDE_PREFIX)touch $${DEPDIR_$(1)}/dircreated

$${OBJDIR_$(1)}/dircreated:
	$$(call showhint,"Creating directory $${OBJDIR_$(1)}")
	$(COMMAND_HIDE_PREFIX)mkdir -p $${OBJDIR_$(1)}
	$(COMMAND_HIDE_PREFIX)touch $${OBJDIR_$(1)}/dircreated

$${DEPDIR_$(1)}/%.d : $${SRCDIR_$(1)}/%.cpp $${DEPDIR_$(1)}/dircreated
	$$(call showhint,"Evaluating dependencies of $$(subst $$(ROOTDIR)/,,$$<)")
	$(COMMAND_HIDE_PREFIX)g++ ${COMMON_CPPFLAGS} $${DEFINE_$(1)} $${INCDIR_$(1)} -MM -MT $${OBJDIR_$(1)}/$$(notdir $$(@:.d=.o)) -MF $$@ $$<

$${OBJDIR_$(1)}/%.o : $${SRCDIR_$(1)}/%.cpp $${DEPDIR_$(1)}/%.d $${OBJDIR_$(1)}/dircreated
	$$(call showhint,"Compiling $$(subst $$(ROOTDIR)/,,$$<)")
	$(COMMAND_HIDE_PREFIX)g++ ${COMMON_CPPFLAGS} $${DEFINE_$(1)} $${INCDIR_$(1)} -c -o $$@ $$<

-include $${DEP_$(1)}
endef

# Defines all the undefined definitions and rules for a library unless already defined
# The first parameter is the name of the library and also the suffix of all defines,
# the second parameter is the parent make rule, third is the parent clean rule.
# Assumes SRCDIR_<name>, SOURCE_<name> DEFINE_<name> and INCDIR_<name> to be defined
# Awaits RESULT_<name>, OBJDIR_<name>, DEPDIR_<name> to be optionally defined, defines if not found
# Defines SRC_<name>, OBJ_<name> and DEP_<name> and all the rules
#(call define_static_library,name,makerule,cleanrule)
define define_static_library
RESULT_$(1) ?= ${LIBDIR}/${BUILD_MODE}.${BUILD_ARCH}/$(1).a

$(call define_common_part,$(1),$(2),$(3))

$${RESULT_$(1)}: $${OBJ_$(1)}
	$$(call showhint,"Creating static library $$(subst $$(ROOTDIR)/,,$${RESULT_$(1)})")
	$(COMMAND_HIDE_PREFIX)ar cr ${COMMON_ARFLAGS} $${RESULT_$(1)} $${OBJ_$(1)}
endef

# Defines all the undefined definitions and rules for a program unless already defined
# The first parameter is the name of the program and also the suffix of all defines,
# the second parameter is the parent make rule, third is the parent clean rule.
# Assumes SRCDIR_<name>, SOURCE_<name> DEFINE_<name> and INCDIR_<name> and LDFLAGS_<name> to be defined
# Awaits RESULT_<name>, OBJDIR_<name>, DEPDIR_<name> to be optionally defined, defines if not found
# Defines SRC_<name>, OBJ_<name> and DEP_<name> and all the rules
#(call define_program,name,makerule,cleanrule)
define define_program
RESULT_$(1) ?= ${BINDIR}/${BUILD_MODE}.${BUILD_ARCH}/$(1)

$(call define_common_part,$(1),$(2),$(3))

$${RESULT_$(1)}: $${OBJ_$(1)}
	$$(call showhint, "Linking program $$(subst $$(ROOTDIR)/,,$${RESULT_$(1)})")
	$(COMMAND_HIDE_PREFIX)g++ $${OBJ_$(1)} ${COMMON_LDFLAGS} $${LDFLAGS_$(1)} -o $${RESULT_$(1)}
endef


########################################
# jjbase
SRCDIR_jjbase := $(realpath .)
SOURCE_jjbase := stream.cpp
DEFINE_jjbase := ${WXDEFINE}
INCDIR_jjbase := -I$(realpath ${SRCDIR_jjbase}/..)
$(eval $(call define_static_library,jjbase,libs,clean))

########################################
# jjgui
SRCDIR_jjgui := $(realpath gui)
SOURCE_jjgui := common_wx.cpp application_wx.cpp window_wx.cpp menu_wx.cpp sizer_wx.cpp control_wx.cpp button_wx.cpp textLabel_wx.cpp textInput_wx.cpp comboBox_wx.cpp
DEFINE_jjgui := ${WXDEFINE}
INCDIR_jjgui := -I$(realpath ${SRCDIR_jjgui}/../..) ${WXINCDIR}
$(eval $(call define_static_library,jjgui,libs,clean))

########################################
# TestApp
SRCDIR_TestApp := $(realpath test)
SOURCE_TestApp := TestAppMain.cpp
LDFLAGS_TestApp := -pthread ${RESULT_jjgui} ${RESULT_jjbase} ${WXLIBS}
INCDIR_TestApp := -I$(realpath ${SRCDIR_TestApp}/../..)
$(eval $(call define_program,TestApp,tests,clean_tests))
