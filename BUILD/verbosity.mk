VERBOSITY_labels ?= 1
VERBOSITY_colors ?= 1
VERBOSITY_commands ?= 1


#$(call showlabel,"label")
ifeq ($(VERBOSITY_labels),1)
define showlabel
	@${TOOL_ECHO} $(1)
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

ifeq ($(VERBOSITY_commands),1)
COMMAND_HIDE_PREFIX := @
else
COMMAND_HIDE_PREFIX :=
endif
