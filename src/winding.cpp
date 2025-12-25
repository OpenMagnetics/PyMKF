#include "winding.h"

namespace PyMKF {

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

json get_layers_by_winding_index(json coilJson, int windingIndex) {
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

json get_layers_by_section(json coilJson, json sectionName) {
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

json get_sections_description_conduction(json coilJson) {
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

std::vector<std::string> get_available_winding_orientations() {
    std::vector<std::string> orientations;
    for (auto& [orientation, _] : magic_enum::enum_entries<WindingOrientation>()) {
        json orientationJson;
        to_json(orientationJson, orientation);
        orientations.push_back(orientationJson);
    }
    return orientations;
}

std::vector<std::string> get_available_coil_alignments() {
    std::vector<std::string> orientations;
    for (auto& [orientation, _] : magic_enum::enum_entries<CoilAlignment>()) {
        json orientationJson;
        to_json(orientationJson, orientation);
        orientations.push_back(orientationJson);
    }
    return orientations;
}

std::vector<int> calculate_number_turns(int numberTurnsPrimary, json designRequirementsJson) {
    DesignRequirements designRequirements(designRequirementsJson);

    OpenMagnetics::NumberTurns numberTurns(numberTurnsPrimary, designRequirements);
    auto numberTurnsCombination = numberTurns.get_next_number_turns_combination();

    std::vector<int> numberTurnsResult;
    for (auto turns : numberTurnsCombination) {
        numberTurnsResult.push_back(static_cast<std::make_signed<int>::type>(turns));
    }
    return numberTurnsResult;
}

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

json calculate_insulation(json inputsJson) {
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
    catch(const std::runtime_error& re) {
        result["errorMessage"] = "Exception: " + std::string{re.what()};
    }
    catch(const std::exception& ex) {
        result["errorMessage"] = "Exception: " + std::string{ex.what()};
    }
    catch(...) {
        result["errorMessage"] = "Unknown failure occurred. Possible memory corruption";
    }
    return result;
}

json get_insulation_layer_insulation_material(json coilJson, std::string layerName) {
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

json get_isolation_side_from_index(size_t index) {
    return OpenMagnetics::get_isolation_side_from_index(index);
}

void register_winding_bindings(py::module& m) {
    // Winding functions
    m.def("wind", &wind, "Wind coils on a magnetic core according to specifications");
    m.def("wind_planar", &wind_planar, "Wind planar coils");
    m.def("wind_by_sections", &wind_by_sections, "Wind coil organized by sections");
    m.def("wind_by_layers", &wind_by_layers, "Wind coil organized by layers");
    m.def("wind_by_turns", &wind_by_turns, "Wind coil turn by turn");
    m.def("delimit_and_compact", &delimit_and_compact, "Delimit and compact winding layout");

    // Layer and section functions
    m.def("get_layers_by_winding_index", &get_layers_by_winding_index, "Get layers for a specific winding index");
    m.def("get_layers_by_section", &get_layers_by_section, "Get layers within a section");
    m.def("get_sections_description_conduction", &get_sections_description_conduction, "Get conduction description for sections");
    m.def("are_sections_and_layers_fitting", &are_sections_and_layers_fitting, "Check if sections and layers fit in window");
    m.def("add_margin_to_section_by_index", &add_margin_to_section_by_index, "Add margin to a section by index");

    // Winding orientation and alignment
    m.def("get_available_winding_orientations", &get_available_winding_orientations, "Get list of available winding orientations");
    m.def("get_available_coil_alignments", &get_available_coil_alignments, "Get list of available coil alignments");

    // Number of turns
    m.def("calculate_number_turns", &calculate_number_turns, "Calculate optimal number of turns");

    // Insulation
    m.def("get_insulation_materials", &get_insulation_materials, "Retrieve all available insulation materials");
    m.def("get_insulation_material_names", &get_insulation_material_names, "Retrieve list of all insulation material names");
    m.def("find_insulation_material_by_name", &find_insulation_material_by_name, "Find insulation material data by name");
    m.def("calculate_insulation", &calculate_insulation, "Calculate insulation requirements");
    m.def("get_insulation_layer_insulation_material", &get_insulation_layer_insulation_material, "Get insulation material for layer insulation");
    m.def("get_isolation_side_from_index", &get_isolation_side_from_index, "Get isolation side from winding index");
}

} // namespace PyMKF
