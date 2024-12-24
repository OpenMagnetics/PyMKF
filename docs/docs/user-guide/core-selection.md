# Core Selection Guide

## Overview

This guide covers the process of selecting appropriate magnetic cores for your application using PyMKF. Core selection is a critical step in magnetic component design, affecting performance, size, and cost.

## Available Core Types

PyMKF supports various core types:
- E cores
- ETD cores
- RM cores
- Pot cores
- Toroidal cores
- PQ cores

## Core Selection Process

### 1. Basic Selection

```python
import pymkf

# List available cores
core_shapes = pymkf.get_core_shapes()
core_materials = pymkf.get_core_materials()

# Find specific core by name
core = pymkf.find_core_shape_by_name("E65/32/27")
```

### 2. Material Selection

The material selection depends on several factors:
- Operating frequency
- Temperature requirements
- Loss requirements
- Cost constraints

```python
# Get material properties
permeability = pymkf.get_material_permeability(
    material_name="N87",
    temperature=25.0,
    magnetic_field_dc_bias=0.0,
    frequency=100000.0
)
```

### 3. Core Size Calculation

Core size selection based on area product:

```python
# Calculate advised cores based on requirements
advised_cores = pymkf.calculate_advised_cores(
    inputsJson,
    weightsJson,
    maximumNumberResults=5,
    useOnlyCoresInStock=True
)
```

### 4. Gap Calculation

Calculating air gap for a specific inductance:

```python
gapping = pymkf.calculate_gapping_from_number_turns_and_inductance(
    core_data,
    coil_data,
    inputs_data,
    "DISTRIBUTED",
    3
)
```

## Design Considerations

### Temperature Effects
!!! warning "Temperature Considerations"
    Always consider the temperature dependency of core materials.
    ```python
    temp_params = pymkf.get_core_temperature_dependant_parameters(core_data, temperature=100)
    ```

### Saturation Limits
Monitor the magnetic flux density to avoid saturation:
```python
core_losses = pymkf.calculate_core_losses(
    core_data,
    coil_data,
    inputs_data,
    models_data
)
```

## Advanced Topics

### 1. Core Loss Models
PyMKF supports various core loss models:
```python
models_info = pymkf.get_core_losses_model_information(material)
```

### 2. Thermal Modeling
Understanding thermal behavior:
```python
thermal_info = pymkf.get_core_temperature_model_information()
```

## Examples

### Basic Core Selection
```python
# Example of complete core selection process
# Step 1: Define requirements
requirements = {
    "inductance": 1e-3,  # 1 mH
    "frequency": 100e3,  # 100 kHz
    "current_peak": 10.0, # 10 A
    "temperature_rise": 40.0 # 40°C
}

# Step 2: Get advised cores
cores = pymkf.calculate_advised_cores(...)

# Step 3: Analyze selected core
analysis = pymkf.calculate_core_data(...)
```

### Advanced Optimization
Example of optimizing core selection for specific criteria:
```python
# Define weights for different optimization criteria
weights = {
    "AREA_PRODUCT": 1.0,
    "ENERGY_STORED": 1.0,
    "COST": 1.0,
    "EFFICIENCY": 1.0,
    "DIMENSIONS": 1.0
}

# Get optimized core suggestions
optimized_cores = pymkf.calculate_advised_cores(inputs, weights, 5, True)
```

## Common Pitfalls

1. **Temperature Consideration**
   - Always account for temperature effects on permeability
   - Consider worst-case scenarios

2. **Saturation Limits**
   - Check peak flux density under maximum current
   - Include temperature effects on saturation

3. **Gapping Considerations**
   - Account for fringing flux
   - Consider distributed gaps for high-frequency applications

## Best Practices

1. **Material Selection**
   - Match material to frequency range
   - Consider temperature stability

2. **Size Selection**
   - Use area product method for initial sizing
   - Account for thermal requirements

3. **Performance Verification**
   - Verify core losses
   - Check temperature rise
   - Validate inductance with gap