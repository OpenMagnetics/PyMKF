#pragma once
#include "common.h"

namespace PyMKF {

// Core plotting functions
json plot_core(json coreDataJson, bool useColors = false);
json plot_core_2d(json coreDataJson, int axis = 1, json windingWindowsJson = json(), bool useColors = false);
json plot_field_2d(json coreDataJson, json operatingPointJson, int axis = 1, bool useColors = false, double currentReference = -1);
json plot_field_map(json coreDataJson, json operatingPointJson, int axis = 1, int mirroringDimension = 1, bool useColors = false);

// Wire and bobbin plotting functions
json plot_wire(json wireDataJson, bool useColors = false);
json plot_bobbin(json bobbinDataJson, bool useColors = false);

// Core Piece plotting functions
json plot_core_piece(json corePieceDataJson, bool useColors = false);
json plot_core_piece_2d(json corePieceDataJson, int axis = 1, json windingWindowsJson = json(), bool useColors = false);

// Coil plotting functions
json plot_coil_2d(json coilDataJson, int axis = 1, bool mirroredTurns = true, bool useColors = false);

// Register all plotting bindings
void register_plotting_bindings(py::module& m);

} // namespace PyMKF