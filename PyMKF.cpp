#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "pybind11_json/pybind11_json.hpp"
#include <magic_enum.hpp>
#include "json.hpp"

#include <MAS.hpp>
#include "advisers/MagneticAdviser.h"
#include "constructive_models/Bobbin.h"
#include "constructive_models/Coil.h"
#include "constructive_models/Core.h"
#include "constructive_models/Insulation.h"
#include "constructive_models/Magnetic.h"
#include "constructive_models/Mas.h"
#include "constructive_models/NumberTurns.h"
#include "constructive_models/Wire.h"
#include "physical_models/InitialPermeability.h"
#include "physical_models/MagneticEnergy.h"
#include "physical_models/Reluctance.h"
#include "physical_models/Temperature.h"
#include "physical_models/MagnetizingInductance.h"
#include "physical_models/CoreLosses.h"
#include "physical_models/Resistivity.h"
#include "physical_models/CoreTemperature.h"
#include "physical_models/LeakageInductance.h"
#include "physical_models/WindingOhmicLosses.h"
#include "physical_models/WindingSkinEffectLosses.h"
#include "processors/Inputs.h"
#include "processors/MagneticSimulator.h"
#include "processors/CircuitSimulatorInterface.h"
#include "support/Painter.h"
#include "support/Utils.h"


using namespace MAS;
using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;


#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x) 

namespace py = pybind11;


std::map<std::string, OpenMagnetics::Mas> masDatabase;

void load_databases(json databasesJson) {
    OpenMagnetics::load_databases(databasesJson, true);
}

std::string read_databases(std::string path, bool addInternalData) {
    try {
        auto masPath = std::filesystem::path{path};
        json data;
        std::string line;
        {
            data["coreMaterials"] = json();
            std::ifstream coreMaterials(masPath.append("core_materials.ndjson"));
            while (getline (coreMaterials, line)) {
                json jf = json::parse(line);
                data["coreMaterials"][jf["name"]] = jf;
            }
        }
        {
            data["coreShapes"] = json();
            std::ifstream coreMaterials(masPath.append("core_shapes.ndjson"));
            while (getline (coreMaterials, line)) {
                json jf = json::parse(line);
                data["coreShapes"][jf["name"]] = jf;
            }
        }
        {
            data["wires"] = json();
            std::ifstream coreMaterials(masPath.append("wires.ndjson"));
            while (getline (coreMaterials, line)) {
                json jf = json::parse(line);
                data["wires"][jf["name"]] = jf;
            }
        }
        {
            data["bobbins"] = json();
            std::ifstream coreMaterials(masPath.append("bobbins.ndjson"));
            while (getline (coreMaterials, line)) {
                json jf = json::parse(line);
                data["bobbins"][jf["name"]] = jf;
            }
        }
        {
            data["insulationMaterials"] = json();
            std::ifstream coreMaterials(masPath.append("insulation_materials.ndjson"));
            while (getline (coreMaterials, line)) {
                json jf = json::parse(line);
                data["insulationMaterials"][jf["name"]] = jf;
            }
        }
        {
            data["wireMaterials"] = json();
            std::ifstream coreMaterials(masPath.append("wire_materials.ndjson"));
            while (getline (coreMaterials, line)) {
                json jf = json::parse(line);
                data["wireMaterials"][jf["name"]] = jf;
            }
        }
        OpenMagnetics::load_databases(data, true, addInternalData);
        return "0";
    }
    catch (const std::exception &exc) {
        return std::string{exc.what()};
    }
}

std::string load_mas(std::string key, json masJson, bool expand) {
    try {
        OpenMagnetics::Mas mas(masJson);
        if (expand) {
            mas.set_magnetic(OpenMagnetics::magnetic_autocomplete(mas.get_mutable_magnetic()));
            mas.set_inputs(OpenMagnetics::inputs_autocomplete(mas.get_mutable_inputs(), mas.get_mutable_magnetic()));
        }
        masDatabase[key] = mas;
        return std::to_string(masDatabase.size());
    }
    catch (const std::exception &exc) {
        return std::string{exc.what()};
    }
}

std::string load_magnetic(std::string key, json magneticJson, bool expand) {
    try {
        OpenMagnetics::Magnetic magnetic(magneticJson);
        if (expand) {
            magnetic = OpenMagnetics::magnetic_autocomplete(magnetic);
        }
        OpenMagnetics::Mas mas;
        mas.set_magnetic(magnetic);
        masDatabase[key] = mas;
        return std::to_string(masDatabase.size());
    }
    catch (const std::exception &exc) {
        return std::string{exc.what()};
    }
}


std::string load_magnetics(std::string keys, json magneticJsons, bool expand) {
    try {
        json keysJson = json::parse(keys);
        for (size_t magneticIndex = 0; magneticIndex < magneticJsons.size(); magneticIndex++) {
            OpenMagnetics::Magnetic magnetic(magneticJsons[magneticIndex]);
            if (expand) {
                magnetic = OpenMagnetics::magnetic_autocomplete(magnetic);
            }
            OpenMagnetics::Mas mas;
            mas.set_magnetic(magnetic);
            masDatabase[to_string(keysJson[magneticIndex])] = mas;
        }
        return std::to_string(masDatabase.size());
    }
    catch (const std::exception &exc) {
        return std::string{exc.what()};
    }
}

// std::string load_magnetics_from_file(std::string path, char separator, size_t magneticIndex, size_t referenceIndex, std::string manufacturerName, bool expand) {
//     try {
//         std::ifstream in(path);
//         // OpenMagnetics::Inputs inputs(inputsJson);

//         bool isHeader = true;

//         std::cout << "Mierda 1" << std::endl;
//         if (in) {
//             std::string line;
//         std::cout << "Mierda 2" << std::endl;

//             while (getline(in, line)) {

//                 if (isHeader) {
//                     std::cout << "isHeader" << std::endl;
//                     isHeader = false;
//                     continue;
//                 }

//                 std::stringstream sep(line);
//                 std::string field;
//         std::cout << "Mierda 3" << std::endl;

//                 std::vector<std::string> row_data;

//                 while (getline(sep, field, separator)) {
//         std::cout << field << std::endl;
//                     row_data.push_back(field);
//                 }
//         std::cout << "Mierda 4" << std::endl;

//                 OpenMagnetics::Magnetic magnetic(json::parse(row_data[magneticIndex]));
//                 MagneticManufacturerInfo manufacturerInfo;
//                 manufacturerInfo.set_name(manufacturerName);
//                 manufacturerInfo.set_reference(row_data[referenceIndex]);
//                 magnetic.set_manufacturer_info(manufacturerInfo);
//                 if (expand) {
//                     magnetic = OpenMagnetics::Mas::expand_magnetic(magnetic);
//                     // inputs = OpenMagnetics::Mas::expand_inputs(magnetic, inputs);
//                 }
//                 OpenMagnetics::Mas mas;
//                 mas.set_magnetic(magnetic);
//                 // mas.set_inputs(inputs);
//                 masDatabase[row_data[referenceIndex]] = mas;
//             }
//         }
//         return std::to_string(masDatabase.size());
//     }
//     catch (const std::exception &exc) {
//         return std::string{exc.what()};
//     }
// }

json read_mas(std::string key) {
    json result;
    to_json(result, masDatabase[key]);
    return result;
}



/**
 * @brief Retrieves a dictionary of constant values used in the OpenMagnetics library.
 * 
 * This function creates a dictionary (py::dict) and populates it with various constant values 
 * from the Constants class. These constants include physical constants, 
 * magnetic properties, and other parameters used in magnetic field calculations.
 * 
 * @return py::dict A dictionary containing the following key-value pairs:
 * - "residualGap": The residual gap value.
 * - "minimumNonResidualGap": The minimum non-residual gap value.
 * - "vacuumPermeability": The permeability of vacuum.
 * - "vacuumPermittivity": The permittivity of vacuum.
 * - "spacerProtudingPercentage": The percentage of spacer protruding.
 * - "coilPainterScale": The scale factor for coil painting.
 * - "minimumDistributedFringingFactor": The minimum distributed fringing factor.
 * - "maximumDistributedFringingFactor": The maximum distributed fringing factor.
 * - "initialGapLengthForSearching": The initial gap length for searching.
 * - "roshenMagneticFieldStrengthStep": The step size for Roshen magnetic field strength.
 * - "foilToSectionMargin": The margin between foil and section.
 * - "planarToSectionMargin": The margin between planar and section.
 */
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


/**
 * @brief Retrieves the default configuration values as a Python dictionary.
 *
 * This function creates an instance of the Defaults class and converts its
 * properties to a Python dictionary using the pybind11 library. The dictionary contains
 * various default settings related to core losses, temperature models, magnetic field
 * strength, and other parameters used in the OpenMagnetics framework.
 *
 * @return py::dict A dictionary containing the default configuration values.
 */
