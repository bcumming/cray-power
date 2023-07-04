test : fortran.exe cpp.exe
	OMP_NUM_THREADS=1 srun --partition=nvgpu ./fortran.exe
	echo
	OMP_NUM_THREADS=8 srun --partition=nvgpu ./fortran.exe
	#OMP_NUM_THREADS=1 srun --partition=nvgpu ./cpp.exe
	#OMP_NUM_THREADS=8 srun --partition=nvgpu ./cpp.exe

fortran.exe : test.f90
	mpifort -O2 test.f90 -fopenmp -o fortran.exe

cpp.exe : test.cpp power.hpp
	mpic++ -O2 test.cpp -fopenmp -o cpp.exe

clean :
	rm -f fortran.exe
	rm -f cpp.exe
