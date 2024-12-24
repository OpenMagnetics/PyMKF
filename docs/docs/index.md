# PyMKF Documentation

Welcome to the PyMKF documentation. PyMKF is a Python wrapper for MKF module of OpenMagnetics

## Features

- Feature 1
- Feature 2
- Feature 3

## Quick Start

```python
import pymkf
```

# Contributing
Contributions are welcome! Please feel free to submit a Pull Request.
## Installation
refer to [Installation](installation.md)

## Examples
refer to [Examples](examples.md)



I'll help create comprehensive documentation templates based on our previous mkdocs configuration. Here's the detailed documentation structure:

1. First, let's create the main `index.md`:

```markdown
# PyMKF Documentation

PyMKF (Python Magnetic Kernel Framework) is a Python library for designing and analyzing magnetic components. It provides a comprehensive set of tools for core selection, winding design, and magnetic performance analysis.

## Features

- Core Selection and Analysis
  - Access to extensive database of magnetic cores
  - Material properties analysis
  - Core loss calculations
  - Temperature modeling

- Winding Design
  - Wire selection and configuration
  - Multi-winding transformer design
  - Layer-by-layer winding specification
  - Insulation requirements calculation

- Magnetic Field Analysis
  - Field strength calculations
  - Proximity effect modeling
  - Skin effect analysis
  - Thermal modeling

## Quick Start

```python
import pymkf

# Get available core materials
core_materials = pymkf.get_core_materials()

# Find a specific core shape
core_shape = pymkf.find_core_shape_by_name("E65/32/27")

# Calculate core losses
core_losses = pymkf.calculate_core_losses(core_data, coil_data, inputs_data, models_data)
```

## Installation

```bash
pip install pymkf
```

For development installation:

```bash
git clone https://github.com/yourusername/PyMKF.git
cd PyMKF
pip install -e ".[dev]"
```

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.
```

2. Create `installation.md`:

```markdown
# Installation Guide

## Requirements

### Core Dependencies
- Python >= 3.7
- numpy >= 1.19.0
- pandas >= 1.3.0

### Build Dependencies
- CMake >= 3.15
- C++ compiler with C++17 support
- pybind11

## Installation Methods

### From PyPI (Recommended)

```bash
pip install pymkf
```

### From Source

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/PyMKF.git
   cd PyMKF
   ```

2. Install development dependencies:
   ```bash
   pip install -e ".[dev]"
   ```

### Build from Source

1. Make sure you have CMake installed:
   ```bash
   cmake --version
   ```

2. Configure and build:
   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```

## Platform-Specific Instructions

### Windows
- Install Visual Studio Build Tools
- Use a compatible Python distribution

### Linux
- Install required development packages:
  ```bash
  sudo apt-get install python3-dev cmake build-essential
  ```

### macOS
- Install Xcode Command Line Tools
- Use Homebrew to install dependencies:
  ```bash
  brew install cmake python
  ```

## Troubleshooting

### Common Issues

1. CMake Not Found
   - Ensure CMake is installed and in your PATH
   - Check CMake version requirements

2. Compiler Errors
   - Verify C++17 compiler support
   - Check compiler compatibility

3. Python Version Mismatch
   - Verify Python version compatibility
   - Check virtual environment settings
```

3. Create `api.md`:

```markdown
# API Reference

## Core Functions

### Core Material Functions

```python
def get_core_materials() -> List[Dict]:
    """Get a list of all available core materials.
    
    Returns:
        List[Dict]: List of core material dictionaries containing properties
    """

def get_material_permeability(
    material_name: str,
    temperature: float,
    magnetic_field_dc_bias: float,
    frequency: float
) -> float:
    """Get the permeability of a core material under specific conditions.
    
    Args:
        material_name: Name of the material
        temperature: Operating temperature in Celsius
        magnetic_field_dc_bias: DC bias field in A/m
        frequency: Operating frequency in Hz
        
    Returns:
        float: Relative permeability value
    """
```

### Core Shape Functions

```python
def get_core_shapes() -> List[Dict]:
    """Get a list of all available core shapes.
    
    Returns:
        List[Dict]: List of core shape dictionaries containing dimensions
    """
```

### Wire Functions

```python
def get_wires() -> List[Dict]:
    """Get a list of all available wires.
    
    Returns:
        List[Dict]: List of wire dictionaries containing properties
    """
```

### Design and Analysis

```python
def calculate_core_losses(
    core_data: Dict,
    coil_data: Dict,
    inputs_data: Dict,
    models_data: Dict
) -> Dict:
    """Calculate core losses under specified conditions.
    
    Args:
        core_data: Core specifications
        coil_data: Coil specifications
        inputs_data: Operating conditions
        models_data: Loss model parameters
        
    Returns:
        Dict: Calculated losses and related parameters
    """
```

## Models and Constants

### Available Models

- Core Loss Models
- Temperature Models
- Reluctance Models

### Physical Constants

```python
def get_constants() -> Dict:
    """Get physical constants used in calculations.
    
    Returns:
        Dict: Dictionary of constant names and values
    """
```

## Examples
refer to [Examples](examples.md)


# TODO
create additional documentation sections for:
1. Advanced usage and modeling details
2. Contribution guidelines
3. Development setup
4. Performance optimization tips
5. Troubleshooting guide


docs/
├── docs/
│   ├── index.md
│   ├── installation.md
│   ├── requirements.md
│   ├── build.md
│   ├── faq.md
│   ├── user-guide/
│   │   ├── core-selection.md
│   │   ├── winding-design.md
│   │   ├── thermal-analysis.md
│   │   └── loss-calculations.md
│   ├── api/
│   │   ├── core.md
│   │   ├── wire.md
│   │   ├── design.md
│   │   └── constants.md
│   ├── examples/
│   │   ├── basic-usage.md
│   │   ├── advanced.md
│   │   └── notebooks.md
│   ├── development/
│   │   ├── contributing.md
│   │   ├── code-style.md
│   │   └── testing.md
│   └── css/
│       └── custom.css
└── mkdocs.yml