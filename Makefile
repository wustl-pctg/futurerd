include common.mk

TARGET = $(LIB_DIR)/libfuturerd.a

.DEFAULT_GOAL := lib

lib:
	$(MAKE) -C ./src

check: $(TARGET)
	$(MAKE) -C ./test/basic check

clean:
	$(MAKE) -C ./src clean
	$(MAKE) -C ./test/basic clean
