# Development Guide

## Setting Up Development Environment

### Prerequisites

1. Install build dependencies:
```bash
# Linux
sudo apt-get install python3-dev cmake build-essential

# macOS
brew install cmake python

# Windows
# Install Visual Studio Build Tools and CMake
```

2. Clone the repository:
```bash
git clone https://github.com/tinix84/PyMKF.git
cd PyMKF
```

3. Create virtual environment:
```bash
python -m venv venv
source venv/bin/activate  # Linux/macOS
.\venv\Scripts\activate   # Windows
```

4. Install development dependencies:
```bash
pip install -e ".[dev]"
```

## Project Structure

```
PyMKF/
├── src/pymkf/           # Source code
│   ├── core.cpp         # Main C++ bindings
│   ├── wrappers/        # C++ wrapper headers
│   └── utils.py         # Python utilities
├── tests/               # Test files
├── examples/            # Example notebooks
└── docs/                # Documentation
```

## Building from Source

```bash
mkdir build
cd build
cmake ..
make
```

## Running Tests

```bash
# Run all tests
pytest

# Run specific test file
pytest tests/test_core.py

# Run with coverage
pytest --cov=pymkf
```

## Code Style

### Python Code

- Follow PEP 8
- Use type hints
- Document using NumPy style docstrings

Example:
```python
def calculate_losses(
    core_data: Dict[str, Any],
    temperature: float
) -> float:
    """Calculate core losses.

    Parameters
    ----------
    core_data : Dict[str, Any]
        Core specifications and properties
    temperature : float
        Operating temperature in Celsius

    Returns
    -------
    float
        Calculated losses in Watts
    """
    pass
```

### C++ Code

- Follow the Google C++ Style Guide
- Use consistent naming conventions
- Document using Doxygen style

Example:
```cpp
/**
 * Calculates core losses under specified conditions.
 * @param coreData Core specifications and properties
 * @param temperature Operating temperature in Celsius
 * @return Calculated losses in Watts
 */
double calculateLosses(const json& coreData, double temperature);
```

## Documentation

### Building Documentation

```bash
# Install documentation dependencies
pip install -r docs/requirements.txt

# Serve documentation locally
mkdocs serve

# Build documentation
mkdocs build
```

### Writing Documentation

1. Update API documentation:
   - Use mkdocstrings annotations
   - Include examples
   - Document exceptions

2. Add examples:
   - Create Jupyter notebooks
   - Include in examples/ directory
   - Update examples/index.md

## Contributing

### Workflow

1. Create a new branch:
```bash
git checkout -b feature/new-feature
```

2. Make changes and commit:
```bash
git add .
git commit -m "feat: add new feature"
```

3. Push changes:
```bash
git push origin feature/new-feature
```

4. Create Pull Request

### Commit Messages

Follow conventional commits:
- feat: New feature
- fix: Bug fix
- docs: Documentation
- style: Formatting
- refactor: Code restructuring
- test: Adding tests
- chore: Maintenance

### Code Review Process

1. Submit PR with:
   - Clear description
   - Test results
   - Documentation updates

2. Address review comments

3. Maintain CI/CD pipeline
   - Tests pass
   - Documentation builds
   - Code style checks

## Release Process

1. Version bump:
   - Update version in pyproject.toml
   - Update CHANGELOG.md

2. Create release:
```bash
# Tag release
git tag -a v1.0.0 -m "Release v1.0.0"
git push origin v1.0.0

# Build distribution
python -m build

# Upload to PyPI
python -m twine upload dist/*
```

## Troubleshooting

### Common Build Issues

1. CMake Configuration
```bash
# Debug CMake
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

2. Compiler Issues
```bash
# Set specific compiler
CC=gcc CXX=g++ cmake ..
```

### Debug Tools

1. GDB for C++:
```bash
gdb python
(gdb) run -c "import pymkf"
```

2. Python Debugger:
```python
import pdb; pdb.set_trace()
```