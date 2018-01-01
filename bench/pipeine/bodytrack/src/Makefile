.PHONY: all clean serial pthread omp cilk

all: bodytrack-serial bodytrack-pthread bodytrack-omp bodytrack-cilk

../build: 
	mkdir ../build

serial: bodytrack-serial
serial-gcc: bodytrack-serial-gcc
pthread: bodytrack-pthread
omp: bodytrack-omp
cilk: bodytrack-cilk

bodytrack-serial: ../build
	$(MAKE) -C FlexImageLib 
	$(MAKE) -C TrackingBenchmark serial
	mv TrackingBenchmark/$@ ../build/

bodytrack-serial-gcc: ../build
	$(MAKE) -f Makefile.gcc -C FlexImageLib 
	$(MAKE) -f Makefile.gcc -C TrackingBenchmark serial
	mv TrackingBenchmark/$@ ../build/

bodytrack-pthread: ../build
	$(MAKE) -f Makefile.gcc -C FlexImageLib
	$(MAKE) -f Makefile.gcc -C TrackingBenchmark pthread
	mv TrackingBenchmark/$@ ../build/

bodytrack-omp: ../build
	$(MAKE) -f Makefile.gcc -C FlexImageLib
	$(MAKE) -f Makefile.gcc -C TrackingBenchmark omp
	mv TrackingBenchmark/$@ ../build/

bodytrack-cilk: ../build
	$(MAKE) -C FlexImageLib 
	$(MAKE) -C TrackingBenchmark cilk
	mv TrackingBenchmark/$@ ../build/

clean:
	$(MAKE) -f Makefile.gcc -C FlexImageLib clean
	$(MAKE) -f Makefile.gcc -C TrackingBenchmark clean
	$(MAKE) -C FlexImageLib clean
	$(MAKE) -C TrackingBenchmark clean
	rm -f ../build/*
