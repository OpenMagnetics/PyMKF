#pragma once

#include "common.h"

namespace PyMKF {

// Simulation
json simulate(json inputsJson, json magneticJson, json modelsData);

// Export
ordered_json export_magnetic_as_subcircuit(json magneticJson);

// Autocomplete
json mas_autocomplete(json masJson, json configuration);
json magnetic_autocomplete(json magneticJson, json configuration);

// Input processing
json process_inputs(json inputsJson);
json extract_operating_point(json fileJson, size_t numberWindings, double frequency, double desiredMagnetizingInductance, json mapColumnNamesJson);
json extract_map_column_names(json fileJson, size_t numberWindings, double frequency);
json extract_column_names(json fileJson);

void register_simulation_bindings(py::module& m);

} // namespace PyMKF
