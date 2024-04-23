#include <cmath>
#include <optional>
#include <vector>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "mmcmb/mmcmb.h"
#include <mmcmb/fpga_mailbox_layout.h>

namespace py = pybind11;

struct FruStatus {
    explicit FruStatus(const mb_fru_status_t& stat)
        : present{static_cast<bool>(stat.present)}
        , compatible{static_cast<bool>(stat.compatible)}
        , powered{static_cast<bool>(stat.powered)}
        , failure{static_cast<bool>(stat.failure)}
        , num_temp_sensors{stat.num_temp_sensors} {
        temperature.resize(num_temp_sensors);
        for (unsigned int i = 0; i < num_temp_sensors; i++) {
            temperature[i] = stat.temperature[i];
        }
    }

    bool present;
    bool compatible;
    bool powered;
    bool failure;
    unsigned int num_temp_sensors;
    std::vector<unsigned int> temperature;
};

std::optional<FruStatus> get_fru_status(size_t fru_id) {
    mb_fru_status_t stat;
    if (!mb_get_fru_status(&stat, fru_id)) {
        return std::nullopt;
    }
    return FruStatus{stat};
}

PYBIND11_MODULE(pymmcmb, m) {
    m.doc() = "Python binding for libmmcmb";
    // m.def("add", &add, "A function that adds two numbers");
    m.def("check_magic", mb_check_magic, "Check MMC Mailbox magic string");

    py::class_<FruStatus>(m, "FruStatus")
        .def_readonly("present", &FruStatus::present)
        .def_readonly("compatible", &FruStatus::compatible)
        .def_readonly("powered", &FruStatus::powered)
        .def_readonly("failure", &FruStatus::failure)
        .def_readonly("num_temp_sensors", &FruStatus::num_temp_sensors)
        .def_readonly("temperature", &FruStatus::temperature);

    m.def("get_fru_status", &get_fru_status, "Get FRU status");
}
