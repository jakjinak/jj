############################################################
# setup the build configuration defaults (unless set from outside)
# note: only the BUILD_* are supposed to be set from outside
BUILD_MODE ?= debug
BUILD_ARCH ?= $(shell ${FRAMEWORKDIR}/detect.sh --arch)
BUILD_OS ?= $(shell ${FRAMEWORKDIR}/detect.sh --os)
MACHINE_ARCH ?= $(shell ${FRAMEWORKDIR}/detect.sh --arch)
MACHINE_OS ?= $(shell ${FRAMEWORKDIR}/detect.sh --os)

ifeq (${BUILD_ARCH},${MACHINE_ARCH})
ifeq (${BUILD_OS},${MACHINE_OS})
BUILD_PLATFORM ?= ${BUILD_OS}.${BUILD_ARCH}
BUILD_CONFIGURATION ?= ${BUILD_MODE}-${BUILD_OS}.${BUILD_ARCH}
else
BUILD_PLATFORM ?= ${BUILD_OS}.${BUILD_ARCH}@${MACHINE_OS}.${MACHINE_ARCH}
BUILD_CONFIGURATION ?= ${BUILD_MODE}-${BUILD_OS}.${BUILD_ARCH}@${MACHINE_OS}.${MACHINE_ARCH}
endif
else
BUILD_PLATFORM ?= ${BUILD_OS}.${BUILD_ARCH}@${MACHINE_OS}.${MACHINE_ARCH}
BUILD_CONFIGURATION ?= ${BUILD_MODE}-${BUILD_OS}.${BUILD_ARCH}@${MACHINE_OS}.${MACHINE_ARCH}
endif

