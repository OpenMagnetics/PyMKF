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

json calculate_harmonics(json waveformJson, double frequency) {
    OpenMagnetics::Waveform waveform;
    OpenMagnetics::from_json(waveformJson, waveform);

    auto sampledCurrentWaveform = OpenMagnetics::InputsWrapper::calculate_sampled_waveform(waveform, frequency);
    auto harmonics = OpenMagnetics::InputsWrapper::calculate_harmonics_data(sampledCurrentWaveform, frequency);

    json result;
    to_json(result, harmonics);
    return result.dump(4);
}

json calculate_processed(json harmonicsJson, json waveformJson) {
    OpenMagnetics::Waveform waveform;
    OpenMagnetics::Harmonics harmonics;
    OpenMagnetics::from_json(waveformJson, waveform);
    OpenMagnetics::from_json(harmonicsJson, harmonics);

    auto processed = OpenMagnetics::InputsWrapper::calculate_processed_data(harmonics, waveform, true);

    json result;
    to_json(result, processed);
    return result.dump(4);
}

json calculate_core_data(json coreDataJson, bool includeMaterialData){
    OpenMagnetics::CoreWrapper core(coreDataJson, includeMaterialData);
    json result;
    to_json(result, core);
    return result.dump(4);
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
    return result.dump(4);
}

json load_core_data(json coresJson){
    json result = json::array();
    for (auto& coreJson : coresJson) {
        OpenMagnetics::CoreWrapper core(coreJson, false);
        json aux;
        to_json(aux, core);
        result.push_back(aux);
    }
    return result.dump(4);
}

json get_material_data(std::string materialName){
    auto materialData = OpenMagnetics::find_core_material_by_name(materialName);
    json result;
    to_json(result, materialData);
    return result.dump(4);
}

json get_core_temperature_dependant_parameters(json coreJson, double temperature){
    OpenMagnetics::CoreWrapper core(coreJson);
    json result;

    result["magneticFluxDensitySaturation"] = core.get_magnetic_flux_density_saturation(temperature, false);
    result["magneticFieldStrengthSaturation"] = core.get_magnetic_field_strength_saturation(temperature);
    result["initialPermeability"] = core.get_initial_permeability(temperature);
    result["effectivePermeability"] = core.get_effective_permeability(temperature);
    result["reluctance"] = core.get_reluctance(temperature);
    result["resistivity"] = core.get_resistivity(temperature);

    return result.dump(4);
}

json get_shape_data(std::string shapeName){
    auto shapeData = OpenMagnetics::find_core_shape_by_name(shapeName);

    json result;
    to_json(result, shapeData);
    return result.dump(4);
}

py::list get_available_shape_families(){
    py::list families;
    for (auto& family : magic_enum::enum_names<OpenMagnetics::CoreShapeFamily>()) {
        std::string familyString(family);
        families.append(familyString);
    }
    return families;
}

py::list get_available_core_materials(std::string manufacturer){
    py::list material_names;
    auto materialNames = OpenMagnetics::get_material_names(manufacturer);
    for (auto material : materialNames) {
        material_names.append(material);
    }
    return material_names;
}

py::list get_available_core_shapes(){
    py::list shape_names;
    auto shapeNames = OpenMagnetics::get_shape_names();
    for (auto shape : shapeNames) {
        shape_names.append(shape);
    }
    return shape_names;
}

json calculate_gap_reluctance(json coreGapJson, std::string modelNameString){
    std::string modelNameStringUpper = modelNameString;
    std::transform(modelNameStringUpper.begin(), modelNameStringUpper.end(), modelNameStringUpper.begin(), ::toupper);
    auto modelName = magic_enum::enum_cast<OpenMagnetics::ReluctanceModels>(modelNameStringUpper);

    auto reluctanceModel = OpenMagnetics::ReluctanceModel::factory(modelName.value());
    OpenMagnetics::CoreGap coreGap(coreGapJson);

    auto coreGapResult = reluctanceModel->get_gap_reluctance(coreGap);
    json result;
    to_json(result, coreGapResult);
    return result.dump(4);
}

