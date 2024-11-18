import PyMKF

# material_data = PyMKF.find_core_material_by_name("3C97")
# print(material_data)


# coil = {'bobbin': {'processedDescription': {'columnDepth': 0.008725, 'columnShape': 'rectangular', 'columnThickness': 0.001225, 'wallThickness': 0.0011680000000000006, 'windingWindows': [{'area': 3.34616e-05, 'coordinates': [0.005363, 0.0, 0.0], 'height': 0.008863999999999999, 'shape': 'rectangular', 'width': 0.003775}], 'columnWidth': 0.0034755, 'coordinates': [0.0, 0.0, 0.0]}}, 'functionalDescription': [{'isolationSide': 'primary', 'name': 'Primary', 'numberParallels': 1, 'numberTurns': 14, 'wire': 'Round 1.00 - Grade 1'}]}
# print(coil)

# result = PyMKF.wind(coil, 1, [], [], [])
# print(result)
wire = {'type': 'round', 'conductingDiameter': {'nominal': 0.001}, 'material': 'copper', 'outerDiameter': {'nominal': 0.001062}, 'coating': {'breakdownVoltage': 2700.0, 'grade': 1, 'type': 'enamelled'}, 'name': 'Round 1.00 - Grade 1', 'numberConductors': 1, 'standard': 'IEC 60317', 'standardName': '1.00 mm'}
print(wire)

result = PyMKF.get_coating_relative_permittivity(wire)
print(result)
