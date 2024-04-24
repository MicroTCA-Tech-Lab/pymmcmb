#include <cmath>
#include <optional>
#include <vector>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "mmcmb/mmcmb.h"
#include "mmcmb/mmcmb.hpp"
#include <mmcmb/fpga_mailbox_layout.h>

namespace py = pybind11;

// Wrapper class to wrap C-bitfields/arrays into Python friendly types
struct FruStatus {
    explicit FruStatus(const mb_fru_status_t& stat)
        : present{static_cast<bool>(stat.present)}
        , compatible{static_cast<bool>(stat.compatible)}
        , powered{static_cast<bool>(stat.powered)}
        , failure{static_cast<bool>(stat.failure)} {
        temperature.resize(stat.num_temp_sensors);
        for (unsigned int i = 0; i < stat.num_temp_sensors; i++) {
            temperature[i] = double(stat.temperature[i]) / 100.f;
        }
    }

    bool present;
    bool compatible;
    bool powered;
    bool failure;
    std::vector<double> temperature;
};

// Getter functions for Python friendly retrieval of data structures

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

static std::optional<mb_fru_description_t> get_fru_description(size_t fru_id) {
    mb_fru_description_t desc;
    if (!mb_get_fru_description(&desc, fru_id)) {
        return std::nullopt;
    }
    return desc;
}

static std::vector<mb_mmc_sensor_t> get_mmc_sensors(size_t first_sensor, size_t n) {
    std::vector<mb_mmc_sensor_t> result;
    for (size_t i = first_sensor; i < first_sensor + n; i++) {
        mb_mmc_sensor_t sen;
        if (!mb_get_mmc_sensors(&sen, i, 1)) {
            break;
        }
        if (sen.name[0] == '\0') {
            break;
        }
        result.push_back(sen);
    }
    return result;
}

PYBIND11_MODULE(pymmcmb, m) {
    m.doc() = "Python binding for libmmcmb";

    py::class_<FruStatus>(m, "FruStatus")
        .def_readonly("present", &FruStatus::present)
        .def_readonly("compatible", &FruStatus::compatible)
        .def_readonly("powered", &FruStatus::powered)
        .def_readonly("failure", &FruStatus::failure)
        .def_readonly("temperature", &FruStatus::temperature);

    py::class_<mb_fru_description_t>(m, "FruDescription")
        .def_property_readonly("uid",
                               [](const mb_fru_description_t& d) {
                                   return py::bytearray(reinterpret_cast<const char*>(d.uid),
                                                        sizeof(d.uid));
                               })
        .def_property_readonly(
            "manufacturer",
            [](const mb_fru_description_t& d) { return mb_to_str(d.manufacturer); })
        .def_property_readonly("product",
                               [](const mb_fru_description_t& d) { return mb_to_str(d.product); })
        .def_property_readonly("part_nr",
                               [](const mb_fru_description_t& d) { return mb_to_str(d.part_nr); })
        .def_property_readonly("serial_nr",
                               [](const mb_fru_description_t& d) { return mb_to_str(d.serial_nr); })
        .def_property_readonly("version",
                               [](const mb_fru_description_t& d) { return mb_to_str(d.version); });

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

    py::class_<mb_mmc_sensor_t>(m, "MMCSensor")
        .def_property_readonly("name",
                               [](const mb_mmc_sensor_t& sen) { return mb_to_str(sen.name); })
        .def_readonly("reading", &mb_mmc_sensor_t::reading)
        .def("__str__", [](const mb_mmc_sensor_t& s) {
            return mb_to_str(s.name) + ": " + std::to_string(s.reading);
        });

    m.def("check_magic", mb_check_magic, "Check MMC Mailbox magic string");
    m.def("get_mmc_information", &get_mmc_information, "Get MMC information");
    m.def("get_mmc_sensors",
          &get_mmc_sensors,
          "Get MMC sensors",
          py::arg("first_sensor") = 0,
          py::arg("n") = MAX_SENS_MMC);
    m.def("get_fru_description", &get_fru_description, "Get FRU description");
    m.def("get_fru_status", &get_fru_status, "Get FRU status");
}
