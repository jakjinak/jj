############################################################
# setup the build configuration defaults (unless set from outside)
# note: only the BUILD_* are supposed to be set from outside
BUILD_MODE ?= debug
BUILD_ARCH ?= $(shell uname -p | tr '[A-Z]' '[a-z]')
BUILD_OS ?= $(shell uname -s | tr '[A-Z]' '[a-z]')
MACHINE_ARCH ?= $(shell uname -p | tr '[A-Z]' '[a-z]')
MACHINE_OS ?= $(shell uname -s | tr '[A-Z]' '[a-z]')

ifeq (${BUILD_ARCH},${MACHINE_ARCH})
SELECTED_ARCH := ${BUILD_ARCH}
else
SELECTED_ARCH := ${BUILD_ARCH}@${MACHINE_ARCH}
endif
ifeq (${BUILD_OS},${MACHINE_OS})
SELECTED_OS := ${BUILD_OS}
else
SELECTED_OS := ${BUILD_OS}@${MACHINE_OS}
endif

BUILDCONFIGURATION ?= ${SELECTED_OS}.${SELECTED_ARCH}.${BUILD_MODE}
