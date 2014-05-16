ifeq ($(PE_ENV),GNU)
	FFLAGS=-O2 -fopenmp
endif
ifeq ($(PE_ENV),CRAY)
	FFLAGS= -O2
endif

test : fortran.exe
	OMP_NUM_THREADS=1 aprun -cc none ./fortran.exe
	OMP_NUM_THREADS=8 aprun -cc none ./fortran.exe

fortran.exe : test.f90
	ftn $(FFLAGS) test.f90 -o fortran.exe

clean :
	rm -f fortran.exe
