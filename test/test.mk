TEST_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
include $(TEST_DIR)/../common.mk

APP_TOOL_ONLY_FLAGS = -fcilktool
APP_RD_FLAGS = $(APP_TOOL_ONLY_FLAGS) -fsanitize=thread
