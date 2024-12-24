# Examples

## Basic Usage

### Core Selection

```python
import pymkf

# Get list of available cores
cores = pymkf.get_core_shapes()

# Find a specific core shape
core = pymkf.find_core_shape_by_name("E65/32/27")

# Calculate core parameters
processed_data = pymkf.calculate_core_data(core, include_material_data=True)
```

### Winding Design

```python
# Get available wires
wires = pymkf.get_wires()

# Design a winding
winding_data = {
    "number_turns": 20,
    "wire_type": "round",
    "wire_name": "AWG24"
}

# Calculate winding losses
losses = pymkf.calculate_winding_losses(magnetic_data, operating_point, temperature=25)
```

## Advanced Examples

### Transformer Design

```python
# Complete transformer design example
# ... code example ...
```

### Thermal Analysis

```python
# Thermal analysis example
# ... code example ...
```

## Interactive Notebooks

For more detailed examples, check out our Jupyter notebooks in the `examples/` directory:

1. [Basic Usage](../examples/basic_usage.ipynb)
   - Core selection
   - Simple calculations
   
2. [Core Analysis](../examples/core_analysis.ipynb)
   - Material properties
   - Loss calculations
   
3. [Winding Design](../examples/winding_design.ipynb)
   - Wire selection
   - Winding configurations

## Best Practices

1. Core Selection
   - Consider temperature range
   - Account for saturation
   
2. Winding Design
   - Consider thermal constraints
   - Account for proximity effects
   
3. Loss Calculations
   - Include all relevant effects
   - Validate assumptions

## Common Pitfalls

1. Temperature Dependencies
   - Always specify temperature
   - Consider thermal limits
   
2. Frequency Effects
   - Account for skin effect
   - Consider core loss frequency dependence