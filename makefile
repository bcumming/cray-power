ifeq ($(PE_ENV),GNU)
	FFLAGS=-O2 -fopenmp
	CCFLAGS=-O2 -fopenmp
endif
ifeq ($(PE_ENV),CRAY)
	FFLAGS= -O2
	CCFLAGS= -O2
endif

test : fortran.exe cpp.exe
	OMP_NUM_THREADS=1 aprun -cc none ./fortran.exe
	OMP_NUM_THREADS=8 aprun -cc none ./fortran.exe
	OMP_NUM_THREADS=1 aprun -cc none ./cpp.exe
	OMP_NUM_THREADS=8 aprun -cc none ./cpp.exe

fortran.exe : test.f90
	ftn $(FFLAGS) test.f90 -o fortran.exe

cpp.exe : test.cpp
	CC $(FFLAGS) test.cpp -o cpp.exe

clean :
	rm -f fortran.exe
	rm -f cpp.exe
