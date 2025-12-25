#include "core.h"

namespace PyMKF {

json get_core_materials() {
    try {
        auto materials = OpenMagnetics::get_materials(std::nullopt);
        json result = json::array();
        for (auto elem: materials) {
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

double get_material_permeability(json materialName, double temperature, double magneticFieldDcBias, double frequency) {
    try {
        auto materialData = OpenMagnetics::find_core_material_by_name(materialName);
        OpenMagnetics::InitialPermeability initialPermeability;
        return initialPermeability.get_initial_permeability(materialData, temperature, magneticFieldDcBias, frequency);
    }
    catch (const std::exception &exc) {
        throw std::runtime_error("Exception: " + std::string{exc.what()});
    }
}

double get_material_resistivity(json materialName, double temperature) {
    try {
        auto materialData = OpenMagnetics::find_core_material_by_name(materialName);
        auto resistivityModel = OpenMagnetics::ResistivityModel::factory(OpenMagnetics::ResistivityModels::CORE_MATERIAL);
        return (*resistivityModel).get_resistivity(materialData, temperature);
    }
    catch (const std::exception &exc) {
        throw std::runtime_error("Exception: " + std::string{exc.what()});
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

json get_core_shape_families() {
    try {
        auto shapes = OpenMagnetics::get_shapes(false);
        std::vector<CoreShapeFamily> families;
        json result = json::array();
        for (auto elem : shapes) {
            auto family = elem.get_family();
            if (std::find(families.begin(), families.end(), family) == families.end()) {
                families.push_back(family);
                json aux;
                to_json(aux, family);
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

json get_core_material_names() {
    try {
        auto materialNames = OpenMagnetics::get_core_material_names(std::nullopt);
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
        auto materialNames = OpenMagnetics::get_core_material_names(manufacturerName);
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
        OpenMagnetics::settings->set_use_toroidal_cores(includeToroidal);
        auto shapeNames = OpenMagnetics::get_core_shape_names();
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

json calculate_core_processed_description(json coreDataJson) {
    try {
        OpenMagnetics::Core core(coreDataJson, false, false, false);
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

json calculate_core_geometrical_description(json coreDataJson) {
    try {
        OpenMagnetics::Core core(coreDataJson, false, false, false);
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

json calculate_core_gapping(json coreDataJson) {
    try {
        OpenMagnetics::Core core(coreDataJson, false, false, false);
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

json calculate_shape_data(json shapeJson) {
    CoreShape shape(shapeJson);
    OpenMagnetics::Core core;
    CoreFunctionalDescription coreFunctionalDescription;
    coreFunctionalDescription.set_shape(shape);
    coreFunctionalDescription.set_material("Dummy");
    coreFunctionalDescription.set_number_stacks(1);
    if (shape.get_magnetic_circuit() == MagneticCircuit::OPEN) {
        coreFunctionalDescription.set_type(CoreType::TWO_PIECE_SET);
    }
    else {
        if (shape.get_family() == CoreShapeFamily::T) {
            coreFunctionalDescription.set_type(CoreType::TOROIDAL);
        }
        else {
            coreFunctionalDescription.set_type(CoreType::CLOSED_SHAPE);
        }
    }
    core.set_functional_description(coreFunctionalDescription);
    core.process_data();

    json result;
    to_json(result, core);
    return result;
}

json calculate_core_data(json coreDataJson, bool includeMaterialData) {
    try {
        OpenMagnetics::Core core(coreDataJson, includeMaterialData, true);
        json result;
        to_json(result, core);
        return result;
    }
    catch (const std::exception &exc) {
        return "Exception: " + std::string{exc.what()};
    }
}

json load_core_data(json coresJson) {
    json result = json::array();
    for (auto& coreJson : coresJson) {
        OpenMagnetics::Core core(coreJson, false);
        json aux;
        to_json(aux, core);
        result.push_back(aux);
    }
    return result;
}

json get_material_data(std::string materialName) {
    auto materialData = OpenMagnetics::find_core_material_by_name(materialName);
    json result;
    to_json(result, materialData);
    return result;
}

json get_core_temperature_dependant_parameters(json coreData, double temperature) {
    OpenMagnetics::Core core(coreData);
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

json get_shape_data(std::string shapeName) {
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

std::vector<std::string> get_available_shape_families() {
    std::vector<std::string> families;
    for (auto& family : magic_enum::enum_names<CoreShapeFamily>()) {
        std::string familyJson(family);
        families.push_back(familyJson);
    }
    return families;
}

std::vector<std::string> get_available_core_manufacturers() {
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

std::vector<std::string> get_available_core_shape_families() {
    std::vector<std::string> families;
    for (auto& family : magic_enum::enum_names<CoreShapeFamily>()) {
        std::string familyJson(family);
        families.push_back(familyJson);
    }
    return families;
}

std::vector<std::string> get_available_core_materials(std::string manufacturer) {
    return OpenMagnetics::get_core_material_names(manufacturer);
}

std::vector<std::string> get_available_core_shapes() {
    return OpenMagnetics::get_core_shape_names();
}

json get_available_cores() {
    if (OpenMagnetics::coreMaterialDatabase.empty()) {
        OpenMagnetics::load_cores();
    }

    try {
        json result = json::array();
        for (auto elem : OpenMagnetics::coreDatabase) {
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

json calculate_gap_reluctance(json coreGapData, std::string modelNameString) {
    OpenMagnetics::ReluctanceModels modelName;
    from_json(modelNameString, modelName);
    auto reluctanceModel = OpenMagnetics::ReluctanceModel::factory(modelName);
    CoreGap coreGap(coreGapData);

    auto coreGapResult = reluctanceModel->get_gap_reluctance(coreGap);
    json result;
    to_json(result, coreGapResult);
    return result;
}

json get_gap_reluctance_model_information() {
    json info;
    info["information"] = OpenMagnetics::ReluctanceModel::get_models_information();
    info["errors"] = OpenMagnetics::ReluctanceModel::get_models_errors();
    info["internal_links"] = OpenMagnetics::ReluctanceModel::get_models_internal_links();
    info["external_links"] = OpenMagnetics::ReluctanceModel::get_models_external_links();
    return info;
}

double calculate_inductance_from_number_turns_and_gapping(json coreData, json coilData, json operatingPointData, json modelsData) {
    OpenMagnetics::Core core(coreData);
    OpenMagnetics::Coil coil(coilData);
    OperatingPoint operatingPoint(operatingPointData);

    std::map<std::string, std::string> models = modelsData.get<std::map<std::string, std::string>>();

    auto reluctanceModelName = OpenMagnetics::defaults.reluctanceModelDefault;
    if (models.find("reluctance") != models.end()) {
        std::string modelNameJsonUpper = models["reluctance"];
        std::transform(modelNameJsonUpper.begin(), modelNameJsonUpper.end(), modelNameJsonUpper.begin(), ::toupper);
        reluctanceModelName = magic_enum::enum_cast<OpenMagnetics::ReluctanceModels>(modelNameJsonUpper).value();
    }

    OpenMagnetics::MagnetizingInductance magnetizingInductanceObj(reluctanceModelName);
    double magnetizingInductance = magnetizingInductanceObj.calculate_inductance_from_number_turns_and_gapping(core, coil, &operatingPoint).get_magnetizing_inductance().get_nominal().value();

    return magnetizingInductance;
}

double calculate_number_turns_from_gapping_and_inductance(json coreData, json inputsData, json modelsData) {
    OpenMagnetics::Core core(coreData);
    OpenMagnetics::Inputs inputs(inputsData);

    std::map<std::string, std::string> models = modelsData.get<std::map<std::string, std::string>>();
    
    auto reluctanceModelName = OpenMagnetics::defaults.reluctanceModelDefault;
    if (models.find("reluctance") != models.end()) {
        OpenMagnetics::from_json(models["reluctance"], reluctanceModelName);
    }

    OpenMagnetics::MagnetizingInductance magnetizingInductanceObj(reluctanceModelName);
    double numberTurns = magnetizingInductanceObj.calculate_number_turns_from_gapping_and_inductance(core, &inputs);

    return numberTurns;
}

json calculate_gapping_from_number_turns_and_inductance(json coreData, json coilData, json inputsData, std::string gappingTypeJson, int decimals, json modelsData) {
    OpenMagnetics::Core core(coreData);
    OpenMagnetics::Coil coil(coilData);
    OpenMagnetics::Inputs inputs(inputsData);

    std::map<std::string, std::string> models = modelsData.get<std::map<std::string, std::string>>();
    std::transform(gappingTypeJson.begin(), gappingTypeJson.end(), gappingTypeJson.begin(), ::toupper);
    OpenMagnetics::GappingType gappingType = magic_enum::enum_cast<OpenMagnetics::GappingType>(gappingTypeJson).value();
    
    auto reluctanceModelName = OpenMagnetics::defaults.reluctanceModelDefault;
    if (models.find("reluctance") != models.end()) {
        OpenMagnetics::from_json(models["reluctance"], reluctanceModelName);
    }

    OpenMagnetics::MagnetizingInductance magnetizingInductanceObj(reluctanceModelName);
    std::vector<CoreGap> gapping = magnetizingInductanceObj.calculate_gapping_from_number_turns_and_inductance(core, coil, &inputs, gappingType, decimals);

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

double calculate_core_maximum_magnetic_energy(json coreDataJson, json operatingPointJson) {
    try {
        OperatingPoint operatingPoint = OperatingPoint(operatingPointJson);
        OpenMagnetics::Core core = OpenMagnetics::Core(coreDataJson, false, false, false);
        if (!core.get_processed_description()) {
            core.process_data();
            core.process_gap();
        }

        auto magneticEnergy = OpenMagnetics::MagneticEnergy();

        double coreMaximumMagneticEnergy;
        if (operatingPoint.get_excitations_per_winding().size() == 0) {
            coreMaximumMagneticEnergy = magneticEnergy.calculate_core_maximum_magnetic_energy(core, std::nullopt);
        }
        else {
            coreMaximumMagneticEnergy = magneticEnergy.calculate_core_maximum_magnetic_energy(core, operatingPoint);
        }
        return coreMaximumMagneticEnergy;
    }
    catch (const std::exception &exc) {
        std::cout << "Exception: " + std::string{exc.what()} << std::endl;
        return -1;
    }
}

double calculate_saturation_current(json magneticJson, double temperature) {
    OpenMagnetics::Magnetic magnetic(magneticJson);
    return magnetic.calculate_saturation_current(temperature);
}

double calculate_temperature_from_core_thermal_resistance(json coreJson, double totalLosses) {
    OpenMagnetics::Core core(coreJson);
    return OpenMagnetics::Temperature::calculate_temperature_from_core_thermal_resistance(core, totalLosses);
}

void register_core_bindings(py::module& m) {
    // Core materials
    m.def("get_core_materials", &get_core_materials, "Retrieve all available core materials as JSON objects");
    m.def("get_material_permeability", &get_material_permeability, 
        "Calculate initial permeability for a material at given temperature, DC bias, and frequency",
        py::arg("material_name"), py::arg("temperature"), py::arg("magnetic_field_dc_bias"), py::arg("frequency"));
    m.def("get_material_resistivity", &get_material_resistivity,
        "Calculate resistivity for a material at given temperature",
        py::arg("material_name"), py::arg("temperature"));
    m.def("get_core_material_steinmetz_coefficients", &get_core_material_steinmetz_coefficients,
        "Retrieve Steinmetz coefficients for core loss calculation at given frequency",
        py::arg("material_name"), py::arg("frequency"));

    // Core shapes
    m.def("get_core_shapes", &get_core_shapes, "Retrieve all available core shapes as JSON objects");
    m.def("get_core_shape_families", &get_core_shape_families, "Retrieve list of unique core shape families");

    // Name retrieval functions
    m.def("get_core_material_names", &get_core_material_names, "Retrieve list of all core material names");
    m.def("get_core_material_names_by_manufacturer", &get_core_material_names_by_manufacturer,
        "Retrieve core material names filtered by manufacturer",
        py::arg("manufacturer_name"));
    m.def("get_core_shape_names", &get_core_shape_names,
        "Retrieve list of core shape names",
        py::arg("include_toroidal"));

    // Lookup functions
    m.def("find_core_material_by_name", &find_core_material_by_name, "Find core material data by name");
    m.def("find_core_shape_by_name", &find_core_shape_by_name, "Find core shape data by name");

    // Core calculations
    m.def("calculate_core_data", &calculate_core_data, "Process core data and return complete description");
    m.def("calculate_core_processed_description", &calculate_core_processed_description, "Calculate processed description for a core");
    m.def("calculate_core_geometrical_description", &calculate_core_geometrical_description, "Calculate geometrical description for a core");
    m.def("calculate_core_gapping", &calculate_core_gapping, "Calculate gapping configuration for a core");
    m.def("load_core_data", &load_core_data, "Load core data from JSON");
    m.def("get_material_data", &get_material_data, "Get material data by name");
    m.def("get_core_temperature_dependant_parameters", &get_core_temperature_dependant_parameters, "Get temperature-dependent core parameters");
    m.def("calculate_shape_data", &calculate_shape_data, "Calculate shape parameters");
    m.def("get_shape_data", &get_shape_data, "Get shape data by name");

    // Availability queries
    m.def("get_available_shape_families", &get_available_shape_families, "Get list of available shape families");
    m.def("get_available_core_materials", &get_available_core_materials, "Get list of available core materials");
    m.def("get_available_core_manufacturers", &get_available_core_manufacturers, "Get list of core manufacturers");
    m.def("get_available_core_shape_families", &get_available_core_shape_families, "Get list of available core shape families");
    m.def("get_available_core_shapes", &get_available_core_shapes, "Get list of available core shapes");
    m.def("get_available_cores", &get_available_cores, "Get list of all available cores");

    // Gap and reluctance
    m.def("calculate_gap_reluctance", &calculate_gap_reluctance, "Calculate magnetic reluctance of an air gap");
    m.def("get_gap_reluctance_model_information", &get_gap_reluctance_model_information, "Get information about gap reluctance models");
    m.def("calculate_inductance_from_number_turns_and_gapping", &calculate_inductance_from_number_turns_and_gapping,
        "Calculate inductance from turns count and gap configuration");
    m.def("calculate_number_turns_from_gapping_and_inductance", &calculate_number_turns_from_gapping_and_inductance,
        "Calculate required number of turns from gap and target inductance");
    m.def("calculate_gapping_from_number_turns_and_inductance", &calculate_gapping_from_number_turns_and_inductance,
        "Calculate required gap from turns count and target inductance");

    // Additional core functions
    m.def("calculate_core_maximum_magnetic_energy", &calculate_core_maximum_magnetic_energy, "Calculate maximum magnetic energy in core");
    m.def("calculate_saturation_current", &calculate_saturation_current, "Calculate saturation current");
    m.def("calculate_temperature_from_core_thermal_resistance", &calculate_temperature_from_core_thermal_resistance, 
        "Calculate temperature rise from thermal resistance");
}

} // namespace PyMKF
