#pragma once

#include "common.h"

namespace PyMKF {

json get_bobbins();
json get_bobbin_names();
json find_bobbin_by_name(json bobbinName);
json create_basic_bobbin(json coreDataJson, bool nullDimensions);
json create_basic_bobbin_by_thickness(json coreDataJson, double thickness);
json calculate_bobbin_data(json magneticJson);
json process_bobbin(json bobbinJson);
bool check_if_fits(json bobbinJson, double dimension, bool isHorizontalOrRadial);

void register_bobbin_bindings(py::module& m);

} // namespace PyMKF
