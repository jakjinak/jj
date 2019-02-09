VERBOSITY_labels ?= 1
VERBOSITY_colors ?= 1
VERBOSITY_commands ?= 1
VERBOSITY_tests ?= 1
VERBOSITY_archive ?= 0


#$(call showlabel,"label")
ifeq ($(VERBOSITY_labels),1)
define showlabel
	@${TOOL_echo} $(1)
endef
else
define showlabel
endef
endif

ifeq ($(VERBOSITY_colors),1)
COLOR_HL ?= \\e[1m
COLOR_0 ?= \\e[0m
COLOR_SUPPORT ?= \\e[34m
COLOR_COMPILE ?= \\e[32m
COLOR_STATLIB ?= \\e[36m
COLOR_SHARLIB ?= \\e[36m
COLOR_PROGRAM ?= \\e[36m
COLOR_CLEAN ?= \\e[31m
COLOR_INFO ?= \\e[33m
COLOR_RUNTEST ?= \\e[36m
else
COLOR_HL :=
COLOR_0 :=
COLOR_SUPPORT :=
COLOR_COMPILE :=
COLOR_STATLIB :=
COLOR_SHARLIB :=
COLOR_PROGRAM :=
COLOR_CLEAN :=
COLOR_INFO :=
COLOR_RUNTEST :=
endif

ifeq ($(VERBOSITY_commands),1)
COMMAND_HIDE_PREFIX :=
else
COMMAND_HIDE_PREFIX := @
endif

ifeq ($(VERBOSITY_colors),1)
RUNTEST_COLORS := -C
endif

ifeq ($(VERBOSITY_tests),0)
RUNTEST_STYLE := -S=none
else
ifeq ($(VERBOSITY_tests),1)
RUNTEST_STYLE := -S=short ${RUNTEST_COLORS}
else
ifeq ($(VERBOSITY_tests),2)
RUNTEST_STYLE := -cn ${RUNTEST_COLORS}
else
ifeq ($(VERBOSITY_tests),3)
RUNTEST_STYLE := -cn ${RUNTEST_COLORS} --results=all
else
RUNTEST_STYLE := -S=none
endif
endif
endif
endif

ifeq ($(VERBOSITY_archive),1)
VERBOSE_TAR := v
else
VERBOSE_TAR :=
endif
