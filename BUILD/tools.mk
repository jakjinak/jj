TOOL_echo ?= $(shell [ "`echo -e 'a' 2> /dev/null`" = a ] && echo "echo -e" || echo "echo")
TOOL_mkdir ?= mkdir -p
TOOL_rm ?= rm -f
TOOL_rmr ?= ${TOOL_rm}r
TOOL_find ?= find
TOOL_cp ?= cp

TOOL_tar ?= tar

#(call ARCHIVE_TAR_XZ,archivename,directory,files
define ARCHIVE_TAR_XZ
	$(COMMAND_HIDE_PREFIX)cd $(2) && ${TOOL_tar} cJ$(VERBOSE_TAR)f "$(1)" --numeric-owner $(3)
endef

TOOL_AR ?= ar

TOOL_CXX ?= g++
#/opt/gcc-5.5/bin/g++
#/opt/gcc-7.2/bin/g++
TOOL_LINKER ?= ${TOOL_CXX}

VARNAMES_TOOLS := TOOL_echo TOOL_mkdir TOOL_rm TOOL_rmr TOOL_find TOOL_cp TOOL_tar TOOL_AR TOOL_CXX

vars_tools:
	$(call dump-vars,${VARNAMES_TOOLS})
