TOOL_ECHO ?= $(shell [ "`echo -e 'a' 2> /dev/null`" = a ] && echo "echo -e" || echo "echo")
TOOL_MKDIR ?= mkdir -p
TOOL_RM ?= rm -f
TOOL_RMR ?= ${TOOL_RM}r

TOOL_AR ?= ar

TOOL_CXX ?= g++
#/opt/gcc-5.5/bin/g++
#/opt/gcc-7.2/bin/g++
