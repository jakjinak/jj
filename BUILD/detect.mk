############################################################
# setup the build configuration defaults (unless set from outside)
# note: only the BUILD_* are supposed to be set from outside
BUILD_MODE ?= debug
BUILD_ARCH ?= $(shell BUILD/detect.sh --arch)
BUILD_OS ?= $(shell BUILD/detect.sh --os)
MACHINE_ARCH ?= $(shell BUILD/detect.sh --arch)
MACHINE_OS ?= $(shell BUILD/detect.sh --os)

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
