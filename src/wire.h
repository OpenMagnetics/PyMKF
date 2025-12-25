#pragma once

#include "common.h"

namespace PyMKF {

// Wire retrieval
json get_wires();
json get_wire_names();
json get_wire_materials();
json get_wire_material_names();
json find_wire_by_name(json wireName);
json find_wire_material_by_name(json wireMaterialName);
json find_wire_by_dimension(double dimension, json wireTypeJson, json wireStandardJson);

// Wire data functions
json get_wire_data(json windingDataJson);
json get_wire_data_by_name(std::string name);
json get_wire_data_by_standard_name(std::string standardName);
json get_strand_by_standard_name(std::string standardName);
double get_wire_conducting_diameter_by_standard_name(std::string standardName);

// Wire dimensions
double get_wire_outer_width_rectangular(double conductingWidth, int grade, json wireStandardJson);
double get_wire_outer_height_rectangular(double conductingHeight, int grade, json wireStandardJson);
double get_wire_outer_diameter_bare_litz(double conductingDiameter, int numberConductors, int grade, json wireStandardJson);
double get_wire_outer_diameter_served_litz(double conductingDiameter, int numberConductors, int grade, int numberLayers, json wireStandardJson);
double get_wire_outer_diameter_insulated_litz(double conductingDiameter, int numberConductors, int numberLayers, double thicknessLayers, int grade, json wireStandardJson);
double get_wire_outer_diameter_enamelled_round(double conductingDiameter, int grade, json wireStandardJson);
double get_wire_outer_diameter_insulated_round(double conductingDiameter, int numberLayers, double thicknessLayers, json wireStandardJson);
std::vector<double> get_outer_dimensions(json wireJson);

// Wire utilities
json get_equivalent_wire(json oldWireJson, json newWireTypeJson, double effectivefrequency);
json get_coating(json wireJson);
json get_coating_label(json wireJson);
json get_wire_coating_by_label(std::string label);
std::vector<std::string> get_coating_labels_by_type(json wireTypeJson);
double get_coating_thickness(json wireJson);
double get_coating_relative_permittivity(json wireJson);
json get_coating_insulation_material(json wireJson);

// Wire availability
std::vector<std::string> get_available_wires();
std::vector<std::string> get_unique_wire_diameters(json wireStandardJson);
std::vector<std::string> get_available_wire_types();
std::vector<std::string> get_available_wire_standards();

void register_wire_bindings(py::module& m);

} // namespace PyMKF
