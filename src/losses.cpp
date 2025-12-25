#include "losses.h"

namespace PyMKF {

json calculate_core_losses(json coreData, json coilData, json inputsData, json modelsData) {
    OpenMagnetics::Core core(coreData);
    OpenMagnetics::Coil coil(coilData);
    OpenMagnetics::Inputs inputs(inputsData);
    auto operatingPoint = inputs.get_operating_point(0);
    OperatingPointExcitation excitation = operatingPoint.get_excitations_per_winding()[0];
    double magnetizingInductance = OpenMagnetics::resolve_dimensional_values(inputs.get_design_requirements().get_magnetizing_inductance());
    if (!excitation.get_current()) {
        auto magnetizingCurrent = OpenMagnetics::Inputs::calculate_magnetizing_current(excitation, magnetizingInductance, true, 0.0);
        excitation.set_current(magnetizingCurrent);
        operatingPoint.get_mutable_excitations_per_winding()[0] = excitation;
    }

    std::map<std::string, std::string> models = modelsData.get<std::map<std::string, std::string>>();

    auto reluctanceModelName = OpenMagnetics::defaults.reluctanceModelDefault;
    if (models.find("reluctance") != models.end()) {
        OpenMagnetics::from_json(models["reluctance"], reluctanceModelName);
    }
    auto coreLossesModelName = OpenMagnetics::defaults.coreLossesModelDefault;
    if (models.find("coreLosses") != models.end()) {
        OpenMagnetics::from_json(models["coreLosses"], coreLossesModelName);
    }
    auto coreTemperatureModelName = OpenMagnetics::defaults.coreTemperatureModelDefault;
    if (models.find("coreTemperature") != models.end()) {
        OpenMagnetics::from_json(models["coreTemperature"], coreTemperatureModelName);
    }

    OpenMagnetics::Magnetic magnetic;
    magnetic.set_core(core);
    magnetic.set_coil(coil);

    OpenMagnetics::MagneticSimulator magneticSimulator;
    magneticSimulator.set_core_losses_model_name(coreLossesModelName);
    magneticSimulator.set_core_temperature_model_name(coreTemperatureModelName);
    magneticSimulator.set_reluctance_model_name(reluctanceModelName);
    auto coreLossesOutput = magneticSimulator.calculate_core_losses(operatingPoint, magnetic);
    json result;
    to_json(result, coreLossesOutput);

    OpenMagnetics::MagnetizingInductance magnetizingInductanceObj(reluctanceModelName);
    auto magneticFluxDensity = magnetizingInductanceObj.calculate_inductance_and_magnetic_flux_density(core, coil, &operatingPoint).second;

    result["magneticFluxDensityPeak"] = magneticFluxDensity.get_processed().value().get_peak().value();
    result["magneticFluxDensityAcPeak"] = magneticFluxDensity.get_processed().value().get_peak().value() - magneticFluxDensity.get_processed().value().get_offset();
    result["voltageRms"] = operatingPoint.get_mutable_excitations_per_winding()[0].get_voltage().value().get_processed().value().get_rms().value();
    result["currentRms"] = operatingPoint.get_mutable_excitations_per_winding()[0].get_current().value().get_processed().value().get_rms().value();
    result["apparentPower"] = operatingPoint.get_mutable_excitations_per_winding()[0].get_voltage().value().get_processed().value().get_rms().value() * operatingPoint.get_mutable_excitations_per_winding()[0].get_current().value().get_processed().value().get_rms().value();
    if (coreLossesOutput.get_temperature()) {
        result["maximumCoreTemperature"] = coreLossesOutput.get_temperature().value();
        result["maximumCoreTemperatureRise"] = coreLossesOutput.get_temperature().value() - operatingPoint.get_conditions().get_ambient_temperature();
    }

    return result;
}

json get_core_losses_model_information(json material) {
    json info;
    info["information"] = OpenMagnetics::CoreLossesModel::get_models_information();
    info["errors"] = OpenMagnetics::CoreLossesModel::get_models_errors();
    info["internal_links"] = OpenMagnetics::CoreLossesModel::get_models_internal_links();
    info["external_links"] = OpenMagnetics::CoreLossesModel::get_models_external_links();
    info["available_models"] = OpenMagnetics::CoreLossesModel::get_methods_string(material);
    return info;
}

json get_core_temperature_model_information() {
    json info;
    info["information"] = OpenMagnetics::CoreTemperatureModel::get_models_information();
    info["errors"] = OpenMagnetics::CoreTemperatureModel::get_models_errors();
    info["internal_links"] = OpenMagnetics::CoreTemperatureModel::get_models_internal_links();
    info["external_links"] = OpenMagnetics::CoreTemperatureModel::get_models_external_links();
    return info;
}

json calculate_steinmetz_coefficients(json dataJson, json rangesJson) {
    try {
        std::vector<std::pair<double, double>> ranges;
        for (auto rangeJson : rangesJson) {
            std::pair<double, double> range{rangeJson[0], rangeJson[1]};
            ranges.push_back(range);
        }
        std::vector<VolumetricLossesPoint> data;
        for (auto datumJson : dataJson) {
            VolumetricLossesPoint datum(datumJson);
            data.push_back(datum);
        }

        auto [coefficientsPerRange, errorPerRange] = OpenMagnetics::CoreLossesSteinmetzModel::calculate_steinmetz_coefficients(data, ranges);

        json result;
        to_json(result, coefficientsPerRange);
        return result;
    }
    catch (const std::exception &exc) {
        return "Exception: " + std::string{exc.what()};
    }
}

json calculate_steinmetz_coefficients_with_error(json dataJson, json rangesJson) {
    try {
        std::vector<std::pair<double, double>> ranges;
        for (auto rangeJson : rangesJson) {
            std::pair<double, double> range{rangeJson[0], rangeJson[1]};
            ranges.push_back(range);
        }
        std::vector<VolumetricLossesPoint> data;
        for (auto datumJson : dataJson) {
            VolumetricLossesPoint datum(datumJson);
            data.push_back(datum);
        }

        auto [coefficientsPerRange, errorPerRange] = OpenMagnetics::CoreLossesSteinmetzModel::calculate_steinmetz_coefficients(data, ranges);

        json aux;
        to_json(aux, coefficientsPerRange);
        json result;
        result["coefficientsPerRange"] = aux;
        result["errorPerRange"] = errorPerRange;
        return result;
    }
    catch (const std::exception &exc) {
        return "Exception: " + std::string{exc.what()};
    }
}

json calculate_winding_losses(json magneticJson, json operatingPointJson, double temperature) {
    try {
        OpenMagnetics::Magnetic magnetic(magneticJson);
        OperatingPoint operatingPoint(operatingPointJson);

        auto windingLossesOutput = OpenMagnetics::WindingLosses().calculate_losses(magnetic, operatingPoint, temperature);

        json result;
        to_json(result, windingLossesOutput);
        return result;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

json calculate_ohmic_losses(json coilJson, json operatingPointJson, double temperature) {
    try {
        OpenMagnetics::Coil coil(coilJson, false);
        OperatingPoint operatingPoint(operatingPointJson);

        auto windingLossesOutput = OpenMagnetics::WindingOhmicLosses::calculate_ohmic_losses(coil, operatingPoint, temperature);

        json result;
        to_json(result, windingLossesOutput);
        return result;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

json calculate_magnetic_field_strength_field(json operatingPointJson, json magneticJson) {
    try {
        OpenMagnetics::Magnetic magnetic(magneticJson);
        OperatingPoint operatingPoint(operatingPointJson);
        OpenMagnetics::MagneticField magneticField;

        auto windingWindowMagneticStrengthFieldOutput = magneticField.calculate_magnetic_field_strength_field(operatingPoint, magnetic);

        json result;
        to_json(result, windingWindowMagneticStrengthFieldOutput);
        return result;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

json calculate_proximity_effect_losses(json coilJson, double temperature, json windingLossesOutputJson, json windingWindowMagneticStrengthFieldOutputJson) {
    try {
        OpenMagnetics::Coil coil(coilJson, false);
        WindingLossesOutput windingLossesOutput(windingLossesOutputJson);
        WindingWindowMagneticStrengthFieldOutput windingWindowMagneticStrengthFieldOutput(windingWindowMagneticStrengthFieldOutputJson);

        auto windingLossesOutputOutput = OpenMagnetics::WindingProximityEffectLosses::calculate_proximity_effect_losses(coil, temperature, windingLossesOutput, windingWindowMagneticStrengthFieldOutput);

        json result;
        to_json(result, windingLossesOutputOutput);
        return result;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

json calculate_skin_effect_losses(json coilJson, json windingLossesOutputJson, double temperature) {
    try {
        OpenMagnetics::Coil coil(coilJson, false);
        WindingLossesOutput windingLossesOutput(windingLossesOutputJson);

        auto windingLossesOutputOutput = OpenMagnetics::WindingSkinEffectLosses::calculate_skin_effect_losses(coil, temperature, windingLossesOutput);
        json result;
        to_json(result, windingLossesOutputOutput);
        return result;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

json calculate_skin_effect_losses_per_meter(json wireJson, json currentJson, double temperature, double currentDivider) {
    try {
        OpenMagnetics::Wire wire(wireJson);
        SignalDescriptor current(currentJson);

        auto skinEffectLossesPerMeter = OpenMagnetics::WindingSkinEffectLosses::calculate_skin_effect_losses_per_meter(wire, current, temperature, currentDivider);

        json result = skinEffectLossesPerMeter;
        return result;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

double calculate_dc_resistance_per_meter(json wireJson, double temperature) {
    OpenMagnetics::Wire wire(wireJson);
    auto dcResistancePerMeter = OpenMagnetics::WindingOhmicLosses::calculate_dc_resistance_per_meter(wire, temperature);
    return dcResistancePerMeter;
}

double calculate_dc_losses_per_meter(json wireJson, json currentJson, double temperature) {
    OpenMagnetics::Wire wire(wireJson);
    SignalDescriptor current(currentJson);
    auto dcLossesPerMeter = OpenMagnetics::WindingOhmicLosses::calculate_ohmic_losses_per_meter(wire, current, temperature);
    return dcLossesPerMeter;
}

double calculate_skin_ac_factor(json wireJson, json currentJson, double temperature) {
    OpenMagnetics::Wire wire(wireJson);
    SignalDescriptor current(currentJson);
    auto dcLossesPerMeter = OpenMagnetics::WindingOhmicLosses::calculate_ohmic_losses_per_meter(wire, current, temperature);
    auto [skinLossesPerMeter, _] = OpenMagnetics::WindingSkinEffectLosses::calculate_skin_effect_losses_per_meter(wire, current, temperature);
    auto skinAcFactor = (skinLossesPerMeter + dcLossesPerMeter) / dcLossesPerMeter;
    return skinAcFactor;
}

double calculate_skin_ac_losses_per_meter(json wireJson, json currentJson, double temperature) {
    OpenMagnetics::Wire wire(wireJson);
    SignalDescriptor current(currentJson);
    auto [skinLossesPerMeter, _] = OpenMagnetics::WindingSkinEffectLosses::calculate_skin_effect_losses_per_meter(wire, current, temperature);
    return skinLossesPerMeter;
}

double calculate_skin_ac_resistance_per_meter(json wireJson, json currentJson, double temperature) {
    OpenMagnetics::Wire wire(wireJson);
    SignalDescriptor current(currentJson);
    auto dcLossesPerMeter = OpenMagnetics::WindingOhmicLosses::calculate_ohmic_losses_per_meter(wire, current, temperature);
    auto [skinLossesPerMeter, _] = OpenMagnetics::WindingSkinEffectLosses::calculate_skin_effect_losses_per_meter(wire, current, temperature);
    auto skinAcFactor = (skinLossesPerMeter + dcLossesPerMeter) / dcLossesPerMeter;
    auto dcResistancePerMeter = OpenMagnetics::WindingOhmicLosses::calculate_dc_resistance_per_meter(wire, temperature);

    return dcResistancePerMeter * skinAcFactor;
}

double calculate_effective_current_density(json wireJson, json currentJson, double temperature) {
    OpenMagnetics::Wire wire(wireJson);
    SignalDescriptor current(currentJson);
    auto effectiveCurrentDensity = wire.calculate_effective_current_density(current, temperature);

    return effectiveCurrentDensity;
}

double calculate_effective_skin_depth(std::string materialName, json currentJson, double temperature) {
    try {
        SignalDescriptor current(currentJson);

        if (!current.get_processed()->get_effective_frequency()) {
            throw std::runtime_error("Current processed is missing field effective frequency");
        }
        auto currentEffectiveFrequency = current.get_processed()->get_effective_frequency().value();
        double effectiveSkinDepth = OpenMagnetics::WindingSkinEffectLosses::calculate_skin_depth(materialName, currentEffectiveFrequency, temperature);
        return effectiveSkinDepth;
    }
    catch(const std::exception& ex) {
        return -1;
    }
}

void register_losses_bindings(py::module& m) {
    // Core losses
    m.def("calculate_core_losses", &calculate_core_losses, "Calculate core losses for given operating conditions");
    m.def("get_core_losses_model_information", &get_core_losses_model_information, "Get information about available core loss models");
    m.def("get_core_temperature_model_information", &get_core_temperature_model_information, "Get information about core temperature models");
    m.def("calculate_steinmetz_coefficients", &calculate_steinmetz_coefficients, "Calculate Steinmetz coefficients from loss data");
    m.def("calculate_steinmetz_coefficients_with_error", &calculate_steinmetz_coefficients_with_error,
        "Calculate Steinmetz coefficients with error estimation");

    // Winding losses
    m.def("calculate_winding_losses", &calculate_winding_losses, "Calculate total winding losses");
    m.def("calculate_ohmic_losses", &calculate_ohmic_losses, "Calculate DC ohmic losses in windings");
    m.def("calculate_magnetic_field_strength_field", &calculate_magnetic_field_strength_field, "Calculate magnetic field strength distribution");
    m.def("calculate_proximity_effect_losses", &calculate_proximity_effect_losses, "Calculate proximity effect losses in windings");
    m.def("calculate_skin_effect_losses", &calculate_skin_effect_losses, "Calculate skin effect losses in windings");
    m.def("calculate_skin_effect_losses_per_meter", &calculate_skin_effect_losses_per_meter, "Calculate skin effect losses per meter of wire");

    // DC resistance and losses
    m.def("calculate_dc_resistance_per_meter", &calculate_dc_resistance_per_meter, "Calculate DC resistance per meter of wire");
    m.def("calculate_dc_losses_per_meter", &calculate_dc_losses_per_meter, "Calculate DC losses per meter of wire");
    m.def("calculate_skin_ac_losses_per_meter", &calculate_skin_ac_losses_per_meter, "Calculate AC skin losses per meter");
    m.def("calculate_skin_ac_factor", &calculate_skin_ac_factor, "Calculate skin effect AC factor");
    m.def("calculate_skin_ac_resistance_per_meter", &calculate_skin_ac_resistance_per_meter, "Calculate AC resistance per meter due to skin effect");
    m.def("calculate_effective_current_density", &calculate_effective_current_density, "Calculate effective current density in wire");
    m.def("calculate_effective_skin_depth", &calculate_effective_skin_depth, "Calculate effective skin depth");
}

} // namespace PyMKF
