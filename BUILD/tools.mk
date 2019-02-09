TOOL_ECHO ?= $(shell [ "`echo -e 'a' 2> /dev/null`" = a ] && echo "echo -e" || echo "echo")
TOOL_MKDIR ?= mkdir -p
TOOL_RM ?= rm -f
TOOL_RMR ?= ${TOOL_RM}r
TOOL_FIND ?= find
TOOL_CP ?= cp

TOOL_TAR ?= tar

#(call ARCHIVE_TAR_XZ,archivename,directory,files
define ARCHIVE_TAR_XZ
	$(COMMAND_HIDE_PREFIX)cd $(2) && ${TOOL_TAR} cJ$(VERBOSE_TAR)f "$(1)" --numeric-owner $(3)
endef

TOOL_AR ?= ar

TOOL_CXX ?= g++
#/opt/gcc-5.5/bin/g++
#/opt/gcc-7.2/bin/g++


VARNAMES_TOOLS := TOOL_ECHO TOOL_MKDIR TOOL_RM TOOL_RMR TOOL_FIND TOOL_CP TOOL_TAR TOOL_AR TOOL_CXX

vars_tools:
	$(call dump-vars,${VARNAMES_TOOLS})
