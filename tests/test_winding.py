"""
Tests for PyMKF Winding functions.

These tests verify wire, bobbin, and insulation material retrieval.
"""
import pytest
import PyMKF


class TestWires:
    """Wire data retrieval tests."""

    def test_get_wires_returns_list(self):
        """Wires should be returned as a list."""
        wires = PyMKF.get_wires()
        assert isinstance(wires, list)
        assert len(wires) > 0

    def test_get_wire_names(self):
        """Wire names should be retrievable."""
        names = PyMKF.get_wire_names()
        assert isinstance(names, list)
        assert len(names) > 0
        assert all(isinstance(name, str) for name in names)

    def test_find_wire_by_name(self):
        """Should be able to find specific wire by name."""
        names = PyMKF.get_wire_names()
        if len(names) > 0:
            wire = PyMKF.find_wire_by_name(names[0])
            assert isinstance(wire, dict)

    def test_wire_has_type(self):
        """Wires should have type information."""
        names = PyMKF.get_wire_names()
        if len(names) > 0:
            wire = PyMKF.find_wire_by_name(names[0])
            assert "type" in wire


class TestWireMaterials:
    """Test wire material data."""

    def test_get_wire_materials(self):
        """Wire materials should be retrievable."""
        materials = PyMKF.get_wire_materials()
        assert isinstance(materials, list)

    def test_get_wire_material_names(self):
        """Wire material names should be retrievable."""
        names = PyMKF.get_wire_material_names()
        assert isinstance(names, list)

    def test_find_wire_material_by_name(self):
        """Should be able to find wire material by name."""
        names = PyMKF.get_wire_material_names()
        if len(names) > 0:
            material = PyMKF.find_wire_material_by_name(names[0])
            assert isinstance(material, dict)


class TestBobbins:
    """Bobbin data tests."""

    def test_get_bobbins_returns_list(self):
        """Bobbins should be returned as a list."""
        bobbins = PyMKF.get_bobbins()
        assert isinstance(bobbins, list)

    def test_get_bobbin_names(self):
        """Bobbin names should be retrievable."""
        names = PyMKF.get_bobbin_names()
        assert isinstance(names, list)

    def test_find_bobbin_by_name(self):
        """Should be able to find specific bobbin by name."""
        names = PyMKF.get_bobbin_names()
        if len(names) > 0:
            bobbin = PyMKF.find_bobbin_by_name(names[0])
            assert isinstance(bobbin, dict)


class TestInsulationMaterials:
    """Test insulation material data."""

    def test_get_insulation_materials(self):
        """Insulation materials should be retrievable."""
        materials = PyMKF.get_insulation_materials()
        assert isinstance(materials, list)

    def test_get_insulation_material_names(self):
        """Insulation material names should be retrievable."""
        names = PyMKF.get_insulation_material_names()
        assert isinstance(names, list)

    def test_find_insulation_material_by_name(self):
        """Should be able to find insulation material by name."""
        names = PyMKF.get_insulation_material_names()
        if len(names) > 0:
            material = PyMKF.find_insulation_material_by_name(names[0])
            assert isinstance(material, dict)