json get_gap_reluctance_model_information(){
    json info;
    info["information"] = OpenMagnetics::ReluctanceModel::get_models_information();
    info["errors"] = OpenMagnetics::ReluctanceModel::get_models_errors();
    info["internal_links"] = OpenMagnetics::ReluctanceModel::get_models_internal_links();
    info["external_links"] = OpenMagnetics::ReluctanceModel::get_models_external_links();
    return info.dump(4);
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
        std::string modelNameStringUpper = models["reluctance"];
        std::transform(modelNameStringUpper.begin(), modelNameStringUpper.end(), modelNameStringUpper.begin(), ::toupper);
        reluctanceModelName = magic_enum::enum_cast<OpenMagnetics::ReluctanceModels>(modelNameStringUpper).value();
    }

    OpenMagnetics::MagnetizingInductance magnetizing_inductance(reluctanceModelName);
    double magnetizingInductance = magnetizing_inductance.calculate_inductance_from_number_turns_and_gapping(core, coil, &operatingPoint).get_magnetizing_inductance().get_nominal().value();

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
        std::string modelNameStringUpper = models["reluctance"];
        std::transform(modelNameStringUpper.begin(), modelNameStringUpper.end(), modelNameStringUpper.begin(), ::toupper);
        reluctanceModelName = magic_enum::enum_cast<OpenMagnetics::ReluctanceModels>(modelNameStringUpper).value();
    }

    OpenMagnetics::MagnetizingInductance magnetizing_inductance(reluctanceModelName);
    double numberTurns = magnetizing_inductance.calculate_number_turns_from_gapping_and_inductance(core, &inputs);

    return numberTurns;
}


