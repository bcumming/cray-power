#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#include <mpi.h>

#include <unistd.h>

namespace power {

static double read_pm_file(const std::string &fname) {
    double result = 0.;
    std::ifstream fid(fname.c_str());

    fid >> result;
    //std::cout << fname << " :: " << result << std::endl;
    return result;
}

static double device_energy(void) {
    return read_pm_file("/sys/cray/pm_counters/accel_energy");
}

static double energy() {
    return read_pm_file("/sys/cray/pm_counters/energy");
}

static double device_power() {
    return read_pm_file("/sys/cray/pm_counters/accel_power");
}

static double power() {
    return read_pm_file("/sys/cray/pm_counters/power");
}

static int num_nodes() {
    // find out the number of nodes
    char *ptr = std::getenv("SLURM_JOB_NUM_NODES");
    if(ptr) {
        return atoi(ptr);
    }
    else {
        return -1;
    }
}

// returns -1 if unable to determine number of nodes
static int ranks_per_node() {
    const int maxlen = 512;
    char name[maxlen];

    // check whether MPI has been initialized
    int is_initialized = 0;
    MPI_Initialized(&is_initialized);
    if(!is_initialized) {
        std::cerr << "ERROR : MPI not initialized" << std::endl;
        return -1;
    }

    // get MPI information
    int rank, size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // get hostname for this node
    int result = gethostname(name, maxlen);

    // error: return -1
    if(result)
        return -1;

    // get integer index for this node, by stripping off first 2 characters
    // on cray systems all compute nodes have hostname set as
    // nid#######
    int node = atoi(name+3);

    // gather list of node identifiers
    std::vector<int> node_ids(size);
    MPI_Allgather(&node, 1, MPI_INT, &node_ids[0], 1, MPI_INT, MPI_COMM_WORLD);

    // find first occurence of this ranks node
    return std::count(node_ids.begin(), node_ids.end(), node);
}

} // namespace power
