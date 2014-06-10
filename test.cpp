#include <iostream>
#include <vector>
#include <cmath>

#include <omp.h>

#include"power.hpp"

int main(void) {
    const int n=4000000;
    const int num_runs=2500;

    std::vector<double> a(n);
    std::vector<double> b(n);
    std::vector<double> c(n);

    std::cout << omp_get_max_threads() << " threads" << std::endl;

    for(int i=0; i<n; i++) {
        a[i] = std::exp(i/(2.0*n));
        b[i] = i/2.0;
        c[i] = 1.;
    }

    // get energy counter at startup
    double energy_init        = power::energy();
    double device_energy_init = power::device_energy();
    double timespent          = -omp_get_wtime();

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
    timespent                 += omp_get_wtime();
    double energy_final        = power::energy();
    double device_energy_final = power::device_energy();

    std::cout << "that took " << energy_final-energy_init << " Joules" << std::endl;
    std::cout << "at rate " << (energy_final-energy_init)/timespent << " Watts" << std::endl;
    std::cout << "that took " << timespent << " seconds" << std::endl;
    std::cout << ssum << std::endl;

    return 0;
}
