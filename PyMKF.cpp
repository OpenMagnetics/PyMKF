#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "pybind11_json/pybind11_json.hpp"
#include <magic_enum.hpp>
#include "json.hpp"

#include "BobbinWrapper.h"
#include "CoreWrapper.h"
#include "CoilWrapper.h"
#include "WireWrapper.h"
#include "InitialPermeability.h"
#include "InputsWrapper.h"
#include <MAS.hpp>
#include "Defaults.h"
#include "Constants.h"
#include "MagneticWrapper.h"
#include "Reluctance.h"
#include "MagnetizingInductance.h"
#include "CoreLosses.h"
#include "MagneticSimulator.h"
#include "Resistivity.h"
#include "CoreTemperature.h"
#include "CircuitSimulatorInterface.h"
#include "Utils.h"
#include "Settings.h"
#include "MagneticAdviser.h"
#include "LeakageInductance.h"
#include "Insulation.h"
#include "NumberTurns.h"
#include "MagneticSimulator.h"
#include "WindingOhmicLosses.h"
#include "WindingSkinEffectLosses.h"
#include "CircuitSimulatorInterface.h"


using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;


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

double get_material_permeability(json materialName, double temperature, double magneticFieldDcBias, double frequency) {
    try {
        auto materialData = OpenMagnetics::find_core_material_by_name(materialName);
        OpenMagnetics::InitialPermeability initialPermeability;

        return initialPermeability.get_initial_permeability(materialData, temperature, magneticFieldDcBias, frequency);
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

double get_material_resistivity(json materialName, double temperature) {
    try {
        auto materialData = OpenMagnetics::find_core_material_by_name(materialName);
        auto resistivityModel = OpenMagnetics::ResistivityModel::factory(OpenMagnetics::ResistivityModels::CORE_MATERIAL);
        return (*resistivityModel).get_resistivity(materialData, temperature);
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

json get_core_material_steinmetz_coefficients(json materialName, double frequency) {
    try {
        auto steinmetzCoreLossesMethodRangeDatum = OpenMagnetics::CoreLossesModel::get_steinmetz_coefficients(materialName, frequency);
        json result;
        to_json(result, steinmetzCoreLossesMethodRangeDatum);
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
        auto settings = OpenMagnetics::Settings::GetInstance();
        settings->set_use_toroidal_cores(includeToroidal);
        auto shapeNames = OpenMagnetics::get_shape_names();
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

json create_basic_bobbin(json coreDataJson, bool nullDimensions){
    try {
        OpenMagnetics::CoreWrapper core(coreDataJson, false, false, false);
        auto bobbin = OpenMagnetics::BobbinWrapper::create_quick_bobbin(core, nullDimensions);

        json result;
        to_json(result, bobbin);
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

json get_default_models() {
    try {
        json models;
        auto reluctanceModelName = magic_enum::enum_name(OpenMagnetics::Defaults().reluctanceModelDefault);
        models["reluctance"] = reluctanceModelName;
        auto coreLossesModelName = magic_enum::enum_name(OpenMagnetics::Defaults().coreLossesModelDefault);
        models["coreLosses"] = coreLossesModelName;
        auto coreTemperatureModelName = magic_enum::enum_name(OpenMagnetics::Defaults().coreTemperatureModelDefault);
        models["coreTemperature"] = coreTemperatureModelName;

        return models;
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
        std::map<std::string, double> weightsKeysJson = weightsJson;
        std::map<OpenMagnetics::CoreAdviser::CoreAdviserFilters, double> weights;

        for (auto const& pair : weightsKeysJson) {
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

        return results;
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

        return results;
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

std::string plot_core(json magneticJson, std::string outFile) {
    try {
        OpenMagnetics::MagneticWrapper magnetic(magneticJson);
        OpenMagnetics::Painter painter(outFile);
        painter.paint_core(magnetic);
        painter.paint_bobbin(magnetic);
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

std::string plot_sections(json magneticJson, std::string outFile) {
    try {
        OpenMagnetics::MagneticWrapper magnetic(magneticJson);
        OpenMagnetics::Painter painter(outFile);
        painter.paint_core(magnetic);
        painter.paint_bobbin(magnetic);
        painter.paint_coil_sections(magnetic);
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

std::string plot_layers(json magneticJson, std::string outFile) {
    try {
        OpenMagnetics::MagneticWrapper magnetic(magneticJson);
        OpenMagnetics::Painter painter(outFile);
        painter.paint_core(magnetic);
        painter.paint_bobbin(magnetic);
        painter.paint_coil_layers(magnetic);
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

std::string plot_turns(json magneticJson, std::string outFile) {
    try {
        OpenMagnetics::MagneticWrapper magnetic(magneticJson);
        OpenMagnetics::Painter painter(outFile);
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

std::string plot_wire(json wireJson, std::string outFile, std::string cci_coords_path) {
    try {
        auto settings = OpenMagnetics::Settings::GetInstance();
        OpenMagnetics::WireWrapper wire(wireJson);
        OpenMagnetics::Painter painter(outFile);
        painter.paint_wire(wire);
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

std::string plot_current_density(json wireJson, json operatingPointJson, std::string outFile) {
    try {
        auto settings = OpenMagnetics::Settings::GetInstance();
        OpenMagnetics::WireWrapper wire(wireJson);
        OpenMagnetics::OperatingPoint operatingPoint(operatingPointJson);
        OpenMagnetics::Painter painter(outFile);
        painter.paint_wire_with_current_density(wire, operatingPoint);
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
        settingsJson["painterColorLines"] = settings->get_painter_color_lines();
        settingsJson["painterColorText"] = settings->get_painter_color_text();
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
        settingsJson["painterSimpleLitz"] = settings->get_painter_simple_litz();
        settingsJson["painterAdvancedLitz"] = settings->get_painter_advanced_litz();
        settingsJson["painterCciCoordinatesPath"] = settings->get_painter_cci_coordinates_path();
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
    settings->set_painter_color_lines(settingsJson["painterColorLines"]);
    settings->set_painter_color_text(settingsJson["painterColorText"]);
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
    settings->set_painter_simple_litz(settingsJson["painterSimpleLitz"]);
    settings->set_painter_advanced_litz(settingsJson["painterAdvancedLitz"]);
    settings->set_painter_cci_coordinates_path(settingsJson["painterCciCoordinatesPath"]);
}

void reset_settings() {
    auto settings = OpenMagnetics::Settings::GetInstance();
    settings->reset();
}

json calculate_harmonics(json waveformJson, double frequency) {
    OpenMagnetics::Waveform waveform;
    OpenMagnetics::from_json(waveformJson, waveform);

    auto sampledCurrentWaveform = OpenMagnetics::InputsWrapper::calculate_sampled_waveform(waveform, frequency);
    auto harmonics = OpenMagnetics::InputsWrapper::calculate_harmonics_data(sampledCurrentWaveform, frequency);

    json result;
    to_json(result, harmonics);
    return result;
}

json calculate_processed(json harmonicsJson, json waveformJson) {
    OpenMagnetics::Waveform waveform;
    OpenMagnetics::Harmonics harmonics;
    OpenMagnetics::from_json(waveformJson, waveform);
    OpenMagnetics::from_json(harmonicsJson, harmonics);

    auto processed = OpenMagnetics::InputsWrapper::calculate_processed_data(harmonics, waveform, true);

    json result;
    to_json(result, processed);
    return result;
}

json calculate_shape_data(json shapeJson){
    OpenMagnetics::CoreShape shape(shapeJson);
    OpenMagnetics::CoreWrapper core;
    OpenMagnetics::CoreFunctionalDescription coreFunctionalDescription;
    coreFunctionalDescription.set_shape(shape);
    coreFunctionalDescription.set_material("Dummy");
    coreFunctionalDescription.set_number_stacks(1);
    if (shape.get_magnetic_circuit() == OpenMagnetics::MagneticCircuit::OPEN) {
        coreFunctionalDescription.set_type(OpenMagnetics::CoreType::TWO_PIECE_SET);
    }
    else {
        if (shape.get_family() == OpenMagnetics::CoreShapeFamily::T) {
            coreFunctionalDescription.set_type(OpenMagnetics::CoreType::TOROIDAL);
        }
        else {
            coreFunctionalDescription.set_type(OpenMagnetics::CoreType::CLOSED_SHAPE);
        }
    }
    core.set_functional_description(coreFunctionalDescription);
    core.process_data();

    json result;
    to_json(result, core);
    return result;
}

json calculate_core_data(json coreDataJson, bool includeMaterialData){
    try {
        OpenMagnetics::CoreWrapper core(coreDataJson, includeMaterialData, true);

        json result;
        to_json(result, core);
        return result;
    }
    catch (const std::exception &exc) {
        return "Exception: " + std::string{exc.what()};
    }
}

json calculate_bobbin_data(json magneticJson){
    try {
        OpenMagnetics::MagneticWrapper magnetic(magneticJson);

        auto optionalBobbin = magnetic.get_coil().get_bobbin();
        OpenMagnetics::BobbinWrapper bobbin;

        if (std::holds_alternative<std::string>(optionalBobbin)) {
            auto bobbinJson = std::get<std::string>(optionalBobbin);
            if (bobbinJson == "Dummy") {
                bobbin = OpenMagnetics::BobbinWrapper::create_quick_bobbin(magnetic.get_mutable_core());
            }
        }
        else {
            bobbin = OpenMagnetics::BobbinWrapper(std::get<std::string>(optionalBobbin));
            bobbin.process_data();
        }

        json result;
        to_json(result, bobbin);
        return result;
    }
    catch (const std::exception &exc) {
        return "Exception: " + std::string{exc.what()};
    }
}

json get_wire_data(json coilFunctionalDescriptionDataJson){
    OpenMagnetics::CoilFunctionalDescription coilFunctionalDescription(coilFunctionalDescriptionDataJson);
    auto wire = OpenMagnetics::CoilWrapper::resolve_wire(coilFunctionalDescription);
    json result;
    to_json(result, wire);
    return result;
}

json get_wire_data_by_name(std::string name){
    auto wireData = OpenMagnetics::find_wire_by_name(name);
    json result;
    to_json(result, wireData);
    return result;
}

json get_wire_data_by_standard_name(std::string standardName){
    auto wires = OpenMagnetics::get_wires();
    for (auto wire : wires) {
        if (!wire.get_standard_name()) {
            continue;
        }
        if (wire.get_standard_name().value() == standardName) {
            auto coating = wire.resolve_coating();
            if (!coating) {
                continue;
            }
            if (!coating->get_grade()) {
                continue;
            }
            // Hardcoded
            if (coating->get_grade().value() == 1) {
                json result;
                to_json(result, wire);
                return result;
            }
        }
    }

    json result;
    result["errorMessage"] = "Wire not found by standard name";
    return result;
}

double get_wire_outer_width_rectangular(double conductingWidth, int grade, json wireStandardJson){
    OpenMagnetics::WireStandard wireStandard;
    OpenMagnetics::from_json(wireStandardJson, wireStandard);
    return OpenMagnetics::WireWrapper::get_outer_width_rectangular(conductingWidth, grade, wireStandard);
}

double get_wire_outer_height_rectangular(double conductingHeight, int grade, json wireStandardJson){
    OpenMagnetics::WireStandard wireStandard;
    OpenMagnetics::from_json(wireStandardJson, wireStandard);
    return OpenMagnetics::WireWrapper::get_outer_height_rectangular(conductingHeight, grade, wireStandard);
}

double get_wire_outer_diameter_bare_litz(double conductingDiameter, int numberConductors, int grade, json wireStandardJson) {
    OpenMagnetics::WireStandard wireStandard;
    OpenMagnetics::from_json(wireStandardJson, wireStandard);
    return OpenMagnetics::WireWrapper::get_outer_diameter_bare_litz(conductingDiameter, numberConductors, grade, wireStandard);
}

double get_wire_outer_diameter_served_litz(double conductingDiameter, int numberConductors, int grade, int numberLayers, json wireStandardJson) {
    OpenMagnetics::WireStandard wireStandard;
    OpenMagnetics::from_json(wireStandardJson, wireStandard);
    return OpenMagnetics::WireWrapper::get_outer_diameter_served_litz(conductingDiameter, numberConductors, grade, numberLayers, wireStandard);
}

double get_wire_outer_diameter_insulated_litz(double conductingDiameter, int numberConductors, int numberLayers, double thicknessLayers, int grade, json wireStandardJson) {
    OpenMagnetics::WireStandard wireStandard;
    OpenMagnetics::from_json(wireStandardJson, wireStandard);
    return OpenMagnetics::WireWrapper::get_outer_diameter_insulated_litz(conductingDiameter, numberConductors, numberLayers, thicknessLayers, grade, wireStandard);
}

double get_wire_outer_diameter_enamelled_round(double conductingDiameter, int grade, json wireStandardJson) {
    OpenMagnetics::WireStandard wireStandard;
    OpenMagnetics::from_json(wireStandardJson, wireStandard);
    return OpenMagnetics::WireWrapper::get_outer_diameter_round(conductingDiameter, grade, wireStandard);
}

double get_wire_outer_diameter_insulated_round(double conductingDiameter, int numberLayers, double thicknessLayers, json wireStandardJson) {
    OpenMagnetics::WireStandard wireStandard;
    OpenMagnetics::from_json(wireStandardJson, wireStandard);
    return OpenMagnetics::WireWrapper::get_outer_diameter_round(conductingDiameter, numberLayers, thicknessLayers, wireStandard);
}

std::vector<double> get_outer_dimensions(json wireJson) {
    OpenMagnetics::WireWrapper wire(wireJson);
    return {wire.get_maximum_outer_width(), wire.get_maximum_outer_height()};
}

json get_strand_by_standard_name(std::string standardName){
    auto wires = OpenMagnetics::get_wires();
    for (auto wire : wires) {
        if (!wire.get_standard_name()) {
            continue;
        }
        auto coating = wire.resolve_coating();
        if (!coating) {
            continue;
        }
        // We are looking for enamelled wires for strands
        if (coating->get_type() != OpenMagnetics::InsulationWireCoatingType::ENAMELLED) {
            continue;
        }

        if (!coating->get_grade()) {
            throw std::runtime_error("Missing grade");
        }

        if (wire.get_standard_name().value() == standardName && coating->get_grade().value() == 1) {
            json result;
            to_json(result, wire);
            return result;
        }
    }

    json result;
    result["errorMessage"] = "Wire not found by standard name";
    return result;
}

double get_wire_conducting_diameter_by_standard_name(std::string standardName){
    auto wires = OpenMagnetics::get_wires();
    for (auto wire : wires) {
        if (!wire.get_standard_name()) {
            continue;
        }
        if (wire.get_standard_name().value() == standardName) {
            return OpenMagnetics::resolve_dimensional_values(wire.get_conducting_diameter().value());
        }
    }

    return -1;
}

json get_equivalent_wire(json oldWireJson, json newWireTypeJson, double effectivefrequency){
    try {
        OpenMagnetics::WireWrapper oldWire(oldWireJson);
        OpenMagnetics::WireType newWireType;
        from_json(newWireTypeJson, newWireType);

        auto newWire = OpenMagnetics::WireWrapper::get_equivalent_wire(oldWire, newWireType, effectivefrequency);

        json result;
        to_json(result, newWire);
        return result;
    }
    catch (const std::exception &exc) {
        std::cout << std::string{exc.what()} << std::endl;

        return "Exception: " + std::string{exc.what()};
    }
}

json get_coating_label(json wireJson){
    try {
        OpenMagnetics::WireWrapper wire(wireJson);
        auto coatingLabel = wire.encode_coating_label();
        return coatingLabel;
    }
    catch(const std::runtime_error& re)
    {
        return "Exception: " + std::string{re.what()};
    }
    catch(const std::exception& ex)
    {
        return "Exception: " + std::string{ex.what()};
    }
    catch(...)
    {
        return "Unknown failure occurred. Possible memory corruption";
    }
}

json get_wire_coating_by_label(std::string label){
    auto wires = OpenMagnetics::get_wires();
    OpenMagnetics::InsulationWireCoating insulationWireCoating;
    for (auto wire : wires) {
        auto coatingLabel = wire.encode_coating_label();
        if (coatingLabel == label) {
            if (wire.resolve_coating()) {
                insulationWireCoating = wire.resolve_coating().value();
            }
            else {
                insulationWireCoating.set_type(OpenMagnetics::InsulationWireCoatingType::BARE);
            }
            break;
        }
    }
    json result;
    to_json(result, insulationWireCoating);
    return result;
}

std::vector<std::string> get_coating_labels_by_type(json wireTypeJson){
    OpenMagnetics::WireType wireType(wireTypeJson);

    auto wires = OpenMagnetics::get_wires(wireType);

    std::vector<std::string> coatingLabels;
    for (auto wire : wires) {
        auto coatingLabel = wire.encode_coating_label();
        if (std::find(coatingLabels.begin(), coatingLabels.end(), coatingLabel) == coatingLabels.end()) {
            coatingLabels.push_back(coatingLabel);
        }
    }

    return coatingLabels;
}

json load_core_data(json coresJson){
    json result = json::array();
    for (auto& coreJson : coresJson) {
        OpenMagnetics::CoreWrapper core(coreJson, false);
        json aux;
        to_json(aux, core);
        result.push_back(aux);
    }
    return result;
}

json get_material_data(std::string materialName){

    auto materialData = OpenMagnetics::find_core_material_by_name(materialName);
    json result;
    to_json(result, materialData);
    return result;
}

json get_core_temperature_dependant_parameters(json coreData, double temperature){
    OpenMagnetics::CoreWrapper core(coreData);
    json result;

    result["magneticFluxDensitySaturation"] = core.get_magnetic_flux_density_saturation(temperature, false);
    result["magneticFieldStrengthSaturation"] = core.get_magnetic_field_strength_saturation(temperature);
    result["initialPermeability"] = core.get_initial_permeability(temperature);
    result["effectivePermeability"] = core.get_effective_permeability(temperature);
    result["reluctance"] = core.get_reluctance(temperature);
    auto reluctanceModel = OpenMagnetics::ReluctanceModel::factory();
    result["permeance"] = 1.0 / reluctanceModel->get_ungapped_core_reluctance(core);
    result["resistivity"] = core.get_resistivity(temperature);

    return result;
}

json get_shape_data(std::string shapeName){
    try {
        auto shapeData = OpenMagnetics::find_core_shape_by_name(shapeName);

        json result;
        to_json(result, shapeData);
        return result;
    }
    catch (const std::exception &exc) {
        return "Exception: " + std::string{exc.what()};
    }
}

std::vector<std::string> get_available_shape_families(){
    std::vector<std::string> families;
    for (auto& family : magic_enum::enum_names<OpenMagnetics::CoreShapeFamily>()) {
        std::string familyJson(family);
        families.push_back(familyJson);
    }
    return families;
}

std::vector<std::string> get_available_core_manufacturers(){
    std::vector<std::string> manufacturers;
    auto materials = OpenMagnetics::get_materials("");
    for (auto material : materials) {
        std::string manufacturer = material.get_manufacturer_info().get_name();
        if (std::find(manufacturers.begin(), manufacturers.end(), manufacturer) == manufacturers.end()) {
            manufacturers.push_back(manufacturer);
        }
    }
    return manufacturers;
}

std::vector<std::string> get_available_core_shape_families(){
    std::vector<std::string> families;
    for (auto& family : magic_enum::enum_names<OpenMagnetics::CoreShapeFamily>()) {
        std::string familyJson(family);
        families.push_back(familyJson);
    }
    return families;
}

std::vector<std::string> get_available_core_materials(std::string manufacturer){
    return OpenMagnetics::get_material_names(manufacturer);
}

std::vector<std::string> get_available_core_shapes(){
    return OpenMagnetics::get_shape_names();
}

std::vector<std::string> get_available_wires(){
    return OpenMagnetics::get_wire_names();
}

std::vector<std::string> get_unique_wire_diameters(json wireStandardJson){
    OpenMagnetics::WireStandard wireStandard(wireStandardJson);

    auto wires = OpenMagnetics::get_wires(OpenMagnetics::WireType::ROUND, wireStandard);

    std::vector<std::string> uniqueStandardName;
    for (auto wire : wires) {
        if (!wire.get_standard_name()) {
            continue;
        }
        auto standardName = wire.get_standard_name().value();
        if (std::find(uniqueStandardName.begin(), uniqueStandardName.end(), standardName) == uniqueStandardName.end()) {
            uniqueStandardName.push_back(standardName);
        }
    }


    return uniqueStandardName;
}

std::vector<std::string> get_available_wire_types(){
    std::vector<std::string> wireTypes;

    for (auto [value, name] : magic_enum::enum_entries<OpenMagnetics::WireType>()) {
        json wireTypeJson;
        if (value == OpenMagnetics::WireType::PLANAR) {
            // TODO Add support for planar
            continue;
        }
        OpenMagnetics::to_json(wireTypeJson, value);
        wireTypes.push_back(wireTypeJson);
    }

    return wireTypes;
}

std::vector<std::string> get_available_wire_standards(){
    std::vector<std::string> wireStandards;

    for (auto [value, name] : magic_enum::enum_entries<OpenMagnetics::WireStandard>()) {
        json wireStandardJson;
        OpenMagnetics::to_json(wireStandardJson, value);
        wireStandards.push_back(wireStandardJson);
    }

    return wireStandards;
}

json calculate_gap_reluctance(json coreGapData, std::string modelNameJson){
    std::string modelNameJsonUpper = modelNameJson;
    std::transform(modelNameJsonUpper.begin(), modelNameJsonUpper.end(), modelNameJsonUpper.begin(), ::toupper);
    auto modelName = magic_enum::enum_cast<OpenMagnetics::ReluctanceModels>(modelNameJsonUpper);

    auto reluctanceModel = OpenMagnetics::ReluctanceModel::factory(modelName.value());
    OpenMagnetics::CoreGap coreGap(coreGapData);

    auto coreGapResult = reluctanceModel->get_gap_reluctance(coreGap);
    json result;
    to_json(result, coreGapResult);
    return result;
}

json get_gap_reluctance_model_information(){
    json info;
    info["information"] = OpenMagnetics::ReluctanceModel::get_models_information();
    info["errors"] = OpenMagnetics::ReluctanceModel::get_models_errors();
    info["internal_links"] = OpenMagnetics::ReluctanceModel::get_models_internal_links();
    info["external_links"] = OpenMagnetics::ReluctanceModel::get_models_external_links();
    return info;
}

double calculate_inductance_from_number_turns_and_gapping(json coreData,
                                                          json coilData,
                                                          json operatingPointData,
                                                          json modelsData){
    OpenMagnetics::CoreWrapper core(coreData);
    OpenMagnetics::CoilWrapper coil(coilData);
    OpenMagnetics::OperatingPoint operatingPoint(operatingPointData);

    std::map<std::string, std::string> models = modelsData.get<std::map<std::string, std::string>>();

    auto reluctanceModelName = OpenMagnetics::Defaults().reluctanceModelDefault;
    if (models.find("reluctance") != models.end()) {
        std::string modelNameJsonUpper = models["reluctance"];
        std::transform(modelNameJsonUpper.begin(), modelNameJsonUpper.end(), modelNameJsonUpper.begin(), ::toupper);
        reluctanceModelName = magic_enum::enum_cast<OpenMagnetics::ReluctanceModels>(modelNameJsonUpper).value();
    }

    OpenMagnetics::MagnetizingInductance magnetizingInductanceObj(reluctanceModelName);
    double magnetizingInductance = magnetizingInductanceObj.calculate_inductance_from_number_turns_and_gapping(core, coil, &operatingPoint).get_magnetizing_inductance().get_nominal().value();

    return magnetizingInductance;
}


double calculate_number_turns_from_gapping_and_inductance(json coreData,
                                                          json inputsData,    
                                                          json modelsData){
    OpenMagnetics::CoreWrapper core(coreData);
    OpenMagnetics::InputsWrapper inputs(inputsData);

    std::map<std::string, std::string> models = modelsData.get<std::map<std::string, std::string>>();
    
    auto reluctanceModelName = OpenMagnetics::Defaults().reluctanceModelDefault;
    if (models.find("reluctance") != models.end()) {
        std::string modelNameJsonUpper = models["reluctance"];
        std::transform(modelNameJsonUpper.begin(), modelNameJsonUpper.end(), modelNameJsonUpper.begin(), ::toupper);
        reluctanceModelName = magic_enum::enum_cast<OpenMagnetics::ReluctanceModels>(modelNameJsonUpper).value();
    }

    OpenMagnetics::MagnetizingInductance magnetizingInductanceObj(reluctanceModelName);
    double numberTurns = magnetizingInductanceObj.calculate_number_turns_from_gapping_and_inductance(core, &inputs);

    return numberTurns;
}


json calculate_gapping_from_number_turns_and_inductance(json coreData,
                                                               json coilData,
                                                               json inputsData,
                                                               std::string gappingTypeJson,
                                                               int decimals,
                                                               json modelsData){
    OpenMagnetics::CoreWrapper core(coreData);
    OpenMagnetics::CoilWrapper coil(coilData);
    OpenMagnetics::InputsWrapper inputs(inputsData);

    std::map<std::string, std::string> models = modelsData.get<std::map<std::string, std::string>>();
    OpenMagnetics::GappingType gappingType = magic_enum::enum_cast<OpenMagnetics::GappingType>(gappingTypeJson).value();
    
    auto reluctanceModelName = OpenMagnetics::Defaults().reluctanceModelDefault;
    if (models.find("reluctance") != models.end()) {
        std::string modelNameJsonUpper = models["reluctance"];
        std::transform(modelNameJsonUpper.begin(), modelNameJsonUpper.end(), modelNameJsonUpper.begin(), ::toupper);
        reluctanceModelName = magic_enum::enum_cast<OpenMagnetics::ReluctanceModels>(modelNameJsonUpper).value();
    }

    OpenMagnetics::MagnetizingInductance magnetizingInductanceObj(reluctanceModelName);
    std::vector<OpenMagnetics::CoreGap> gapping = magnetizingInductanceObj.calculate_gapping_from_number_turns_and_inductance(core,
                                                                                                       coil,
                                                                                                       &inputs,
                                                                                                       gappingType,
                                                                                                       decimals);

    core.set_processed_description(std::nullopt);
    core.set_geometrical_description(std::nullopt);
    core.get_mutable_functional_description().set_gapping(gapping);
    core.process_data();
    core.process_gap();
    auto geometricalDescription = core.create_geometrical_description();
    core.set_geometrical_description(geometricalDescription);

    json result;
    to_json(result, core);
    return result;
}

json calculate_core_losses(json coreData,
                                  json coilData,
                                  json inputsData,    
                                  json modelsData){

    OpenMagnetics::CoreWrapper core(coreData);
    OpenMagnetics::CoilWrapper coil(coilData);
    OpenMagnetics::InputsWrapper inputs(inputsData);
    auto operatingPoint = inputs.get_operating_point(0);
    OpenMagnetics::OperatingPointExcitation excitation = operatingPoint.get_excitations_per_winding()[0];
    double magnetizingInductance = OpenMagnetics::resolve_dimensional_values(inputs.get_design_requirements().get_magnetizing_inductance());
    if (!excitation.get_current()) {
        auto magnetizingCurrent = OpenMagnetics::InputsWrapper::calculate_magnetizing_current(excitation, magnetizingInductance);
        excitation.set_current(magnetizingCurrent);
        operatingPoint.get_mutable_excitations_per_winding()[0] = excitation;
    }

    auto defaults = OpenMagnetics::Defaults();

    std::map<std::string, std::string> models = modelsData.get<std::map<std::string, std::string>>();

    auto reluctanceModelName = defaults.reluctanceModelDefault;
    if (models.find("reluctance") != models.end()) {
        std::string modelNameJsonUpper = models["reluctance"];
        std::transform(modelNameJsonUpper.begin(), modelNameJsonUpper.end(), modelNameJsonUpper.begin(), ::toupper);
        reluctanceModelName = magic_enum::enum_cast<OpenMagnetics::ReluctanceModels>(modelNameJsonUpper).value();
    }
    auto coreLossesModelName = defaults.coreLossesModelDefault;
    if (models.find("coreLosses") != models.end()) {
        std::string modelNameJsonUpper = models["coreLosses"];
        std::transform(modelNameJsonUpper.begin(), modelNameJsonUpper.end(), modelNameJsonUpper.begin(), ::toupper);
        coreLossesModelName = magic_enum::enum_cast<OpenMagnetics::CoreLossesModels>(modelNameJsonUpper).value();
    }
    auto coreTemperatureModelName = defaults.coreTemperatureModelDefault;
    if (models.find("coreTemperature") != models.end()) {
        std::string modelNameJsonUpper = models["coreTemperature"];
        std::transform(modelNameJsonUpper.begin(), modelNameJsonUpper.end(), modelNameJsonUpper.begin(), ::toupper);
        coreTemperatureModelName = magic_enum::enum_cast<OpenMagnetics::CoreTemperatureModels>(modelNameJsonUpper).value();
    }

    OpenMagnetics::MagneticWrapper magnetic;
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

json get_core_losses_model_information(json material){
    json info;
    info["information"] = OpenMagnetics::CoreLossesModel::get_models_information();
    info["errors"] = OpenMagnetics::CoreLossesModel::get_models_errors();
    info["internal_links"] = OpenMagnetics::CoreLossesModel::get_models_internal_links();
    info["external_links"] = OpenMagnetics::CoreLossesModel::get_models_external_links();
    info["available_models"] = OpenMagnetics::CoreLossesModel::get_methods_string(material);
    return info;
}

json get_core_temperature_model_information(){
    json info;
    info["information"] = OpenMagnetics::CoreTemperatureModel::get_models_information();
    info["errors"] = OpenMagnetics::CoreTemperatureModel::get_models_errors();
    info["internal_links"] = OpenMagnetics::CoreTemperatureModel::get_models_internal_links();
    info["external_links"] = OpenMagnetics::CoreTemperatureModel::get_models_external_links();
    return info;
}

json calculate_induced_voltage(json excitationJson, double magnetizingInductance){
    OpenMagnetics::OperatingPointExcitation excitation(excitationJson);

    auto voltage = OpenMagnetics::InputsWrapper::calculate_induced_voltage(excitation, magnetizingInductance);

    json result;
    to_json(result, voltage);
    return result;
}

json calculate_induced_current(json excitationJson, double magnetizingInductance){
    OpenMagnetics::OperatingPointExcitation excitation(excitationJson);

    auto current = OpenMagnetics::InputsWrapper::calculate_magnetizing_current(excitation, magnetizingInductance);

    if (excitation.get_voltage()) {
        if (excitation.get_voltage().value().get_processed()) {
            if (excitation.get_voltage().value().get_processed().value().get_duty_cycle()) {
                auto processed = current.get_processed().value();
                processed.set_duty_cycle(excitation.get_voltage().value().get_processed().value().get_duty_cycle().value());
                current.set_processed(processed);
            }
        }
    }

    json result;
    to_json(result, current);
    return result;
}

json calculate_reflected_secondary(json primaryExcitationJson, double turnRatio){
    OpenMagnetics::OperatingPointExcitation primaryExcitation(primaryExcitationJson);

    OpenMagnetics::OperatingPointExcitation excitationOfThisWinding(primaryExcitation);
    auto currentSignalDescriptorProcessed = OpenMagnetics::InputsWrapper::calculate_basic_processed_data(primaryExcitation.get_current().value().get_waveform().value());
    auto voltageSignalDescriptorProcessed = OpenMagnetics::InputsWrapper::calculate_basic_processed_data(primaryExcitation.get_voltage().value().get_waveform().value());

    auto voltageSignalDescriptor = OpenMagnetics::InputsWrapper::reflect_waveform(primaryExcitation.get_voltage().value(), 1.0 / turnRatio, voltageSignalDescriptorProcessed.get_label());
    auto currentSignalDescriptor = OpenMagnetics::InputsWrapper::reflect_waveform(primaryExcitation.get_current().value(), turnRatio, currentSignalDescriptorProcessed.get_label());

    auto voltageSampledWaveform = OpenMagnetics::InputsWrapper::calculate_sampled_waveform(voltageSignalDescriptor.get_waveform().value(), excitationOfThisWinding.get_frequency());
    voltageSignalDescriptor.set_harmonics(OpenMagnetics::InputsWrapper::calculate_harmonics_data(voltageSampledWaveform, excitationOfThisWinding.get_frequency()));
    voltageSignalDescriptor.set_processed(OpenMagnetics::InputsWrapper::calculate_processed_data(voltageSignalDescriptor, voltageSampledWaveform, true));

    auto currentSampledWaveform = OpenMagnetics::InputsWrapper::calculate_sampled_waveform(currentSignalDescriptor.get_waveform().value(), excitationOfThisWinding.get_frequency());
    currentSignalDescriptor.set_harmonics(OpenMagnetics::InputsWrapper::calculate_harmonics_data(currentSampledWaveform, excitationOfThisWinding.get_frequency()));
    currentSignalDescriptor.set_processed(OpenMagnetics::InputsWrapper::calculate_processed_data(currentSignalDescriptor, currentSampledWaveform, true));

    excitationOfThisWinding.set_voltage(voltageSignalDescriptor);
    excitationOfThisWinding.set_current(currentSignalDescriptor);

    json result;
    to_json(result, excitationOfThisWinding);
    return result;
}

json calculate_reflected_primary(json secondaryExcitationJson, double turnRatio){
    OpenMagnetics::OperatingPointExcitation secondaryExcitation(secondaryExcitationJson);

    OpenMagnetics::OperatingPointExcitation excitationOfThisWinding(secondaryExcitation);
    auto voltageSignalDescriptor = OpenMagnetics::InputsWrapper::reflect_waveform(secondaryExcitation.get_voltage().value(), turnRatio);
    auto currentSignalDescriptor = OpenMagnetics::InputsWrapper::reflect_waveform(secondaryExcitation.get_current().value(), 1.0 / turnRatio);

    auto voltageSampledWaveform = OpenMagnetics::InputsWrapper::calculate_sampled_waveform(voltageSignalDescriptor.get_waveform().value(), excitationOfThisWinding.get_frequency());
    voltageSignalDescriptor.set_harmonics(OpenMagnetics::InputsWrapper::calculate_harmonics_data(voltageSampledWaveform, excitationOfThisWinding.get_frequency()));
    voltageSignalDescriptor.set_processed(OpenMagnetics::InputsWrapper::calculate_processed_data(voltageSignalDescriptor, voltageSampledWaveform, true));

    auto currentSampledWaveform = OpenMagnetics::InputsWrapper::calculate_sampled_waveform(currentSignalDescriptor.get_waveform().value(), excitationOfThisWinding.get_frequency());
    currentSignalDescriptor.set_harmonics(OpenMagnetics::InputsWrapper::calculate_harmonics_data(currentSampledWaveform, excitationOfThisWinding.get_frequency()));
    currentSignalDescriptor.set_processed(OpenMagnetics::InputsWrapper::calculate_processed_data(currentSignalDescriptor, currentSampledWaveform, true));

    excitationOfThisWinding.set_voltage(voltageSignalDescriptor);
    excitationOfThisWinding.set_current(currentSignalDescriptor);

    json result;
    to_json(result, excitationOfThisWinding);
    return result;
}

double calculate_instantaneous_power(json excitationJson){
    OpenMagnetics::OperatingPointExcitation excitation(excitationJson);

    if (!excitation.get_current().value().get_processed().value().get_rms().value()) {
        auto current = excitation.get_current().value();
        auto processed = OpenMagnetics::InputsWrapper::calculate_processed_data(current.get_harmonics().value(), current.get_waveform().value(), true);
        current.set_processed(processed);
        excitation.set_current(current);
    }
    if (!excitation.get_voltage().value().get_processed().value().get_rms().value()) {
        auto voltage = excitation.get_voltage().value();
        auto processed = OpenMagnetics::InputsWrapper::calculate_processed_data(voltage.get_harmonics().value(), voltage.get_waveform().value(), true);
        voltage.set_processed(processed);
        excitation.set_voltage(voltage);
    }

    auto instantaneousPower = OpenMagnetics::InputsWrapper::calculate_instantaneous_power(excitation);

    return instantaneousPower;
}

double calculate_rms_power(json excitationJson){
    OpenMagnetics::OperatingPointExcitation excitation(excitationJson);

    auto voltageSignalDescriptor = excitation.get_voltage().value();
    auto currentSignalDescriptor = excitation.get_current().value();

    if (!voltageSignalDescriptor.get_processed() || !voltageSignalDescriptor.get_processed()->get_rms()) {
        auto voltageSampledWaveform = OpenMagnetics::InputsWrapper::calculate_sampled_waveform(voltageSignalDescriptor.get_waveform().value(), excitation.get_frequency());
        voltageSignalDescriptor.set_harmonics(OpenMagnetics::InputsWrapper::calculate_harmonics_data(voltageSampledWaveform, excitation.get_frequency()));
        voltageSignalDescriptor.set_processed(OpenMagnetics::InputsWrapper::calculate_processed_data(voltageSignalDescriptor, voltageSampledWaveform, true));
    }

    if (!currentSignalDescriptor.get_processed() || !currentSignalDescriptor.get_processed()->get_rms()) {
        auto currentSampledWaveform = OpenMagnetics::InputsWrapper::calculate_sampled_waveform(currentSignalDescriptor.get_waveform().value(), excitation.get_frequency());
        currentSignalDescriptor.set_harmonics(OpenMagnetics::InputsWrapper::calculate_harmonics_data(currentSampledWaveform, excitation.get_frequency()));
        currentSignalDescriptor.set_processed(OpenMagnetics::InputsWrapper::calculate_processed_data(currentSignalDescriptor, currentSampledWaveform, true));
    }

    double rmsPower = currentSignalDescriptor.get_processed().value().get_rms().value() * voltageSignalDescriptor.get_processed().value().get_rms().value();

    return rmsPower;
}

double resolve_dimension_with_tolerance(json dimensionWithToleranceJson) {
    OpenMagnetics::DimensionWithTolerance dimensionWithTolerance(dimensionWithToleranceJson);
    return OpenMagnetics::resolve_dimensional_values(dimensionWithTolerance);
}

json calculate_basic_processed_data(json waveformJson) {
    OpenMagnetics::Waveform waveform(waveformJson);
    auto processed = OpenMagnetics::InputsWrapper::calculate_basic_processed_data(waveform);
    json result;
    to_json(result, processed);
    return result;
}

json create_waveform(json processedJson, double frequency) {
    OpenMagnetics::Processed processed(processedJson);
    auto waveform = OpenMagnetics::InputsWrapper::create_waveform(processed, frequency);
    json result;
    to_json(result, waveform);
    return result;
}

json scale_waveform_time_to_frequency(json waveformJson, double newFrequency) {
    OpenMagnetics::Waveform waveform(waveformJson);
    auto scaledWaveform = OpenMagnetics::InputsWrapper::scale_time_to_frequency(waveform, newFrequency);
    json result;
    to_json(result, scaledWaveform);
    return result;
}

json calculate_insulation(json inputsJson){
    auto standard = OpenMagnetics::InsulationCoordinator();
    OpenMagnetics::InputsWrapper inputs(inputsJson, false);

    json result;
    try {
        result["creepageDistance"] = standard.calculate_creepage_distance(inputs);
        result["clearance"] = standard.calculate_clearance(inputs);
        result["withstandVoltage"] = standard.calculate_withstand_voltage(inputs);
        result["distanceThroughInsulation"] = standard.calculate_distance_through_insulation(inputs);
        result["errorMessage"] = "";
    }
    catch(const std::runtime_error& re)
    {
        result["errorMessage"] = "Exception: " + std::string{re.what()};
    }
    catch(const std::exception& ex)
    {
        result["errorMessage"] = "Exception: " + std::string{ex.what()};
    }
    catch(...)
    {
        result["errorMessage"] = "Unknown failure occurred. Possible memory corruption";
    }
    return result;
}

json extract_operating_point(json fileJson, size_t numberWindings, double frequency, double desiredMagnetizingInductance, json mapColumnNamesJson){
    try {
        std::vector<std::map<std::string, std::string>> mapColumnNames = mapColumnNamesJson.get<std::vector<std::map<std::string, std::string>>>();
        auto reader = OpenMagnetics::CircuitSimulationReader(fileJson);
        auto operatingPoint = reader.extract_operating_point(numberWindings, frequency, mapColumnNames);
        operatingPoint = OpenMagnetics::InputsWrapper::process_operating_point(operatingPoint, desiredMagnetizingInductance);
        json result;
        to_json(result, operatingPoint);
        return result;
    }
    catch(...)
    {
        return "Error processing waveforms, please check column names and frequency";
    }
}

json extract_map_column_names(json fileJson, size_t numberWindings, double frequency){
    auto reader = OpenMagnetics::CircuitSimulationReader(fileJson);
    auto columnNames = reader.extract_map_column_names(numberWindings, frequency);

    json result = json::array();
    for (auto& columnName : columnNames) {
        json aux;
        for (auto& [signal, name] : columnName) {
            aux[signal] = name;
        }
        result.push_back(aux);
    }
    return result;
}

json extract_column_names(json fileJson){
    auto reader = OpenMagnetics::CircuitSimulationReader(fileJson);
    auto columnNames = reader.extract_column_names();

    json result = json::array();
    for (auto& columnName : columnNames) {
        result.push_back(columnName);
    }
    return result;
}

std::vector<int> calculate_number_turns(int numberTurnsPrimary, json designRequirementsJson){
    OpenMagnetics::DesignRequirements designRequirements(designRequirementsJson);

    OpenMagnetics::NumberTurns numberTurns(numberTurnsPrimary, designRequirements);
    auto numberTurnsCombination = numberTurns.get_next_number_turns_combination();

    std::vector<int> numberTurnsResult;
    for (auto turns : numberTurnsCombination) {
        numberTurnsResult.push_back(static_cast<std::make_signed<int>::type>(turns));
    }
    return numberTurnsResult;
}

double calculate_dc_resistance_per_meter(json wireJson, double temperature){
    OpenMagnetics::WireWrapper wire(wireJson);
    auto dcResistancePerMeter = OpenMagnetics::WindingOhmicLosses::calculate_dc_resistance_per_meter(wire, temperature);
    return dcResistancePerMeter;
}

double calculate_dc_losses_per_meter(json wireJson, json currentJson, double temperature){
    OpenMagnetics::WireWrapper wire(wireJson);
    OpenMagnetics::SignalDescriptor current(currentJson);
    auto dcLossesPerMeter = OpenMagnetics::WindingOhmicLosses::calculate_ohmic_losses_per_meter(wire, current, temperature);
    return dcLossesPerMeter;
}

double calculate_skin_ac_factor(json wireJson, json currentJson, double temperature){
    OpenMagnetics::WireWrapper wire(wireJson);
    OpenMagnetics::SignalDescriptor current(currentJson);
    auto dcLossesPerMeter = OpenMagnetics::WindingOhmicLosses::calculate_ohmic_losses_per_meter(wire, current, temperature);
    auto [skinLossesPerMeter, _] = OpenMagnetics::WindingSkinEffectLosses::calculate_skin_effect_losses_per_meter(wire, current, temperature);
    auto skinAcFactor = (skinLossesPerMeter + dcLossesPerMeter) / dcLossesPerMeter;
    return skinAcFactor;
}

double calculate_skin_ac_losses_per_meter(json wireJson, json currentJson, double temperature){
    OpenMagnetics::WireWrapper wire(wireJson);
    OpenMagnetics::SignalDescriptor current(currentJson);
    auto [skinLossesPerMeter, _] = OpenMagnetics::WindingSkinEffectLosses::calculate_skin_effect_losses_per_meter(wire, current, temperature);
    return skinLossesPerMeter;
}

double calculate_skin_ac_resistance_per_meter(json wireJson, json currentJson, double temperature){
    OpenMagnetics::WireWrapper wire(wireJson);
    OpenMagnetics::SignalDescriptor current(currentJson);
    auto dcLossesPerMeter = OpenMagnetics::WindingOhmicLosses::calculate_ohmic_losses_per_meter(wire, current, temperature);
    auto [skinLossesPerMeter, _] = OpenMagnetics::WindingSkinEffectLosses::calculate_skin_effect_losses_per_meter(wire, current, temperature);
    auto skinAcFactor = (skinLossesPerMeter + dcLossesPerMeter) / dcLossesPerMeter;
    auto dcResistancePerMeter = OpenMagnetics::WindingOhmicLosses::calculate_dc_resistance_per_meter(wire, temperature);

    return dcResistancePerMeter * skinAcFactor;
}

double calculate_effective_current_density(json wireJson, json currentJson, double temperature){
    OpenMagnetics::WireWrapper wire(wireJson);
    OpenMagnetics::SignalDescriptor current(currentJson);
    auto effectiveCurrentDensity = wire.calculate_effective_current_density(current, temperature);

    return effectiveCurrentDensity;
}

double calculate_effective_skin_depth(std::string materialName, json currentJson, double temperature){
    try {
        OpenMagnetics::SignalDescriptor current(currentJson);

        if (!current.get_processed()->get_effective_frequency()) {
            throw std::runtime_error("Current processed is missing field effective frequency");
        }
        auto currentEffectiveFrequency = current.get_processed()->get_effective_frequency().value();
        double effectiveSkinDepth = OpenMagnetics::WindingSkinEffectLosses::calculate_skin_depth(materialName, currentEffectiveFrequency, temperature);
        return effectiveSkinDepth;
    }
    catch(const std::exception& ex)
    {
        return -1;
    }
}

std::vector<std::string> get_available_winding_orientations(){
    std::vector<std::string> orientations;
    for (auto& [orientation, _] : magic_enum::enum_entries<OpenMagnetics::WindingOrientation>()) {
        json orientationJson;
        to_json(orientationJson, orientation);
        orientations.push_back(orientationJson);
    }
    return orientations;
}

std::vector<std::string> get_available_coil_alignments(){
    std::vector<std::string> orientations;
    for (auto& [orientation, _] : magic_enum::enum_entries<OpenMagnetics::CoilAlignment>()) {
        json orientationJson;
        to_json(orientationJson, orientation);
        orientations.push_back(orientationJson);
    }
    return orientations;
}

bool check_requirement(json requirementJson, double value){
    try {
        OpenMagnetics::DimensionWithTolerance requirement(requirementJson);
        bool result = OpenMagnetics::check_requirement(requirement, value);
        return result;
    }
    catch(const std::exception& ex)
    {
        return false;
    }
}

json wind(json coilJson, size_t repetitions, json proportionPerWindingJson, json patternJson, json marginPairsJson) {
    try {
        std::vector<std::vector<double>> marginPairs;
        for (auto elem : marginPairsJson) {
            std::vector<double> vectorElem;
            for (auto value : elem) {
                vectorElem.push_back(value);
            }
            marginPairs.push_back(vectorElem);
        }

        std::vector<double> proportionPerWinding = proportionPerWindingJson;
        std::vector<size_t> pattern = patternJson;
        std::vector<OpenMagnetics::CoilFunctionalDescription> coilFunctionalDescription;
        for (auto elem : coilJson["functionalDescription"]) {
            coilFunctionalDescription.push_back(OpenMagnetics::CoilFunctionalDescription(elem));
        }
        OpenMagnetics::CoilWrapper coil;
        coil.set_bobbin(coilJson["bobbin"]);
        coil.set_functional_description(coilFunctionalDescription);
        coil.preload_margins(marginPairs);
        if (coilJson.contains("_layersOrientation")) {

            if (coilJson["_layersOrientation"].is_object()) {
                std::map<std::string, OpenMagnetics::WindingOrientation> layersOrientationPerSection;
                for (auto [key, value] : coilJson["_layersOrientation"].items()) {
                    layersOrientationPerSection[key] = value;
                }

                for (auto [sectionName, layerOrientation] : layersOrientationPerSection) {
                    coil.set_layers_orientation(layerOrientation, sectionName);
                }
            }
            else if (coilJson["_layersOrientation"].is_array()) {
                coil.wind_by_sections(proportionPerWinding, pattern, repetitions);
                if (coil.get_sections_description()) {
                    auto sections = coil.get_sections_description_conduction();

                    std::vector<OpenMagnetics::WindingOrientation> layersOrientationPerSection;
                    for (auto elem : coilJson["_layersOrientation"]) {
                        layersOrientationPerSection.push_back(OpenMagnetics::WindingOrientation(elem));
                    }

                    for (size_t sectionIndex = 0; sectionIndex < sections.size(); ++sectionIndex) {
                        if (sectionIndex < layersOrientationPerSection.size()) {
                            coil.set_layers_orientation(layersOrientationPerSection[sectionIndex], sections[sectionIndex].get_name());
                        }
                    }
                }
            }
            else {
                OpenMagnetics::WindingOrientation layerOrientation(coilJson["_layersOrientation"]);
                coil.set_layers_orientation(layerOrientation);

            }
        }

        if (coilJson.contains("_turnsAlignment")) {
            if (coilJson["_turnsAlignment"].is_object()) {
                std::map<std::string, OpenMagnetics::CoilAlignment> turnsAlignmentPerSection;
                for (auto [key, value] : coilJson["_turnsAlignment"].items()) {
                    turnsAlignmentPerSection[key] = value;
                }


                for (auto [sectionName, turnsAlignment] : turnsAlignmentPerSection) {
                    coil.set_turns_alignment(turnsAlignment, sectionName);
                }
            }
            else if (coilJson["_turnsAlignment"].is_array()) {
                coil.wind_by_sections(proportionPerWinding, pattern, repetitions);
                if (coil.get_sections_description()) {
                    auto sections = coil.get_sections_description_conduction();

                    std::vector<OpenMagnetics::CoilAlignment> turnsAlignmentPerSection;
                    for (auto elem : coilJson["_turnsAlignment"]) {
                        turnsAlignmentPerSection.push_back(OpenMagnetics::CoilAlignment(elem));
                    }

                    for (size_t sectionIndex = 0; sectionIndex < sections.size(); ++sectionIndex) {
                        if (sectionIndex < turnsAlignmentPerSection.size()) {
                            coil.set_turns_alignment(turnsAlignmentPerSection[sectionIndex], sections[sectionIndex].get_name());
                        }
                    }
                }
            }
            else {
                OpenMagnetics::CoilAlignment turnsAlignment(coilJson["_turnsAlignment"]);
                coil.set_turns_alignment(turnsAlignment);
            }
        }

        bool windResult = false;

        if (proportionPerWinding.size() == coilFunctionalDescription.size()) {
            if (pattern.size() > 0 && repetitions > 0) {
                windResult = coil.wind(proportionPerWinding, pattern, repetitions);
            }
            else if (repetitions > 0) {
                windResult = coil.wind(repetitions);
            }
            else {
                windResult = coil.wind();
            }
        }
        else {
            if (pattern.size() > 0 && repetitions > 0) {
                windResult = coil.wind(pattern, repetitions);
            }
            else if (repetitions > 0) {
                windResult = coil.wind(repetitions);
            }
            else {
                windResult = coil.wind();
            }
        }

        if (!coil.get_turns_description()) {
            throw std::runtime_error("Turns not created");
        }

        json result;
        to_json(result, coil);
        return result;
    }
    catch (const std::exception &exc) {
        std::cout << coilJson << std::endl;
        std::cout << repetitions << std::endl;
        std::cout << proportionPerWindingJson << std::endl;
        std::cout << patternJson << std::endl;
        return "Exception: " + std::string{exc.what()};
    }
}

json wind_by_sections(json coilJson, size_t repetitions, json proportionPerWindingJson, json patternJson) {
    try {

        std::vector<double> proportionPerWinding = proportionPerWindingJson;
        std::vector<size_t> pattern = patternJson;
        std::vector<OpenMagnetics::CoilFunctionalDescription> coilFunctionalDescription;
        for (auto elem : coilJson["functionalDescription"]) {
            coilFunctionalDescription.push_back(OpenMagnetics::CoilFunctionalDescription(elem));
        }
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
                coil.wind_by_sections(proportionPerWinding, pattern, repetitions);
            }
            else if (repetitions > 0) {
                coil.wind_by_sections(repetitions);
            }
            else {
                coil.wind_by_sections();
            }
        }
        else {
            if (pattern.size() > 0 && repetitions > 0) {
                coil.wind_by_sections(pattern, repetitions);
            }
            else if (repetitions > 0) {
                coil.wind_by_sections(repetitions);
            }
            else {
                coil.wind_by_sections();
            }
        }

        json result;
        to_json(result, coil);
        return result;
    }
    catch (const std::exception &exc) {
        return "Exception: " + std::string{exc.what()};
    }
}

json wind_by_layers(json coilJson, json insulationLayersJson) {
    try {
        std::map<std::pair<size_t, size_t>, std::vector<OpenMagnetics::Layer>> insulationLayers;

        for (auto [key, layersJson] : insulationLayersJson.items()) {
            auto keys = OpenMagnetics::split(key, ",");
            std::pair<size_t, size_t> windingsMapKey(stoi(keys[0]), stoi(keys[1]));
            std::vector<OpenMagnetics::Layer> layers;
            for (auto layerJson : layersJson) {
                layers.push_back(OpenMagnetics::Layer(layerJson));
            }
            insulationLayers[windingsMapKey] = layers;
        }

        std::vector<OpenMagnetics::CoilFunctionalDescription> coilFunctionalDescription;
        for (auto elem : coilJson["functionalDescription"]) {
            coilFunctionalDescription.push_back(OpenMagnetics::CoilFunctionalDescription(elem));
        }

        std::vector<OpenMagnetics::Section> coilSectionsDescription;
        for (auto elem : coilJson["sectionsDescription"]) {
            coilSectionsDescription.push_back(OpenMagnetics::Section(elem));
        }
        OpenMagnetics::CoilWrapper coil;


        if (insulationLayers.size() > 0) {
            coil.set_insulation_layers(insulationLayers);
        }

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
        coil.set_sections_description(coilSectionsDescription);
        coil.wind_by_layers();

        json result;
        to_json(result, coil);
        return result;
    }
    catch (const std::exception &exc) {
        return "Exception: " + std::string{exc.what()};
    }
}

json wind_by_turns(json coilJson) {
    try {

        std::vector<OpenMagnetics::CoilFunctionalDescription> coilFunctionalDescription;
        for (auto elem : coilJson["functionalDescription"]) {
            coilFunctionalDescription.push_back(OpenMagnetics::CoilFunctionalDescription(elem));
        }
        std::vector<OpenMagnetics::Section> coilSectionsDescription;
        for (auto elem : coilJson["sectionsDescription"]) {
            coilSectionsDescription.push_back(OpenMagnetics::Section(elem));
        }
        std::vector<OpenMagnetics::Layer> coilLayersDescription;
        for (auto elem : coilJson["layersDescription"]) {
            coilLayersDescription.push_back(OpenMagnetics::Layer(elem));
        }

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
        coil.set_sections_description(coilSectionsDescription);
        coil.set_layers_description(coilLayersDescription);
        coil.wind_by_turns();

        json result;
        to_json(result, coil);
        return result;
    }
    catch (const std::exception &exc) {
        return "Exception: " + std::string{exc.what()};
    }
}

json delimit_and_compact(json coilJson) {
    try {

        std::vector<OpenMagnetics::CoilFunctionalDescription> coilFunctionalDescription;
        for (auto elem : coilJson["functionalDescription"]) {
            coilFunctionalDescription.push_back(OpenMagnetics::CoilFunctionalDescription(elem));
        }
        std::vector<OpenMagnetics::Section> coilSectionsDescription;
        for (auto elem : coilJson["sectionsDescription"]) {
            coilSectionsDescription.push_back(OpenMagnetics::Section(elem));
        }
        std::vector<OpenMagnetics::Layer> coilLayersDescription;
        for (auto elem : coilJson["layersDescription"]) {
            coilLayersDescription.push_back(OpenMagnetics::Layer(elem));
        }
        std::vector<OpenMagnetics::Turn> coilTurnsDescription;
        for (auto elem : coilJson["turnsDescription"]) {
            coilTurnsDescription.push_back(OpenMagnetics::Turn(elem));
        }


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
        coil.set_sections_description(coilSectionsDescription);
        coil.set_layers_description(coilLayersDescription);
        coil.set_turns_description(coilTurnsDescription);
        coil.delimit_and_compact();

        json result;
        to_json(result, coil);
        return result;
    }
    catch (const std::exception &exc) {
        return "Exception: " + std::string{exc.what()};
    }
}

json get_layers_by_winding_index(json coilJson, int windingIndex){
    try {
        OpenMagnetics::CoilWrapper coil(coilJson);

        json result = json::array();
        for (auto& layer : coil.get_layers_by_winding_index(windingIndex)) {
            json aux;
            to_json(aux, layer);
            result.push_back(aux);
        }
        return result;
    }
    catch (const std::exception &exc) {
        return "Exception: " + std::string{exc.what()};
    }
}

json get_layers_by_section(json coilJson, json sectionName){
    try {
        json result = json::array();
        OpenMagnetics::CoilWrapper coil(coilJson);
        for (auto& layer : coil.get_layers_by_section(sectionName)) {
            json aux;
            to_json(aux, layer);
            result.push_back(aux);
        }
        return result;
    }
    catch (const std::exception &exc) {
        return "Exception: " + std::string{exc.what()};
    }
}

json get_sections_description_conduction(json coilJson){
    try {
        json result = json::array();
        OpenMagnetics::CoilWrapper coil(coilJson);
        for (auto& section : coil.get_sections_description_conduction()) {
            json aux;
            to_json(aux, section);
            result.push_back(aux);
        }
        return result;
    }
    catch (const std::exception &exc) {
        return "Exception: " + std::string{exc.what()};
    }
}

bool are_sections_and_layers_fitting(json coilJson) {
    try {
        json result = json::array();
        OpenMagnetics::CoilWrapper coil(coilJson);
        return coil.are_sections_and_layers_fitting();
    }
    catch (const std::exception &exc) {
        std::cout << "Exception: " + std::string{exc.what()} << std::endl;
        return false;
    }
}

json add_margin_to_section_by_index(json coilJson, int sectionIndex, double top_or_left_margin, double bottom_or_right_margin) {
    try {
        OpenMagnetics::CoilWrapper coil(coilJson);
        coil.add_margin_to_section_by_index(sectionIndex, {top_or_left_margin, bottom_or_right_margin});

        json result;
        to_json(result, coil);
        return result;
    }
    catch (const std::exception &exc) {
        return "Exception: " + std::string{exc.what()};
    }
}

json simulate(json inputsJson,
                     json magneticJson,
                     json modelsData){
    try {
        OpenMagnetics::MagneticWrapper magnetic(magneticJson);
        OpenMagnetics::InputsWrapper inputs(inputsJson);

        auto defaults = OpenMagnetics::Defaults();

        std::map<std::string, std::string> models = modelsData.get<std::map<std::string, std::string>>();

        auto reluctanceModelName = defaults.reluctanceModelDefault;
        if (models.find("reluctance") != models.end()) {
            std::string modelNameJsonUpper = models["reluctance"];
            std::transform(modelNameJsonUpper.begin(), modelNameJsonUpper.end(), modelNameJsonUpper.begin(), ::toupper);
            reluctanceModelName = magic_enum::enum_cast<OpenMagnetics::ReluctanceModels>(modelNameJsonUpper).value();
        }
        auto coreLossesModelName = defaults.coreLossesModelDefault;
        if (models.find("coreLosses") != models.end()) {
            std::string modelNameJsonUpper = models["coreLosses"];
            std::transform(modelNameJsonUpper.begin(), modelNameJsonUpper.end(), modelNameJsonUpper.begin(), ::toupper);
            coreLossesModelName = magic_enum::enum_cast<OpenMagnetics::CoreLossesModels>(modelNameJsonUpper).value();
        }
        auto coreTemperatureModelName = defaults.coreTemperatureModelDefault;
        if (models.find("coreTemperature") != models.end()) {
            std::string modelNameJsonUpper = models["coreTemperature"];
            std::transform(modelNameJsonUpper.begin(), modelNameJsonUpper.end(), modelNameJsonUpper.begin(), ::toupper);
            coreTemperatureModelName = magic_enum::enum_cast<OpenMagnetics::CoreTemperatureModels>(modelNameJsonUpper).value();
        }

        OpenMagnetics::MagneticSimulator magneticSimulator;

        magneticSimulator.set_core_losses_model_name(coreLossesModelName);
        magneticSimulator.set_core_temperature_model_name(coreTemperatureModelName);
        magneticSimulator.set_reluctance_model_name(reluctanceModelName);
        auto mas = magneticSimulator.simulate(inputs, magnetic);

        json result;
        to_json(result, mas);

        return result;
    }
    catch (const std::exception &exc) {
        return "Exception: " + std::string{exc.what()};
    }
}

bool check_if_fits(json bobbinJson, double dimension, bool isHorizontalOrRadial) {
    try {
        OpenMagnetics::BobbinWrapper bobbin(bobbinJson);
        return bobbin.check_if_fits(dimension, isHorizontalOrRadial);
    }
    catch (const std::exception &exc) {
        std::cout << "Exception: " + std::string{exc.what()} << std::endl;
        return false;
    }
}

double get_coating_thickness(json wireJson){
    try {
        OpenMagnetics::WireWrapper wire(wireJson);
        return wire.get_coating_thickness();
    }
    catch (const std::exception &exc) {
        std::cout << std::string{exc.what()}  << std::endl;
        return -1;
    }
}

double get_coating_relative_permittivity(json wireJson){
    try {
        OpenMagnetics::WireWrapper wire(wireJson);
        return wire.get_coating_relative_permittivity();
    }
    catch (const std::exception &exc) {
        std::cout << std::string{exc.what()}  << std::endl;
        return -1;
    }
}

json get_coating_insulation_material(json wireJson){
    try {
        OpenMagnetics::WireWrapper wire(wireJson);
        auto material = wire.resolve_coating_insulation_material();

        json result;
        to_json(result, material);
        return result;
    }
    catch (const std::exception &exc) {
        return "Exception: " + std::string{exc.what()};
    }
}

json get_insulation_layer_insulation_material(json coilJson, std::string layerName){
    try {
        OpenMagnetics::CoilWrapper coil(coilJson);
        auto material = OpenMagnetics::CoilWrapper::resolve_insulation_layer_insulation_material(coil, layerName);

        json result;
        to_json(result, material);
        return result;
    }
    catch (const std::exception &exc) {
        return "Exception: " + std::string{exc.what()};
    }
}


ordered_json export_magnetic_as_subcircuit(json magneticJson) {
    try {
        OpenMagnetics::MagneticWrapper magnetic(magneticJson);

        ordered_json subcircuit = OpenMagnetics::CircuitSimulatorExporter().export_magnetic_as_subcircuit(magnetic);

        return subcircuit.dump(4);
    }
    catch (const std::exception &exc) {
        return "Exception: " + std::string{exc.what()};
    }
}


PYBIND11_MODULE(PyMKF, m) {
    m.def("get_constants", &get_constants, "");
    m.def("get_core_materials", &get_core_materials, "");
    m.def("get_material_permeability", &get_material_permeability, "");
    m.def("get_material_resistivity", &get_material_resistivity, "");
    m.def("get_core_material_steinmetz_coefficients", &get_core_material_steinmetz_coefficients, "");
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
    m.def("create_basic_bobbin", &create_basic_bobbin, "");
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
    m.def("plot_wire", &plot_wire, "");
    m.def("plot_current_density", &plot_current_density, "");
    m.def("set_settings", &set_settings, "");
    m.def("get_settings", &get_settings, "");
    m.def("reset_settings", &reset_settings, "");
    m.def("calculate_harmonics", &calculate_harmonics, "");
    m.def("calculate_processed", &calculate_processed, "");
    m.def("calculate_bobbin_data", &calculate_bobbin_data, "");
    m.def("get_wire_data", &get_wire_data, "");
    m.def("get_wire_data_by_name", &get_wire_data_by_name, "");
    m.def("get_wire_data_by_standard_name", &get_wire_data_by_standard_name, "");
    m.def("get_strand_by_standard_name", &get_strand_by_standard_name, "");
    m.def("get_wire_outer_width_rectangular", &get_wire_outer_width_rectangular, "");
    m.def("get_wire_outer_height_rectangular", &get_wire_outer_height_rectangular, "");
    m.def("get_wire_outer_diameter_bare_litz", &get_wire_outer_diameter_bare_litz, "");
    m.def("get_wire_outer_diameter_served_litz", &get_wire_outer_diameter_served_litz, "");
    m.def("get_wire_outer_diameter_insulated_litz", &get_wire_outer_diameter_insulated_litz, "");
    m.def("get_wire_outer_diameter_enamelled_round", &get_wire_outer_diameter_enamelled_round, "");
    m.def("get_wire_outer_diameter_insulated_round", &get_wire_outer_diameter_insulated_round, "");
    m.def("get_wire_conducting_diameter_by_standard_name", &get_wire_conducting_diameter_by_standard_name, "");
    m.def("get_outer_dimensions", &get_outer_dimensions, "");
    m.def("get_equivalent_wire", &get_equivalent_wire, "");
    m.def("get_coating_label", &get_coating_label, "");
    m.def("get_wire_coating_by_label", &get_wire_coating_by_label, "");
    m.def("get_coating_labels_by_type", &get_coating_labels_by_type, "");
    m.def("load_core_data", &load_core_data, "");
    m.def("get_material_data", &get_material_data, "");
    m.def("get_core_temperature_dependant_parameters", &get_core_temperature_dependant_parameters, "");
    m.def("calculate_shape_data", &calculate_shape_data, "");
    m.def("get_shape_data", &get_shape_data, "");
    m.def("get_available_shape_families", &get_available_shape_families, "");
    m.def("get_available_core_materials", &get_available_core_materials, "");
    m.def("get_available_core_manufacturers", &get_available_core_manufacturers, "");
    m.def("get_available_core_shape_families", &get_available_core_shape_families, "");
    m.def("get_available_core_shapes", &get_available_core_shapes, "");
    m.def("get_available_wires", &get_available_wires, "");
    m.def("get_unique_wire_diameters", &get_unique_wire_diameters, "");
    m.def("get_available_wire_types", &get_available_wire_types, "");
    m.def("get_available_wire_standards", &get_available_wire_standards, "");
    m.def("calculate_gap_reluctance", &calculate_gap_reluctance, "");
    m.def("get_gap_reluctance_model_information", &get_gap_reluctance_model_information, "");
    m.def("calculate_number_turns_from_gapping_and_inductance", &calculate_number_turns_from_gapping_and_inductance, "");
    m.def("calculate_gapping_from_number_turns_and_inductance", &calculate_gapping_from_number_turns_and_inductance, "");
    m.def("get_core_losses_model_information", &get_core_losses_model_information, "");
    m.def("get_core_temperature_model_information", &get_core_temperature_model_information, "");
    m.def("resolve_dimension_with_tolerance", &resolve_dimension_with_tolerance, "");
    m.def("calculate_induced_voltage", &calculate_induced_voltage, "");
    m.def("calculate_induced_current", &calculate_induced_current, "");
    m.def("calculate_reflected_secondary", &calculate_reflected_secondary, "");
    m.def("calculate_reflected_primary", &calculate_reflected_primary, "");
    m.def("calculate_instantaneous_power", &calculate_instantaneous_power, "");
    m.def("calculate_rms_power", &calculate_rms_power, "");
    m.def("calculate_basic_processed_data", &calculate_basic_processed_data, "");
    m.def("create_waveform", &create_waveform, "");
    m.def("scale_waveform_time_to_frequency", &scale_waveform_time_to_frequency, "");
    m.def("calculate_insulation", &calculate_insulation, "");
    m.def("extract_operating_point", &extract_operating_point, "");
    m.def("extract_map_column_names", &extract_map_column_names, "");
    m.def("extract_column_names", &extract_column_names, "");
    m.def("calculate_number_turns", &calculate_number_turns, "");
    m.def("calculate_dc_resistance_per_meter", &calculate_dc_resistance_per_meter, "");
    m.def("calculate_dc_losses_per_meter", &calculate_dc_losses_per_meter, "");
    m.def("calculate_skin_ac_losses_per_meter", &calculate_skin_ac_losses_per_meter, "");
    m.def("calculate_skin_ac_factor", &calculate_skin_ac_factor, "");
    m.def("calculate_skin_ac_resistance_per_meter", &calculate_skin_ac_resistance_per_meter, "");
    m.def("calculate_effective_current_density", &calculate_effective_current_density, "");
    m.def("calculate_effective_skin_depth", &calculate_effective_skin_depth, "");
    m.def("get_available_winding_orientations", &get_available_winding_orientations, "");
    m.def("get_available_coil_alignments", &get_available_coil_alignments, "");
    m.def("check_requirement", &check_requirement, "");
    m.def("wind_by_sections", &wind_by_sections, "");
    m.def("wind_by_layers", &wind_by_layers, "");
    m.def("wind_by_turns", &wind_by_turns, "");
    m.def("delimit_and_compact", &delimit_and_compact, "");
    m.def("get_layers_by_winding_index", &get_layers_by_winding_index, "");
    m.def("get_layers_by_section", &get_layers_by_section, "");
    m.def("get_sections_description_conduction", &get_sections_description_conduction, "");
    m.def("simulate", &simulate, "");
    m.def("are_sections_and_layers_fitting", &are_sections_and_layers_fitting, "");
    m.def("add_margin_to_section_by_index", &add_margin_to_section_by_index, "");
    m.def("check_if_fits", &check_if_fits, "");
    m.def("get_coating_thickness", &get_coating_thickness, "");
    m.def("get_coating_relative_permittivity", &get_coating_relative_permittivity, "");
    m.def("get_coating_insulation_material", &get_coating_insulation_material, "");
    m.def("get_insulation_layer_insulation_material", &get_insulation_layer_insulation_material, "");
    m.def("export_magnetic_as_subcircuit", &export_magnetic_as_subcircuit, "");
}