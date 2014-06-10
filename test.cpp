#include <iostream>
#include <vector>

#include <cmath>
#include <cstdlib>
#include <cassert>

#include <mpi.h>
#include <omp.h>

#include"power.hpp"

int main(int argc, char** argv) {
    const int n=2000000;
    const int num_runs=1000;

    // initialize MPI
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    std::vector<double> a(n);
    std::vector<double> b(n);
    std::vector<double> c(n);

    const int nodes = power::num_nodes();
    // assert that the number of nodes is sane
    assert(nodes<=size && nodes>0);

    const int ranks_per_node = size/nodes;
    if(!rank) {
        std::cout << size << " MPI ranks with "
                  << omp_get_max_threads() << " threads on "
                  << nodes << " nodes, that is "
                  << ranks_per_node << " MPI tasks per node"
                  << std::endl;
    }

    for(int i=0; i<n; i++) {
        a[i] = std::exp(i/(2.0*n));
        b[i] = i/2.0;
        c[i] = 1.;
    }

    // get energy counter at startup
    MPI_Barrier(MPI_COMM_WORLD);
    double node_energy    = -power::energy();
    double device_energy  = -power::device_energy();
    double timespent      = -omp_get_wtime();

    #pragma omp parallel
    for(int run=0; run<num_runs; run++) {
        #pragma omp for
        for(int i=0; i<n; ++i)
            a[i] = a[i] + b[i]*c[i];
    }


    double ssum=0.;
    #pragma omp parallel for reduction(+:ssum)
    for(int i=0; i<n; ++i)
        ssum = ssum+a[i];

    // get energy counter at end
    MPI_Barrier(MPI_COMM_WORLD);
    timespent       += omp_get_wtime();
    node_energy     += power::energy();
    device_energy   += power::device_energy();

    node_energy   /= double(ranks_per_node);
    device_energy /= double(ranks_per_node);
    //std::cout << "MPI " << rank << " :: " << node_energy << " | " << device_energy << std::endl;

    double total_node_energy = 0.;
    double total_device_energy = 0.;
    MPI_Reduce(&node_energy, &total_node_energy, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&device_energy, &total_device_energy, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if(!rank) {
        std::cout << "======================== node   ========================" << std::endl;
        std::cout << total_node_energy << " Joules, "
                  << total_node_energy/timespent << " Watts, "
                  << timespent << " seconds, ("
                  << ssum << ")" << std::endl;
        std::cout << "======================== device ========================" << std::endl;
        std::cout << total_device_energy << " Joules, "
                  << total_device_energy/timespent << " Watts, "
                  << timespent << " seconds, ("
                  << ssum << ")" << std::endl;
        std::cout << "========================================================" << std::endl;
    }

    MPI_Finalize();

    return 0;
}
