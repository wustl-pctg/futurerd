include ../../../../bench.mk		

.PHONEY: all clean serial cilk
 
# All the *_SOURCES variables are unused; for reference only
SOURCES = DMatrix.h GeneralUtil.h ParticleFilter.h \
	SmallVectors.h Vector3.h system.h AnnealingFactor.h \
	AnnealingFactor.cpp BodyGeometry.h BodyGeometry.cpp BodyPose.h \
	BodyPose.cpp CameraModel.h CameraModel.cpp CovarianceMatrix.h \
	CovarianceMatrix.cpp ImageMeasurements.h ImageMeasurements.cpp \
	ImageProjection.h ImageProjection.cpp RandomGenerator.h \
	RandomGenerator.cpp TrackingModel.h TrackingModel.cpp main.cpp \

OBJDIR = clang-obj

CILK_SOURCE = ParticleFilterCilk.h  TrackingModelCilk.h TrackingModelCilk.cpp

# object files for each configuration
OBJECTS = $(OBJDIR)/AnnealingFactor.o $(OBJDIR)/BodyGeometry.o $(OBJDIR)/BodyPose.o \
	$(OBJDIR)/CameraModel.o $(OBJDIR)/CovarianceMatrix.o $(OBJDIR)/ImageMeasurements.o \
	$(OBJDIR)/ImageProjection.o $(OBJDIR)/RandomGenerator.o $(OBJDIR)/TrackingModel.o

CILK_OBJECTS = $(OBJDIR)/main-cilk.o $(OBJDIR)/TrackingModelCilk.o

DEPENDENCIES = ../FlexImageLib/$(OBJDIR)/libflex.a

# Need to use += since they are partially defined in bench.mk and common.mk
CXXFLAGS += -O3 -g -funroll-loops -fprefetch-loop-arrays -fpermissive -fexceptions -Wall -Wno-unknown-pragmas # -static-libgcc -Wl,--hash-style=both,--as-needed -fno-exceptions 
LDFLAGS += ../FlexImageLib/$(OBJDIR)/libflex.a

INCLUDES = -I../FlexImageLib
LIBS = -lm

all: serial cilk

serial: $(OBJDIR) bodytrack-serial 
cilk: CXXFLAGS += $(APPFLAGS) $(RDFLAGS) 
cilk: LDFLAGS += $(LIB)
cilk: INCLUDES += $(INC)
cilk: $(OBJDIR) bodytrack-cilk
	
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Only the main file is compiled differently 
$(OBJDIR)/main-serial.o: main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

$(OBJDIR)/main-cilk.o: main.cpp
	$(CXX) $(CXXFLAGS) -DUSE_CILK_FUTURE=1 $(INCLUDES) -c -o $@ $<

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

bodytrack-serial: $(OBJDIR)/main-serial.o $(OBJECTS) $(DEPENDENCIES)
	$(CXX) -o $@ $^ $(LDFLAGS) $(LIBS)

bodytrack-cilk: $(OBJECTS) $(CILK_OBJECTS) $(DEPENDENCIES)
	$(CXX) -o $@ $^ $(LDFLAGS) $(LIBS)

clean:
	rm -f $(OBJDIR)/*.o bodytrack-*
