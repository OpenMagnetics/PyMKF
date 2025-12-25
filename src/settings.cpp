#include "settings.h"

namespace PyMKF {

py::dict get_constants() {
    py::dict constantsMap;
    constantsMap["residualGap"] = OpenMagnetics::constants.residualGap;
    constantsMap["minimumNonResidualGap"] = OpenMagnetics::constants.minimumNonResidualGap;
    constantsMap["vacuumPermeability"] = OpenMagnetics::constants.vacuumPermeability;
    constantsMap["vacuumPermittivity"] = OpenMagnetics::constants.vacuumPermittivity;
    constantsMap["spacerProtudingPercentage"] = OpenMagnetics::constants.spacerProtudingPercentage;
    constantsMap["coilPainterScale"] = OpenMagnetics::constants.coilPainterScale;
    constantsMap["minimumDistributedFringingFactor"] = OpenMagnetics::constants.minimumDistributedFringingFactor;
    constantsMap["maximumDistributedFringingFactor"] = OpenMagnetics::constants.maximumDistributedFringingFactor;
    constantsMap["initialGapLengthForSearching"] = OpenMagnetics::constants.initialGapLengthForSearching;
    constantsMap["roshenMagneticFieldStrengthStep"] = OpenMagnetics::constants.roshenMagneticFieldStrengthStep;
    constantsMap["foilToSectionMargin"] = OpenMagnetics::constants.foilToSectionMargin;
    constantsMap["planarToSectionMargin"] = OpenMagnetics::constants.planarToSectionMargin;
    return constantsMap;
}

py::dict get_defaults() {
    py::dict defaultsMap;
    json aux;
    to_json(aux, OpenMagnetics::defaults.coreLossesModelDefault);
    defaultsMap["coreLossesModelDefault"] = aux;
    to_json(aux, OpenMagnetics::defaults.coreTemperatureModelDefault);
    defaultsMap["coreTemperatureModelDefault"] = aux;
    to_json(aux, OpenMagnetics::defaults.reluctanceModelDefault);
    defaultsMap["reluctanceModelDefault"] = aux;
    to_json(aux, OpenMagnetics::defaults.magneticFieldStrengthModelDefault);
    defaultsMap["magneticFieldStrengthModelDefault"] = aux;
    to_json(aux, OpenMagnetics::defaults.magneticFieldStrengthFringingEffectModelDefault);
    defaultsMap["magneticFieldStrengthFringingEffectModelDefault"] = aux;
    defaultsMap["maximumProportionMagneticFluxDensitySaturation"] = OpenMagnetics::defaults.maximumProportionMagneticFluxDensitySaturation;
    defaultsMap["coreAdviserFrequencyReference"] = OpenMagnetics::defaults.coreAdviserFrequencyReference;
    defaultsMap["coreAdviserMagneticFluxDensityReference"] = OpenMagnetics::defaults.coreAdviserMagneticFluxDensityReference;
    defaultsMap["coreAdviserThresholdValidity"] = OpenMagnetics::defaults.coreAdviserThresholdValidity;
    defaultsMap["coreAdviserMaximumCoreTemperature"] = OpenMagnetics::defaults.coreAdviserMaximumCoreTemperature;
    defaultsMap["coreAdviserMaximumPercentagePowerCoreLosses"] = OpenMagnetics::defaults.coreAdviserMaximumPercentagePowerCoreLosses;
    defaultsMap["coreAdviserMaximumMagneticsAfterFiltering"] = OpenMagnetics::defaults.coreAdviserMaximumMagneticsAfterFiltering;
    defaultsMap["coreAdviserMaximumNumberStacks"] = OpenMagnetics::defaults.coreAdviserMaximumNumberStacks;
    defaultsMap["maximumCurrentDensity"] = OpenMagnetics::defaults.maximumCurrentDensity;
    defaultsMap["maximumEffectiveCurrentDensity"] = OpenMagnetics::defaults.maximumEffectiveCurrentDensity;
    defaultsMap["maximumNumberParallels"] = OpenMagnetics::defaults.maximumNumberParallels;
    defaultsMap["magneticFluxDensitySaturation"] = OpenMagnetics::defaults.magneticFluxDensitySaturation;
    defaultsMap["magnetizingInductanceThresholdValidity"] = OpenMagnetics::defaults.magnetizingInductanceThresholdValidity;
    defaultsMap["harmonicAmplitudeThreshold"] = OpenMagnetics::defaults.harmonicAmplitudeThreshold;
    defaultsMap["ambientTemperature"] = OpenMagnetics::defaults.ambientTemperature;
    defaultsMap["measurementFrequency"] = OpenMagnetics::defaults.measurementFrequency;
    defaultsMap["magneticFieldMirroringDimension"] = OpenMagnetics::defaults.magneticFieldMirroringDimension;
    defaultsMap["maximumCoilPattern"] = OpenMagnetics::defaults.maximumCoilPattern;
    to_json(aux, OpenMagnetics::defaults.defaultRoundWindowSectionsOrientation);
    defaultsMap["defaultRoundWindowSectionsOrientation"] = aux;
    to_json(aux, OpenMagnetics::defaults.defaultRectangularWindowSectionsOrientation);
    defaultsMap["defaultRectangularWindowSectionsOrientation"] = aux;
    defaultsMap["defaultEnamelledInsulationMaterial"] = OpenMagnetics::defaults.defaultEnamelledInsulationMaterial;
    defaultsMap["defaultInsulationMaterial"] = OpenMagnetics::defaults.defaultInsulationMaterial;
    defaultsMap["defaultLayerInsulationMaterial"] = OpenMagnetics::defaults.defaultLayerInsulationMaterial;
    defaultsMap["overlappingFactorSurroundingTurns"] = OpenMagnetics::defaults.overlappingFactorSurroundingTurns;
    to_json(aux, OpenMagnetics::defaults.commonWireStandard);
    defaultsMap["commonWireStandard"] = aux;
    return defaultsMap;
}

json get_settings() {
    try {
        if (OpenMagnetics::settings == nullptr) {
            OpenMagnetics::settings = OpenMagnetics::Settings::GetInstance();
        }
        json settingsJson;
        settingsJson["coilAllowMarginTape"] = OpenMagnetics::settings->get_coil_allow_margin_tape();
        settingsJson["coilAllowInsulatedWire"] = OpenMagnetics::settings->get_coil_allow_insulated_wire();
        settingsJson["coilFillSectionsWithMarginTape"] = OpenMagnetics::settings->get_coil_fill_sections_with_margin_tape();
        settingsJson["coilWindEvenIfNotFit"] = OpenMagnetics::settings->get_coil_wind_even_if_not_fit();
        settingsJson["coilDelimitAndCompact"] = OpenMagnetics::settings->get_coil_delimit_and_compact();
        settingsJson["coilTryRewind"] = OpenMagnetics::settings->get_coil_try_rewind();
        settingsJson["coilOnlyOneTurnPerLayerInContiguousRectangular"] = OpenMagnetics::settings->get_coil_only_one_turn_per_layer_in_contiguous_rectangular();
        settingsJson["useOnlyCoresInStock"] = OpenMagnetics::settings->get_use_only_cores_in_stock();
        settingsJson["coilMaximumLayersPlanar"] = OpenMagnetics::settings->get_coil_maximum_layers_planar();

        settingsJson["painterNumberPointsX"] = OpenMagnetics::settings->get_painter_number_points_x();
        settingsJson["painterNumberPointsY"] = OpenMagnetics::settings->get_painter_number_points_y();
        settingsJson["painterMode"] = OpenMagnetics::settings->get_painter_mode();
        settingsJson["painterLogarithmicScale"] = OpenMagnetics::settings->get_painter_logarithmic_scale();
        settingsJson["painterIncludeFringing"] = OpenMagnetics::settings->get_painter_include_fringing();
        if (OpenMagnetics::settings->get_painter_maximum_value_colorbar()) {
            settingsJson["painterMaximumValueColorbar"] = OpenMagnetics::settings->get_painter_maximum_value_colorbar();
        }
        if (OpenMagnetics::settings->get_painter_minimum_value_colorbar()) {
            settingsJson["painterMinimumValueColorbar"] = OpenMagnetics::settings->get_painter_minimum_value_colorbar();
        }
        settingsJson["painterColorLines"] = OpenMagnetics::settings->get_painter_color_lines();
        settingsJson["painterColorText"] = OpenMagnetics::settings->get_painter_color_text();
        settingsJson["painterColorFerrite"] = OpenMagnetics::settings->get_painter_color_ferrite();
        settingsJson["painterColorBobbin"] = OpenMagnetics::settings->get_painter_color_bobbin();
        settingsJson["painterColorCopper"] = OpenMagnetics::settings->get_painter_color_copper();
        settingsJson["painterColorInsulation"] = OpenMagnetics::settings->get_painter_color_insulation();
        settingsJson["painterColorMargin"] = OpenMagnetics::settings->get_painter_color_margin();
        settingsJson["painterMirroringDimension"] = OpenMagnetics::settings->get_painter_mirroring_dimension();
        settingsJson["magneticFieldNumberPointsX"] = OpenMagnetics::settings->get_magnetic_field_number_points_x();
        settingsJson["magneticFieldNumberPointsY"] = OpenMagnetics::settings->get_magnetic_field_number_points_y();
        settingsJson["magneticFieldIncludeFringing"] = OpenMagnetics::settings->get_magnetic_field_include_fringing();
        settingsJson["magneticFieldMirroringDimension"] = OpenMagnetics::settings->get_magnetic_field_mirroring_dimension();
        settingsJson["painterSimpleLitz"] = OpenMagnetics::settings->get_painter_simple_litz();
        settingsJson["painterAdvancedLitz"] = OpenMagnetics::settings->get_painter_advanced_litz();
        settingsJson["painterCciCoordinatesPath"] = OpenMagnetics::settings->get_painter_cci_coordinates_path();
        return settingsJson;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

void set_settings(json settingsJson) {
    try {
        if (OpenMagnetics::settings == nullptr) {
            OpenMagnetics::settings = OpenMagnetics::Settings::GetInstance();
        }
        OpenMagnetics::settings->set_coil_allow_margin_tape(settingsJson["coilAllowMarginTape"]);
        OpenMagnetics::settings->set_coil_allow_insulated_wire(settingsJson["coilAllowInsulatedWire"]);
        OpenMagnetics::settings->set_coil_fill_sections_with_margin_tape(settingsJson["coilFillSectionsWithMarginTape"]);
        OpenMagnetics::settings->set_coil_wind_even_if_not_fit(settingsJson["coilWindEvenIfNotFit"]);
        OpenMagnetics::settings->set_coil_delimit_and_compact(settingsJson["coilDelimitAndCompact"]);
        OpenMagnetics::settings->set_coil_try_rewind(settingsJson["coilTryRewind"]);
        OpenMagnetics::settings->set_coil_only_one_turn_per_layer_in_contiguous_rectangular(settingsJson["coilOnlyOneTurnPerLayerInContiguousRectangular"]);
        OpenMagnetics::settings->set_coil_maximum_layers_planar(settingsJson["coilMaximumLayersPlanar"]);

        OpenMagnetics::settings->set_painter_mode(settingsJson["painterMode"]);
        OpenMagnetics::settings->set_use_only_cores_in_stock(settingsJson["useOnlyCoresInStock"]);
        OpenMagnetics::settings->set_painter_number_points_x(settingsJson["painterNumberPointsX"]);
        OpenMagnetics::settings->set_painter_number_points_y(settingsJson["painterNumberPointsY"]);
        OpenMagnetics::settings->set_painter_logarithmic_scale(settingsJson["painterLogarithmicScale"]);
        OpenMagnetics::settings->set_painter_include_fringing(settingsJson["painterIncludeFringing"]);
        if (settingsJson.contains("painterMaximumValueColorbar")) {
            OpenMagnetics::settings->set_painter_maximum_value_colorbar(settingsJson["painterMaximumValueColorbar"]);
        }
        if (settingsJson.contains("painterMinimumValueColorbar")) {
            OpenMagnetics::settings->set_painter_minimum_value_colorbar(settingsJson["painterMinimumValueColorbar"]);
        }
        OpenMagnetics::settings->set_painter_color_lines(settingsJson["painterColorLines"]);
        OpenMagnetics::settings->set_painter_color_text(settingsJson["painterColorText"]);
        OpenMagnetics::settings->set_painter_color_ferrite(settingsJson["painterColorFerrite"]);
        OpenMagnetics::settings->set_painter_color_bobbin(settingsJson["painterColorBobbin"]);
        OpenMagnetics::settings->set_painter_color_copper(settingsJson["painterColorCopper"]);
        OpenMagnetics::settings->set_painter_color_insulation(settingsJson["painterColorInsulation"]);
        OpenMagnetics::settings->set_painter_color_margin(settingsJson["painterColorMargin"]);
        OpenMagnetics::settings->set_painter_mirroring_dimension(settingsJson["painterMirroringDimension"]);
        OpenMagnetics::settings->set_magnetic_field_number_points_x(settingsJson["magneticFieldNumberPointsX"]);
        OpenMagnetics::settings->set_magnetic_field_number_points_y(settingsJson["magneticFieldNumberPointsY"]);
        OpenMagnetics::settings->set_magnetic_field_include_fringing(settingsJson["magneticFieldIncludeFringing"]);
        OpenMagnetics::settings->set_magnetic_field_mirroring_dimension(settingsJson["magneticFieldMirroringDimension"]);
        OpenMagnetics::settings->set_painter_simple_litz(settingsJson["painterSimpleLitz"]);
        OpenMagnetics::settings->set_painter_advanced_litz(settingsJson["painterAdvancedLitz"]);
        OpenMagnetics::settings->set_painter_cci_coordinates_path(settingsJson["painterCciCoordinatesPath"]);
    }
    catch (const std::exception &exc) {
        throw std::runtime_error("Exception: " + std::string{exc.what()});
    }
}

void reset_settings() {
    if (OpenMagnetics::settings == nullptr) {
        OpenMagnetics::settings = OpenMagnetics::Settings::GetInstance();
    }
    OpenMagnetics::settings->reset();
}

json get_default_models() {
    try {
        json models;
        models["coreLosses"] = magic_enum::enum_name(OpenMagnetics::defaults.coreLossesModelDefault);
        models["coreTemperature"] = magic_enum::enum_name(OpenMagnetics::defaults.coreTemperatureModelDefault);
        models["reluctance"] = magic_enum::enum_name(OpenMagnetics::defaults.reluctanceModelDefault);
        models["magneticFieldStrength"] = magic_enum::enum_name(OpenMagnetics::defaults.magneticFieldStrengthModelDefault);
        models["magneticFieldStrengthFringingEffect"] = magic_enum::enum_name(OpenMagnetics::defaults.magneticFieldStrengthFringingEffectModelDefault);
        return models;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

void register_settings_bindings(py::module& m) {
    m.def("get_constants", &get_constants);
    m.def("get_defaults", &get_defaults);
    m.def("get_settings", &get_settings);
    m.def("set_settings", &set_settings);
    m.def("reset_settings", &reset_settings);
    m.def("get_default_models", &get_default_models);
}

} // namespace PyMKF