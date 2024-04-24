#include <cmath>
#include <optional>
#include <vector>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "mmcmb/mmcmb.h"
#include "mmcmb/mmcmb.hpp"
#include <mmcmb/fpga_mailbox_layout.h>

namespace py = pybind11;

struct FruStatus {
    explicit FruStatus(const mb_fru_status_t& stat)
        : present{static_cast<bool>(stat.present)}
        , compatible{static_cast<bool>(stat.compatible)}
        , powered{static_cast<bool>(stat.powered)}
        , failure{static_cast<bool>(stat.failure)} {
        temperature.resize(stat.num_temp_sensors);
        for (unsigned int i = 0; i < stat.num_temp_sensors; i++) {
            temperature[i] = float(stat.temperature[i]) / 100.f;
        }
    }

    bool present;
    bool compatible;
    bool powered;
    bool failure;
    std::vector<float> temperature;
};

static std::optional<FruStatus> get_fru_status(size_t fru_id) {
    mb_fru_status_t stat;
    if (!mb_get_fru_status(&stat, fru_id)) {
        return std::nullopt;
    }
    return FruStatus{stat};
}

static std::optional<mb_mmc_information_t> get_mmc_information() {
    mb_mmc_information_t info;
    if (!mb_get_mmc_information(&info)) {
        return std::nullopt;
    }
    return info;
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
        .def_readonly("temperature", &FruStatus::temperature);

    py::class_<mb_version_number_t>(m, "VersionNumber")
        .def_readonly("major", &mb_version_number_t::major)
        .def_readonly("minor", &mb_version_number_t::minor)
        .def("__str__", [](const mb_version_number_t& v) {
            return std::to_string(v.major) + "." + std::to_string(v.minor);
        });

    py::class_<mb_mmc_information_t>(m, "MMCInformation")
        .def_readonly("application_version", &mb_mmc_information_t::application_version)
        .def_readonly("library_version", &mb_mmc_information_t::library_version)
        .def_readonly("cpld_board_version", &mb_mmc_information_t::cpld_board_version)
        .def_readonly("cpld_library_version", &mb_mmc_information_t::cpld_library_version)
        .def_readonly("stamp_hw_revision", &mb_mmc_information_t::stamp_hw_revision)
        .def_readonly("amc_slot_nr", &mb_mmc_information_t::amc_slot_nr)
        .def_readonly("ipmb_addr", &mb_mmc_information_t::ipmb_addr)
        .def_property_readonly(
            "board_name",
            [](const mb_mmc_information_t& inf) { return mb_to_str(inf.board_name); })
        .def_readonly("vendor_id", &mb_mmc_information_t::vendor_id)
        .def_readonly("product_id", &mb_mmc_information_t::product_id)
        .def_readonly("mmc_uptime", &mb_mmc_information_t::mmc_uptime);

    m.def("get_fru_status", &get_fru_status, "Get FRU status");
    m.def("get_mmc_information", &get_mmc_information, "Get MMC information");
}
