#pragma once

#include "common.h"

namespace PyMKF {

// Settings and defaults
py::dict get_constants();
py::dict get_defaults();
json get_settings();
void set_settings(json settingsJson);
void reset_settings();
json get_default_models();

void register_settings_bindings(py::module& m);

} // namespace PyMKF
