
############################################################
# setup basic definitions
ifeq ("${ROOTDIR}","")
$(error Please define ROOTDIR first.)
endif
ifeq ("${FRAMEWORKDIR}","")
$(error Please define FRAMEWORKDIR first.)
endif

include ${FRAMEWORKDIR}/tools.mk
include ${FRAMEWORKDIR}/verbosity.mk

BINDIR ?= ${ROOTDIR}/.bin/${BUILD_CONFIGURATION}
LIBDIR ?= ${ROOTDIR}/.bin/${BUILD_CONFIGURATION}
OBJDIR ?= ${ROOTDIR}/.bin/${BUILD_CONFIGURATION}
TMPDIR ?= ${ROOTDIR}/.bin/${BUILD_CONFIGURATION}/tmp

ERASEDIRS := "${BINDIR}"
ifneq ("${LIBDIR}","${BINDIR}")
ERASEDIRS += "${LIBDIR}"
endif
ifneq ("${OBJDIR}","${BINDIR}")
ERASEDIRS += "${OBJDIR}"
endif

############################################################
# include specific defines
include ${FRAMEWORKDIR}/specific/${BUILD_PLATFORM}.mk
include ${FRAMEWORKDIR}/specific/${BUILD_MODE}.mk

COMMON_CXXFLAGS ?= ${PLATFORMSPECIFIC_CXXFLAGS} ${MODESPECIFIC_CXXFLAGS} ${CUSTOM_CXXFLAGS}
COMMON_ARFLAGS ?= ${PLATFORMSPECIFIC_ARFLAGS} ${MODESPECIFIC_ARFLAGS} ${CUSTOM_ARFLAGS}
COMMON_LDFLAGS ?= ${PLATFORMSPECIFIC_LDFLAGS} ${MODESPECIFIC_LDFLAGS} ${CUSTOM_LDFLAGS}
SO_COMMON_CXXFLAGS ?= -fPIC ${COMMON_CXXFLAGS}
SO_COMMON_LDFLAGS ?= -fPIC ${COMMON_LDFLAGS}

VARNAMES_SPECIFIC := PLATFORMSPECIFIC_CXXFLAGS MODESPECIFIC_CXXFLAGS CUSTOM_CXXFLAGS \
        PLATFORMSPECIFIC_ARFLAGS MODESPECIFIC_ARFLAGS CUSTOM_ARFLAGS \
        PLATFORMSPECIFIC_LDFLAGS MODESPECIFIC_LDFLAGS CUSTOM_LDFLAGS

vars_specific:
	$(call dump-vars,${VARNAMES_SPECIFIC})

# Prints all the pairs of given macros and their values
# $(call dump-vars,vars)
# 1 vars is a list of variable names to be printed
define dump-vars
	$(foreach var,$1,@${TOOL_echo} "${COLOR_HL}${var}${COLOR_0}=[${COLOR_INFO}${${var}}${COLOR_0}] defined by $(origin ${var})"
	)
endef



# The macro locate_files takes 3 arguments and is basically a simple wrapper over the program find. It expands to a list of the files found. Parameters:
# 1 (dir) - the directory in which to start searching
# 2 (pattern) - the file pattern to be searched for
# 3 (maxdepth) - (optional) gives the maximum depth to search, 1 means search only in the given dir, unlimited by default
#(call locate_files(dir,pattern,maxdepth)
define locate_files
$(patsubst ./%,%,$(shell cd '$1' && ${TOOL_find} . $(if $3,-maxdepth $3,) -type f -name '$2'))
endef

########################################
# this file contains below macros used to generate the targets for individual projects
# see definition of define_static_library, define_shared_library, define_static_and_shared_library or
# define_program below or the common part right above them on how to use it

.PHONY: help info infobody infotargets infotargetscommon infovariables infovariablescommon list list2 erase