json calculate_gapping_from_number_turns_and_inductance(json coreData,
                                                        json coilData,
                                                        json inputsData,
                                                        std::string gappingTypeString,
                                                        int decimals,
                                                        json modelsData){
    OpenMagnetics::CoreWrapper core(coreData);
    OpenMagnetics::CoilWrapper coil(coilData);
    OpenMagnetics::InputsWrapper inputs(inputsData);

    std::map<std::string, std::string> models = modelsData.get<std::map<std::string, std::string>>();
    OpenMagnetics::GappingType gappingType = magic_enum::enum_cast<OpenMagnetics::GappingType>(gappingTypeString).value();
    
    auto reluctanceModelName = OpenMagnetics::Defaults().reluctanceModelDefault;
    if (models.find("reluctance") != models.end()) {
        std::string modelNameStringUpper = models["reluctance"];
        std::transform(modelNameStringUpper.begin(), modelNameStringUpper.end(), modelNameStringUpper.begin(), ::toupper);
        reluctanceModelName = magic_enum::enum_cast<OpenMagnetics::ReluctanceModels>(modelNameStringUpper).value();
    }

    OpenMagnetics::MagnetizingInductance magnetizing_inductance(reluctanceModelName);
    std::vector<OpenMagnetics::CoreGap> gapping = magnetizing_inductance.calculate_gapping_from_number_turns_and_inductance(core,
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
    return result.dump(4);
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

    OpenMagnetics::Magnetic magnetic;
    magnetic.set_core(core);
    magnetic.set_coil(coil);

    OpenMagnetics::MagneticSimulator magneticSimulator;
    magneticSimulator.set_core_losses_model_name(coreLossesModelName);
    magneticSimulator.set_core_temperature_model_name(coreTemperatureModelName);
    magneticSimulator.set_reluctance_model_name(reluctanceModelName);
    auto coreLossesOutput = magneticSimulator.calculate_core_loses(operatingPoint, magnetic);
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

    return result.dump(4);
}

json get_core_losses_model_information(std::string material){
    json info;
    info["information"] = OpenMagnetics::CoreLossesModel::get_models_information();
    info["errors"] = OpenMagnetics::CoreLossesModel::get_models_errors();
    info["internal_links"] = OpenMagnetics::CoreLossesModel::get_models_internal_links();
    info["external_links"] = OpenMagnetics::CoreLossesModel::get_models_external_links();
    info["available_models"] = OpenMagnetics::CoreLossesModel::get_methods_string(material);
    return info.dump(4);
}

json get_core_temperature_model_information(){
    json info;
    info["information"] = OpenMagnetics::CoreTemperatureModel::get_models_information();
    info["errors"] = OpenMagnetics::CoreTemperatureModel::get_models_errors();
    info["internal_links"] = OpenMagnetics::CoreTemperatureModel::get_models_internal_links();
    info["external_links"] = OpenMagnetics::CoreTemperatureModel::get_models_external_links();
    return info.dump(4);
}

json calculate_induced_voltage(json excitationJson, double magnetizingInductance){
    OpenMagnetics::OperatingPointExcitation excitation(excitationJson);

    auto voltage = OpenMagnetics::InputsWrapper::calculate_induced_voltage(excitation, magnetizingInductance);

    json result;
    to_json(result, voltage);
    return result.dump(4);
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
    return result.dump(4);
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
    return result.dump(4);
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
    return result.dump(4);
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

    if (!voltageSignalDescriptor.get_processed()) {
        auto voltageSampledWaveform = OpenMagnetics::InputsWrapper::calculate_sampled_waveform(voltageSignalDescriptor.get_waveform().value(), excitation.get_frequency());
        voltageSignalDescriptor.set_harmonics(OpenMagnetics::InputsWrapper::calculate_harmonics_data(voltageSampledWaveform, excitation.get_frequency()));
        voltageSignalDescriptor.set_processed(OpenMagnetics::InputsWrapper::calculate_processed_data(voltageSignalDescriptor, voltageSampledWaveform, true));
    }

    if (!currentSignalDescriptor.get_processed()) {
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
    return result.dump(4);
}

json create_waveform(json processedJson, double frequency) {
    OpenMagnetics::Processed processed(processedJson);
    auto waveform = OpenMagnetics::InputsWrapper::create_waveform(processed, frequency);
    json result;
    to_json(result, waveform);
    return result.dump(4);
}

json scale_waveform_time_to_frequency(json waveformJson, double newFrequency) {
    OpenMagnetics::Waveform waveform(waveformJson);
    auto scaledWaveform = OpenMagnetics::InputsWrapper::scale_time_to_frequency(waveform, newFrequency);
    json result;
    to_json(result, scaledWaveform);
    return result.dump(4);
}

json calculate_advised_cores(json inputsJson, json weightsJson, int maximumNumberResults, bool useOnlyCoresInStock){
    // std::cout << inputsJson << std::endl;
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

    OpenMagnetics::CoreAdviser coreAdviser(false);
    auto masMagnetics = coreAdviser.get_advised_core(inputs, weights, maximumNumberResults);
    auto log = coreAdviser.read_log();
    auto scoring = coreAdviser.get_scorings();
    std::map<std::string, std::map<std::string, double>> filteredScoring;

    json results = json();
    results["data"] = json::array();
    for (auto& masMagnetic : masMagnetics) {
        std::string name = masMagnetic.first.get_magnetic().get_manufacturer_info().value().get_reference().value();
        auto mas = masMagnetic.first;
        // Extra outputs
        {
            OpenMagnetics::MagnetizingInductance magnetizingInductanceModel;
            for (size_t operatingPointIndex = 0; operatingPointIndex < inputs.get_operating_points().size(); ++operatingPointIndex) {
                auto operatingPoint = inputs.get_operating_point(operatingPointIndex);
                auto magnetizingInductanceOutput = magnetizingInductanceModel.calculate_inductance_from_number_turns_and_gapping(mas.get_magnetic().get_core(), mas.get_magnetic().get_coil(), &operatingPoint);
                auto magnetizingInductanceOutputEnergy = mas.get_mutable_outputs()[operatingPointIndex].get_magnetizing_inductance();
                magnetizingInductanceOutput.set_maximum_magnetic_energy_core(magnetizingInductanceOutputEnergy->get_maximum_magnetic_energy_core());
                mas.get_mutable_outputs()[operatingPointIndex].set_magnetizing_inductance(magnetizingInductanceOutput);
                masMagnetic.first = mas;
            }
        }

        json result;
        json masJson;
        to_json(masJson, masMagnetic.first);
        result["mas"] = masJson;
        // result["weightedTotalScoring"] = masMagnetic.second;
        result["weightedTotalScoring"] = scoring[name][OpenMagnetics::CoreAdviser::CoreAdviserFilters::COST] + scoring[name][OpenMagnetics::CoreAdviser::CoreAdviserFilters::EFFICIENCY] + scoring[name][OpenMagnetics::CoreAdviser::CoreAdviserFilters::DIMENSIONS];
        result["scoringPerFilter"] = json();
        for (auto& filter : magic_enum::enum_names<OpenMagnetics::CoreAdviser::CoreAdviserFilters>()) {
            std::string filterString(filter);
            result["scoringPerFilter"][filterString] = scoring[name][magic_enum::enum_cast<OpenMagnetics::CoreAdviser::CoreAdviserFilters>(filterString).value()];
        };
        result["scoringPerFilter"].erase(magic_enum::enum_name(OpenMagnetics::CoreAdviser::CoreAdviserFilters::AREA_PRODUCT));
        result["scoringPerFilter"].erase(magic_enum::enum_name(OpenMagnetics::CoreAdviser::CoreAdviserFilters::ENERGY_STORED));
        results["data"].push_back(result);
    }
    results["log"] = log;

    return results.dump(4);
}

std::vector<std::string> get_available_core_filters(){
    std::vector<std::string> filters;
    for (auto& filter : magic_enum::enum_names<OpenMagnetics::CoreAdviser::CoreAdviserFilters>()) {
        std::string filterString(filter);
        filters.push_back(filterString);
    }
    return filters;
}

json calculate_insulation(json inputsJson){
    auto insulationCoordinator = OpenMagnetics::InsulationCoordinator();
    OpenMagnetics::InputsWrapper inputs(inputsJson, false);

    json result;
    try
    {

        result["creepageDistance"] = insulationCoordinator.calculate_creepage_distance(inputs);
        result["clearance"] = insulationCoordinator.calculate_clearance(inputs);
        result["withstandVoltage"] = insulationCoordinator.calculate_withstand_voltage(inputs);
        result["distanceThroughInsulation"] = insulationCoordinator.calculate_distance_through_insulation(inputs);
        result["errorMessage"] = "";
    }
    catch(const std::runtime_error& re)
    {
        result["errorMessage"] = re.what();
    }
    catch(const std::exception& ex)
    {
        result["errorMessage"] = ex.what();
    }
    catch(...)
    {
        result["errorMessage"] = "Unknown failure occurred. Possible memory corruption";
    }
    return result.dump(4);
}

void load_cores(bool includeToroids, bool useOnlyCoresInStock){
    OpenMagnetics::load_cores(includeToroids, useOnlyCoresInStock);
}

void clear_loaded_cores(){
    OpenMagnetics::clear_loaded_cores();
}


PYBIND11_MODULE(PyMKF, m) {
    m.def("get_constants", &get_constants, "Returns the constants");
    m.def("calculate_harmonics", &calculate_harmonics, "calculate_harmonics");
    m.def("calculate_processed", &calculate_processed, "calculate_processed");
    m.def("calculate_core_data", &calculate_core_data, "Returns the processed data from a core");
    m.def("calculate_shape_data", &calculate_shape_data, "calculate_shape_data");
    m.def("load_core_data", &load_core_data, "load_core_data");
    m.def("get_material_data", &get_material_data, "Returns the all data about a given core material");
    m.def("get_core_temperature_dependant_parameters", &get_core_temperature_dependant_parameters, "get_core_temperature_dependant_parameters");
    m.def("get_shape_data", &get_shape_data, "Returns the all data about a given core shape");
    m.def("get_available_shape_families", &get_available_shape_families, "Returns the available shape families");
    m.def("get_available_core_materials", &get_available_core_materials, "Returns the names of all the core materials");
    m.def("get_available_core_shapes", &get_available_core_shapes, "Returns the names of all the core shapes");
    m.def("calculate_gap_reluctance", &calculate_gap_reluctance, "Returns the reluctance and fringing flux factor of a gap");
    m.def("get_gap_reluctance_model_information", &get_gap_reluctance_model_information, "Returns the information and average error for gap reluctance models");
    m.def("calculate_inductance_from_number_turns_and_gapping", &calculate_inductance_from_number_turns_and_gapping, "Returns the inductance of a core, given its number of turns and gapping");
    m.def("calculate_number_turns_from_gapping_and_inductance", &calculate_number_turns_from_gapping_and_inductance, "Returns the number of turns needed to achieve a given inductance with a given gapping");
    m.def("calculate_gapping_from_number_turns_and_inductance", &calculate_gapping_from_number_turns_and_inductance, "Returns the gapping needed to achieve a given inductance with a given number of turns");
    m.def("calculate_core_losses", &calculate_core_losses, "Returns the core losses according to given model");
    m.def("get_core_losses_model_information", &get_core_losses_model_information, "Returns the information and average error for core losses models");
    m.def("get_core_temperature_model_information", &get_core_temperature_model_information, "Returns the information and average error for core temperature models");
    m.def("resolve_dimension_with_tolerance", &resolve_dimension_with_tolerance, "resolve_dimension_with_tolerance");
    m.def("calculate_induced_voltage", &calculate_induced_voltage, "calculate_induced_voltage");
    m.def("calculate_induced_current", &calculate_induced_current, "calculate_induced_current");
    m.def("calculate_reflected_secondary", &calculate_reflected_secondary, "calculate_reflected_secondary");
    m.def("calculate_reflected_primary", &calculate_reflected_primary, "calculate_reflected_primary");
    m.def("calculate_instantaneous_power", &calculate_instantaneous_power, "calculate_instantaneous_power");
    m.def("calculate_rms_power", &calculate_rms_power, "calculate_rms_power");
    m.def("calculate_basic_processed_data", &calculate_basic_processed_data, "calculate_basic_processed_data");
    m.def("create_waveform", &create_waveform, "create_waveform");
    m.def("scale_waveform_time_to_frequency", &scale_waveform_time_to_frequency, "scale_waveform_time_to_frequency");
    m.def("calculate_advised_cores", &calculate_advised_cores, "calculate_advised_cores");
    m.def("get_available_core_filters", &get_available_core_filters, "get_available_core_filters");
    m.def("calculate_insulation", &calculate_insulation, "calculate_insulation");
    m.def("load_cores", &load_cores, "load_cores");
    m.def("clear_loaded_cores", &clear_loaded_cores, "clear_loaded_cores");
}