py::dict get_defaults() {
    py::dict defaultsMap;
    json aux;
    to_json(aux, OpenMagnetics::defaults.coreLossesModelDefault);
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

/**
 * @brief Retrieves a list of core materials.
 * 
 * This function calls the get_materials function to obtain a list of core materials.
 * It then converts each material to a JSON object and adds it to a JSON array.
 * If an exception occurs during this process, it catches the exception and returns a JSON object
 * containing the exception message.
 * 
 * @return json A JSON array of core materials or a JSON object containing an exception message.
 */
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

/**
 * @brief Retrieves the initial permeability of a given material based on specified conditions.
 *
 * This function finds the core material by its name and calculates its initial permeability
 * based on the provided temperature, magnetic field DC bias, and frequency.
 *
 * @param materialName The name of the material in JSON format.
 * @param temperature The temperature at which the permeability is to be calculated.
 * @param magneticFieldDcBias The DC bias of the magnetic field.
 * @param frequency The frequency at which the permeability is to be calculated.
 * @return The initial permeability of the material as a double.
 * @throws std::exception If an error occurs during the process, an exception is caught and its message is returned in JSON format.
 */
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

/**
 * @brief Retrieves the resistivity of a given material at a specified temperature.
 *
 * This function uses the OpenMagnetics library to find the core material by name and then
 * calculates its resistivity at the given temperature using the appropriate resistivity model.
 *
 * @param materialName A JSON object containing the name of the material.
 * @param temperature The temperature at which to calculate the resistivity.
 * @return The resistivity of the material at the specified temperature.
 * @throws std::exception If an error occurs while retrieving the material data or calculating resistivity.
 */
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

/**
 * @brief Retrieves the Steinmetz coefficients for a given core material at a specified frequency.
 *
 * This function calls the CoreLossesModel::get_steinmetz_coefficients method to obtain
 * the Steinmetz coefficients for the specified core material and frequency. The result is then converted
 * to a JSON object and returned. If an exception occurs during the process, the exception message is
 * caught and returned as a JSON object.
 *
 * @param materialName The name of the core material for which the Steinmetz coefficients are to be retrieved.
 * @param frequency The frequency at which the Steinmetz coefficients are to be calculated.
 * @return A JSON object containing the Steinmetz coefficients or an exception message if an error occurs.
 */
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

/**
 * @brief Retrieves core shapes and converts them to JSON format.
 *
 * This function calls the get_shapes function to obtain a list of core shapes.
 * It then converts each shape to a JSON object and adds it to a JSON array.
 * If an exception occurs during this process, the exception message is caught and returned in JSON format.
 *
 * @return A JSON array containing the core shapes, or a JSON object with an exception message if an error occurs.
 */
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

/**
 * @brief Retrieves a list of unique core shape families.
 *
 * This function fetches all available core shapes using the OpenMagnetics library,
 * extracts their families, and returns a JSON array of unique core shape families.
 * If an exception occurs during the process, it catches the exception and returns
 * a JSON object containing the exception message.
 *
 * @return json A JSON array of unique core shape families or a JSON object with an exception message.
 */
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

/**
 * @brief Retrieves a list of wires and converts them to JSON format.
 *
 * This function calls the get_wires() function to obtain a list of wires.
 * It then converts each wire to a JSON object using the to_json() function
 * and stores them in a JSON array. If an exception occurs during this process, it catches
 * the exception and returns a JSON object containing the exception message.
 *
 * @return A JSON array containing the wire data, or a JSON object with an exception message if an error occurs.
 */
json get_wires() {
    try {
        auto wires = OpenMagnetics::get_wires();
        json result = json::array();
        for (auto elem : wires) {
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

/**
 * @brief Retrieves a list of bobbins in JSON format.
 *
 * This function calls the get_bobbins() function to get a list of bobbins,
 * converts each bobbin to a JSON object using to_json(), and returns the 
 * list of JSON objects. If an exception occurs during the process, it catches the exception 
 * and returns a JSON object containing the exception message.
 *
 * @return json A JSON array of bobbins or a JSON object with exception details.
 */
json get_bobbins() {
    try {
        auto bobbins = OpenMagnetics::get_bobbins();
        json result = json::array();
        for (auto elem : bobbins) {
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

/**
 * @brief Retrieves a list of insulation materials.
 *
 * This function calls the get_insulation_materials() function to obtain a list of insulation materials.
 * It then converts each material to a JSON object and adds it to a JSON array.
 * If an exception occurs during this process, the exception message is caught and returned as a JSON object.
 *
 * @return A JSON array containing the insulation materials, or a JSON object with an exception message if an error occurs.
 */
json get_insulation_materials() {
    try {
        auto insulationMaterials = OpenMagnetics::get_insulation_materials();
        json result = json::array();
        for (auto elem : insulationMaterials) {
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

/**
 * @brief Retrieves the wire materials and converts them to JSON format.
 *
 * This function calls the get_wire_materials() function to obtain a list of wire materials.
 * It then converts each wire material to a JSON object and adds it to a JSON array.
 * If an exception occurs during this process, it catches the exception and returns a JSON object containing the exception message.
 *
 * @return A JSON array containing the wire materials, or a JSON object with an exception message if an error occurs.
 */
json get_wire_materials() {
    try {
        auto wireMaterials = OpenMagnetics::get_wire_materials();
        json result = json::array();
        for (auto elem : wireMaterials) {
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

/**
 * @brief Retrieves the names of core materials.
 * 
 * This function calls the get_core_material_names function to obtain a list of core material names.
 * It then converts this list into a JSON array and returns it.
 * 
 * @return json A JSON array containing the names of core materials. If an exception occurs, a JSON object with the exception message is returned.
 * 
 * @throws std::exception If an error occurs while retrieving the material names.
 */
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

/**
 * @brief Retrieves the core material names by manufacturer.
 *
 * This function fetches the core material names associated with a given manufacturer
 * and returns them in a JSON array. If an exception occurs during the process, 
 * it catches the exception and returns a JSON object containing the exception message.
 *
 * @param manufacturerName The name of the manufacturer whose core material names are to be retrieved.
 * @return A JSON array containing the core material names. If an exception occurs, 
 *         a JSON object with the exception message is returned.
 */
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

/**
 * @brief Retrieves the names of core shapes.
 *
 * This function fetches the names of core shapes based on the specified settings.
 * It can include or exclude toroidal cores based on the input parameter.
 *
 * @param includeToroidal A boolean flag indicating whether to include toroidal cores in the result.
 * @return A JSON array containing the names of the core shapes. If an exception occurs, 
 *         a JSON object with the exception message is returned.
 */
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

/**
 * @brief Retrieves the names of wires.
 * 
 * This function calls the get_wire_names() function to obtain a list of wire names.
 * It then converts this list into a JSON array and returns it.
 * 
 * @return json A JSON array containing the names of wires. If an exception occurs, a JSON object
 *         with the exception message is returned.
 */
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

/**
 * @brief Retrieves the names of bobbins.
 *
 * This function calls the get_bobbin_names() function to get a list of bobbin names.
 * It then converts this list into a JSON array and returns it.
 *
 * @return json A JSON array containing the names of bobbins. If an exception occurs, a JSON object
 *         containing the exception message is returned.
 */
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

/**
 * @brief Retrieves the names of insulation materials.
 *
 * This function calls the get_insulation_material_names() function to get a list of insulation material names.
 * It then converts the list into a JSON array and returns it.
 * If an exception occurs during the process, it catches the exception and returns a JSON object containing the exception message.
 *
 * @return json A JSON array containing the names of insulation materials, or a JSON object with an exception message if an error occurs.
 */
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

/**
 * @brief Retrieves the names of wire materials.
 *
 * This function calls the get_wire_material_names() function to obtain a list of wire material names.
 * It then converts this list into a JSON array and returns it.
 * If an exception occurs during the process, it catches the exception and returns a JSON object containing the exception message.
 *
 * @return json A JSON array containing the names of wire materials, or a JSON object with an exception message if an error occurs.
 */
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

/**
 * @brief Finds core material data by name.
 *
 * This function searches for core material data using the provided material name.
 * It utilizes the OpenMagnetics library to perform the search and converts the
 * resulting data to a JSON format.
 *
 * @param materialName A JSON object containing the name of the material to search for.
 * @return A JSON object containing the core material data if found, or an exception message if an error occurs.
 */
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

/**
 * @brief Finds core shape data by name.
 *
 * This function attempts to find the core shape data corresponding to the given shape name.
 * If successful, it converts the shape data to a JSON object and returns it.
 * If an exception occurs during the process, it catches the exception and returns a JSON object
 * containing the exception message.
 *
 * @param shapeName The name of the core shape to find.
 * @return A JSON object containing the core shape data if found, or an exception message if an error occurs.
 */
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

/**
 * @brief Finds wire data by its name.
 *
 * This function searches for wire data using the provided wire name.
 * If the wire is found, it returns the wire data in JSON format.
 * If an exception occurs during the search, it catches the exception
 * and returns a JSON object containing the exception message.
 *
 * @param wireName The name of the wire to search for, in JSON format.
 * @return A JSON object containing the wire data if found, or an exception message if an error occurs.
 */
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

/**
 * @brief Finds bobbin data by its name.
 *
 * This function attempts to find the bobbin data corresponding to the given bobbin name.
 * It uses the OpenMagnetics library to perform the search and converts the result to a JSON object.
 * If an exception occurs during the search, it catches the exception and returns a JSON object
 * containing the exception message.
 *
 * @param bobbinName The name of the bobbin to search for, provided as a JSON object.
 * @return A JSON object containing the bobbin data if found, or an exception message if an error occurs.
 */
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


/**
 * @brief Finds insulation material data by its name.
 *
 * This function searches for insulation material data using the provided name.
 * It returns the data in JSON format if found, or an exception message in JSON format if an error occurs.
 *
 * @param insulationMaterialName The name of the insulation material to search for, in JSON format.
 * @return A JSON object containing the insulation material data if found, or an exception message if an error occurs.
 */
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

/**
 * @brief Finds wire material data by name.
 *
 * This function searches for wire material data using the provided wire material name.
 * It utilizes the OpenMagnetics library to perform the search and converts the result
 * to a JSON object.
 *
 * @param wireMaterialName A JSON object containing the name of the wire material to search for.
 * @return A JSON object containing the wire material data if found, or an exception message if an error occurs.
 */
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



json find_wire_by_dimension(double dimension, json wireTypeJson, json wireStandardJson) {
    try {
        WireType wireType;
        from_json(wireTypeJson, wireType);
        WireStandard wireStandard;
        from_json(wireStandardJson, wireStandard);
        auto wireMaterialData = OpenMagnetics::find_wire_by_dimension(dimension, wireType, wireStandard, false);
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


/**
 * @brief Creates a basic bobbin from the given core data.
 *
 * This function takes a JSON object containing core data and a boolean flag indicating whether to use null dimensions.
 * It creates a core object using the provided core data and then creates a bobbin using the core object.
 * The resulting bobbin is converted to a JSON object and returned.
 * If an exception occurs during the process, the exception message is caught and returned as a JSON object.
 *
 * @param coreDataJson A JSON object containing the core data.
 * @param nullDimensions A boolean flag indicating whether to use null dimensions.
 * @return A JSON object representing the created bobbin, or an exception message if an error occurs.
 */
json create_basic_bobbin(json coreDataJson, bool nullDimensions){
    try {
        OpenMagnetics::Core core(coreDataJson, false, false, false);
        auto bobbin = OpenMagnetics::Bobbin::create_quick_bobbin(core, nullDimensions);

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

json create_basic_bobbin_by_thickness(json coreDataJson, double thickness){
    try {
        OpenMagnetics::Core core(coreDataJson, false, false, false);
        auto bobbin = OpenMagnetics::Bobbin::create_quick_bobbin(core, thickness);

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



/**
 * @brief Processes core data and returns the processed description.
 *
 * This function takes a JSON object containing core data, processes it using the Core,
 * and returns the processed description as a JSON object. If an exception occurs during processing, 
 * it catches the exception and returns a JSON object containing the exception message.
 *
 * @param coreDataJson A JSON object containing the core data to be processed.
 * @return A JSON object containing the processed core description or an exception message.
 */
json calculate_core_processed_description(json coreDataJson){
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

json calculate_core_geometrical_description(json coreDataJson){
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

json calculate_core_gapping(json coreDataJson){
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

json get_default_models() {
    try {
        json models;
        auto reluctanceModelName = OpenMagnetics::to_string(OpenMagnetics::defaults.reluctanceModelDefault);
        models["reluctance"] = reluctanceModelName;
        auto coreLossesModelName = OpenMagnetics::to_string(OpenMagnetics::defaults.coreLossesModelDefault);
        models["coreLosses"] = coreLossesModelName;
        auto coreTemperatureModelName = OpenMagnetics::to_string(OpenMagnetics::defaults.coreTemperatureModelDefault);
        models["coreTemperature"] = coreTemperatureModelName;

        return models;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

json calculate_advised_cores(json inputsJson, json weightsJson, int maximumNumberResults, json coreModeJson){
    try {
        OpenMagnetics::Inputs inputs(inputsJson);
        OpenMagnetics::CoreAdviser::CoreAdviserModes coreMode;
        from_json(coreModeJson, coreMode);
        std::map<std::string, double> weightsKeysJson = weightsJson;
        std::map<OpenMagnetics::CoreAdviser::CoreAdviserFilters, double> weights;

        weights[OpenMagnetics::CoreAdviser::CoreAdviserFilters::COST] = 1;
        weights[OpenMagnetics::CoreAdviser::CoreAdviserFilters::EFFICIENCY] = 1;
        weights[OpenMagnetics::CoreAdviser::CoreAdviserFilters::DIMENSIONS] = 1;

        for (auto const& [filterName, weight] : weightsKeysJson) {
            OpenMagnetics::CoreAdviser::CoreAdviserFilters filter;
            OpenMagnetics::from_json(filterName, filter);
            weights[filter] = weight;
        }

        OpenMagnetics::CoreAdviser coreAdviser;
        coreAdviser.set_mode(coreMode);
        auto masMagnetics = coreAdviser.get_advised_core(inputs, weights, maximumNumberResults);

        json results = json::array();
        for (auto& masMagnetic : masMagnetics) {
            json aux;
            to_json(aux, masMagnetic.first);
            results.push_back(aux);
        }
        OpenMagnetics::settings->reset();

        return results;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}


json calculate_advised_magnetics(json inputsJson, int maximumNumberResults, json coreModeJson){
    try {
        OpenMagnetics::Inputs inputs(inputsJson);
        OpenMagnetics::CoreAdviser::CoreAdviserModes coreMode;
        from_json(coreModeJson, coreMode);

        OpenMagnetics::MagneticAdviser magneticAdviser;
        magneticAdviser.set_core_mode(coreMode);
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

std::string plot_core(json magneticJson, std::string outFile) {
    try {
        OpenMagnetics::Magnetic magnetic(magneticJson);
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
        OpenMagnetics::Magnetic magnetic(magneticJson);
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
        OpenMagnetics::Magnetic magnetic(magneticJson);
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
        OpenMagnetics::Magnetic magnetic(magneticJson);
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
        OpenMagnetics::Magnetic magnetic(magneticJson);
        OperatingPoint operatingPoint(operatingPointJson);
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
        OpenMagnetics::Wire wire(wireJson);
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
        OpenMagnetics::Wire wire(wireJson);
        OperatingPoint operatingPoint(operatingPointJson);
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

void reset_settings() {
    OpenMagnetics::settings->reset();
}

/**
 * @brief Calculate the harmonics of a given waveform.
 *
 * This function takes a JSON representation of a waveform and a frequency, 
 * then calculates the harmonics of the waveform.
 *
 * @param waveformJson A JSON object representing the waveform.
 * @param frequency The frequency at which to sample the waveform.
 * @return A JSON object containing the harmonics data.
 */
json calculate_harmonics(json waveformJson, double frequency) {
    Waveform waveform;
    from_json(waveformJson, waveform);

    auto sampledCurrentWaveform = OpenMagnetics::Inputs::calculate_sampled_waveform(waveform, frequency);
    auto harmonics = OpenMagnetics::Inputs::calculate_harmonics_data(sampledCurrentWaveform, frequency);

    json result;
    to_json(result, harmonics);
    return result;
}

/**
 * @brief Calculate processed data from harmonics and waveform JSON inputs.
 *
 * This function takes JSON representations of harmonics and waveform data,
 * converts them to their respective OpenMagnetics types, and then calculates
 * the processed data using the OpenMagnetics::Inputs::calculate_processed_data
 * method. The result is then converted back to JSON and returned.
 *
 * @param harmonicsJson JSON object containing harmonics data.
 * @param waveformJson JSON object containing waveform data.
 * @return JSON object containing the processed data.
 */
json calculate_processed(json harmonicsJson, json waveformJson) {
    Waveform waveform;
    Harmonics harmonics;
    from_json(waveformJson, waveform);
    from_json(harmonicsJson, harmonics);

    auto processed = OpenMagnetics::Inputs::calculate_processed_data(harmonics, waveform, true);

    json result;
    to_json(result, processed);
    return result;
}

/**
 * @brief Calculate shape data based on the provided JSON input.
 *
 * This function takes a JSON object representing the shape data, processes it using the OpenMagnetics library,
 * and returns the processed data as a JSON object.
 *
 * @param shapeJson A JSON object containing the shape data.
 * @return A JSON object containing the processed shape data.
 */
json calculate_shape_data(json shapeJson){
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

/**
 * @brief Calculate core data from the given JSON input.
 *
 * This function takes a JSON object representing core data and processes it
 * to generate a result JSON object. It optionally includes material data
 * based on the provided flag.
 *
 * @param coreDataJson A JSON object containing the core data to be processed.
 * @param includeMaterialData A boolean flag indicating whether to include material data in the processing.
 * @return A JSON object containing the processed core data. If an exception occurs, a JSON object with the exception message is returned.
 */
json calculate_core_data(json coreDataJson, bool includeMaterialData){
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

json calculate_bobbin_data(json magneticJson){
    try {
        OpenMagnetics::Magnetic magnetic(magneticJson);

        auto optionalBobbin = magnetic.get_coil().get_bobbin();
        OpenMagnetics::Bobbin bobbin;

        if (std::holds_alternative<std::string>(optionalBobbin)) {
            auto bobbinJson = std::get<std::string>(optionalBobbin);
            if (bobbinJson == "Dummy") {
                bobbin = OpenMagnetics::Bobbin::create_quick_bobbin(magnetic.get_mutable_core());
            }
        }
        else {
            bobbin = OpenMagnetics::Bobbin(std::get<OpenMagnetics::Bobbin>(optionalBobbin));
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

json process_bobbin(json bobbinJson){
    try {
        OpenMagnetics::Bobbin bobbin(bobbinJson);
        bobbin.process_data();

        json result;
        to_json(result, bobbin);
        return result;
    }
    catch (const std::exception &exc) {
        return "Exception: " + std::string{exc.what()};
    }
}

json get_wire_data(json windingDataJson){
    OpenMagnetics::Winding winding(windingDataJson);
    auto wire = OpenMagnetics::Coil::resolve_wire(winding);
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
    WireStandard wireStandard;
    from_json(wireStandardJson, wireStandard);
    return OpenMagnetics::Wire::get_outer_width_rectangular(conductingWidth, grade, wireStandard);
}

double get_wire_outer_height_rectangular(double conductingHeight, int grade, json wireStandardJson){
    WireStandard wireStandard;
    from_json(wireStandardJson, wireStandard);
    return OpenMagnetics::Wire::get_outer_height_rectangular(conductingHeight, grade, wireStandard);
}

double get_wire_outer_diameter_bare_litz(double conductingDiameter, int numberConductors, int grade, json wireStandardJson) {
    WireStandard wireStandard;
    from_json(wireStandardJson, wireStandard);
    return OpenMagnetics::Wire::get_outer_diameter_bare_litz(conductingDiameter, numberConductors, grade, wireStandard);
}

double get_wire_outer_diameter_served_litz(double conductingDiameter, int numberConductors, int grade, int numberLayers, json wireStandardJson) {
    WireStandard wireStandard;
    from_json(wireStandardJson, wireStandard);
    return OpenMagnetics::Wire::get_outer_diameter_served_litz(conductingDiameter, numberConductors, grade, numberLayers, wireStandard);
}

double get_wire_outer_diameter_insulated_litz(double conductingDiameter, int numberConductors, int numberLayers, double thicknessLayers, int grade, json wireStandardJson) {
    WireStandard wireStandard;
    from_json(wireStandardJson, wireStandard);
    return OpenMagnetics::Wire::get_outer_diameter_insulated_litz(conductingDiameter, numberConductors, numberLayers, thicknessLayers, grade, wireStandard);
}

double get_wire_outer_diameter_enamelled_round(double conductingDiameter, int grade, json wireStandardJson) {
    WireStandard wireStandard;
    from_json(wireStandardJson, wireStandard);
    return OpenMagnetics::Wire::get_outer_diameter_round(conductingDiameter, grade, wireStandard);
}

double get_wire_outer_diameter_insulated_round(double conductingDiameter, int numberLayers, double thicknessLayers, json wireStandardJson) {
    WireStandard wireStandard;
    from_json(wireStandardJson, wireStandard);
    return OpenMagnetics::Wire::get_outer_diameter_round(conductingDiameter, numberLayers, thicknessLayers, wireStandard);
}

std::vector<double> get_outer_dimensions(json wireJson) {
    OpenMagnetics::Wire wire(wireJson);
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
        if (coating->get_type() != InsulationWireCoatingType::ENAMELLED) {
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
        OpenMagnetics::Wire oldWire(oldWireJson);
        WireType newWireType;
        from_json(newWireTypeJson, newWireType);

        auto newWire = OpenMagnetics::Wire::get_equivalent_wire(oldWire, newWireType, effectivefrequency);

        json result;
        to_json(result, newWire);
        return result;
    }
    catch (const std::exception &exc) {
        std::cout << std::string{exc.what()} << std::endl;

        return "Exception: " + std::string{exc.what()};
    }
}

json get_coating(json wireJson){
    try {
        OpenMagnetics::Wire wire(wireJson);
        InsulationWireCoating insulationWireCoating;
        if (wire.resolve_coating()) {
            insulationWireCoating = wire.resolve_coating().value();
        }
        else {
            insulationWireCoating.set_type(InsulationWireCoatingType::BARE);
        }
        json result;
        to_json(result, insulationWireCoating);
        return result;
    }
    catch (const std::exception &exc) {
        return "Exception: " + std::string{exc.what()};
    }
}

json get_coating_label(json wireJson){
    try {
        OpenMagnetics::Wire wire(wireJson);
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

/**
 * @brief Retrieves the wire coating information based on the provided label.
 *
 * This function iterates through a collection of wires, encodes their coating labels,
 * and compares them with the provided label. If a match is found, it resolves the coating
 * information of the wire. If the coating information cannot be resolved, it sets the 
 * coating type to BARE. The resulting coating information is then converted to a JSON object.
 *
 * @param label The label used to identify the wire coating.
 * @return A JSON object containing the wire coating information.
 */
json get_wire_coating_by_label(std::string label){
    auto wires = OpenMagnetics::get_wires();
    InsulationWireCoating insulationWireCoating;
    for (auto wire : wires) {
        auto coatingLabel = wire.encode_coating_label();
        if (coatingLabel == label) {
            if (wire.resolve_coating()) {
                insulationWireCoating = wire.resolve_coating().value();
            }
            else {
                insulationWireCoating.set_type(InsulationWireCoatingType::BARE);
            }
            break;
        }
    }
    json result;
    to_json(result, insulationWireCoating);
    return result;
}

/**
 * @brief Retrieves a list of unique coating labels for a given wire type.
 * 
 * This function takes a JSON object representing a wire type, converts it to an 
 * WireType object, and retrieves a list of wires of that type. 
 * It then extracts the coating labels from these wires, ensuring that each label 
 * is unique, and returns the list of unique coating labels.
 * 
 * @param wireTypeJson A JSON object representing the wire type.
 * @return std::vector<std::string> A vector containing unique coating labels.
 */
std::vector<std::string> get_coating_labels_by_type(json wireTypeJson){
    WireType wireType(wireTypeJson);

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

/**
 * @brief Loads core data from a JSON array and converts it to a new JSON array.
 *
 * This function takes a JSON array of core data, processes each core using the 
 * Core class, and converts it back to JSON format. The 
 * resulting JSON array is returned.
 *
 * @param coresJson A JSON array containing core data.
 * @return A JSON array with processed core data.
 */
json load_core_data(json coresJson){
    json result = json::array();
    for (auto& coreJson : coresJson) {
        OpenMagnetics::Core core(coreJson, false);
        json aux;
        to_json(aux, core);
        result.push_back(aux);
    }
    return result;
}

/**
 * @brief Retrieves material data for a given material name.
 *
 * This function searches for core material data by its name using the 
 * OpenMagnetics library and converts the result to a JSON object.
 *
 * @param materialName The name of the material to search for.
 * @return A JSON object containing the material data.
 */
json get_material_data(std::string materialName){

    auto materialData = OpenMagnetics::find_core_material_by_name(materialName);
    json result;
    to_json(result, materialData);
    return result;
}

/**
 * @brief Retrieves core temperature-dependent parameters.
 *
 * This function takes core data and a temperature value as input and returns a JSON object containing various 
 * temperature-dependent parameters of the core. These parameters include magnetic flux density saturation, 
 * magnetic field strength saturation, initial permeability, effective permeability, reluctance, permeance, 
 * and resistivity.
 *
 * @param coreData A JSON object containing the core data.
 * @param temperature A double representing the temperature at which the parameters are to be evaluated.
 * @return A JSON object containing the core temperature-dependent parameters.
 */
json get_core_temperature_dependant_parameters(json coreData, double temperature){
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

/**
 * @brief Retrieves shape data for a given shape name.
 *
 * This function attempts to find the core shape data associated with the provided shape name
 * using the OpenMagnetics library. If successful, it converts the shape data to a JSON object
 * and returns it. If an exception occurs during the process, it catches the exception and 
 * returns an error message.
 *
 * @param shapeName The name of the shape for which data is to be retrieved.
 * @return A JSON object containing the shape data if successful, or an error message if an exception occurs.
 */
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

/**
 * @brief Retrieves a list of available shape families.
 *
 * This function iterates through the enumeration of CoreShapeFamily and
 * collects their names into a vector of strings.
 *
 * @return std::vector<std::string> A vector containing the names of all available shape families.
 */
std::vector<std::string> get_available_shape_families(){
    std::vector<std::string> families;
    for (auto& family : magic_enum::enum_names<CoreShapeFamily>()) {
        std::string familyJson(family);
        families.push_back(familyJson);
    }
    return families;
}

/**
 * @brief Retrieves a list of available core manufacturers.
 *
 * This function queries the available materials and extracts the manufacturer information
 * from each material. It ensures that each manufacturer is included only once in the 
 * returned list.
 *
 * @return A vector of strings containing the names of the available core manufacturers.
 */
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

/**
 * @brief Retrieves a list of available core shape families.
 *
 * This function iterates through the enumeration of core shape families
 * defined in the CoreShapeFamily enum and collects their
 * names into a vector of strings.
 *
 * @return std::vector<std::string> A vector containing the names of all
 * available core shape families.
 */
std::vector<std::string> get_available_core_shape_families(){
    std::vector<std::string> families;
    for (auto& family : magic_enum::enum_names<CoreShapeFamily>()) {
        std::string familyJson(family);
        families.push_back(familyJson);
    }
    return families;
}

/**
 * @brief Retrieves a list of available core materials from a specified manufacturer.
 * 
 * This function queries the OpenMagnetics library to obtain the names of core materials
 * provided by the given manufacturer.
 * 
 * @param manufacturer The name of the manufacturer whose core materials are to be retrieved.
 * @return std::vector<std::string> A vector containing the names of the available core materials.
 */
std::vector<std::string> get_available_core_materials(std::string manufacturer){
    return OpenMagnetics::get_core_material_names(manufacturer);
}

/**
 * @brief Retrieves a list of available core shapes.
 *
 * This function calls the get_core_shape_names() function to obtain
 * a vector of strings representing the names of available core shapes.
 *
 * @return std::vector<std::string> A vector containing the names of available core shapes.
 */
std::vector<std::string> get_available_core_shapes(){
    return OpenMagnetics::get_core_shape_names();
}


/**
 * @brief Retrieves a list of available cores.
 *
 * This function calls the get_core_shape_names() function to obtain
 * a vector of strings representing the names of available cores.
 *
 * @return std::vector<std::string> A vector containing the names of available cores.
 */
json get_available_cores(){
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

/**
 * @brief Retrieves a list of available wire names.
 * 
 * This function calls the get_wire_names() function to obtain
 * a vector of strings, each representing the name of an available wire.
 * 
 * @return std::vector<std::string> A vector containing the names of available wires.
 */
std::vector<std::string> get_available_wires(){
    return OpenMagnetics::get_wire_names();
}

/**
 * @brief Retrieves a list of unique wire standard names from a given JSON object.
 *
 * This function takes a JSON object representing wire standards, extracts the wire information,
 * and returns a vector of unique wire standard names.
 *
 * @param wireStandardJson A JSON object containing wire standard information.
 * @return std::vector<std::string> A vector containing unique wire standard names.
 */
std::vector<std::string> get_unique_wire_diameters(json wireStandardJson){
    WireStandard wireStandard(wireStandardJson);

    auto wires = OpenMagnetics::get_wires(WireType::ROUND, wireStandard);

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

/**
 * @brief Retrieves a list of available wire types.
 *
 * This function iterates through all entries of the WireType enum and converts each entry to a JSON string,
 * which is then added to a vector of strings. The function skips the WireType::PLANAR type as it is not 
 * currently supported.
 *
 * @return std::vector<std::string> A vector containing JSON strings representing the available wire types.
 */
std::vector<std::string> get_available_wire_types(){
    std::vector<std::string> wireTypes;

    for (auto [value, name] : magic_enum::enum_entries<WireType>()) {
        json wireTypeJson;
        if (value == WireType::PLANAR) {
            // TODO Add support for planar
            continue;
        }
        to_json(wireTypeJson, value);
        wireTypes.push_back(wireTypeJson);
    }

    return wireTypes;
}

/**
 * @brief Retrieves a list of available wire standards.
 *
 * This function iterates through all entries of the WireStandard enumeration
 * and converts each entry to a JSON string representation. The resulting list
 * of JSON strings is returned.
 *
 * @return std::vector<std::string> A vector containing JSON string representations
 *                                  of all available wire standards.
 */
std::vector<std::string> get_available_wire_standards(){
    std::vector<std::string> wireStandards;

    for (auto [value, name] : magic_enum::enum_entries<WireStandard>()) {
        json wireStandardJson;
        to_json(wireStandardJson, value);
        wireStandards.push_back(wireStandardJson);
    }

    return wireStandards;
}

/**
 * @brief Calculate the gap reluctance based on core gap data and model name.
 *
 * This function takes core gap data in JSON format and a model name as a string,
 * converts the model name to uppercase, and uses it to create a reluctance model.
 * It then calculates the gap reluctance using the provided core gap data and
 * returns the result in JSON format.
 *
 * @param coreGapData A JSON object containing the core gap data.
 * @param modelNameString A string representing the model name in JSON format.
 * @return A JSON object containing the calculated gap reluctance.
 */
json calculate_gap_reluctance(json coreGapData, std::string modelNameString){
    OpenMagnetics::ReluctanceModels modelName;
    from_json(modelNameString, modelName);
    auto reluctanceModel = OpenMagnetics::ReluctanceModel::factory(modelName);
    CoreGap coreGap(coreGapData);

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


double calculate_number_turns_from_gapping_and_inductance(json coreData,
                                                          json inputsData,    
                                                          json modelsData){
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


json calculate_gapping_from_number_turns_and_inductance(json coreData,
                                                        json coilData,
                                                        json inputsData,
                                                        std::string gappingTypeJson,
                                                        int decimals,
                                                        json modelsData){

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
    std::vector<CoreGap> gapping = magnetizingInductanceObj.calculate_gapping_from_number_turns_and_inductance(core,
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


/**
 * @brief Retrieves information about core losses models for a given material.
 *
 * This function gathers various pieces of information related to core losses models,
 * including general information, errors, internal and external links, and available models
 * specific to the provided material.
 *
 * @param material A JSON object representing the material for which core losses model information is requested.
 * @return A JSON object containing the following keys:
 * - "information": General information about core losses models.
 * - "errors": Any errors related to core losses models.
 * - "internal_links": Internal links related to core losses models.
 * - "external_links": External links related to core losses models.
 * - "available_models": A list of available core losses models for the specified material.
 */
json get_core_losses_model_information(json material){
    json info;
    info["information"] = OpenMagnetics::CoreLossesModel::get_models_information();
    info["errors"] = OpenMagnetics::CoreLossesModel::get_models_errors();
    info["internal_links"] = OpenMagnetics::CoreLossesModel::get_models_internal_links();
    info["external_links"] = OpenMagnetics::CoreLossesModel::get_models_external_links();
    info["available_models"] = OpenMagnetics::CoreLossesModel::get_methods_string(material);
    return info;
}

/**
 * @brief Retrieves information about the core temperature models.
 *
 * This function gathers various details about the core temperature models, including general information,
 * errors, internal links, and external links. The information is returned in a JSON object.
 *
 * @return json A JSON object containing the following keys:
 * - "information": General information about the core temperature models.
 * - "errors": Any errors related to the core temperature models.
 * - "internal_links": Internal links related to the core temperature models.
 * - "external_links": External links related to the core temperature models.
 */
json get_core_temperature_model_information(){
    json info;
    info["information"] = OpenMagnetics::CoreTemperatureModel::get_models_information();
    info["errors"] = OpenMagnetics::CoreTemperatureModel::get_models_errors();
    info["internal_links"] = OpenMagnetics::CoreTemperatureModel::get_models_internal_links();
    info["external_links"] = OpenMagnetics::CoreTemperatureModel::get_models_external_links();
    return info;
}

json calculate_induced_voltage(json excitationJson, double magnetizingInductance){
    OperatingPointExcitation excitation(excitationJson);

    auto voltage = OpenMagnetics::Inputs::calculate_induced_voltage(excitation, magnetizingInductance);

    json result;
    to_json(result, voltage);
    return result;
}

json calculate_induced_current(json excitationJson, double magnetizingInductance){
    OperatingPointExcitation excitation(excitationJson);

    auto current = OpenMagnetics::Inputs::calculate_magnetizing_current(excitation, magnetizingInductance, true, 0.0);

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
    OperatingPointExcitation primaryExcitation(primaryExcitationJson);

    OperatingPointExcitation excitationOfThisWinding(primaryExcitation);
    auto currentSignalDescriptorProcessed = OpenMagnetics::Inputs::calculate_basic_processed_data(primaryExcitation.get_current().value().get_waveform().value());
    auto voltageSignalDescriptorProcessed = OpenMagnetics::Inputs::calculate_basic_processed_data(primaryExcitation.get_voltage().value().get_waveform().value());

    auto voltageSignalDescriptor = OpenMagnetics::Inputs::reflect_waveform(primaryExcitation.get_voltage().value(), 1.0 / turnRatio, voltageSignalDescriptorProcessed.get_label());
    auto currentSignalDescriptor = OpenMagnetics::Inputs::reflect_waveform(primaryExcitation.get_current().value(), turnRatio, currentSignalDescriptorProcessed.get_label());

    auto voltageSampledWaveform = OpenMagnetics::Inputs::calculate_sampled_waveform(voltageSignalDescriptor.get_waveform().value(), excitationOfThisWinding.get_frequency());
    voltageSignalDescriptor.set_harmonics(OpenMagnetics::Inputs::calculate_harmonics_data(voltageSampledWaveform, excitationOfThisWinding.get_frequency()));
    voltageSignalDescriptor.set_processed(OpenMagnetics::Inputs::calculate_processed_data(voltageSignalDescriptor, voltageSampledWaveform, true));

    auto currentSampledWaveform = OpenMagnetics::Inputs::calculate_sampled_waveform(currentSignalDescriptor.get_waveform().value(), excitationOfThisWinding.get_frequency());
    currentSignalDescriptor.set_harmonics(OpenMagnetics::Inputs::calculate_harmonics_data(currentSampledWaveform, excitationOfThisWinding.get_frequency()));
    currentSignalDescriptor.set_processed(OpenMagnetics::Inputs::calculate_processed_data(currentSignalDescriptor, currentSampledWaveform, true));

    excitationOfThisWinding.set_voltage(voltageSignalDescriptor);
    excitationOfThisWinding.set_current(currentSignalDescriptor);

    json result;
    to_json(result, excitationOfThisWinding);
    return result;
}

json calculate_reflected_primary(json secondaryExcitationJson, double turnRatio){
    OperatingPointExcitation secondaryExcitation(secondaryExcitationJson);

    OperatingPointExcitation excitationOfThisWinding(secondaryExcitation);
    auto voltageSignalDescriptor = OpenMagnetics::Inputs::reflect_waveform(secondaryExcitation.get_voltage().value(), turnRatio);
    auto currentSignalDescriptor = OpenMagnetics::Inputs::reflect_waveform(secondaryExcitation.get_current().value(), 1.0 / turnRatio);

    auto voltageSampledWaveform = OpenMagnetics::Inputs::calculate_sampled_waveform(voltageSignalDescriptor.get_waveform().value(), excitationOfThisWinding.get_frequency());
    voltageSignalDescriptor.set_harmonics(OpenMagnetics::Inputs::calculate_harmonics_data(voltageSampledWaveform, excitationOfThisWinding.get_frequency()));
    voltageSignalDescriptor.set_processed(OpenMagnetics::Inputs::calculate_processed_data(voltageSignalDescriptor, voltageSampledWaveform, true));

    auto currentSampledWaveform = OpenMagnetics::Inputs::calculate_sampled_waveform(currentSignalDescriptor.get_waveform().value(), excitationOfThisWinding.get_frequency());
    currentSignalDescriptor.set_harmonics(OpenMagnetics::Inputs::calculate_harmonics_data(currentSampledWaveform, excitationOfThisWinding.get_frequency()));
    currentSignalDescriptor.set_processed(OpenMagnetics::Inputs::calculate_processed_data(currentSignalDescriptor, currentSampledWaveform, true));

    excitationOfThisWinding.set_voltage(voltageSignalDescriptor);
    excitationOfThisWinding.set_current(currentSignalDescriptor);

    json result;
    to_json(result, excitationOfThisWinding);
    return result;
}

double calculate_instantaneous_power(json excitationJson){
    OperatingPointExcitation excitation(excitationJson);

    if (!excitation.get_current().value().get_processed().value().get_rms().value()) {
        auto current = excitation.get_current().value();
        auto processed = OpenMagnetics::Inputs::calculate_processed_data(current.get_harmonics().value(), current.get_waveform().value(), true);
        current.set_processed(processed);
        excitation.set_current(current);
    }
    if (!excitation.get_voltage().value().get_processed().value().get_rms().value()) {
        auto voltage = excitation.get_voltage().value();
        auto processed = OpenMagnetics::Inputs::calculate_processed_data(voltage.get_harmonics().value(), voltage.get_waveform().value(), true);
        voltage.set_processed(processed);
        excitation.set_voltage(voltage);
    }

    auto instantaneousPower = OpenMagnetics::Inputs::calculate_instantaneous_power(excitation);

    return instantaneousPower;
}

double calculate_rms_power(json excitationJson){
    OperatingPointExcitation excitation(excitationJson);

    auto voltageSignalDescriptor = excitation.get_voltage().value();
    auto currentSignalDescriptor = excitation.get_current().value();

    if (!voltageSignalDescriptor.get_processed() || !voltageSignalDescriptor.get_processed()->get_rms()) {
        auto voltageSampledWaveform = OpenMagnetics::Inputs::calculate_sampled_waveform(voltageSignalDescriptor.get_waveform().value(), excitation.get_frequency());
        voltageSignalDescriptor.set_harmonics(OpenMagnetics::Inputs::calculate_harmonics_data(voltageSampledWaveform, excitation.get_frequency()));
        voltageSignalDescriptor.set_processed(OpenMagnetics::Inputs::calculate_processed_data(voltageSignalDescriptor, voltageSampledWaveform, true));
    }

    if (!currentSignalDescriptor.get_processed() || !currentSignalDescriptor.get_processed()->get_rms()) {
        auto currentSampledWaveform = OpenMagnetics::Inputs::calculate_sampled_waveform(currentSignalDescriptor.get_waveform().value(), excitation.get_frequency());
        currentSignalDescriptor.set_harmonics(OpenMagnetics::Inputs::calculate_harmonics_data(currentSampledWaveform, excitation.get_frequency()));
        currentSignalDescriptor.set_processed(OpenMagnetics::Inputs::calculate_processed_data(currentSignalDescriptor, currentSampledWaveform, true));
    }

    double rmsPower = currentSignalDescriptor.get_processed().value().get_rms().value() * voltageSignalDescriptor.get_processed().value().get_rms().value();

    return rmsPower;
}

double resolve_dimension_with_tolerance(json dimensionWithToleranceJson) {
    DimensionWithTolerance dimensionWithTolerance(dimensionWithToleranceJson);
    return OpenMagnetics::resolve_dimensional_values(dimensionWithTolerance);
}

/**
 * @brief Calculate basic processed data from a given waveform JSON.
 *
 * This function takes a JSON object representing a waveform, processes it using
 * the OpenMagnetics library, and returns the processed data as a JSON object.
 *
 * @param waveformJson A JSON object containing the waveform data to be processed.
 * @return A JSON object containing the basic processed data.
 */
json calculate_basic_processed_data(json waveformJson) {
    Waveform waveform(waveformJson);
    auto processed = OpenMagnetics::Inputs::calculate_basic_processed_data(waveform);
    json result;
    to_json(result, processed);
    return result;
}

/**
 * @brief Creates a waveform based on the given processed JSON data and frequency.
 *
 * This function takes a JSON object containing processed data and a frequency value,
 * and generates a waveform using the OpenMagnetics library. The resulting waveform
 * is then converted back to a JSON object and returned.
 *
 * @param processedJson A JSON object containing the processed data.
 * @param frequency A double representing the frequency for the waveform.
 * @return A JSON object representing the created waveform.
 */
json create_waveform(json processedJson, double frequency) {
    Processed processed(processedJson);
    auto waveform = OpenMagnetics::Inputs::create_waveform(processed, frequency);
    json result;
    to_json(result, waveform);
    return result;
}

/**
 * @brief Scales the time domain of a waveform to a specified frequency domain.
 *
 * This function takes a JSON representation of a waveform and scales its time domain
 * to match a new specified frequency. The scaled waveform is then returned as a JSON object.
 *
 * @param waveformJson A JSON object representing the original waveform.
 * @param newFrequency The new frequency to which the waveform's time domain should be scaled.
 * @return A JSON object representing the scaled waveform.
 */
json scale_waveform_time_to_frequency(json waveformJson, double newFrequency) {
    Waveform waveform(waveformJson);
    auto scaledWaveform = OpenMagnetics::Inputs::scale_time_to_frequency(waveform, newFrequency);
    json result;
    to_json(result, scaledWaveform);
    return result;
}

/**
 * @brief Calculate various insulation parameters based on the provided JSON inputs.
 *
 * This function uses the InsulationCoordinator to calculate several insulation-related
 * parameters such as creepage distance, clearance, withstand voltage, and distance through insulation.
 * The results are returned in a JSON object.
 *
 * @param inputsJson A JSON object containing the necessary inputs for the calculations.
 * @return A JSON object containing the calculated insulation parameters and any error messages.
 *
 * The returned JSON object has the following structure:
 * {
 *     "creepageDistance": <calculated_value>,
 *     "clearance": <calculated_value>,
 *     "withstandVoltage": <calculated_value>,
 *     "distanceThroughInsulation": <calculated_value>,
 *     "errorMessage": <error_message_if_any>
 * }
 *
 * If an exception occurs during the calculations, the "errorMessage" field will contain the
 * corresponding error message.
 */
json calculate_insulation(json inputsJson){
    auto standard = OpenMagnetics::InsulationCoordinator();
    OpenMagnetics::Inputs inputs(inputsJson, false);

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

/**
 * @brief Extracts the operating point from the given JSON file.
 *
 * This function reads the operating point from a JSON file, processes it with the given parameters,
 * and returns the result as a JSON object.
 *
 * @param fileJson The JSON object containing the file data.
 * @param numberWindings The number of windings in the circuit.
 * @param frequency The frequency at which the circuit operates.
 * @param desiredMagnetizingInductance The desired magnetizing inductance value.
 * @param mapColumnNamesJson A JSON object containing the mapping of column names.
 * @return A JSON object containing the processed operating point, or an error message if processing fails.
 */
json extract_operating_point(json fileJson, size_t numberWindings, double frequency, double desiredMagnetizingInductance, json mapColumnNamesJson){
    try {
        std::vector<std::map<std::string, std::string>> mapColumnNames = mapColumnNamesJson.get<std::vector<std::map<std::string, std::string>>>();
        auto reader = OpenMagnetics::CircuitSimulationReader(fileJson);
        auto operatingPoint = reader.extract_operating_point(numberWindings, frequency, mapColumnNames);
        operatingPoint = OpenMagnetics::Inputs::process_operating_point(operatingPoint, desiredMagnetizingInductance);
        json result;
        to_json(result, operatingPoint);
        return result;
    }
    catch(...)
    {
        return "Error processing waveforms, please check column names and frequency";
    }
}

/**
 * @brief Extracts the column names from a given JSON file for a specified number of windings and frequency.
 *
 * This function utilizes the CircuitSimulationReader to read the column names from the provided JSON file.
 * It then formats the extracted column names into a JSON array, where each element is a JSON object mapping signals to their respective names.
 *
 * @param fileJson The JSON object containing the file data.
 * @param numberWindings The number of windings to consider in the extraction process.
 * @param frequency The frequency to consider in the extraction process.
 * @return A JSON array where each element is a JSON object mapping signals to their respective names.
 */
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

/**
 * @brief Extracts column names from a given JSON file.
 *
 * This function takes a JSON object representing a file, uses the 
 * CircuitSimulationReader to read the file, and extracts the column names.
 * The column names are then returned as a JSON array.
 *
 * @param fileJson A JSON object representing the file from which to extract column names.
 * @return A JSON array containing the extracted column names.
 */
json extract_column_names(json fileJson){
    auto reader = OpenMagnetics::CircuitSimulationReader(fileJson);
    auto columnNames = reader.extract_column_names();

    json result = json::array();
    for (auto& columnName : columnNames) {
        result.push_back(columnName);
    }
    return result;
}

/**
 * @brief Calculate the number of turns for a transformer design.
 *
 * This function calculates the number of turns for a transformer primary winding
 * based on the given design requirements in JSON format. It uses the OpenMagnetics
 * library to determine the next combination of turns.
 *
 * @param numberTurnsPrimary The number of turns in the primary winding.
 * @param designRequirementsJson A JSON object containing the design requirements.
 * @return A vector of integers representing the calculated number of turns.
 */
std::vector<int> calculate_number_turns(int numberTurnsPrimary, json designRequirementsJson){
    DesignRequirements designRequirements(designRequirementsJson);

    OpenMagnetics::NumberTurns numberTurns(numberTurnsPrimary, designRequirements);
    auto numberTurnsCombination = numberTurns.get_next_number_turns_combination();

    std::vector<int> numberTurnsResult;
    for (auto turns : numberTurnsCombination) {
        numberTurnsResult.push_back(static_cast<std::make_signed<int>::type>(turns));
    }
    return numberTurnsResult;
}

/**
 * @brief Calculate the DC resistance per meter of a wire at a given temperature.
 * 
 * This function takes a JSON object representing the wire properties and a temperature value,
 * and calculates the DC resistance per meter using the OpenMagnetics library.
 * 
 * @param wireJson A JSON object containing the wire properties.
 * @param temperature The temperature at which to calculate the DC resistance.
 * @return The DC resistance per meter of the wire at the given temperature.
 */
double calculate_dc_resistance_per_meter(json wireJson, double temperature){
    OpenMagnetics::Wire wire(wireJson);
    auto dcResistancePerMeter = OpenMagnetics::WindingOhmicLosses::calculate_dc_resistance_per_meter(wire, temperature);
    return dcResistancePerMeter;
}

double calculate_dc_losses_per_meter(json wireJson, json currentJson, double temperature){
    OpenMagnetics::Wire wire(wireJson);
    SignalDescriptor current(currentJson);
    auto dcLossesPerMeter = OpenMagnetics::WindingOhmicLosses::calculate_ohmic_losses_per_meter(wire, current, temperature);
    return dcLossesPerMeter;
}

double calculate_skin_ac_factor(json wireJson, json currentJson, double temperature){
    OpenMagnetics::Wire wire(wireJson);
    SignalDescriptor current(currentJson);
    auto dcLossesPerMeter = OpenMagnetics::WindingOhmicLosses::calculate_ohmic_losses_per_meter(wire, current, temperature);
    auto [skinLossesPerMeter, _] = OpenMagnetics::WindingSkinEffectLosses::calculate_skin_effect_losses_per_meter(wire, current, temperature);
    auto skinAcFactor = (skinLossesPerMeter + dcLossesPerMeter) / dcLossesPerMeter;
    return skinAcFactor;
}

double calculate_skin_ac_losses_per_meter(json wireJson, json currentJson, double temperature){
    OpenMagnetics::Wire wire(wireJson);
    SignalDescriptor current(currentJson);
    auto [skinLossesPerMeter, _] = OpenMagnetics::WindingSkinEffectLosses::calculate_skin_effect_losses_per_meter(wire, current, temperature);
    return skinLossesPerMeter;
}

double calculate_skin_ac_resistance_per_meter(json wireJson, json currentJson, double temperature){
    OpenMagnetics::Wire wire(wireJson);
    SignalDescriptor current(currentJson);
    auto dcLossesPerMeter = OpenMagnetics::WindingOhmicLosses::calculate_ohmic_losses_per_meter(wire, current, temperature);
    auto [skinLossesPerMeter, _] = OpenMagnetics::WindingSkinEffectLosses::calculate_skin_effect_losses_per_meter(wire, current, temperature);
    auto skinAcFactor = (skinLossesPerMeter + dcLossesPerMeter) / dcLossesPerMeter;
    auto dcResistancePerMeter = OpenMagnetics::WindingOhmicLosses::calculate_dc_resistance_per_meter(wire, temperature);

    return dcResistancePerMeter * skinAcFactor;
}

double calculate_effective_current_density(json wireJson, json currentJson, double temperature){
    OpenMagnetics::Wire wire(wireJson);
    SignalDescriptor current(currentJson);
    auto effectiveCurrentDensity = wire.calculate_effective_current_density(current, temperature);

    return effectiveCurrentDensity;
}

double calculate_effective_skin_depth(std::string materialName, json currentJson, double temperature){
    try {
        SignalDescriptor current(currentJson);

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

/**
 * @brief Retrieves a list of available winding orientations.
 *
 * This function iterates through all entries of the `WindingOrientation` enum,
 * converts each orientation to a JSON string, and adds it to a vector of strings.
 *
 * @return std::vector<std::string> A vector containing JSON string representations of all available winding orientations.
 */
std::vector<std::string> get_available_winding_orientations(){
    std::vector<std::string> orientations;
    for (auto& [orientation, _] : magic_enum::enum_entries<WindingOrientation>()) {
        json orientationJson;
        to_json(orientationJson, orientation);
        orientations.push_back(orientationJson);
    }
    return orientations;
}

/**
 * @brief Retrieves a list of available coil alignments.
 *
 * This function iterates through all entries of the CoilAlignment enumeration,
 * converts each alignment to a JSON string, and collects these strings into a vector.
 *
 * @return std::vector<std::string> A vector containing JSON string representations of the available coil alignments.
 */
std::vector<std::string> get_available_coil_alignments(){
    std::vector<std::string> orientations;
    for (auto& [orientation, _] : magic_enum::enum_entries<CoilAlignment>()) {
        json orientationJson;
        to_json(orientationJson, orientation);
        orientations.push_back(orientationJson);
    }
    return orientations;
}

/**
 * @brief Checks if a given value meets the specified requirement.
 *
 * This function takes a JSON object representing a requirement and a double value,
 * and checks if the value meets the requirement using the OpenMagnetics library.
 *
 * @param requirementJson A JSON object containing the requirement details.
 * @param value The value to be checked against the requirement.
 * @return true if the value meets the requirement, false otherwise.
 */
bool check_requirement(json requirementJson, double value){
    try {
        DimensionWithTolerance requirement(requirementJson);
        bool result = check_requirement(requirement, value);
        return result;
    }
    catch(const std::exception& ex)
    {
        return false;
    }
}

/**
 * @brief Function to wind a coil based on the provided JSON configuration.
 *
 * This function takes in various JSON objects describing the coil configuration, 
 * including the coil's functional description, bobbin, margins, layers orientation, 
 * and turns alignment. It then processes these configurations and winds the coil 
 * accordingly, returning the resulting coil configuration as a JSON object.
 *
 * @param coilJson JSON object containing the coil's functional description and bobbin.
 * @param repetitions Number of repetitions for the winding pattern.
 * @param proportionPerWindingJson JSON array containing the proportion per winding.
 * @param patternJson JSON array containing the winding pattern.
 * @param marginPairsJson JSON array containing margin pairs.
 * @return JSON object representing the resulting coil configuration.
 *
 * @throws std::runtime_error if the turns are not created successfully.
 */
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
        std::vector<OpenMagnetics::Winding> winding;
        for (auto elem : coilJson["functionalDescription"]) {
            winding.push_back(OpenMagnetics::Winding(elem));
        }
        OpenMagnetics::Coil coil;
        coil.set_bobbin(coilJson["bobbin"]);
        coil.set_functional_description(winding);
        coil.preload_margins(marginPairs);
        if (coilJson.contains("layersOrientation")) {

            if (coilJson["layersOrientation"].is_object()) {
                std::map<std::string, WindingOrientation> layersOrientationPerSection;
                for (auto [key, value] : coilJson["layersOrientation"].items()) {
                    layersOrientationPerSection[key] = value;
                }

                for (auto [sectionName, layerOrientation] : layersOrientationPerSection) {
                    coil.set_layers_orientation(layerOrientation, sectionName);
                }
            }
            else if (coilJson["layersOrientation"].is_array()) {
                coil.wind_by_sections(proportionPerWinding, pattern, repetitions);
                if (coil.get_sections_description()) {
                    auto sections = coil.get_sections_description_conduction();

                    std::vector<WindingOrientation> layersOrientationPerSection;
                    for (auto elem : coilJson["layersOrientation"]) {
                        layersOrientationPerSection.push_back(WindingOrientation(elem));
                    }

                    for (size_t sectionIndex = 0; sectionIndex < sections.size(); ++sectionIndex) {
                        if (sectionIndex < layersOrientationPerSection.size()) {
                            coil.set_layers_orientation(layersOrientationPerSection[sectionIndex], sections[sectionIndex].get_name());
                        }
                    }
                }
            }
            else {
                WindingOrientation layerOrientation(coilJson["layersOrientation"]);
                coil.set_layers_orientation(layerOrientation);

            }
        }

        if (coilJson.contains("turnsAlignment")) {
            if (coilJson["turnsAlignment"].is_object()) {
                std::map<std::string, CoilAlignment> turnsAlignmentPerSection;
                for (auto [key, value] : coilJson["turnsAlignment"].items()) {
                    turnsAlignmentPerSection[key] = value;
                }


                for (auto [sectionName, turnsAlignment] : turnsAlignmentPerSection) {
                    coil.set_turns_alignment(turnsAlignment, sectionName);
                }
            }
            else if (coilJson["turnsAlignment"].is_array()) {
                coil.wind_by_sections(proportionPerWinding, pattern, repetitions);
                if (coil.get_sections_description()) {
                    auto sections = coil.get_sections_description_conduction();

                    std::vector<CoilAlignment> turnsAlignmentPerSection;
                    for (auto elem : coilJson["turnsAlignment"]) {
                        turnsAlignmentPerSection.push_back(CoilAlignment(elem));
                    }

                    for (size_t sectionIndex = 0; sectionIndex < sections.size(); ++sectionIndex) {
                        if (sectionIndex < turnsAlignmentPerSection.size()) {
                            coil.set_turns_alignment(turnsAlignmentPerSection[sectionIndex], sections[sectionIndex].get_name());
                        }
                    }
                }
            }
            else {
                CoilAlignment turnsAlignment(coilJson["turnsAlignment"]);
                coil.set_turns_alignment(turnsAlignment);
            }
        }

        if (proportionPerWinding.size() == winding.size()) {
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

        if (!coil.get_turns_description()) {
            throw std::runtime_error("Turns not created");
        }

        json result;
        to_json(result, coil);
        return result;
    }
    catch (const std::exception &exc) {
        std::cout << "coilJson" << std::endl;
        std::cout << coilJson << std::endl;
        std::cout << "repetitions" << std::endl;
        std::cout << repetitions << std::endl;
        std::cout << "proportionPerWindingJson" << std::endl;
        std::cout << proportionPerWindingJson << std::endl;
        std::cout << "patternJson" << std::endl;
        std::cout << patternJson << std::endl;
        std::cout << "marginPairsJson" << std::endl;
        std::cout << marginPairsJson << std::endl;
        return "Exception: " + std::string{exc.what()};
    }
}

json wind_planar(json coilJson, json stackUpJson, double borderToWireDistance, json wireToWireDistanceJson, json insulationThicknessJson, double coreToLayerDistance) {
    try {
        OpenMagnetics::settings->set_coil_wind_even_if_not_fit(true);
        auto coil = OpenMagnetics::Coil(coilJson, false);
        std::vector<size_t> stackUp = stackUpJson;
        std::map<std::pair<size_t, size_t>, double> insulationThickness = insulationThicknessJson.get<std::map<std::pair<size_t, size_t>, double>>();
        std::map<size_t, double> wireToWireDistance = wireToWireDistanceJson.get<std::map<size_t, double>>();

        coil.set_strict(false);
        coil.wind_planar(stackUp, borderToWireDistance, wireToWireDistance, insulationThickness, coreToLayerDistance);

        if (!coil.get_turns_description()) {
            throw std::runtime_error("Turns not created");
        }

        return coil;
    }
    catch (const std::exception &exc) {
        return "Exception: " + std::string{exc.what()};
    }
}

/**
 * @brief Generates a winding configuration for a coil based on the provided parameters.
 * 
 * This function configures a coil's winding based on the given JSON inputs for coil properties,
 * winding proportions, and winding patterns. It supports different winding configurations
 * depending on the presence of specific parameters.
 * 
 * @param coilJson A JSON object containing the coil's properties and functional description.
 * @param repetitions The number of times the winding pattern should be repeated.
 * @param proportionPerWindingJson A JSON array representing the proportion of each winding.
 * @param patternJson A JSON array representing the winding pattern.
 * 
 * @return A JSON object representing the configured coil, or an error message in case of an exception.
 */
json wind_by_sections(json coilJson, size_t repetitions, json proportionPerWindingJson, json patternJson, double insulationThickness) {
    try {

        std::vector<double> proportionPerWinding = proportionPerWindingJson;
        std::vector<size_t> pattern = patternJson;
        std::vector<OpenMagnetics::Winding> winding;
        for (auto elem : coilJson["functionalDescription"]) {
            winding.push_back(OpenMagnetics::Winding(elem));
        }
        OpenMagnetics::Coil coil;

        if (coilJson.contains("interleavingLevel")) {
            coil.set_interleaving_level(coilJson["interleavingLevel"]);
        }
        if (coilJson.contains("windingOrientation")) {
            coil.set_winding_orientation(coilJson["windingOrientation"]);
        }
        if (coilJson.contains("layersOrientation")) {
            coil.set_layers_orientation(coilJson["layersOrientation"]);
        }
        if (coilJson.contains("turnsAlignment")) {
            coil.set_turns_alignment(coilJson["turnsAlignment"]);
        }
        if (coilJson.contains("sectionAlignment")) {
            coil.set_section_alignment(coilJson["sectionAlignment"]);
        }

        coil.set_bobbin(coilJson["bobbin"]);
        coil.set_functional_description(winding);

        if (insulationThickness > 0) {
            coil.calculate_custom_thickness_insulation(insulationThickness);
        }

        if (proportionPerWinding.size() == winding.size()) {
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

/**
 * @brief Processes coil and insulation layer data to generate a detailed coil description.
 *
 * This function takes JSON objects representing coil data and insulation layer data, processes them,
 * and returns a JSON object with the detailed coil description, including insulation layers, functional
 * description, and sections description.
 *
 * @param coilJson A JSON object containing the coil data, including functional description, sections description,
 *                 and various coil properties such as interleaving level, winding orientation, layers orientation,
 *                 turns alignment, section alignment, and bobbin.
 * @param insulationLayersJson A JSON object containing the insulation layers data, where each key is a string
 *                             representing a pair of winding indices, and each value is a JSON array of layers.
 * @return A JSON object containing the detailed coil description, or an error message if an exception occurs.
 */
json wind_by_layers(json coilJson, json insulationLayersJson, double insulationThickness) {
    try {
        std::map<std::pair<size_t, size_t>, std::vector<Layer>> insulationLayers;

        for (auto [key, layersJson] : insulationLayersJson.items()) {
            auto keys = OpenMagnetics::split(key, ",");
            std::pair<size_t, size_t> windingsMapKey(stoi(keys[0]), stoi(keys[1]));
            std::vector<Layer> layers;
            for (auto layerJson : layersJson) {
                layers.push_back(Layer(layerJson));
            }
            insulationLayers[windingsMapKey] = layers;
        }

        std::vector<OpenMagnetics::Winding> winding;
        for (auto elem : coilJson["functionalDescription"]) {
            winding.push_back(OpenMagnetics::Winding(elem));
        }

        std::vector<Section> coilSectionsDescription;
        for (auto elem : coilJson["sectionsDescription"]) {
            coilSectionsDescription.push_back(Section(elem));
        }
        OpenMagnetics::Coil coil;

        if (insulationThickness > 0) {
            coil.calculate_custom_thickness_insulation(insulationThickness);
        }

        if (insulationLayers.size() > 0) {
            coil.set_insulation_layers(insulationLayers);
        }

        if (coilJson.contains("interleavingLevel")) {
            coil.set_interleaving_level(coilJson["interleavingLevel"]);
        }
        if (coilJson.contains("windingOrientation")) {
            coil.set_winding_orientation(coilJson["windingOrientation"]);
        }
        if (coilJson.contains("layersOrientation")) {
            coil.set_layers_orientation(coilJson["layersOrientation"]);
        }
        if (coilJson.contains("turnsAlignment")) {
            coil.set_turns_alignment(coilJson["turnsAlignment"]);
        }
        if (coilJson.contains("sectionAlignment")) {
            coil.set_section_alignment(coilJson["sectionAlignment"]);
        }

        coil.set_bobbin(coilJson["bobbin"]);
        coil.set_functional_description(winding);
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

/**
 * @brief Winds a coil based on the provided JSON configuration.
 *
 * This function takes a JSON object containing the configuration for a coil,
 * including its functional description, sections description, and layers description.
 * It initializes a Coil object with these descriptions and additional parameters
 * if they are present in the JSON. The coil is then wound by turns, and the resulting
 * coil configuration is returned as a JSON object.
 *
 * @param coilJson A JSON object containing the coil configuration.
 * @return A JSON object representing the wound coil configuration.
 *         If an exception occurs, a JSON object with the exception message is returned.
 */
json wind_by_turns(json coilJson) {
    try {

        std::vector<OpenMagnetics::Winding> winding;
        for (auto elem : coilJson["functionalDescription"]) {
            winding.push_back(OpenMagnetics::Winding(elem));
        }
        std::vector<Section> coilSectionsDescription;
        for (auto elem : coilJson["sectionsDescription"]) {
            coilSectionsDescription.push_back(Section(elem));
        }
        std::vector<Layer> coilLayersDescription;
        for (auto elem : coilJson["layersDescription"]) {
            coilLayersDescription.push_back(Layer(elem));
        }

        OpenMagnetics::Coil coil;

        if (coilJson.contains("interleavingLevel")) {
            coil.set_interleaving_level(coilJson["interleavingLevel"]);
        }
        if (coilJson.contains("windingOrientation")) {
            coil.set_winding_orientation(coilJson["windingOrientation"]);
        }
        if (coilJson.contains("layersOrientation")) {
            coil.set_layers_orientation(coilJson["layersOrientation"]);
        }
        if (coilJson.contains("turnsAlignment")) {
            coil.set_turns_alignment(coilJson["turnsAlignment"]);
        }
        if (coilJson.contains("sectionAlignment")) {
            coil.set_section_alignment(coilJson["sectionAlignment"]);
        }

        coil.set_bobbin(coilJson["bobbin"]);
        coil.set_functional_description(winding);
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

/**
 * @brief Processes a JSON object representing a coil, compacts its data, and returns the result as a JSON object.
 *
 * This function takes a JSON object containing various descriptions of a coil, processes them into their respective
 * data structures, and then compacts the coil data using the `delimit_and_compact` method of the `Coil` class.
 * The resulting compacted coil data is then converted back to a JSON object and returned.
 *
 * @param coilJson The input JSON object containing the coil descriptions. It should have the following structure:
 * - "functionalDescription": Array of functional descriptions for the coil.
 * - "sectionsDescription": Array of section descriptions for the coil.
 * - "layersDescription": Array of layer descriptions for the coil.
 * - "turnsDescription": Array of turn descriptions for the coil.
 * - "interleavingLevel" (optional): Interleaving level of the coil.
 * - "windingOrientation" (optional): Winding orientation of the coil.
 * - "layersOrientation" (optional): Layers orientation of the coil.
 * - "turnsAlignment" (optional): Turns alignment of the coil.
 * - "sectionAlignment" (optional): Section alignment of the coil.
 * - "bobbin": Bobbin description of the coil.
 *
 * @return A JSON object containing the compacted coil data. If an exception occurs during processing, a JSON object
 * containing the exception message is returned.
 */
json delimit_and_compact(json coilJson) {
    try {

        std::vector<OpenMagnetics::Winding> winding;
        for (auto elem : coilJson["functionalDescription"]) {
            winding.push_back(OpenMagnetics::Winding(elem));
        }
        std::vector<Section> coilSectionsDescription;
        for (auto elem : coilJson["sectionsDescription"]) {
            coilSectionsDescription.push_back(Section(elem));
        }
        std::vector<Layer> coilLayersDescription;
        for (auto elem : coilJson["layersDescription"]) {
            coilLayersDescription.push_back(Layer(elem));
        }
        std::vector<Turn> coilTurnsDescription;
        for (auto elem : coilJson["turnsDescription"]) {
            coilTurnsDescription.push_back(Turn(elem));
        }

        OpenMagnetics::Coil coil;

        if (coilJson.contains("interleavingLevel")) {
            coil.set_interleaving_level(coilJson["interleavingLevel"]);
        }
        if (coilJson.contains("windingOrientation")) {
            coil.set_winding_orientation(coilJson["windingOrientation"]);
        }
        if (coilJson.contains("layersOrientation")) {
            coil.set_layers_orientation(coilJson["layersOrientation"]);
        }
        if (coilJson.contains("turnsAlignment")) {
            coil.set_turns_alignment(coilJson["turnsAlignment"]);
        }
        if (coilJson.contains("sectionAlignment")) {
            coil.set_section_alignment(coilJson["sectionAlignment"]);
        }

        coil.set_bobbin(coilJson["bobbin"]);
        coil.set_functional_description(winding);
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

/**
 * @brief Retrieves the layers of a coil by the specified winding index.
 * 
 * This function takes a JSON representation of a coil and an integer winding index,
 * and returns a JSON array containing the layers associated with the given winding index.
 * 
 * @param coilJson A JSON object representing the coil.
 * @param windingIndex An integer specifying the winding index for which layers are to be retrieved.
 * @return A JSON array containing the layers for the specified winding index. If an exception occurs,
 *         a JSON string with the exception message is returned.
 */
json get_layers_by_winding_index(json coilJson, int windingIndex){
    try {
        OpenMagnetics::Coil coil(coilJson, false);

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

/**
* @brief Retrieves the layers of a coil by its section name.
*
* This function takes a JSON object representing a coil and a section name, extracts the layers
* of the coil that belong to the specified section, and returns them as a JSON array.
*
* @param coilJson A JSON object representing the coil.
* @param sectionName The name of the section to which the layers belong.
*
* @return A JSON array containing the layers of the coil that belong to the specified section.
 */
json get_layers_by_section(json coilJson, json sectionName){
    try {
        json result = json::array();
        OpenMagnetics::Coil coil(coilJson, false);
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

/**
 * @brief Retrieves the description of conduction sections from a coil JSON object.
 *
 * This function takes a JSON object representing a coil, extracts the conduction
 * sections descriptions, and returns them as a JSON array.
 *
 * @param coilJson A JSON object representing the coil.
 * @return A JSON array containing the descriptions of the conduction sections.
 *         If an exception occurs, a JSON object with the exception message is returned.
 */
json get_sections_description_conduction(json coilJson){
    try {
        json result = json::array();
        OpenMagnetics::Coil coil(coilJson, false);
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

/**
 * @brief Checks if the sections and layers of a coil fit according to the provided JSON configuration.
 *
 * This function takes a JSON object representing the coil configuration and checks if the sections
 * and layers of the coil fit properly. It uses the Coil class to perform the
 * check.
 *
 * @param coilJson A JSON object containing the coil configuration.
 * @return true if the sections and layers fit properly, false otherwise.
 *
 * @throws std::exception if there is an error during the fitting check.
 */
bool are_sections_and_layers_fitting(json coilJson) {
    try {
        json result = json::array();
        OpenMagnetics::Coil coil(coilJson, false);
        return coil.are_sections_and_layers_fitting();
    }
    catch (const std::exception &exc) {
        std::cout << "Exception: " + std::string{exc.what()} << std::endl;
        return false;
    }
}

/**
 * @brief Adds margin to a specific section of a coil by its index.
 * 
 * This function takes a JSON representation of a coil, adds margin to a specified section
 * by its index, and returns the updated JSON representation of the coil.
 * 
 * @param coilJson The JSON object representing the coil.
 * @param sectionIndex The index of the section to which the margin will be added.
 * @param top_or_left_margin The margin to be added to the top or left side of the section.
 * @param bottom_or_right_margin The margin to be added to the bottom or right side of the section.
 * @return json The updated JSON object representing the coil with the added margin.
 * 
 * @throws std::exception If an error occurs during the margin addition process.
 */
json add_margin_to_section_by_index(json coilJson, int sectionIndex, double top_or_left_margin, double bottom_or_right_margin) {
    try {
        OpenMagnetics::Coil coil(coilJson, false);
        coil.add_margin_to_section_by_index(sectionIndex, {top_or_left_margin, bottom_or_right_margin});

        json result;
        to_json(result, coil);
        return result;
    }
    catch (const std::exception &exc) {
        return "Exception: " + std::string{exc.what()};
    }
}

/**
 * @brief Simulates the magnetic behavior based on the provided inputs, magnetic properties, and model data.
 * 
 * @param inputsJson A JSON object containing the input parameters for the simulation.
 * @param magneticJson A JSON object containing the magnetic properties for the simulation.
 * @param modelsData A JSON object containing the model names to be used for the simulation.
 * @return json A JSON object containing the simulation results.
 * 
 * This function initializes the magnetic and input s using the provided JSON data. It then sets the default
 * models for reluctance, core losses, and core temperature. If specific models are provided in the modelsData JSON,
 * those models are used instead of the OpenMagnetics::defaults. The function then creates a MagneticSimulator object, sets the
 * model names, and performs the simulation. The results are converted to JSON and returned. If an exception occurs,
 * an error message is returned.
 */
json simulate(json inputsJson,
                     json magneticJson,
                     json modelsData){
    try {
        OpenMagnetics::Magnetic magnetic(magneticJson);
        OpenMagnetics::Inputs inputs(inputsJson);

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

/**
 * @brief Checks if a given dimension fits within a bobbin.
 * 
 * This function takes a JSON representation of a bobbin, a dimension, and a flag indicating 
 * whether the dimension is horizontal or radial. It then checks if the given dimension fits 
 * within the bobbin.
 * 
 * @param bobbinJson A JSON object representing the bobbin.
 * @param dimension The dimension to check.
 * @param isHorizontalOrRadial A boolean flag indicating whether the dimension is horizontal or radial.
 * @return true if the dimension fits within the bobbin, false otherwise.
 * 
 * @throws std::exception If there is an error during the check.
 */
bool check_if_fits(json bobbinJson, double dimension, bool isHorizontalOrRadial) {
    try {
        OpenMagnetics::Bobbin bobbin(bobbinJson);
        return bobbin.check_if_fits(dimension, isHorizontalOrRadial);
    }
    catch (const std::exception &exc) {
        std::cout << "Exception: " + std::string{exc.what()} << std::endl;
        return false;
    }
}

/**
 * @brief Retrieves the coating thickness of a wire from a JSON object.
 *
 * This function creates a Wire object using the provided JSON data and
 * returns the coating thickness of the wire. If an exception occurs during the
 * process, it catches the exception, prints the error message, and returns -1.
 *
 * @param wireJson A JSON object containing the wire data.
 * @return The coating thickness of the wire. Returns -1 if an exception occurs.
 */
double get_coating_thickness(json wireJson){
    try {
        OpenMagnetics::Wire wire(wireJson);
        return wire.get_coating_thickness();
    }
    catch (const std::exception &exc) {
        std::cout << std::string{exc.what()}  << std::endl;
        return -1;
    }
}

/**
 * @brief Retrieves the relative permittivity of the coating from the given wire JSON object.
 * 
 * This function attempts to create a Wire object from the provided JSON representation
 * of a wire and then retrieves the relative permittivity of its coating. If an exception occurs
 * during this process, the exception message is printed to the standard output and the function
 * returns -1.
 * 
 * @param wireJson A JSON object representing the wire.
 * @return double The relative permittivity of the wire's coating, or -1 if an error occurs.
 */
double get_coating_relative_permittivity(json wireJson){
    try {
        OpenMagnetics::Wire wire(wireJson);
        return wire.get_coating_relative_permittivity();
    }
    catch (const std::exception &exc) {
        std::cout << std::string{exc.what()}  << std::endl;
        return -1;
    }
}

/**
 * @brief Retrieves the coating insulation material for a given wire.
 *
 * This function takes a JSON object representing a wire and attempts to resolve its coating insulation material.
 * If the material information is missing, it defaults to a predefined enamelled insulation material.
 *
 * @param wireJson A JSON object containing the wire information.
 * @return A JSON object representing the resolved insulation material. If an exception occurs, returns a JSON object with the exception message.
 */
json get_coating_insulation_material(json wireJson){
    try {
        OpenMagnetics::Wire wire(wireJson);
        OpenMagnetics::InsulationMaterial material;

        try {
            material = wire.resolve_coating_insulation_material();
        }
        catch (const std::exception &e) {
            if (std::string{e.what()} == "Coating is missing material information") {
                material = find_insulation_material_by_name(OpenMagnetics::defaults.defaultEnamelledInsulationMaterial);
            }
        }

        json result;
        to_json(result, material);
        return result;
    }
    catch (const std::exception &exc) {
        return "Exception: " + std::string{exc.what()};
    }
}

/**
 * @brief Retrieves the insulation material of a specified insulation layer from a coil JSON object.
 *
 * This function takes a JSON object representing a coil and a string specifying the name of an insulation layer.
 * It returns the insulation material of the specified insulation layer in JSON format.
 *
 * @param coilJson A JSON object representing the coil.
 * @param layerName A string specifying the name of the insulation layer.
 * @return A JSON object representing the insulation material of the specified insulation layer.
 *         If an exception occurs, a JSON object containing the exception message is returned.
 */
json get_insulation_layer_insulation_material(json coilJson, std::string layerName){
    try {
        OpenMagnetics::Coil coil(coilJson, false);
        auto material = OpenMagnetics::Coil::resolve_insulation_layer_insulation_material(coil, layerName);

        json result;
        to_json(result, material);
        return result;
    }
    catch (const std::exception &exc) {
        return "Exception: " + std::string{exc.what()};
    }
}

/**
 * @brief Exports a magnetic component as a subcircuit.
 *
 * This function takes a JSON representation of a magnetic component, wraps it using the 
 * Magnetic, and then exports it as a subcircuit using the 
 * CircuitSimulatorExporter. The resulting subcircuit is returned as a 
 * formatted JSON string.
 *
 * @param magneticJson A JSON object representing the magnetic component.
 * @return A formatted JSON string representing the subcircuit, or an error message if an exception occurs.
 */
ordered_json export_magnetic_as_subcircuit(json magneticJson) {
    try {
        OpenMagnetics::Magnetic magnetic(magneticJson);

        ordered_json subcircuit = OpenMagnetics::CircuitSimulatorExporter().export_magnetic_as_subcircuit(magnetic);

        return subcircuit.dump(4);
    }
    catch (const std::exception &exc) {
        return "Exception: " + std::string{exc.what()};
    }
}


json process_inputs(json inputsJson){
    OpenMagnetics::Inputs inputs(inputsJson, true);
    json result;
    to_json(result, inputs);
    return result;
}

size_t load_core_materials(std::string fileToLoad){
    if (fileToLoad != "") {
        OpenMagnetics::load_core_materials(fileToLoad);
    }
    else {
        OpenMagnetics::load_core_materials();
    }

    return OpenMagnetics::coreMaterialDatabase.size();
}

size_t load_core_shapes(std::string fileToLoad){
    if (fileToLoad != "") {
        OpenMagnetics::load_core_shapes(true, fileToLoad);
    }
    else {
        OpenMagnetics::load_core_shapes();
    }
    return OpenMagnetics::coreShapeDatabase.size();
}

size_t load_wires(std::string fileToLoad){
    if (fileToLoad != "") {
        OpenMagnetics::load_wires(fileToLoad);
    }
    else {
        OpenMagnetics::load_wires();
    }
    return OpenMagnetics::wireDatabase.size();
}

void clear_databases(){
    OpenMagnetics::clear_databases();
}

bool is_core_material_database_empty(){
    return OpenMagnetics::coreMaterialDatabase.size() == 0;
}

bool is_core_shape_database_empty(){
    return OpenMagnetics::coreShapeDatabase.size() == 0;
}

bool is_wire_database_empty(){
    return OpenMagnetics::wireDatabase.size() == 0;
}

json get_isolation_side_from_index(size_t index){
    return OpenMagnetics::get_isolation_side_from_index(index);
}

double calculate_core_maximum_magnetic_energy(json coreDataJson, json operatingPointJson){
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
json calculate_signal_from_harmonics(json harmonicsJson, double frequency) {
    Harmonics harmonics;
    from_json(harmonicsJson, harmonics);

    auto waveform = OpenMagnetics::Inputs::reconstruct_signal(harmonics, frequency);
    auto processed = OpenMagnetics::Inputs::calculate_processed_data(harmonics, waveform, true);


    SignalDescriptor signal;
    signal.set_processed(processed);
    signal.set_waveform(waveform);
    signal.set_harmonics(harmonics);

    json result;
    to_json(result, signal);
    return result;
}

json mas_autocomplete(json masJson, json configuration) {
    try {
        OpenMagnetics::Mas mas(masJson);
        auto autocompletedMas = mas_autocomplete(mas, true, configuration);

        json result;
        to_json(result, autocompletedMas);
        return result;
    }
    catch (const std::exception &exc) {
        std::cout << "Exception: " + std::string{exc.what()} << std::endl;
        return -1;
    }
}

json magnetic_autocomplete(json magneticJson, json configuration) {
    try {
        OpenMagnetics::Magnetic magnetic(magneticJson);
        auto autocompletedMagnetic = magnetic_autocomplete(magnetic, configuration);

        json result;
        to_json(result, autocompletedMagnetic);
        return result;
    }
    catch (const std::exception &exc) {
        std::cout << "Exception: " + std::string{exc.what()} << std::endl;
        return -1;
    }
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

std::string clear_magnetic_cache() {
    try {
        OpenMagnetics::magneticsCache.clear();
        return std::to_string(OpenMagnetics::magneticsCache.size());
    }
    catch (const std::exception &exc) {
        return std::string{exc.what()};
    }
}


std::string load_magnetics_from_file(std::string path, bool expand) {
    try {
        std::ifstream in(path);
        if (in) {
            std::string line;
            while (getline(in, line)) {
                json jf = json::parse(line);
                OpenMagnetics::Magnetic magnetic(jf);
                if (expand) {
                    magnetic = OpenMagnetics::magnetic_autocomplete(magnetic);
                }
                std::string key = magnetic.get_manufacturer_info()->get_reference().value();
                OpenMagnetics::magneticsCache.load(key, magnetic);
            }
        }
        return std::to_string(OpenMagnetics::magneticsCache.size());
    }
    catch (const std::exception &exc) {
        return std::string{exc.what()};
    }
}

json calculate_advised_magnetics_from_catalog(json inputsJson, json catalogJson, int maximumNumberResults){
    try {
        OpenMagnetics::settings->set_coil_delimit_and_compact(true);
        OpenMagnetics::Inputs inputs(inputsJson);
        std::map<OpenMagnetics::MagneticFilters, double> weights;

        std::vector <OpenMagnetics::Magnetic> catalog;

        for (auto magneticJson : catalogJson) {
            OpenMagnetics::Magnetic magnetic(magneticJson);
            catalog.push_back(magnetic);
        }

        OpenMagnetics::MagneticAdviser magneticAdviser;
        auto masMagnetics = magneticAdviser.get_advised_magnetic(inputs, catalog, maximumNumberResults);

        auto scorings = magneticAdviser.get_scorings();

        json results = json();
        results["data"] = json::array();
        for (auto& [masMagnetic, scoring] : masMagnetics) {
            std::string name = masMagnetic.get_magnetic().get_manufacturer_info().value().get_reference().value();
            json result;
            json masJson;
            to_json(masJson, masMagnetic);
            result["mas"] = masJson;
            result["scoring"] = scoring;
            results["data"].push_back(result);
        }

        sort(results["data"].begin(), results["data"].end(), [](json& b1, json& b2) {
            return b1["scoring"] > b2["scoring"];
        });

        return results;
    }
    catch (const std::exception &exc) {
        std::cout << inputsJson << std::endl;
        std::cout << catalogJson << std::endl;
        std::cout << maximumNumberResults << std::endl;
        return "Exception: " + std::string{exc.what()};
    }
}

json calculate_advised_magnetics_from_cache(json inputsJson, json filterFlowJson, int maximumNumberResults){
    try {
        OpenMagnetics::settings->set_coil_delimit_and_compact(true);
        std::cout << "Mierda 1" << std::endl;
        OpenMagnetics::Inputs inputs(inputsJson);

        std::vector<OpenMagnetics::MagneticFilterOperation> filterFlow;
        std::cout << "Mierda 2" << std::endl;
        for (auto filterJson : filterFlowJson) {
            OpenMagnetics::MagneticFilterOperation filter(filterJson);
            filterFlow.push_back(filter);
        }

        std::cout << "Mierda 3" << std::endl;
        if (OpenMagnetics::magneticsCache.size() == 0) {
            return "Exception: No magnetics found in cache";
        }

        OpenMagnetics::MagneticAdviser magneticAdviser;
        std::cout << "Mierda 4" << std::endl;
        auto masMagnetics = magneticAdviser.get_advised_magnetic(inputs, OpenMagnetics::magneticsCache.get(), filterFlow, maximumNumberResults);
        std::cout << "Mierda 5" << std::endl;

        auto scorings = magneticAdviser.get_scorings();
        std::cout << "Mierda 6" << std::endl;

        json results = json();
        results["data"] = json::array();
        std::cout << "Mierda 7" << std::endl;
        for (auto& [masMagnetic, scoring] : masMagnetics) {
            std::string name = masMagnetic.get_magnetic().get_manufacturer_info().value().get_reference().value();
            json result;
            json masJson;
            to_json(masJson, masMagnetic);
            result["mas"] = masJson;
            result["scoring"] = scoring;
            results["data"].push_back(result);
        }

        std::cout << "Mierda 8" << std::endl;
        sort(results["data"].begin(), results["data"].end(), [](json& b1, json& b2) {
            return b1["scoring"] > b2["scoring"];
        });
        std::cout << "Mierda 9" << std::endl;

        return results;
    }
    catch (const std::exception &exc) {
        // std::cout << inputsJson << std::endl;
        // std::cout << filterFlowJson << std::endl;
        // std::cout << maximumNumberResults << std::endl;
        return "Exception: " + std::string{exc.what()};
    }
}



PYBIND11_MODULE(PyMKF, m) {
    m.doc() = "Python wrapper for MKF (Magnetics Knowledge Foundation) - the simulation engine of OpenMagnetics";

    // Constants and defaults
    m.def("get_constants", &get_constants, "Retrieve physical and simulation constants used in OpenMagnetics");
    m.def("get_defaults", &get_defaults, "Retrieve default model configurations and parameters");

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

    // Wires, bobbins, and materials
    m.def("get_wires", &get_wires, "Retrieve all available wires as JSON objects");
    m.def("get_bobbins", &get_bobbins, "Retrieve all available bobbins as JSON objects");
    m.def("get_insulation_materials", &get_insulation_materials, "Retrieve all available insulation materials");
    m.def("get_wire_materials", &get_wire_materials, "Retrieve all available wire materials");

    // Name retrieval functions
    m.def("get_core_material_names", &get_core_material_names, "Retrieve list of all core material names");
    m.def("get_core_material_names_by_manufacturer", &get_core_material_names_by_manufacturer,
        "Retrieve core material names filtered by manufacturer",
        py::arg("manufacturer_name"));
    m.def("get_core_shape_names", &get_core_shape_names,
        "Retrieve list of core shape names",
        py::arg("include_toroidal"));
    m.def("get_wire_names", &get_wire_names, "Retrieve list of all wire names");
    m.def("get_bobbin_names", &get_bobbin_names, "Retrieve list of all bobbin names");
    m.def("get_insulation_material_names", &get_insulation_material_names, "Retrieve list of all insulation material names");
    m.def("get_wire_material_names", &get_wire_material_names, "Retrieve list of all wire material names");

    // Lookup functions
    m.def("find_core_material_by_name", &find_core_material_by_name, "Find core material data by name");
    m.def("find_core_shape_by_name", &find_core_shape_by_name, "Find core shape data by name");
    m.def("find_wire_by_name", &find_wire_by_name, "Find wire data by name");
    m.def("find_bobbin_by_name", &find_bobbin_by_name, "Find bobbin data by name");
    m.def("find_insulation_material_by_name", &find_insulation_material_by_name, "Find insulation material data by name");
    m.def("find_wire_material_by_name", &find_wire_material_by_name, "Find wire material data by name");
    m.def("find_wire_by_dimension", &find_wire_by_dimension, "Find wire by dimension, type, and standard");

    // Bobbin creation
    m.def("create_basic_bobbin", &create_basic_bobbin, "Create a basic bobbin from core data");
    m.def("create_basic_bobbin_by_thickness", &create_basic_bobbin_by_thickness, "Create a basic bobbin with specified thickness");

    // Core calculations
    m.def("calculate_core_data", &calculate_core_data, "Process core data and return complete description");
    m.def("calculate_core_processed_description", &calculate_core_processed_description, "Calculate processed description for a core");
    m.def("calculate_core_geometrical_description", &calculate_core_geometrical_description, "Calculate geometrical description for a core");
    m.def("calculate_core_gapping", &calculate_core_gapping, "Calculate gapping configuration for a core");

    // Winding
    m.def("wind", &wind, "Wind coils on a magnetic core according to specifications");
    m.def("get_default_models", &get_default_models, "Retrieve default physical models configuration");

    // Inductance and losses
    m.def("calculate_inductance_from_number_turns_and_gapping", &calculate_inductance_from_number_turns_and_gapping,
        "Calculate inductance from turns count and gap configuration");
    m.def("calculate_core_losses", &calculate_core_losses, "Calculate core losses for given operating conditions");
    m.def("calculate_advised_cores", &calculate_advised_cores, "Get recommended cores for design requirements");
    m.def("calculate_advised_magnetics", &calculate_advised_magnetics, "Get recommended magnetic designs for requirements");
    m.def("calculate_winding_losses", &calculate_winding_losses, "Calculate total winding losses");
    m.def("calculate_ohmic_losses", &calculate_ohmic_losses, "Calculate DC ohmic losses in windings");
    m.def("calculate_magnetic_field_strength_field", &calculate_magnetic_field_strength_field, "Calculate magnetic field strength distribution");
    m.def("calculate_proximity_effect_losses", &calculate_proximity_effect_losses, "Calculate proximity effect losses in windings");
    m.def("calculate_skin_effect_losses", &calculate_skin_effect_losses, "Calculate skin effect losses in windings");
    m.def("calculate_skin_effect_losses_per_meter", &calculate_skin_effect_losses_per_meter, "Calculate skin effect losses per meter of wire");

    // Plotting functions
    m.def("plot_core", &plot_core, "Generate SVG plot of core geometry");
    m.def("plot_sections", &plot_sections, "Generate SVG plot of winding sections");
    m.def("plot_layers", &plot_layers, "Generate SVG plot of winding layers");
    m.def("plot_turns", &plot_turns, "Generate SVG plot of individual turns");
    m.def("plot_field", &plot_field, "Generate SVG plot of magnetic field");
    m.def("plot_wire", &plot_wire, "Generate SVG plot of wire cross-section");
    m.def("plot_current_density", &plot_current_density, "Generate SVG plot of current density distribution");

    // Settings
    m.def("set_settings", &set_settings, "Configure simulation settings");
    m.def("get_settings", &get_settings, "Retrieve current simulation settings");
    m.def("reset_settings", &reset_settings, "Reset settings to defaults");

    // Signal processing
    m.def("calculate_harmonics", &calculate_harmonics, "Calculate harmonic components of a waveform");
    m.def("calculate_signal_from_harmonics", &calculate_signal_from_harmonics, "Reconstruct signal from harmonic components");
    m.def("calculate_processed", &calculate_processed, "Process raw signal data");

    // Bobbin processing
    m.def("calculate_bobbin_data", &calculate_bobbin_data, "Calculate bobbin specifications");
    m.def("process_bobbin", &process_bobbin, "Process bobbin geometry");

    // Wire data functions
    m.def("get_wire_data", &get_wire_data, "Get complete wire data from specification");
    m.def("get_wire_data_by_name", &get_wire_data_by_name, "Get wire data by name");
    m.def("get_wire_data_by_standard_name", &get_wire_data_by_standard_name, "Get wire data by standard designation");
    m.def("get_strand_by_standard_name", &get_strand_by_standard_name, "Get strand data by standard designation");
    m.def("get_wire_outer_width_rectangular", &get_wire_outer_width_rectangular, "Get outer width of rectangular wire");
    m.def("get_wire_outer_height_rectangular", &get_wire_outer_height_rectangular, "Get outer height of rectangular wire");
    m.def("get_wire_outer_diameter_bare_litz", &get_wire_outer_diameter_bare_litz, "Get outer diameter of bare litz wire");
    m.def("get_wire_outer_diameter_served_litz", &get_wire_outer_diameter_served_litz, "Get outer diameter of served litz wire");
    m.def("get_wire_outer_diameter_insulated_litz", &get_wire_outer_diameter_insulated_litz, "Get outer diameter of insulated litz wire");
    m.def("get_wire_outer_diameter_enamelled_round", &get_wire_outer_diameter_enamelled_round, "Get outer diameter of enamelled round wire");
    m.def("get_wire_outer_diameter_insulated_round", &get_wire_outer_diameter_insulated_round, "Get outer diameter of insulated round wire");
    m.def("get_wire_conducting_diameter_by_standard_name", &get_wire_conducting_diameter_by_standard_name, "Get conducting diameter by standard name");
    m.def("get_outer_dimensions", &get_outer_dimensions, "Get outer dimensions of a wire");
    m.def("get_equivalent_wire", &get_equivalent_wire, "Get equivalent wire for comparison");
    m.def("get_coating_label", &get_coating_label, "Get coating label for a wire");
    m.def("get_wire_coating_by_label", &get_wire_coating_by_label, "Get wire coating data by label");
    m.def("get_coating_labels_by_type", &get_coating_labels_by_type, "Get available coating labels by type");

    // Database and material loading
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
    m.def("get_available_wires", &get_available_wires, "Get list of all available wires");
    m.def("get_unique_wire_diameters", &get_unique_wire_diameters, "Get list of unique wire diameters");
    m.def("get_available_wire_types", &get_available_wire_types, "Get list of available wire types");
    m.def("get_available_wire_standards", &get_available_wire_standards, "Get list of available wire standards");
    m.def("calculate_gap_reluctance", &calculate_gap_reluctance, "Calculate magnetic reluctance of an air gap");
    m.def("get_gap_reluctance_model_information", &get_gap_reluctance_model_information, "Get information about gap reluctance models");
    m.def("calculate_number_turns_from_gapping_and_inductance", &calculate_number_turns_from_gapping_and_inductance,
        "Calculate required number of turns from gap and target inductance");
    m.def("calculate_gapping_from_number_turns_and_inductance", &calculate_gapping_from_number_turns_and_inductance,
        "Calculate required gap from turns count and target inductance");
    m.def("get_core_losses_model_information", &get_core_losses_model_information, "Get information about available core loss models");
    m.def("get_core_temperature_model_information", &get_core_temperature_model_information, "Get information about core temperature models");
    m.def("resolve_dimension_with_tolerance", &resolve_dimension_with_tolerance, "Resolve dimension value from tolerance specification");
    m.def("calculate_induced_voltage", &calculate_induced_voltage, "Calculate induced voltage in a winding");
    m.def("calculate_induced_current", &calculate_induced_current, "Calculate induced current in a winding");
    m.def("calculate_reflected_secondary", &calculate_reflected_secondary, "Calculate secondary-side reflected values");
    m.def("calculate_reflected_primary", &calculate_reflected_primary, "Calculate primary-side reflected values");
    m.def("calculate_instantaneous_power", &calculate_instantaneous_power, "Calculate instantaneous power");
    m.def("calculate_rms_power", &calculate_rms_power, "Calculate RMS power");
    m.def("calculate_basic_processed_data", &calculate_basic_processed_data, "Process basic signal data");
    m.def("create_waveform", &create_waveform, "Create a waveform from parameters");
    m.def("scale_waveform_time_to_frequency", &scale_waveform_time_to_frequency, "Scale waveform time base to frequency");
    m.def("calculate_insulation", &calculate_insulation, "Calculate insulation requirements");
    m.def("extract_operating_point", &extract_operating_point, "Extract operating point from inputs");
    m.def("extract_map_column_names", &extract_map_column_names, "Extract column names from map data");
    m.def("extract_column_names", &extract_column_names, "Extract column names from data");
    m.def("calculate_number_turns", &calculate_number_turns, "Calculate optimal number of turns");
    m.def("calculate_dc_resistance_per_meter", &calculate_dc_resistance_per_meter, "Calculate DC resistance per meter of wire");
    m.def("calculate_dc_losses_per_meter", &calculate_dc_losses_per_meter, "Calculate DC losses per meter of wire");
    m.def("calculate_skin_ac_losses_per_meter", &calculate_skin_ac_losses_per_meter, "Calculate AC skin losses per meter");
    m.def("calculate_skin_ac_factor", &calculate_skin_ac_factor, "Calculate skin effect AC factor");
    m.def("calculate_skin_ac_resistance_per_meter", &calculate_skin_ac_resistance_per_meter, "Calculate AC resistance per meter due to skin effect");
    m.def("calculate_effective_current_density", &calculate_effective_current_density, "Calculate effective current density in wire");
    m.def("calculate_effective_skin_depth", &calculate_effective_skin_depth, "Calculate effective skin depth");
    m.def("get_available_winding_orientations", &get_available_winding_orientations, "Get list of available winding orientations");
    m.def("get_available_coil_alignments", &get_available_coil_alignments, "Get list of available coil alignments");
    m.def("check_requirement", &check_requirement, "Check if a design requirement is met");
    m.def("wind_planar", &wind_planar, "Wind planar coils");
    m.def("wind_by_sections", &wind_by_sections, "Wind coil organized by sections");
    m.def("wind_by_layers", &wind_by_layers, "Wind coil organized by layers");
    m.def("wind_by_turns", &wind_by_turns, "Wind coil turn by turn");
    m.def("delimit_and_compact", &delimit_and_compact, "Delimit and compact winding layout");
    m.def("get_layers_by_winding_index", &get_layers_by_winding_index, "Get layers for a specific winding index");
    m.def("get_layers_by_section", &get_layers_by_section, "Get layers within a section");
    m.def("get_sections_description_conduction", &get_sections_description_conduction, "Get conduction description for sections");
    m.def("simulate", &simulate, "Run magnetic simulation");
    m.def("are_sections_and_layers_fitting", &are_sections_and_layers_fitting, "Check if sections and layers fit in window");
    m.def("add_margin_to_section_by_index", &add_margin_to_section_by_index, "Add margin to a section by index");
    m.def("check_if_fits", &check_if_fits, "Check if winding fits in available space");
    m.def("get_coating_thickness", &get_coating_thickness, "Get thickness of wire coating");
    m.def("get_coating_relative_permittivity", &get_coating_relative_permittivity, "Get relative permittivity of coating");
    m.def("get_coating_insulation_material", &get_coating_insulation_material, "Get insulation material of coating");
    m.def("get_insulation_layer_insulation_material", &get_insulation_layer_insulation_material, "Get insulation material for layer insulation");
    m.def("export_magnetic_as_subcircuit", &export_magnetic_as_subcircuit, "Export magnetic as SPICE subcircuit");
    m.def("process_inputs", &process_inputs, "Process and validate inputs");
    m.def("load_core_materials", &load_core_materials, "Load core materials into database");
    m.def("load_core_shapes", &load_core_shapes, "Load core shapes into database");
    m.def("load_wires", &load_wires, "Load wires into database");
    m.def("clear_databases", &clear_databases, "Clear all loaded databases");
    m.def("is_core_material_database_empty", &is_core_material_database_empty, "Check if core material database is empty");
    m.def("is_core_shape_database_empty", &is_core_shape_database_empty, "Check if core shape database is empty");
    m.def("is_wire_database_empty", &is_wire_database_empty, "Check if wire database is empty");
    m.def("get_isolation_side_from_index", &get_isolation_side_from_index, "Get isolation side from winding index");
    m.def("calculate_core_maximum_magnetic_energy", &calculate_core_maximum_magnetic_energy, "Calculate maximum magnetic energy in core");
    m.def("calculate_saturation_current", &calculate_saturation_current, "Calculate saturation current");
    m.def("calculate_temperature_from_core_thermal_resistance", &calculate_temperature_from_core_thermal_resistance, 
        "Calculate temperature rise from thermal resistance");
    m.def("get_coating", &get_coating, "Get coating data for a wire");
    m.def("load_databases", &load_databases, "Load all databases from JSON");
    m.def("read_databases", &read_databases, "Read databases from file path");
    m.def("load_mas", &load_mas, "Load a MAS (Magnetic Agnostic Structure) object");
    m.def("load_magnetic", &load_magnetic, "Load a magnetic component");
    m.def("load_magnetics", &load_magnetics, "Load multiple magnetic components");
    // m.def("load_magnetics_from_file", &load_magnetics_from_file, "");
    m.def("read_mas", &read_mas, "Read a MAS object by key");
    m.def("mas_autocomplete", &mas_autocomplete, "Autocomplete missing fields in MAS object");
    m.def("magnetic_autocomplete", &magnetic_autocomplete, "Autocomplete missing fields in magnetic component");
    m.def("calculate_steinmetz_coefficients", &calculate_steinmetz_coefficients, "Calculate Steinmetz coefficients from loss data");
    m.def("calculate_steinmetz_coefficients_with_error", &calculate_steinmetz_coefficients_with_error,
        "Calculate Steinmetz coefficients with error estimation");

    m.def("clear_magnetic_cache", &clear_magnetic_cache, "Clear cached magnetic calculations");
    // m.def("load_magnetic", &load_magnetic, "");
    // m.def("load_magnetics", &load_magnetics, "");
    m.def("load_magnetics_from_file", &load_magnetics_from_file, "Load magnetic components from file");
    m.def("calculate_advised_magnetics_from_catalog", &calculate_advised_magnetics_from_catalog, 
        "Get recommended magnetics from component catalog");
    m.def("calculate_advised_magnetics_from_cache", &calculate_advised_magnetics_from_cache,
        "Get recommended magnetics from cached designs");
}