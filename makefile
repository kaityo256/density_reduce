all: a.out b.out

CC=g++

a.out: dump2dat.cpp
	$(CC) $< -o $@

b.out: dump2dat_mpi.cpp
	$(CC) $< -lmpi -lmpi_cxx -o $@

test: a.out b.out
	ruby makedump.rb
	./a.out
	mpirun --oversubscribe -np 4 ./b.out
	ruby dat2vtk.rb test.dat
	ruby dat2vtk.rb test_mpi.dat
	diff test.vtk test_mpi.vtk

clean:
	rm -f a.out b.out
	rm -f *.vtk *.dump rank*.txt
	rm -f test.dat test_mpi.dat
