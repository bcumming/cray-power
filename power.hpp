#include <fstream>
#include <string>

namespace power {

static double read_pm_file(const std::string &fname) {
    double result = 0.;
    std::ifstream fid(fname.c_str());

    fid >> result;
    return result;
}

static double device_energy(void) {
    return read_pm_file("/sys/cray/pm_counters/device_energy");
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

} // namespace power
