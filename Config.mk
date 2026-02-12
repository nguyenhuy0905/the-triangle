# one of Debug, Release, RelWithDeb
export BUILD_TYPE := Debug
export BUILD_DIR := build/$(BUILD_TYPE)
export DEBUG_BUILD_FLAG := -ggdb
export RELEASE_BUILD_FLAG := -O2
export REL_WITH_DEB_BUILD_FLAG := -O2 -g
