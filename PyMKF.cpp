#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "pybind11_json/pybind11_json.hpp"
#include <pybind11/embed.h>
#include <magic_enum.hpp>
#include "json.hpp"

#include "CoreWrapper.h"
#include "CoilWrapper.h"
#include "InputsWrapper.h"
#include <MAS.hpp>
#include "Defaults.h"
#include "Constants.h"
#include "MagneticWrapper.h"
#include "Reluctance.h"
#include "MagnetizingInductance.h"
#include "CoreLosses.h"
#include "MagneticSimulator.h"
#include "CoreTemperature.h"
#include "Utils.h"
#include "Settings.h"
#include "MagneticAdviser.h"
#include "LeakageInductance.h"

using json = nlohmann::json;


#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x) 

namespace py = pybind11;

py::dict get_constants() {
    auto constants = OpenMagnetics::Constants();
    py::dict constantsMap;
    constantsMap["residualGap"] = constants.residualGap;
    constantsMap["minimumNonResidualGap"] = constants.minimumNonResidualGap;
    constantsMap["vacuumPermeability"] = constants.vacuumPermeability;
    return constantsMap;
}

json get_core_materials() {
    try {
        auto materials = OpenMagnetics::get_materials(std::nullopt);
        json result = json::array();
        for (auto elem: materials) {
            json aux;
            OpenMagnetics::to_json(aux, elem);
            result.push_back(aux);
        }
        return result;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}
json get_core_shapes() {
    try {
        auto shapes = OpenMagnetics::get_shapes(true);
        json result = json::array();
        for (auto elem : shapes) {
            json aux;
            OpenMagnetics::to_json(aux, elem);
            result.push_back(aux);
        }
        return result;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}
json get_core_shape_families() {
    try {
        auto shapes = OpenMagnetics::get_shapes(false);
        std::vector<OpenMagnetics::CoreShapeFamily> families;
        json result = json::array();
        for (auto elem : shapes) {
            auto family = elem.get_family();
            if (std::find(families.begin(), families.end(), family) == families.end()) {
                families.push_back(family);
                json aux;
                OpenMagnetics::to_json(aux, family);
                result.push_back(aux);
            }
        }
        return result;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}
json get_wires() {
    try {
        auto wires = OpenMagnetics::get_wires();
        json result = json::array();
        for (auto elem : wires) {
            json aux;
            OpenMagnetics::to_json(aux, elem);
            result.push_back(aux);
        }
        return result;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}
json get_bobbins() {
    try {
        auto bobbins = OpenMagnetics::get_bobbins();
        json result = json::array();
        for (auto elem : bobbins) {
            json aux;
            OpenMagnetics::to_json(aux, elem);
            result.push_back(aux);
        }
        return result;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}
json get_insulation_materials() {
    try {
        auto insulationMaterials = OpenMagnetics::get_insulation_materials();
        json result = json::array();
        for (auto elem : insulationMaterials) {
            json aux;
            OpenMagnetics::to_json(aux, elem);
            result.push_back(aux);
        }
        return result;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}
json get_wire_materials() {
    try {
        auto wireMaterials = OpenMagnetics::get_wire_materials();
        json result = json::array();
        for (auto elem : wireMaterials) {
            json aux;
            OpenMagnetics::to_json(aux, elem);
            result.push_back(aux);
        }
        return result;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

json get_core_material_names() {
    try {
        auto materialNames = OpenMagnetics::get_material_names(std::nullopt);
        json result = json::array();
        for (auto elem : materialNames) {
            result.push_back(elem);
        }
        return result;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

json get_core_material_names_by_manufacturer(std::string manufacturerName) {
    try {
        auto materialNames = OpenMagnetics::get_material_names(manufacturerName);
        json result = json::array();
        for (auto elem : materialNames) {
            result.push_back(elem);
        }
        return result;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

json get_core_shape_names(bool includeToroidal) {
    try {
        auto shapeNames = OpenMagnetics::get_shape_names(includeToroidal);
        json result = json::array();
        for (auto elem : shapeNames) {
            result.push_back(elem);
        }
        return result;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

json get_wire_names() {
    try {
        auto wireNames = OpenMagnetics::get_wire_names();
        json result = json::array();
        for (auto elem : wireNames) {
            result.push_back(elem);
        }
        return result;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

json get_bobbin_names() {
    try {
        auto bobbinNames = OpenMagnetics::get_bobbin_names();
        json result = json::array();
        for (auto elem : bobbinNames) {
            result.push_back(elem);
        }
        return result;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

json get_insulation_material_names() {
    try {
        auto insulationMaterialNames = OpenMagnetics::get_insulation_material_names();
        json result = json::array();
        for (auto elem : insulationMaterialNames) {
            result.push_back(elem);
        }
        return result;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

json get_wire_material_names() {
    try {
        auto wireMaterialNames = OpenMagnetics::get_wire_material_names();
        json result = json::array();
        for (auto elem : wireMaterialNames) {
            result.push_back(elem);
        }
        return result;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

json find_core_material_by_name(json materialName) {
    try {
        auto materialData = OpenMagnetics::find_core_material_by_name(materialName);
        json result;
        to_json(result, materialData);
        return result;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

json find_core_shape_by_name(json shapeName) {
    try {
        auto shapeData = OpenMagnetics::find_core_shape_by_name(shapeName);
        json result;
        to_json(result, shapeData);
        return result;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

json find_wire_by_name(json wireName) {
    try {
        auto wireData = OpenMagnetics::find_wire_by_name(wireName);
        json result;
        to_json(result, wireData);
        return result;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

json find_bobbin_by_name(json bobbinName) {
    try {
        auto bobbinData = OpenMagnetics::find_bobbin_by_name(bobbinName);
        json result;
        to_json(result, bobbinData);
        return result;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

json find_insulation_material_by_name(json insulationMaterialName) {
    try {
        auto insulationMaterialData = OpenMagnetics::find_insulation_material_by_name(insulationMaterialName);
        json result;
        to_json(result, insulationMaterialData);
        return result;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

json find_wire_material_by_name(json wireMaterialName) {
    try {
        auto wireMaterialData = OpenMagnetics::find_wire_material_by_name(wireMaterialName);
        json result;
        to_json(result, wireMaterialData);
        return result;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

json calculate_core_data(json coreDataJson, bool includeMaterialData){
    try {
        OpenMagnetics::CoreWrapper core(coreDataJson, includeMaterialData);
        json result;
        to_json(result, core);
        return result;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

json calculate_core_processed_description(json coreDataJson){
    try {
        OpenMagnetics::CoreWrapper core(coreDataJson, false, false, false);
        core.process_data();
        json result;
        to_json(result, core.get_processed_description().value());
        return result;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

json calculate_core_geometrical_description(json coreDataJson){
    try {
        OpenMagnetics::CoreWrapper core(coreDataJson, false, false, false);
        auto geometricalDescription = core.create_geometrical_description().value();
        json result = json::array();
        for (auto& elem : geometricalDescription) {
            json aux;
            to_json(aux, elem);
            result.push_back(aux);
        }
        return result;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

json calculate_core_gapping(json coreDataJson){
    try {
        OpenMagnetics::CoreWrapper core(coreDataJson, false, false, false);
        core.process_gap();
        json result = json::array();
        for (auto& gap : core.get_functional_description().get_gapping()) {
            json aux;
            to_json(aux, gap);
            result.push_back(aux);
        }
        return result;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

json wind(json coilJson, size_t repetitions, json proportionPerWindingJson, json patternJson) {
    try {
        std::vector<double> proportionPerWinding = proportionPerWindingJson;
        std::vector<size_t> pattern = patternJson;
        auto coilFunctionalDescription = std::vector<OpenMagnetics::CoilFunctionalDescription>(coilJson["functionalDescription"]);
        OpenMagnetics::CoilWrapper coil;
        if (coilJson.contains("_interleavingLevel")) {
            coil.set_interleaving_level(coilJson["_interleavingLevel"]);
        }
        if (coilJson.contains("_windingOrientation")) {
            coil.set_winding_orientation(coilJson["_windingOrientation"]);
        }
        if (coilJson.contains("_layersOrientation")) {
            coil.set_layers_orientation(coilJson["_layersOrientation"]);
        }
        if (coilJson.contains("_turnsAlignment")) {
            coil.set_turns_alignment(coilJson["_turnsAlignment"]);
        }
        if (coilJson.contains("_sectionAlignment")) {
            coil.set_section_alignment(coilJson["_sectionAlignment"]);
        }

        coil.set_bobbin(coilJson["bobbin"]);
        coil.set_functional_description(coilFunctionalDescription);
        if (proportionPerWinding.size() == coilFunctionalDescription.size()) {
            if (pattern.size() > 0 && repetitions > 0) {
                coil.wind(proportionPerWinding, pattern, repetitions);
            }
            else if (repetitions > 0) {
                coil.wind(repetitions);
            }
            else {
                coil.wind();
            }
        }
        else {
            if (pattern.size() > 0 && repetitions > 0) {
                coil.wind(pattern, repetitions);
            }
            else if (repetitions > 0) {
                coil.wind(repetitions);
            }
            else {
                coil.wind();
            }
        }

        json result;
        to_json(result, coil);
        return result;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

json get_default_models() {
    try {
        json models;
        auto reluctanceModelName = magic_enum::enum_name(OpenMagnetics::Defaults().reluctanceModelDefault);
        models["reluctance"] = reluctanceModelName;
        auto coreLossesModelName = magic_enum::enum_name(OpenMagnetics::Defaults().coreLossesModelDefault);
        models["coreLosses"] = coreLossesModelName;
        auto coreTemperatureModelName = magic_enum::enum_name(OpenMagnetics::Defaults().coreTemperatureModelDefault);
        models["coreTemperature"] = coreTemperatureModelName;

        return models.dump();
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

json calculate_core_losses(json magneticJson, json inputsJson, json modelsJson) {
    try {
        OpenMagnetics::MagneticWrapper magnetic(magneticJson);
        OpenMagnetics::CoreWrapper core = magnetic.get_core();
        OpenMagnetics::CoilWrapper coil = magnetic.get_coil();
        OpenMagnetics::InputsWrapper inputs(inputsJson);
        auto operatingPoint = inputs.get_operating_point(0);
        OpenMagnetics::OperatingPointExcitation excitation = operatingPoint.get_excitations_per_winding()[0];
        double magnetizingInductance = OpenMagnetics::resolve_dimensional_values(inputs.get_design_requirements().get_magnetizing_inductance());
        if (!excitation.get_current()) {
            auto magnetizingCurrent = OpenMagnetics::InputsWrapper::calculate_magnetizing_current(excitation, magnetizingInductance);
            excitation.set_current(magnetizingCurrent);
            operatingPoint.get_mutable_excitations_per_winding()[0] = excitation;
        }

        auto defaults = OpenMagnetics::Defaults();

        std::map<std::string, std::string> models = modelsJson.get<std::map<std::string, std::string>>();

        auto reluctanceModelName = defaults.reluctanceModelDefault;
        if (models.find("reluctance") != models.end()) {
            std::string modelNameStringUpper = models["reluctance"];
            std::transform(modelNameStringUpper.begin(), modelNameStringUpper.end(), modelNameStringUpper.begin(), ::toupper);
            reluctanceModelName = magic_enum::enum_cast<OpenMagnetics::ReluctanceModels>(modelNameStringUpper).value();
        }
        auto coreLossesModelName = defaults.coreLossesModelDefault;
        if (models.find("coreLosses") != models.end()) {
            std::string modelNameStringUpper = models["coreLosses"];
            std::transform(modelNameStringUpper.begin(), modelNameStringUpper.end(), modelNameStringUpper.begin(), ::toupper);
            coreLossesModelName = magic_enum::enum_cast<OpenMagnetics::CoreLossesModels>(modelNameStringUpper).value();
        }
        auto coreTemperatureModelName = defaults.coreTemperatureModelDefault;
        if (models.find("coreTemperature") != models.end()) {
            std::string modelNameStringUpper = models["coreTemperature"];
            std::transform(modelNameStringUpper.begin(), modelNameStringUpper.end(), modelNameStringUpper.begin(), ::toupper);
            coreTemperatureModelName = magic_enum::enum_cast<OpenMagnetics::CoreTemperatureModels>(modelNameStringUpper).value();
        }

        OpenMagnetics::MagneticSimulator magneticSimulator;
        magneticSimulator.set_core_losses_model_name(coreLossesModelName);
        magneticSimulator.set_core_temperature_model_name(coreTemperatureModelName);
        magneticSimulator.set_reluctance_model_name(reluctanceModelName);
        auto coreLossesOutput = magneticSimulator.calculate_core_losses(operatingPoint, magnetic);
        json result;
        to_json(result, coreLossesOutput);

        OpenMagnetics::MagnetizingInductance magnetizing_inductance(reluctanceModelName);
        auto magneticFluxDensity = magnetizing_inductance.calculate_inductance_and_magnetic_flux_density(core, coil, &operatingPoint).second;

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
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

json calculate_inductance_from_number_turns_and_gapping(json coreJson, json coilJson, json operatingPointJson, json modelsJson) {
    try {
        OpenMagnetics::CoreWrapper core(coreJson);
        OpenMagnetics::CoilWrapper coil(coilJson);
        OpenMagnetics::OperatingPoint operatingPoint(operatingPointJson);

        auto defaults = OpenMagnetics::Defaults();
        std::map<std::string, std::string> models = modelsJson.get<std::map<std::string, std::string>>();

        auto reluctanceModelName = defaults.reluctanceModelDefault;
        if (models.find("reluctance") != models.end()) {
            std::string modelNameStringUpper = models["reluctance"];
            std::transform(modelNameStringUpper.begin(), modelNameStringUpper.end(), modelNameStringUpper.begin(), ::toupper);
            reluctanceModelName = magic_enum::enum_cast<OpenMagnetics::ReluctanceModels>(modelNameStringUpper).value();
        }

        OpenMagnetics::MagnetizingInductance magnetizingInductance(reluctanceModelName);

        auto magnetizingInductanceOutput = magnetizingInductance.calculate_inductance_from_number_turns_and_gapping(core, coil, &operatingPoint);
        json result;
        to_json(result, magnetizingInductanceOutput);
        return result;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

json calculate_advised_cores(json inputsJson, json weightsJson, int maximumNumberResults, bool useOnlyCoresInStock){
    try {
        OpenMagnetics::InputsWrapper inputs(inputsJson);
        std::map<std::string, double> weightsKeysString = weightsJson;
        std::map<OpenMagnetics::CoreAdviser::CoreAdviserFilters, double> weights;

        for (auto const& pair : weightsKeysString) {
            weights[magic_enum::enum_cast<OpenMagnetics::CoreAdviser::CoreAdviserFilters>(pair.first).value()] = pair.second;
        }
        weights[OpenMagnetics::CoreAdviser::CoreAdviserFilters::AREA_PRODUCT] = 1;
        weights[OpenMagnetics::CoreAdviser::CoreAdviserFilters::ENERGY_STORED] = 1;
        weights[OpenMagnetics::CoreAdviser::CoreAdviserFilters::COST] = 1;
        weights[OpenMagnetics::CoreAdviser::CoreAdviserFilters::EFFICIENCY] = 1;
        weights[OpenMagnetics::CoreAdviser::CoreAdviserFilters::DIMENSIONS] = 1;

        auto settings = OpenMagnetics::Settings::GetInstance();
        settings->set_use_only_cores_in_stock(useOnlyCoresInStock);

        OpenMagnetics::CoreAdviser coreAdviser;
        auto masMagnetics = coreAdviser.get_advised_core(inputs, weights, maximumNumberResults);

        json results = json::array();
        for (auto& masMagnetic : masMagnetics) {
            json aux;
            to_json(aux, masMagnetic.first);
            results.push_back(aux);
        }
        settings->reset();

        return results.dump(4);
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

json calculate_advised_magnetics(json inputsJson, int maximumNumberResults){
    try {
        OpenMagnetics::InputsWrapper inputs(inputsJson);

        OpenMagnetics::MagneticAdviser magneticAdviser;
        auto masMagnetics = magneticAdviser.get_advised_magnetic(inputs, maximumNumberResults);

        json results = json::array();
        for (auto& [masMagnetic, scoring] : masMagnetics) {
            json aux;
            to_json(aux, masMagnetic);
            results.push_back(aux);
        }

        return results.dump(4);
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

json calculate_winding_losses(json magneticJson, json operatingPointJson, double temperature) {
    try {
        OpenMagnetics::MagneticWrapper magnetic(magneticJson);
        OpenMagnetics::OperatingPoint operatingPoint(operatingPointJson);

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
        OpenMagnetics::CoilWrapper coil(coilJson);
        OpenMagnetics::OperatingPoint operatingPoint(operatingPointJson);

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
        OpenMagnetics::MagneticWrapper magnetic(magneticJson);
        OpenMagnetics::OperatingPoint operatingPoint(operatingPointJson);
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
        OpenMagnetics::CoilWrapper coil(coilJson);
        OpenMagnetics::WindingLossesOutput windingLossesOutput(windingLossesOutputJson);
        OpenMagnetics::WindingWindowMagneticStrengthFieldOutput windingWindowMagneticStrengthFieldOutput(windingWindowMagneticStrengthFieldOutputJson);

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
        OpenMagnetics::CoilWrapper coil(coilJson);
        OpenMagnetics::WindingLossesOutput windingLossesOutput(windingLossesOutputJson);

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
        OpenMagnetics::WireWrapper wire(wireJson);
        OpenMagnetics::SignalDescriptor current(currentJson);

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

bool plot_core(json magneticJson, std::string outFile) {
    try {
        OpenMagnetics::MagneticWrapper magnetic(magneticJson);
        OpenMagnetics::Painter painter(outFile);
        painter.paint_core(magnetic);
        painter.paint_bobbin(magnetic);
        painter.export_svg();
        return true;
    }
    catch (...) {
        return false;
    }
}

bool plot_sections(json magneticJson, std::string outFile) {
    try {
        OpenMagnetics::MagneticWrapper magnetic(magneticJson);
        OpenMagnetics::Painter painter(outFile);
        painter.paint_core(magnetic);
        painter.paint_bobbin(magnetic);
        painter.paint_coil_sections(magnetic);
        painter.export_svg();
        return true;
    }
    catch (...) {
        return false;
    }
}

bool plot_layers(json magneticJson, std::string outFile) {
    try {
        OpenMagnetics::MagneticWrapper magnetic(magneticJson);
        OpenMagnetics::Painter painter(outFile);
        painter.paint_core(magnetic);
        painter.paint_bobbin(magnetic);
        painter.paint_coil_layers(magnetic);
        painter.export_svg();
        return true;
    }
    catch (...) {
        return false;
    }
}

bool plot_turns(json magneticJson, std::string outFile) {
    try {
        OpenMagnetics::MagneticWrapper magnetic(magneticJson);
        OpenMagnetics::Painter painter(outFile);
        painter.paint_core(magnetic);
        painter.paint_bobbin(magnetic);
        painter.paint_coil_turns(magnetic);
        painter.export_svg();
        return true;
    }
    catch (...) {
        return false;
    }
}

std::string plot_field(json magneticJson, json operatingPointJson, std::string outFile) {
    try {
        auto settings = OpenMagnetics::Settings::GetInstance();
        OpenMagnetics::MagneticWrapper magnetic(magneticJson);
        OpenMagnetics::OperatingPoint operatingPoint(operatingPointJson);
        OpenMagnetics::Painter painter(outFile, true);
        painter.paint_magnetic_field(operatingPoint, magnetic);
        painter.paint_core(magnetic);
        painter.paint_bobbin(magnetic);
        painter.paint_coil_turns(magnetic);
        painter.export_svg();
        return outFile;
    }
    catch(const std::runtime_error& re)
    {
        return re.what();
    }
    catch(const std::exception& ex)
    {
        return ex.what();
    }
    catch(...)
    {
        return "Unknown failure occurred. Possible memory corruption";
    }
}

json get_settings() {
    try {
        auto settings = OpenMagnetics::Settings::GetInstance();
        json settingsJson;
        settingsJson["coilAllowMarginTape"] = settings->get_coil_allow_margin_tape();
        settingsJson["coilAllowInsulatedWire"] = settings->get_coil_allow_insulated_wire();
        settingsJson["coilFillSectionsWithMarginTape"] = settings->get_coil_fill_sections_with_margin_tape();
        settingsJson["coilWindEvenIfNotFit"] = settings->get_coil_wind_even_if_not_fit();
        settingsJson["coilDelimitAndCompact"] = settings->get_coil_delimit_and_compact();
        settingsJson["coilTryRewind"] = settings->get_coil_try_rewind();
        settingsJson["useOnlyCoresInStock"] = settings->get_use_only_cores_in_stock();
        settingsJson["painterNumberPointsX"] = settings->get_painter_number_points_x();
        settingsJson["painterNumberPointsY"] = settings->get_painter_number_points_y();
        settingsJson["painterMode"] = settings->get_painter_mode();
        settingsJson["painterLogarithmicScale"] = settings->get_painter_logarithmic_scale();
        settingsJson["painterIncludeFringing"] = settings->get_painter_include_fringing();
        if (settings->get_painter_maximum_value_colorbar()) {
            settingsJson["painterMaximumValueColorbar"] = settings->get_painter_maximum_value_colorbar();
        }
        if (settings->get_painter_minimum_value_colorbar()) {
            settingsJson["painterMinimumValueColorbar"] = settings->get_painter_minimum_value_colorbar();
        }
        settingsJson["painterColorFerrite"] = settings->get_painter_color_ferrite();
        settingsJson["painterColorBobbin"] = settings->get_painter_color_bobbin();
        settingsJson["painterColorCopper"] = settings->get_painter_color_copper();
        settingsJson["painterColorInsulation"] = settings->get_painter_color_insulation();
        settingsJson["painterColorMargin"] = settings->get_painter_color_margin();
        settingsJson["painterMirroringDimension"] = settings->get_painter_mirroring_dimension();
        settingsJson["magneticFieldNumberPointsX"] = settings->get_magnetic_field_number_points_x();
        settingsJson["magneticFieldNumberPointsY"] = settings->get_magnetic_field_number_points_y();
        settingsJson["magneticFieldIncludeFringing"] = settings->get_magnetic_field_include_fringing();
        settingsJson["magneticFieldMirroringDimension"] = settings->get_magnetic_field_mirroring_dimension();
        return settingsJson;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

void set_settings(json settingsJson) {
    auto settings = OpenMagnetics::Settings::GetInstance();
    settings->set_coil_allow_margin_tape(settingsJson["coilAllowMarginTape"]);
    settings->set_coil_allow_insulated_wire(settingsJson["coilAllowInsulatedWire"]);
    settings->set_coil_fill_sections_with_margin_tape(settingsJson["coilFillSectionsWithMarginTape"]);
    settings->set_coil_wind_even_if_not_fit(settingsJson["coilWindEvenIfNotFit"]);
    settings->set_coil_delimit_and_compact(settingsJson["coilDelimitAndCompact"]);
    settings->set_coil_try_rewind(settingsJson["coilTryRewind"]);
    settings->set_painter_mode(settingsJson["painterMode"]);
    settings->set_use_only_cores_in_stock(settingsJson["useOnlyCoresInStock"]);
    settings->set_painter_number_points_x(settingsJson["painterNumberPointsX"]);
    settings->set_painter_number_points_y(settingsJson["painterNumberPointsY"]);
    settings->set_painter_logarithmic_scale(settingsJson["painterLogarithmicScale"]);
    settings->set_painter_include_fringing(settingsJson["painterIncludeFringing"]);
    if (settingsJson.contains("painterMaximumValueColorbar")) {
        settings->set_painter_maximum_value_colorbar(settingsJson["painterMaximumValueColorbar"]);
    }
    if (settingsJson.contains("painterMinimumValueColorbar")) {
        settings->set_painter_minimum_value_colorbar(settingsJson["painterMinimumValueColorbar"]);
    }
    settings->set_painter_color_ferrite(settingsJson["painterColorFerrite"]);
    settings->set_painter_color_bobbin(settingsJson["painterColorBobbin"]);
    settings->set_painter_color_copper(settingsJson["painterColorCopper"]);
    settings->set_painter_color_insulation(settingsJson["painterColorInsulation"]);
    settings->set_painter_color_margin(settingsJson["painterColorMargin"]);
    settings->set_painter_mirroring_dimension(settingsJson["painterMirroringDimension"]);
    settings->set_magnetic_field_number_points_x(settingsJson["magneticFieldNumberPointsX"]);
    settings->set_magnetic_field_number_points_y(settingsJson["magneticFieldNumberPointsY"]);
    settings->set_magnetic_field_include_fringing(settingsJson["magneticFieldIncludeFringing"]);
    settings->set_magnetic_field_mirroring_dimension(settingsJson["magneticFieldMirroringDimension"]);
}

void reset_settings() {
    auto settings = OpenMagnetics::Settings::GetInstance();
    settings->reset();
}


PYBIND11_MODULE(PyMKF, m) {
    m.def("get_constants", &get_constants, "");
    m.def("get_core_materials", &get_core_materials, "");
    m.def("get_core_shapes", &get_core_shapes, "");
    m.def("get_core_shape_families", &get_core_shape_families, "");
    m.def("get_wires", &get_wires, "");
    m.def("get_bobbins", &get_bobbins, "");
    m.def("get_insulation_materials", &get_insulation_materials, "");
    m.def("get_wire_materials", &get_wire_materials, "");
    m.def("get_core_material_names", &get_core_material_names, "");
    m.def("get_core_material_names_by_manufacturer", &get_core_material_names_by_manufacturer, "");
    m.def("get_core_shape_names", &get_core_shape_names, "");
    m.def("get_wire_names", &get_wire_names, "");
    m.def("get_bobbin_names", &get_bobbin_names, "");
    m.def("get_insulation_material_names", &get_insulation_material_names, "");
    m.def("get_wire_material_names", &get_wire_material_names, "");
    m.def("find_core_material_by_name", &find_core_material_by_name, "");
    m.def("find_core_shape_by_name", &find_core_shape_by_name, "");
    m.def("find_wire_by_name", &find_wire_by_name, "");
    m.def("find_bobbin_by_name", &find_bobbin_by_name, "");
    m.def("find_insulation_material_by_name", &find_insulation_material_by_name, "");
    m.def("find_wire_material_by_name", &find_wire_material_by_name, "");
    m.def("calculate_core_data", &calculate_core_data, "");
    m.def("calculate_core_processed_description", &calculate_core_processed_description, "");
    m.def("calculate_core_geometrical_description", &calculate_core_geometrical_description, "");
    m.def("calculate_core_gapping", &calculate_core_gapping, "");
    m.def("wind", &wind, "");
    m.def("get_default_models", &get_default_models, "");
    m.def("calculate_inductance_from_number_turns_and_gapping", &calculate_inductance_from_number_turns_and_gapping, "");
    m.def("calculate_core_losses", &calculate_core_losses, "");
    m.def("calculate_advised_cores", &calculate_advised_cores, "");
    m.def("calculate_advised_magnetics", &calculate_advised_magnetics, "");
    m.def("calculate_winding_losses", &calculate_winding_losses, "");
    m.def("calculate_ohmic_losses", &calculate_ohmic_losses, "");
    m.def("calculate_magnetic_field_strength_field", &calculate_magnetic_field_strength_field, "");
    m.def("calculate_proximity_effect_losses", &calculate_proximity_effect_losses, "");
    m.def("calculate_skin_effect_losses", &calculate_skin_effect_losses, "");
    m.def("calculate_skin_effect_losses_per_meter", &calculate_skin_effect_losses_per_meter, "");
    m.def("plot_core", &plot_core, "");
    m.def("plot_sections", &plot_sections, "");
    m.def("plot_layers", &plot_layers, "");
    m.def("plot_turns", &plot_turns, "");
    m.def("plot_field", &plot_field, "");
    m.def("set_settings", &set_settings, "");
    m.def("get_settings", &get_settings, "");
    m.def("reset_settings", &reset_settings, "");
}
