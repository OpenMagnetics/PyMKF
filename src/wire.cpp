#include "wire.h"

namespace PyMKF {

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

json get_wire_data(json windingDataJson) {
    OpenMagnetics::Winding winding(windingDataJson);
    auto wire = OpenMagnetics::Coil::resolve_wire(winding);
    json result;
    to_json(result, wire);
    return result;
}

json get_wire_data_by_name(std::string name) {
    auto wireData = OpenMagnetics::find_wire_by_name(name);
    json result;
    to_json(result, wireData);
    return result;
}

json get_wire_data_by_standard_name(std::string standardName) {
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

json get_strand_by_standard_name(std::string standardName) {
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

double get_wire_conducting_diameter_by_standard_name(std::string standardName) {
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

double get_wire_outer_width_rectangular(double conductingWidth, int grade, json wireStandardJson) {
    WireStandard wireStandard;
    from_json(wireStandardJson, wireStandard);
    return OpenMagnetics::Wire::get_outer_width_rectangular(conductingWidth, grade, wireStandard);
}

double get_wire_outer_height_rectangular(double conductingHeight, int grade, json wireStandardJson) {
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

json get_equivalent_wire(json oldWireJson, json newWireTypeJson, double effectivefrequency) {
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

json get_coating(json wireJson) {
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

json get_coating_label(json wireJson) {
    try {
        OpenMagnetics::Wire wire(wireJson);
        auto coatingLabel = wire.encode_coating_label();
        return coatingLabel;
    }
    catch(const std::runtime_error& re) {
        return "Exception: " + std::string{re.what()};
    }
    catch(const std::exception& ex) {
        return "Exception: " + std::string{ex.what()};
    }
    catch(...) {
        return "Unknown failure occurred. Possible memory corruption";
    }
}

json get_wire_coating_by_label(std::string label) {
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

std::vector<std::string> get_coating_labels_by_type(json wireTypeJson) {
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

double get_coating_thickness(json wireJson) {
    try {
        OpenMagnetics::Wire wire(wireJson);
        return wire.get_coating_thickness();
    }
    catch (const std::exception &exc) {
        std::cout << std::string{exc.what()} << std::endl;
        return -1;
    }
}

double get_coating_relative_permittivity(json wireJson) {
    try {
        OpenMagnetics::Wire wire(wireJson);
        return wire.get_coating_relative_permittivity();
    }
    catch (const std::exception &exc) {
        std::cout << std::string{exc.what()} << std::endl;
        return -1;
    }
}

json get_coating_insulation_material(json wireJson) {
    try {
        OpenMagnetics::Wire wire(wireJson);
        OpenMagnetics::InsulationMaterial material;

        try {
            material = wire.resolve_coating_insulation_material();
        }
        catch (const std::exception &e) {
            if (std::string{e.what()} == "Coating is missing material information") {
                material = OpenMagnetics::find_insulation_material_by_name(OpenMagnetics::defaults.defaultEnamelledInsulationMaterial);
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

std::vector<std::string> get_available_wires() {
    return OpenMagnetics::get_wire_names();
}

std::vector<std::string> get_unique_wire_diameters(json wireStandardJson) {
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

std::vector<std::string> get_available_wire_types() {
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

std::vector<std::string> get_available_wire_standards() {
    std::vector<std::string> wireStandards;

    for (auto [value, name] : magic_enum::enum_entries<WireStandard>()) {
        json wireStandardJson;
        to_json(wireStandardJson, value);
        wireStandards.push_back(wireStandardJson);
    }

    return wireStandards;
}

void register_wire_bindings(py::module& m) {
    // Wires and materials
    m.def("get_wires", &get_wires, "Retrieve all available wires as JSON objects");
    m.def("get_wire_materials", &get_wire_materials, "Retrieve all available wire materials");
    m.def("get_wire_names", &get_wire_names, "Retrieve list of all wire names");
    m.def("get_wire_material_names", &get_wire_material_names, "Retrieve list of all wire material names");

    // Lookup functions
    m.def("find_wire_by_name", &find_wire_by_name, "Find wire data by name");
    m.def("find_wire_material_by_name", &find_wire_material_by_name, "Find wire material data by name");
    m.def("find_wire_by_dimension", &find_wire_by_dimension, "Find wire by dimension, type, and standard");

    // Wire data functions
    m.def("get_wire_data", &get_wire_data, "Get complete wire data from specification");
    m.def("get_wire_data_by_name", &get_wire_data_by_name, "Get wire data by name");
    m.def("get_wire_data_by_standard_name", &get_wire_data_by_standard_name, "Get wire data by standard designation");
    m.def("get_strand_by_standard_name", &get_strand_by_standard_name, "Get strand data by standard designation");
    m.def("get_wire_conducting_diameter_by_standard_name", &get_wire_conducting_diameter_by_standard_name, "Get conducting diameter by standard name");

    // Wire dimensions
    m.def("get_wire_outer_width_rectangular", &get_wire_outer_width_rectangular, "Get outer width of rectangular wire");
    m.def("get_wire_outer_height_rectangular", &get_wire_outer_height_rectangular, "Get outer height of rectangular wire");
    m.def("get_wire_outer_diameter_bare_litz", &get_wire_outer_diameter_bare_litz, "Get outer diameter of bare litz wire");
    m.def("get_wire_outer_diameter_served_litz", &get_wire_outer_diameter_served_litz, "Get outer diameter of served litz wire");
    m.def("get_wire_outer_diameter_insulated_litz", &get_wire_outer_diameter_insulated_litz, "Get outer diameter of insulated litz wire");
    m.def("get_wire_outer_diameter_enamelled_round", &get_wire_outer_diameter_enamelled_round, "Get outer diameter of enamelled round wire");
    m.def("get_wire_outer_diameter_insulated_round", &get_wire_outer_diameter_insulated_round, "Get outer diameter of insulated round wire");
    m.def("get_outer_dimensions", &get_outer_dimensions, "Get outer dimensions of a wire");

    // Wire utilities
    m.def("get_equivalent_wire", &get_equivalent_wire, "Get equivalent wire for comparison");
    m.def("get_coating", &get_coating, "Get coating data for a wire");
    m.def("get_coating_label", &get_coating_label, "Get coating label for a wire");
    m.def("get_wire_coating_by_label", &get_wire_coating_by_label, "Get wire coating data by label");
    m.def("get_coating_labels_by_type", &get_coating_labels_by_type, "Get available coating labels by type");
    m.def("get_coating_thickness", &get_coating_thickness, "Get thickness of wire coating");
    m.def("get_coating_relative_permittivity", &get_coating_relative_permittivity, "Get relative permittivity of coating");
    m.def("get_coating_insulation_material", &get_coating_insulation_material, "Get insulation material of coating");

    // Availability queries
    m.def("get_available_wires", &get_available_wires, "Get list of all available wires");
    m.def("get_unique_wire_diameters", &get_unique_wire_diameters, "Get list of unique wire diameters");
    m.def("get_available_wire_types", &get_available_wire_types, "Get list of available wire types");
    m.def("get_available_wire_standards", &get_available_wire_standards, "Get list of available wire standards");
}

} // namespace PyMKF