infobody:
	@${TOOL_echo} "These makefiles provide a generic framework how to build static/shared libraries and"
	@${TOOL_echo} "programs. It allows to define simple rules as makefile targets to ease the build usage."
	@${TOOL_echo} "There are also several variables that can tweak the behavior."
	@${TOOL_echo} ""

infotargetscommon:
	@${TOOL_echo} "The highlevel targets are (invoked as 'make <targetname>', of course):"
	@${TOOL_echo} "${COLOR_HL}info${COLOR_0} ... prints this overview"
	@${TOOL_echo} "${COLOR_HL}help${COLOR_0} ... same as ${COLOR_HL}info${COLOR_0}"
	@${TOOL_echo} "${COLOR_HL}list${COLOR_0}, ${COLOR_HL}list2${COLOR_0} ... lists all predefined names, further targets and info are available after doing 'make ${COLOR_HL}info_<name>${COLOR_0}'"
	@${TOOL_echo} "${COLOR_HL}vars${COLOR_0} ... lists all 'global' variables and their values, check also ${COLOR_HL}vars_specific${COLOR_0} or ${COLOR_HL}vars_tools${COLOR_0}"
	@${TOOL_echo} "${COLOR_HL}erase${COLOR_0} ... delete the whole directory containing all intermendiate and result files"

infotargets: infotargetscommon
	@${TOOL_echo} ""

VARNAMES_GLOBAL := BUILD_MODE BUILD_ARCH BUILD_OS MACHINE_ARCH MACHINE_OS \
        VERBOSITY_labels VERBOSITY_colors VERBOSITY_commands VERBOSITY_tests VERBOSITY_archive \
        COMMON_CXXFLAGS COMMON_ARFLAGS COMMON_LDFLAGS SO_COMMON_CXXFLAGS SO_COMMON_LDFLAGS \
        TESTRUN_NO_DEPENDENCIES

infovariablescommon:
	@${TOOL_echo} "Build configuration variables:"
	@${TOOL_echo} "${COLOR_HL}BUILD_MODE${COLOR_0} ... debug or release [current value=${COLOR_HL}${BUILD_MODE}${COLOR_0}]"
	@${TOOL_echo} "${COLOR_HL}BUILD_ARCH${COLOR_0} ... the target system architecture, x86 or x86_64 [${COLOR_HL}${BUILD_ARCH}${COLOR_0}]"
	@${TOOL_echo} "${COLOR_HL}BUILD_OS${COLOR_0} ... the target operating system, usually linux [${COLOR_HL}${BUILD_OS}${COLOR_0}]"
	@${TOOL_echo} "${COLOR_HL}MACHINE_ARCH${COLOR_0} ... actual system architecture, x86 or x86_64 (assumed to be autodetected) [${COLOR_HL}${MACHINE_ARCH}${COLOR_0}]"
	@${TOOL_echo} "${COLOR_HL}MACHINE_OS${COLOR_0} ... actual system name, usually linux (assumed to be autodetected) [${COLOR_HL}${MACHINE_OS}${COLOR_0}]"
	@${TOOL_echo} ""
	@${TOOL_echo} "Output related variables:"
	@${TOOL_echo} "${COLOR_HL}VERBOSITY_labels${COLOR_0} ... Show extra output describing what is going on, enabled if set to 1. [${COLOR_HL}${VERBOSITY_labels}${COLOR_0}]"
	@${TOOL_echo} "${COLOR_HL}VERBOSITY_colors${COLOR_0} ... Print hints in color, enabled if set to 1. [${COLOR_HL}${VERBOSITY_colors}${COLOR_0}]"
	@${TOOL_echo} "         Note: also tweaks the info target output"
	@${TOOL_echo} "${COLOR_HL}VERBOSITY_commands${COLOR_0} ... Forces to not echo the commands executed if enabled by setting to 1. [${COLOR_HL}${VERBOSITY_commands}${COLOR_0}]"
	@${TOOL_echo} "${COLOR_HL}VERBOSITY_tests${COLOR_0} ... Unless set to 1 then tests are setup so that only the number of success/fail tests is printed. [${COLOR_HL}${VERBOSITY_commands}${COLOR_0}]"
	@${TOOL_echo} "${COLOR_HL}VERBOSITY_archive${COLOR_0} ... When processing archive files then the contents of the archives are shown only if this is 1. [${COLOR_HL}${VERBOSITY_commands}${COLOR_0}]"
	@${TOOL_echo} ""
	@${TOOL_echo} "${COLOR_HL}TESTRUN_NO_DEPENDENCIES${COLOR_0} ... Define to one to ONLY run tests without checking for dependencies up-to-date. [${COLOR_HL}${TESTRUN_NO_DEPENDENCIES}${COLOR_0}]"
	@${TOOL_echo} ""

