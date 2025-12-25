#pragma once
#include "common.h"

namespace PyMKF {

// Utility functions for processing and calculations
double resolve_dimension_with_tolerance(json dimensionWithToleranceJson);
json calculate_basic_processed_data(json waveformJson);
json calculate_harmonics(json waveformJson, double frequency);
json calculate_sampled_waveform(json waveformJson, double frequency);
json calculate_processed_data(json signalDescriptorJson, json sampledWaveformJson, bool includeDcComponent);

// Power calculation utilities  
double calculate_instantaneous_power(json excitationJson);
double calculate_rms_power(json excitationJson);

// Reflection utilities
json calculate_reflected_secondary(json primaryExcitationJson, double turnRatio);
json calculate_reflected_primary(json secondaryExcitationJson, double turnRatio);

// Array conversion utilities
py::list list_of_list_to_python_list(std::vector<std::vector<double>> arrayOfArrays);
std::vector<double> python_list_to_vector(py::list pythonList);

// Register all utility bindings
void register_utils_bindings(py::module& m);

} // namespace PyMKF