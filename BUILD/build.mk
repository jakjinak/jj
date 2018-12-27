BUILD_MODE ?= debug
BUILD_ARCH ?= x86_64

ifeq ("${ROOTDIR}","")
$(error Please define ROOTDIR first.)
endif
BINDIR ?= ${ROOTDIR}/.bin
LIBDIR ?= ${ROOTDIR}/.bin
OBJDIR ?= ${ROOTDIR}/.bin


include BUILD/style.mk
include BUILD/tools.mk

########################################
# this file contains macros used to generate the targets for individual projects
# see definition of define_static_library/define_program below on how to use it

.PHONY: all libs tests clean_all clean clean_tests

libs:
all: libs tests
clean_all: clean clean_tests

# Defines all the undefined definitions and rules that are needed in the macros below.
# The first parameter is the name of the library and also the suffix of all defines,
# the second parameter is the parent make rule, third is the parent clean rule.
# The fourth parameter provides additional targets that the make rule depends on.
# Assumes SRCDIR_<name>, SOURCE_<name> and RESULT_<name> to be defined.
# Awaits OBJDIR_<name>, DEPDIR_<name> to be optionally defined, defines them if not found.
# Assumes any <*>FLAGS_<name> (where <*> stands for CXX, LD, AR,...) to be defined where
# necessary; if not defined these will be defined to COMMON_<*>FLAGS, except that
# undefined CXXFLAGS_<name> will be defined to COMMON_CXXFLAGS and INCDIR_<name>,
# undefined LDFLAGS_<name> to COMMON_LDFLAGS and LIBS_<name>
# Defines SRC_<name>, OBJ_<name> and DEP_<name> and all the rules (except the final one that produces the result)
#(call define_common_part,name,makerule,cleanrule,adddeps)
define define_common_part
OBJDIR_$(1) ?= ${OBJDIR}/${BUILD_MODE}.${BUILD_ARCH}/obj/$(1)
DEPDIR_$(1) ?= ${OBJDIR}/${BUILD_MODE}.${BUILD_ARCH}/dep/$(1)

SRC_$(1) := $$(addprefix $${SRCDIR_$(1)}/,$${SOURCE_$(1)})
OBJ_$(1) := $$(addprefix $${OBJDIR_$(1)}/,$$(SOURCE_$(1):.cpp=.o))
DEP_$(1) := $$(addprefix $${DEPDIR_$(1)}/,$$(SOURCE_$(1):.cpp=.d))

CXXFLAGS_$(1) ?= ${COMMON_CXXFLAGS} $${INCDIR_$(1)}
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
	@echo -e "CXXFLAGS_$(1) = [$${COLOR_INFO}$$(CXXFLAGS_$(1))$${COLOR_0}]"
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