infovariables: infovariablescommon

info: infobody infotargets infovariables

help: info

vars:
	$(call dump-vars,${VARNAMES_GLOBAL})

erase:
	$(call showlabel,"${COLOR_CLEAN}=== Erasing all in ${COLOR_HL}${ERASEDIRS}${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)${TOOL_rmr} ${ERASEDIRS}

${TMPDIR}:
	$(call showlabel,"${COLOR_SUPPORT}=== Creating directory ${COLOR_HL}${TMPDIR}${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)${TOOL_mkdir} ${TMPDIR}

#--------------------------
#------- COMMON DEFINES ---
# Defines all macros and targets common to all main targets (static/shared/program).
#(call define_common_part,name,makerule,cleanrule,adddeps)
define define_common_part
SRC_$(1) := $$(addprefix $${SRCDIR_$(1)}/,$${SOURCE_$(1)})

.PHONY: $(1) $(1)_only clean_$(1) clean_$(1)_only info_$(1) infocommon_$(1) list_$(1) list2_$(1) vars_$(1)

$(2): $(1)
$(3): clean_$(1)

info_$(1): infocommon_$(1)

infocommon_$(1):
	@${TOOL_echo} "$${COLOR_HL}$(1)$${COLOR_0} is a $${COLOR_INFO}$${BUILDTYPE_$(1)}$${COLOR_0} defined in this makefile"

list_$(1):
	@${TOOL_echo} "$(1)"

list2_$(1):
	@${TOOL_echo} "$${BUILDTYPE_$(1)} $(1)"

list: list_$(1)

list2: list2_$(1)

VARNAMES_$(1) := SRCDIR_$(1) SOURCE_$(1) SRC_$(1)

vars_$(1):
	$$(call dump-vars,$${VARNAMES_$(1)})
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

VARNAMES_$(1) += OBJDIR_$(1) DEPDIR_$(1) OBJ_$(1) DEP_$(1) CXXFLAGS_$(1) INCDIR_$(1) LDFLAGS_$(1) LIBS_$(1)

-include $${DEP_$(1)}

.PHONY: infostatic_$(1)

$(1): $${RESULT_$(1)}
$${RESULT_$(1)}: $(addsuffix },$(addprefix $${RESULT_,$(4)))
clean_$(1): clean_$(1)_only $(addprefix clean_,$(4))

clean_$(1)_only:
	$$(call showlabel,"$${COLOR_CLEAN}=== Clean all for $${COLOR_HL}$(1)$${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)${TOOL_rm} $${RESULT_$(1)} $${OBJ_$(1)} $${DEP_$(1)}

infostatic_$(1):
	@${TOOL_echo} "Defines these targets: [$${COLOR_INFO}$(1) $(1)_only$${COLOR_0}] [$${COLOR_INFO}clean_$(1) clean_$(1)_only$${COLOR_0}] [$${COLOR_INFO}info_$(1)$${COLOR_0}]"
	@${TOOL_echo} "Depends on these libs: [$${COLOR_INFO}$(4)$${COLOR_0}]"
	@${TOOL_echo} "Is part of these targets: [$${COLOR_INFO}$(2)$${COLOR_0}] [$${COLOR_INFO}$(3)$${COLOR_0}]"

info_$(1): infostatic_$(1)

$${DEPDIR_$(1)}:
	$$(call showlabel,"$${COLOR_SUPPORT}=== Creating directory $${COLOR_HL}$${DEPDIR_$(1)}$${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)${TOOL_mkdir} $${DEPDIR_$(1)}

$${OBJDIR_$(1)}:
	$$(call showlabel,"$${COLOR_SUPPORT}=== Creating directory $${COLOR_HL}$${OBJDIR_$(1)}$${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)${TOOL_mkdir} $${OBJDIR_$(1)}

$${DEPDIR_$(1)}/%.d : ;

$${OBJDIR_$(1)}/%.o : $${SRCDIR_$(1)}/%.cpp | $${OBJDIR_$(1)} $${DEPDIR_$(1)}
	$$(call showlabel,"$${COLOR_COMPILE}=== Compiling $${COLOR_HL}$$(subst $$(ROOTDIR)/,,$$<)$${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)${TOOL_CXX} $${CXXFLAGS_$(1)} -MMD -MT $$@ -MF $${DEPDIR_$(1)}/$$(notdir $$(@:.o=.d)) -c -o $$@ $$<
endef

# The following forms the beginning of a static library definitions.
#(call define_static_part_alpha,name,makerule,cleanrule,adddeps)
define define_static_part_alpha
RESULT_$(1) ?= ${LIBDIR}/lib$(1).a
ARFLAGS_$(1) ?= ${COMMON_ARFLAGS}

VARNAMES_$(1) += RESULT_$(1) ARFLAGS_$(1)
endef

# The following forms the end of a static library definitions.
#(call define_static_part_omega,name,makerule,cleanrule,adddeps)
define define_static_part_omega
$(call define_staticcommon_part,$(1),$(2),$(3),$(4))

$${RESULT_$(1)}: $${OBJ_$(1)}
	$$(call showlabel,"$${COLOR_STATLIB}=== Creating static library $${COLOR_HL}$$(subst $$(ROOTDIR)/,,$${RESULT_$(1)})$${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)${TOOL_AR} cr $${ARFLAGS_$(1)} $${RESULT_$(1)} $${OBJ_$(1)}
$(1)_only: $${OBJ_$(1)}
	$$(call showlabel,"$${COLOR_STATLIB}=== Creating static library $${COLOR_HL}$$(subst $$(ROOTDIR)/,,$${RESULT_$(1)})$${COLOR_0}")
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

VARNAMES_$(1) += SO_RESULT_$(1) RESULT_$(1)_so SO_SOURCE_$(1) SO_SRCDIR_$(1)
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

VARNAMES_$(1) += SO_OBJDIR_$(1) SO_DEPDIR_$(1) SO_OBJ_$(1) SO_DEP_$(1) SO_CXXFLAGS_$(1) SO_INCDIR_$(1) SO_LDFLAGS_$(1) SO_LIBS_$(1)

-include $${SO_DEP_$(1)}

$(1)_so: $${SO_RESULT_$(1)}
clean_$(1)_so: clean_$(1)_so_only $(addprefix clean_,$(4))

clean_$(1)_so_only:
	$$(call showlabel,"$${COLOR_CLEAN}=== Clean all for $${COLOR_HL}$(1)_so$${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)${TOOL_rm} $${SO_RESULT_$(1)} $${SO_OBJ_$(1)} $${SO_DEP_$(1)}

$${SO_DEPDIR_$(1)}:
	$$(call showlabel,"$${COLOR_SUPPORT}=== Creating directory $${COLOR_HL}$${SO_DEPDIR_$(1)}$${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)${TOOL_mkdir} $${SO_DEPDIR_$(1)}

$${SO_OBJDIR_$(1)}:
	$$(call showlabel,"$${COLOR_SUPPORT}=== Creating directory $${COLOR_HL}$${SO_OBJDIR_$(1)}$${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)${TOOL_mkdir} $${SO_OBJDIR_$(1)}

$${SO_DEPDIR_$(1)}/%.d : ;

$${SO_OBJDIR_$(1)}/%.o : $${SO_SRCDIR_$(1)}/%.cpp | $${SO_OBJDIR_$(1)} $${SO_DEPDIR_$(1)}
	$$(call showlabel,"$${COLOR_COMPILE}=== Compiling $${COLOR_HL}$$(subst $$(ROOTDIR)/,,$$<)$${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)${TOOL_CXX} $${SO_CXXFLAGS_$(1)} -MMD -MT $$@ -MF $${SO_DEPDIR_$(1)}/$$(notdir $$(@:.o=.d)) -c -o $$@ $$<

.PHONY: infoshared_$(1)

infoshared_$(1):
	@${TOOL_echo} "The shared part defines these targets: [$${COLOR_INFO}$(1)_so $(1)_so_only$${COLOR_0}] [$${COLOR_INFO}clean_$(1)_so clean_$(1)_so_only$${COLOR_0}]"
	@${TOOL_echo} "The shared part depends on these libs: [$${COLOR_INFO}$(4)$${COLOR_0}]"
	@${TOOL_echo} "The shared part is part of these targets: [$${COLOR_INFO}$(2)$${COLOR_0}] [$${COLOR_INFO}$(3)$${COLOR_0}]"

info_$(1): infoshared_$(1)


$${SO_RESULT_$(1)}: $${SO_OBJ_$(1)} $(addsuffix },$(addprefix $${RESULT_,$(4)))
	$$(call showlabel,"$${COLOR_SHARLIB}=== Creating shared library $${COLOR_HL}$$(subst $$(ROOTDIR)/,,$${SO_RESULT_$(1)})$${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)${TOOL_LINKER} -shared $${SO_LDFLAGS_$(1)} -o $$@ $${SO_OBJ_$(1)}

$(1)_so_only: $${SO_OBJ_$(1)}
	$$(call showlabel,"$${COLOR_SHARLIB}=== Creating shared library $${COLOR_HL}$$(subst $$(ROOTDIR)/,,$${SO_RESULT_$(1)})$${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)${TOOL_LINKER} -shared $${SO_LDFLAGS_$(1)} -o $$@ $${SO_OBJ_$(1)}
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
BUILDTYPE_$(1) := staticlib
$(call define_static_part_alpha,$(1),$(2),$(3),$(4))
$(call define_common_part,$(1),$(2),$(3),$(4))
$(call define_static_part_omega,$(1),$(2),$(3),$(4))
endef

# Defines everything for a shared library.
#(call define_shared_library,name,makerule,cleanrule,adddeps)
define define_shared_library
BUILDTYPE_$(1) := sharedlib
$(call define_shared_part_alpha,$(1),$(2),$(3),$(4))
$(call define_common_part,$(1),$(2),$(3),$(4))
$(call define_shared_part_omega,$(1),$(2),$(3),$(4))
endef

# Defines everything for a library that needs to be both static and shared.
#(call define_static_and_shared_library,name,makerule,cleanrule,adddeps)
define define_static_and_shared_library
BUILDTYPE_$(1) := staticandsharedlib
$(call define_static_part_alpha,$(1),$(2),$(3),$(4))
$(call define_shared_part_alpha,$(1),$(2),$(3),$(4))
$(call define_common_part,$(1),$(2),$(3),$(4))
$(call define_static_part_omega,$(1),$(2),$(3),$(4))
$(call define_shared_part_omega,$(1),$(2),$(3),$(4))
endef

# Defines everything for an executable program.
#(call define_program,name,makerule,cleanrule,adddeps)
define define_program
BUILDTYPE_$(1) := program
RESULT_$(1) ?= ${BINDIR}/$(1)

VARNAMES_$(1) += LDFLAGS_$(1) LIBS_$(1) RESULT_$(1)

$(call define_common_part,$(1),$(2),$(3),$(4))
$(call define_staticcommon_part,$(1),$(2),$(3),$(4))

$${RESULT_$(1)}: $${OBJ_$(1)}
	$$(call showlabel, "$${COLOR_PROGRAM}=== Linking program $${COLOR_HL}$$(subst $$(ROOTDIR)/,,$${RESULT_$(1)})$${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)${TOOL_LINKER} $${OBJ_$(1)} $${LDFLAGS_$(1)} -o $${RESULT_$(1)}

$(1)_only: $${OBJ_$(1)}
	$$(call showlabel, "$${COLOR_PROGRAM}=== Linking program $${COLOR_HL}$$(subst $$(ROOTDIR)/,,$${RESULT_$(1)})$${COLOR_0}")
	$(COMMAND_HIDE_PREFIX)${TOOL_LINKER} $${OBJ_$(1)} $${LDFLAGS_$(1)} -o $${RESULT_$(1)}
endef

.PHONY: testrun

# define_testrun registers a new unit test binary, the first parameter is the stem of all names for this target, it usually
# is same as the first parameter of define_program, the second parameter is the "parent" rule name and third is rules on which
# the binary run depends on (can be empty); the third parameter is ignored if TESTRUN_NO_DEPENDENCIES is defined in environment.
# One can override the following variables to tweak the behavior:
# TESTRUN_NAME_<name> ... the name of the program to run, defaults to RESULT_<name> (defined in define_program)
# TESTRUN_VARS_<name> ... the variables to set for the program; defaults to empty
# TESTRUN_PARS_<name> ... the parameter to pass to the program; defaults to empty
#       note: based on the VERBOSITY_tests formatting parameters will be passed beyond this variable
# TESTRUN_PREPARE_<name> ... can provide commands to prepare environment for the test target before the program is invoked
# TESTRUN_CLEANUP_<name> ... can provide commands to cleanup environment invoked after the program finishes (succeeds)
#(call define_testrun,name,makerule,deprule)
define define_testrun
TESTRUN_NAME_$(1) ?= $${RESULT_$(1)}${PLATFORM_BINARY_SUFFIX}

.PHONY: testrun_$(1) infotestrun_$(1) info_$(1) listtestrun_$(1) listtestrun2_$(1)

$(2) : testrun_$(1)

testrun: testrun_$(1)

testrun_$(1): $(if ${TESTRUN_NO_DEPENDENCIES},,$(3))
	$$(call showlabel, "$${COLOR_RUNTEST}=== Running test binary $${COLOR_HL}$${TESTRUN_NAME_$(1)}$${COLOR_0}")
	$${TESTRUN_PREPARE_$(1)}
	$(COMMAND_HIDE_PREFIX)$${TESTRUN_VARS_$(1)} $${TESTRUN_NAME_$(1)} $${TESTRUN_PARS_$(1)} $${RUNTEST_STYLE}
	$${TESTRUN_CLEANUP_$(1)}

infotestrun_$(1):
	@${TOOL_echo} "$${COLOR_HL}$(1)$${COLOR_0} defines a target that runs tests; can be run with $${COLOR_HL}make testrun_$(1)$${COLOR_0}"
	@${TOOL_echo} "It is associated with $${COLOR_INFO}$(2)$${COLOR_0} and depends on $${COLOR_INFO}$(if ${TESTRUN_NO_DEPENDENCIES},,$(3))$${COLOR_0}".

info_$(1): infotestrun_$(1)

listtestrun_$(1):
	@${TOOL_echo} "$(1)"

list: listtestrun_$(1)

listtestrun2_$(1):
	@${TOOL_echo} "testrun $(1)"

list2: listtestrun2_$(1)

ifeq (${VARNAMES_$(1)},)
# with standalone test targets need to define own var dumper
.PHONY: vars_$(1)

vars_$(1):
	$$(call dump-vars,$${VARNAMES_$(1)})
endif

VARNAMES_$(1) += TESTRUN_NAME_$(1) TESTRUN_VARS_$(1) TESTRUN_PARS_$(1)
endef
