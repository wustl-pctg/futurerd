.PHONY: all clean serial pthread omp 

all: bodytrack

build: 
	mkdir build

serial: bodytrack-serial
pthread: bodytrack-pthread
omp: bodytrack-omp

bodytrack-serial: build
	$(MAKE) -C FlexImageLib 
	$(MAKE) -C TrackingBenchmark serial
	mv TrackingBenchmark/$@ build/

bodytrack-pthread: build
	$(MAKE) -C FlexImageLib 
	$(MAKE) -C TrackingBenchmark pthread
	mv TrackingBenchmark/$@ build/

bodytrack-omp: build
	$(MAKE) -C FlexImageLib 
	$(MAKE) -C TrackingBenchmark omp 
	mv TrackingBenchmark/$@ build/

clean:
	$(MAKE) -C FlexImageLib clean
	$(MAKE) -C TrackingBenchmark clean
	rm build/*
