#include "simulation.h"

namespace PyMKF {

json simulate(json inputsJson, json magneticJson, json modelsData) {
    try {
        OpenMagnetics::Inputs inputs(inputsJson);
        OpenMagnetics::Magnetic magnetic(magneticJson);
        
        auto reluctanceModelName = OpenMagnetics::defaults.reluctanceModelDefault;
        if (!modelsData.is_null() && modelsData.find("reluctance") != modelsData.end()) {
            OpenMagnetics::from_json(modelsData["reluctance"], reluctanceModelName);
        }
        auto coreLossesModelName = OpenMagnetics::defaults.coreLossesModelDefault;
        if (!modelsData.is_null() && modelsData.find("coreLosses") != modelsData.end()) {
            OpenMagnetics::from_json(modelsData["coreLosses"], coreLossesModelName);
        }
        auto coreTemperatureModelName = OpenMagnetics::defaults.coreTemperatureModelDefault;
        if (!modelsData.is_null() && modelsData.find("coreTemperature") != modelsData.end()) {
            OpenMagnetics::from_json(modelsData["coreTemperature"], coreTemperatureModelName);
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
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

ordered_json export_magnetic_as_subcircuit(json magneticJson) {
    try {
        OpenMagnetics::Magnetic magnetic(magneticJson);
        ordered_json subcircuit = OpenMagnetics::CircuitSimulatorExporter().export_magnetic_as_subcircuit(magnetic);
        return subcircuit.dump(4);
    }
    catch (const std::exception &exc) {
        ordered_json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

json mas_autocomplete(json masJson, json configuration) {
    try {
        OpenMagnetics::Mas mas(masJson);
        auto completedMas = OpenMagnetics::mas_autocomplete(mas, configuration);
        json result;
        to_json(result, completedMas);
        return result;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

json magnetic_autocomplete(json magneticJson, json configuration) {
    try {
        OpenMagnetics::Magnetic magnetic(magneticJson);
        auto completedMagnetic = OpenMagnetics::magnetic_autocomplete(magnetic, configuration);
        json result;
        to_json(result, completedMagnetic);
        return result;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

json process_inputs(json inputsJson) {
    try {
        OpenMagnetics::Inputs inputs(inputsJson);
        auto operatingPoints = inputs.get_mutable_operating_points();
        for (size_t operatingPointIndex = 0; operatingPointIndex < operatingPoints.size(); ++operatingPointIndex) {
            auto excitationsPerWinding = operatingPoints[operatingPointIndex].get_excitations_per_winding();
            for (size_t excitationIndex = 0; excitationIndex < excitationsPerWinding.size(); ++excitationIndex) {
                auto excitation = operatingPoints[operatingPointIndex].get_mutable_excitations_per_winding()[excitationIndex];
                if (excitation.get_current()) {
                    auto current = excitation.get_current().value();
                    if (!current.get_processed() && current.get_waveform()) {
                        auto processed = OpenMagnetics::Inputs::calculate_basic_processed_data(current.get_waveform().value());
                        current.set_processed(processed);
                    }
                    if (!current.get_harmonics() && current.get_waveform()) {
                        auto harmonics = OpenMagnetics::Inputs::calculate_harmonics_data(current.get_waveform().value(), excitation.get_frequency());
                        current.set_harmonics(harmonics);
                    }
                    if (!current.get_waveform() && current.get_processed()) {
                        auto waveform = OpenMagnetics::Inputs::create_waveform(current.get_processed().value(), excitation.get_frequency());
                        current.set_waveform(waveform);
                    }
                    excitation.set_current(current);
                }
                if (excitation.get_voltage()) {
                    auto voltage = excitation.get_voltage().value();
                    if (!voltage.get_processed() && voltage.get_waveform()) {
                        auto processed = OpenMagnetics::Inputs::calculate_basic_processed_data(voltage.get_waveform().value());
                        voltage.set_processed(processed);
                    }
                    if (!voltage.get_harmonics() && voltage.get_waveform()) {
                        auto harmonics = OpenMagnetics::Inputs::calculate_harmonics_data(voltage.get_waveform().value(), excitation.get_frequency());
                        voltage.set_harmonics(harmonics);
                    }
                    if (!voltage.get_waveform() && voltage.get_processed()) {
                        auto waveform = OpenMagnetics::Inputs::create_waveform(voltage.get_processed().value(), excitation.get_frequency());
                        voltage.set_waveform(waveform);
                    }
                    excitation.set_voltage(voltage);
                }
                operatingPoints[operatingPointIndex].get_mutable_excitations_per_winding()[excitationIndex] = excitation;
            }
            inputs.get_mutable_operating_points()[operatingPointIndex] = operatingPoints[operatingPointIndex];
        }
        json result;
        to_json(result, inputs);
        return result;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

json extract_operating_point(json fileJson, size_t numberWindings, double frequency, double desiredMagnetizingInductance, json mapColumnNamesJson) {
    try {
        std::vector<std::map<std::string, std::string>> mapColumnNames = mapColumnNamesJson.get<std::vector<std::map<std::string, std::string>>>();
        auto reader = OpenMagnetics::CircuitSimulationReader(fileJson);
        auto operatingPoint = reader.extract_operating_point(numberWindings, frequency, mapColumnNames);
        operatingPoint = OpenMagnetics::Inputs::process_operating_point(operatingPoint, desiredMagnetizingInductance);
        json result;
        to_json(result, operatingPoint);
        return result;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

json extract_map_column_names(json fileJson, size_t numberWindings, double frequency) {
    try {
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
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

json extract_column_names(json fileJson) {
    try {
        auto reader = OpenMagnetics::CircuitSimulationReader(fileJson);
        auto columnNames = reader.extract_column_names();
        json result = json::array();
        for (auto& columnName : columnNames) {
            result.push_back(columnName);
        }
        return result;
    }
    catch (const std::exception &exc) {
        json exception;
        exception["data"] = "Exception: " + std::string{exc.what()};
        return exception;
    }
}

void register_simulation_bindings(py::module& m) {
    m.def("simulate", &simulate);
    m.def("export_magnetic_as_subcircuit", &export_magnetic_as_subcircuit);
    m.def("mas_autocomplete", &mas_autocomplete);
    m.def("magnetic_autocomplete", &magnetic_autocomplete);
    m.def("process_inputs", &process_inputs);
    m.def("extract_operating_point", &extract_operating_point);
    m.def("extract_map_column_names", &extract_map_column_names);
    m.def("extract_column_names", &extract_column_names);
}

} // namespace PyMKF
