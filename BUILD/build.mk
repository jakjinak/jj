
############################################################
# setup basic definitions
ifeq ("${ROOTDIR}","")
$(error Please define ROOTDIR first.)
endif

include BUILD/tools.mk
include BUILD/style.mk

BINDIR ?= ${ROOTDIR}/.bin/${BUILDCONFIGURATION}
LIBDIR ?= ${ROOTDIR}/.bin/${BUILDCONFIGURATION}
OBJDIR ?= ${ROOTDIR}/.bin/${BUILDCONFIGURATION}
TMPDIR ?= ${ROOTDIR}/.bin/${BUILDCONFIGURATION}/tmp

WIPEDIRS := "${BINDIR}"
ifneq ("${LIBDIR}","${BINDIR}")
WIPEDIRS += "${LIBDIR}"
endif
ifneq ("${OBJDIR}","${BINDIR}")
WIPEDIRS += "${OBJDIR}"
endif

############################################################
# include specific defines
include BUILD/specific/${SELECTED_OS}.${SELECTED_ARCH}.mk
include BUILD/specific/${BUILD_MODE}.mk

COMMON_CXXFLAGS ?= ${PLATFORMSPECIFIC_CXXFLAGS} ${MODESPECIFIC_CXXFLAGS} ${CUSTOM_CXXFLAGS}
COMMON_ARFLAGS ?= ${PLATFORMSPECIFIC_ARFLAGS} ${MODESPECIFIC_ARFLAGS} ${CUSTOM_ARFLAGS}
COMMON_LDFLAGS ?= ${PLATFORMSPECIFIC_LDFLAGS} ${MODESPECIFIC_LDFLAGS} ${CUSTOM_LDFLAGS}
SO_COMMON_CXXFLAGS ?= -fPIC ${COMMON_CXXFLAGS}
SO_COMMON_LDFLAGS ?= -fPIC ${COMMON_LDFLAGS}


########################################
# this file contains below macros used to generate the targets for individual projects
# see definition of define_static_library, define_shared_library, define_static_and_shared_library or
# define_program below or the common part right above them on how to use it

wipeout:
	$(call showhint,${COLOR_CLEAN}=== Wiping all for ${COLOR_HL}${WIPEDIRS}${COLOR_0})
	$(COMMAND_HIDE_PREFIX)${TOOL_RMR} ${WIPEDIRS}

