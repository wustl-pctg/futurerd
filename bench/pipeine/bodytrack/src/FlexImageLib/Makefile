.PHONEY: all clean

# SOURCES is not used; for reference only
SOURCES = BinaryImage.h FlexColor.h FlexDataExchange.h FlexDefs.h \
FlexDraw.h FlexFilter.h FlexImage.h FlexImageStatus.h FlexImageStore.h \
FlexIO.h FlexLib.h FlexOperators.h FlexThreshold.h FlexTransform.h \
FlexImage.cpp FlexIO.cpp

OBJECTS = FlexImage.o FlexIO.o

CXXFLAGS = -O3 -g -funroll-loops -fprefetch-loop-arrays -fpermissive -fno-exceptions -static-libgcc -Wl,--hash-style=both,--as-needed -fexceptions -Wall -Wno-unknown-pragmas

AR = ar
RANLIB = ranlib
LIBRARY = libflex.a

all: $(LIBRARY)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(LIBRARY): $(OBJECTS)
	$(AR) rcs $@ $^
	$(RANLIB) $@

clean:
	rm -f *.o $(LIBRARY)
