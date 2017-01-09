include common.mk

TARGET = $(LIB_DIR)/libfuturerd.a

default: 
	$(MAKE) -C ./src

check: $(TARGET)
	$(MAKE) -C ./test/basic check