${TMPDIR}:
	$(call showhint,"${COLOR_SUPPORT}=== Creating directory ${COLOR_HL}${TMPDIR}${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)${TOOL_MKDIR} ${TMPDIR}

#--------------------------
#------- COMMON DEFINES ---
# Defines all macros and targets common to all main targets (static/shared/program).
#(call define_common_part,name,makerule,cleanrule,adddeps)
define define_common_part
SRC_$(1) := $$(addprefix $${SRCDIR_$(1)}/,$${SOURCE_$(1)})

.PHONY: $(1) $(1)_only clean_$(1) clean_$(1)_only info_$(1) infoprelude_$(1) infofinale_$(1) infopreludecommon_$(1)

$(2): $(1)
$(3): clean_$(1)

infopreludecommon_$(1):
	@${TOOL_ECHO} "SRCDIR_$(1) = [$${COLOR_INFO}$$(SRCDIR_$(1))$${COLOR_0}]"
	@${TOOL_ECHO} "SOURCE_$(1) = [$${COLOR_INFO}$$(SOURCE_$(1))$${COLOR_0}]"

infoprelude_$(1): infopreludecommon_$(1)
	@${TOOL_ECHO} "SRC_$(1) = [$${COLOR_INFO}$$(SRC_$(1))$${COLOR_0}]"

info_$(1): infoprelude_$(1) infofinale_$(1)
endef

#---------------------------------
#------- STATIC COMMON DEFINES ---

# Defines all the stuff common to static libs and programs.
#(call define_staticcommon_part,name,makerule,cleanrule,adddeps)
define define_staticcommon_part
OBJDIR_$(1) ?= ${OBJDIR}/obj/$(1)
DEPDIR_$(1) ?= ${OBJDIR}/dep/$(1)

OBJ_$(1) := $$(addprefix $${OBJDIR_$(1)}/,$$(SOURCE_$(1):.cpp=.o))
DEP_$(1) := $$(addprefix $${DEPDIR_$(1)}/,$$(SOURCE_$(1):.cpp=.d))

CXXFLAGS_$(1) ?= ${COMMON_CXXFLAGS} $${INCDIR_$(1)}
LDFLAGS_$(1) ?= ${COMMON_LDFLAGS} $${LIBS_$(1)}
ARFLAGS_$(1) ?= ${COMMON_ARFLAGS}

-include $${DEP_$(1)}

.PHONY: infopreludestaticcommon_$(1) infofinalestaticcommon_$(1)

$(1): $${RESULT_$(1)}
$${RESULT_$(1)}: $(addsuffix },$(addprefix $${RESULT_,$(4)))
clean_$(1): clean_$(1)_only $(addprefix clean_,$(4))

clean_$(1)_only:
	$$(call showhint,"$${COLOR_CLEAN}=== Clean all for $${COLOR_HL}$(1)$${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)${TOOL_RM} $${RESULT_$(1)} $${OBJ_$(1)} $${DEP_$(1)}

infopreludestaticcommon_$(1):
	@${TOOL_ECHO} "CXXFLAGS_$(1) = [$${COLOR_INFO}$$(CXXFLAGS_$(1))$${COLOR_0}]"
	@${TOOL_ECHO} "OBJDIR_$(1) = [$${COLOR_INFO}$$(OBJDIR_$(1))$${COLOR_0}]"
	@${TOOL_ECHO} "DEPDIR_$(1) = [$${COLOR_INFO}$$(DEPDIR_$(1))$${COLOR_0}]"
	@${TOOL_ECHO} "OBJ_$(1) = [$${COLOR_INFO}$$(OBJ_$(1))$${COLOR_0}]"
	@${TOOL_ECHO} "DEP_$(1) = [$${COLOR_INFO}$$(DEP_$(1))$${COLOR_0}]"

infofinalestaticcommon_$(1):
	@${TOOL_ECHO}
	@${TOOL_ECHO} "Defines these rules: [$${COLOR_INFO}$(1) $(1)_only$${COLOR_0}] [$${COLOR_INFO}clean_$(1) clean_$(1)_only$${COLOR_0}] [$${COLOR_INFO}info_$(1)$${COLOR_0}]"
	@${TOOL_ECHO} "Depends on these rules: [$${COLOR_INFO}$(4)$${COLOR_0}]"
	@${TOOL_ECHO} "Is part of these rules: [$${COLOR_INFO}$(2)$${COLOR_0}] [$${COLOR_INFO}$(3)$${COLOR_0}]"


$${DEPDIR_$(1)}:
	$$(call showhint,"$${COLOR_SUPPORT}=== Creating directory $${COLOR_HL}$${DEPDIR_$(1)}$${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)${TOOL_MKDIR} $${DEPDIR_$(1)}

$${OBJDIR_$(1)}:
	$$(call showhint,"$${COLOR_SUPPORT}=== Creating directory $${COLOR_HL}$${OBJDIR_$(1)}$${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)${TOOL_MKDIR} $${OBJDIR_$(1)}

$${DEPDIR_$(1)}/%.d : ;

$${OBJDIR_$(1)}/%.o : $${SRCDIR_$(1)}/%.cpp | $${OBJDIR_$(1)} $${DEPDIR_$(1)}
	$$(call showhint,"$${COLOR_COMPILE}=== Compiling $${COLOR_HL}$$(subst $$(ROOTDIR)/,,$$<)$${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)${TOOL_CXX} $${CXXFLAGS_$(1)} -MMD -MT $$@ -MF $${DEPDIR_$(1)}/$$(notdir $$(@:.o=.d)) -c -o $$@ $$<
endef

# The following forms the beginning of a static library definitions.
#(call define_static_part_alpha,name,makerule,cleanrule,adddeps)
define define_static_part_alpha
RESULT_$(1) ?= ${LIBDIR}/lib$(1).a
endef

# The following forms the end of a static library definitions.
#(call define_static_part_omega,name,makerule,cleanrule,adddeps)
define define_static_part_omega
$(call define_staticcommon_part,$(1),$(2),$(3),$(4))

infopreludestatic_$(1): infopreludestaticcommon_$(1)
	@${TOOL_ECHO} "ARFLAGS_$(1) = [$${COLOR_INFO}$$(ARFLAGS_$(1))$${COLOR_0}]"

infoprelude_$(1): infopreludestatic_$(1)
infofinale_$(1): infofinalestaticcommon_$(1)

.PHONY: infopreludestatic_$(1)

$${RESULT_$(1)}: $${OBJ_$(1)}
	$$(call showhint,"$${COLOR_STATLIB}=== Creating static library $${COLOR_HL}$$(subst $$(ROOTDIR)/,,$${RESULT_$(1)})$${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)${TOOL_AR} cr $${ARFLAGS_$(1)} $${RESULT_$(1)} $${OBJ_$(1)}
$(1)_only: $${OBJ_$(1)}
	$$(call showhint,"$${COLOR_STATLIB}=== Creating static library $${COLOR_HL}$$(subst $$(ROOTDIR)/,,$${RESULT_$(1)})$${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)${TOOL_AR} cr $${ARFLAGS_$(1)} $${RESULT_$(1)} $${OBJ_$(1)}
endef

#----------------------------------
#------- SHARED LIBRARY DEFINES ---
# The following forms the beginning of a shared library definitions.
#(call define_shared_part_alpha,name,makerule,cleanrule,adddeps)
define define_shared_part_alpha
SO_RESULT_$(1) ?= ${LIBDIR}/lib$(1).so
RESULT_$(1)_so := $${SO_RESULT_$(1)}
SO_SOURCE_$(1) ?= $${SOURCE_$(1)}
SO_SRCDIR_$(1) ?= $${SRCDIR_$(1)}
SO_INCDIR_$(1) ?= $${INCDIR_$(1)}
SO_LIBS_$(1) ?= $${LIBS_$(1)}
endef

# The following forms the end of a shared library definitions.
#(call define_shared_part_omega,name,makerule,cleanrule,adddeps)
define define_shared_part_omega
SO_OBJDIR_$(1) ?= ${OBJDIR}/obj/$(1)_so
SO_DEPDIR_$(1) ?= ${OBJDIR}/dep/$(1)_so

SO_OBJ_$(1) := $$(addprefix $${SO_OBJDIR_$(1)}/,$$(SO_SOURCE_$(1):.cpp=.o))
SO_DEP_$(1) := $$(addprefix $${SO_DEPDIR_$(1)}/,$$(SO_SOURCE_$(1):.cpp=.d))

SO_CXXFLAGS_$(1) ?= ${SO_COMMON_CXXFLAGS} $${SO_INCDIR_$(1)}
SO_LDFLAGS_$(1) ?= ${SO_COMMON_LDFLAGS} $${SO_LIBS_$(1)}

-include $${SO_DEP_$(1)}

$(1)_so: $${SO_RESULT_$(1)}
clean_$(1)_so: clean_$(1)_so_only $(addprefix clean_,$(4))

clean_$(1)_so_only:
	$$(call showhint,"$${COLOR_CLEAN}=== Clean all for $${COLOR_HL}$(1)_so$${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)${TOOL_RM} $${SO_RESULT_$(1)} $${SO_OBJ_$(1)} $${SO_DEP_$(1)}

$${SO_DEPDIR_$(1)}:
	$$(call showhint,"$${COLOR_SUPPORT}=== Creating directory $${COLOR_HL}$${SO_DEPDIR_$(1)}$${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)${TOOL_MKDIR} $${SO_DEPDIR_$(1)}

$${SO_OBJDIR_$(1)}:
	$$(call showhint,"$${COLOR_SUPPORT}=== Creating directory $${COLOR_HL}$${SO_OBJDIR_$(1)}$${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)${TOOL_MKDIR} $${SO_OBJDIR_$(1)}

$${SO_DEPDIR_$(1)}/%.d : ;

$${SO_OBJDIR_$(1)}/%.o : $${SO_SRCDIR_$(1)}/%.cpp | $${SO_OBJDIR_$(1)} $${SO_DEPDIR_$(1)}
	$$(call showhint,"$${COLOR_COMPILE}=== Compiling $${COLOR_HL}$$(subst $$(ROOTDIR)/,,$$<)$${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)${TOOL_CXX} $${SO_CXXFLAGS_$(1)} -MMD -MT $$@ -MF $${SO_DEPDIR_$(1)}/$$(notdir $$(@:.o=.d)) -c -o $$@ $$<

infopreludeshared_$(1):
	@${TOOL_ECHO} "SO_CXXFLAGS_$(1) = [$${COLOR_INFO}$$(SO_CXXFLAGS_$(1))$${COLOR_0}]"
	@${TOOL_ECHO} "SO_OBJDIR_$(1) = [$${COLOR_INFO}$$(SO_OBJDIR_$(1))$${COLOR_0}]"
	@${TOOL_ECHO} "SO_DEPDIR_$(1) = [$${COLOR_INFO}$$(SO_DEPDIR_$(1))$${COLOR_0}]"
	@${TOOL_ECHO} "SO_OBJ_$(1) = [$${COLOR_INFO}$$(SO_OBJ_$(1))$${COLOR_0}]"
	@${TOOL_ECHO} "SO_DEP_$(1) = [$${COLOR_INFO}$$(SO_DEP_$(1))$${COLOR_0}]"
	@${TOOL_ECHO} "SO_LDFLAGS_$(1) = [$${COLOR_INFO}$$(SO_LDFLAGS_$(1))$${COLOR_0}]"

infofinaleshared_$(1):
	@${TOOL_ECHO} "The shared part efines these rules: [$${COLOR_INFO}$(1)_so $(1)_so_only$${COLOR_0}] [$${COLOR_INFO}clean_$(1)_so clean_$(1)_so_only$${COLOR_0}]"
	@${TOOL_ECHO} "The shared part depends on these rules: [$${COLOR_INFO}$(4)$${COLOR_0}]"
	@${TOOL_ECHO} "The shared part is part of these rules: [$${COLOR_INFO}$(2)$${COLOR_0}] [$${COLOR_INFO}$(3)$${COLOR_0}]"

infoprelude_$(1): infopreludeshared_$(1)
infofinale_$(1): infofinaleshared_$(1)

.PHONY: infopreludeshared_$(1) infofinaleshared_$(1)

$${SO_RESULT_$(1)}: $${SO_OBJ_$(1)} $(addsuffix },$(addprefix $${RESULT_,$(4)))
	$$(call showhint,"$${COLOR_STATLIB}=== Creating shared library $${COLOR_HL}$$(subst $$(ROOTDIR)/,,$${SO_RESULT_$(1)})$${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)${TOOL_CXX} -shared $${SO_LDFLAGS_$(1)} -o $$@ $${SO_OBJ_$(1)}

$(1)_so_only: $${SO_OBJ_$(1)}
	$$(call showhint,"$${COLOR_STATLIB}=== Creating shared library $${COLOR_HL}$$(subst $$(ROOTDIR)/,,$${SO_RESULT_$(1)})$${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)${TOOL_CXX} -shared $${SO_LDFLAGS_$(1)} -o $$@ $${SO_OBJ_$(1)}
endef

############################################################
# HERE STARTS THE MAIN PART

#------- COMMON DESCRIPTION ---
# All of the macros below (define_static_library/define_shared_library/
# define_static_and_shared_library/define_program) and also the helper macros above
# take four parameters in the following order and meaning (unless state otherwise
# in the macro description).
# The first parameter is the name of the library (stem of name) or program and also 
# the suffix of all defines and also defines the main targets, check "make info_<name>".
# The second parameter is the parent make rule, third is the parent clean rule, the
# fourth parameter provides additional targets that the make rule depends on. These
# provide a way to organize individual targets in a (tree-like) hierarchy.
# Use these macros through $(eval $(call...)), example:
# $(eval $(call define_static_library,myshinylib,libs,clean_libs,myutils1 myutils2))
# All the macros operate with several values that they assume to be defined or define them
# on their own. (Most of these have defaults which are set if not defined explictly.)
# SRCDIR_<name> ... root directory containing source files for the library <name>; this
#       must be defined by the user
# SOURCE_<name> ... list of source files that <name> consists of; this must be defined by
#       the user; TODO provide autodetect all in SRCDIR and/or recursively
# RESULT_<name> ... the full path to the product of the library/program; defaults to
#       path.../lib<name>.a, path.../lib<name>.so or path.../<name>
# OBJDIR_<name>, DEPDIR_<name> ... directory containing the .o, .d files; usually
#       the defaults provided by framework are used
# CXXFLAGS_<name> ... flags used to compile c++ sources in <name>; unless overriden it will
#       default to COMMON_CXXFLAGS INCDIR_<name>
# ARFLAGS_<name> ... flags used for creating static libraries; defaults to COMMON_ARFLAGS
# LDFLAGS_<name> ... flags used when linking programs or shared libraries;
#       defaults to COMMON_LDFLAGS LIBS_<name>
# INCDIR_<name> ... can be used to only add extra include paths (and/or defines) instead
#       of redefining the whole CXXFLAGS_<name>
# LIBS_<name> ... can be used to only add extra libraries (and maybe library paths) instead
#       of redefining the whole LDFLAGS_<name>
# Additionally the following are defined as part of the process:
# SRC_<name> ... list of all source file full paths
# OBJ_<name> ... list of all .o file full paths
# DEP_<name> ... list of all .d file full paths
# Note that for the shared libraries all the names have a SO_ prefix, namely SO_SRCDIR_<name>,
# SO_CXXFLAGS_<name>, etc... otherwise the meaning is the same. These defines will fallback
# to those without SO_ prefix if not defined, except SO_CXXFLAGS_<name> (fallbacks to
# SO_COMMON_CXXFLAGS SO_INCDIR_<name>) and SO_LDFLAGS_<name> (fallbacks to SO_COMMON_LDFLAGS SO_LIBS_<name>).
# Again, check "make info_<name>" for the targets defined.
# Also, not all defines make always sense, obviously there is no LDFLAGS for a static library and
# ARFLAGS on the other hand only exists for a static library.


#------- MAIN MACROS ---
# Defines everything for a static library.
#(call define_static_library,name,makerule,cleanrule,adddeps)
define define_static_library
$(call define_static_part_alpha,$(1),$(2),$(3),$(4))
$(call define_common_part,$(1),$(2),$(3),$(4))
$(call define_static_part_omega,$(1),$(2),$(3),$(4))
endef

# Defines everything for a shared library.
#(call define_shared_library,name,makerule,cleanrule,adddeps)
define define_shared_library
$(call define_shared_part_alpha,$(1),$(2),$(3),$(4))
$(call define_common_part,$(1),$(2),$(3),$(4))
$(call define_shared_part_omega,$(1),$(2),$(3),$(4))
endef

# Defines everything for a library that needs to be both static and shared.
#(call define_static_and_shared_library,name,makerule,cleanrule,adddeps)
define define_static_and_shared_library
$(call define_static_part_alpha,$(1),$(2),$(3),$(4))
$(call define_shared_part_alpha,$(1),$(2),$(3),$(4))
$(call define_common_part,$(1),$(2),$(3),$(4))
$(call define_static_part_omega,$(1),$(2),$(3),$(4))
$(call define_shared_part_omega,$(1),$(2),$(3),$(4))
endef

# Defines everything for an executable program.
#(call define_program,name,makerule,cleanrule,adddeps)
define define_program
RESULT_$(1) ?= ${BINDIR}/$(1)

$(call define_common_part,$(1),$(2),$(3),$(4))
$(call define_staticcommon_part,$(1),$(2),$(3),$(4))

infopreludeprogram_$(1): infopreludestaticcommon_$(1)
	@${TOOL_ECHO} "LDFLAGS_$(1) = [$${COLOR_INFO}$$(LDFLAGS_$(1))$${COLOR_0}]"

infoprelude_$(1): infopreludeprogram_$(1)
infofinale_$(1): infofinalestaticcommon_$(1)

.PHONY: infopreludeprogram_$(1)

$${RESULT_$(1)}: $${OBJ_$(1)}
	$$(call showhint, "$${COLOR_PROGRAM}=== Linking program $${COLOR_HL}$$(subst $$(ROOTDIR)/,,$${RESULT_$(1)})$${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)${TOOL_CXX} $${OBJ_$(1)} $${LDFLAGS_$(1)} -o $${RESULT_$(1)}

$(1)_only: $${OBJ_$(1)}
	$$(call showhint, "$${COLOR_PROGRAM}=== Linking program $${COLOR_HL}$$(subst $$(ROOTDIR)/,,$${RESULT_$(1)})$${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)${TOOL_CXX} $${OBJ_$(1)} $${LDFLAGS_$(1)} -o $${RESULT_$(1)}
endef
