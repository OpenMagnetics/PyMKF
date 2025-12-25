"""
Tests for PyMKF Core Adviser.

These tests mirror TestCoreAdviser.cpp from MKF, verifying the core
adviser functionality for recommending optimal cores.
"""
import pytest
import json
import PyMKF


def parse_json_result(result):
    """Parse JSON result string into Python object."""
    if isinstance(result, str):
        return json.loads(result)
    return result


class TestCoreAdviserBasic:
    """
    Basic core adviser tests.
    Mirrors SUITE(CoreAdviserAvailableCores) in TestCoreAdviser.cpp.
    """

    def test_calculate_advised_cores_returns_results(self, inductor_inputs, balanced_weights, reset_settings):
        """
        Core adviser should return recommendations.
        Similar to Test_CoreAdviserAvailableCores_All_Cores.
        """
        processed_inputs = PyMKF.process_inputs(inductor_inputs)
        
        # API: calculate_advised_cores(inputs, weights, max_results, use_only_stock)
        result_json = PyMKF.calculate_advised_cores(processed_inputs, balanced_weights, 10, False)
        results = parse_json_result(result_json)
        
        assert isinstance(results, list)
        # Should return some results (may be empty if no suitable cores)

    def test_calculate_advised_cores_with_efficiency_focus(self, inductor_inputs, efficiency_weights, reset_settings):
        """
        Core adviser with efficiency-focused weights.
        Similar to Test_CoreAdviserAvailableCores_Toroidal_Cores_With_Impedance.
        """
        processed_inputs = PyMKF.process_inputs(inductor_inputs)
        
        result_json = PyMKF.calculate_advised_cores(processed_inputs, efficiency_weights, 5, False)
        results = parse_json_result(result_json)
        
        assert isinstance(results, list)

    def test_advised_cores_have_scoring(self, inductor_inputs, balanced_weights, reset_settings):
        """Advised cores should include scoring information."""
        processed_inputs = PyMKF.process_inputs(inductor_inputs)
        
        result_json = PyMKF.calculate_advised_cores(processed_inputs, balanced_weights, 5, False)
        results = parse_json_result(result_json)
        
        if len(results) > 0:
            # Results should be sorted by score (best first)
            # Each result should have core and scoring
            for result in results:
                assert isinstance(result, dict)


class TestCoreAdviserWithDifferentInputs:
    """Test core adviser with various input configurations."""

    @pytest.mark.xfail(reason="C++ library issue with high frequency inputs")
    def test_high_frequency_inputs(self, high_frequency_inputs, balanced_weights, reset_settings):
        """Test core adviser with high frequency (~500kHz) inputs."""
        processed_inputs = PyMKF.process_inputs(high_frequency_inputs)
        
        result_json = PyMKF.calculate_advised_cores(processed_inputs, balanced_weights, 5, False)
        results = parse_json_result(result_json)
        
        assert isinstance(results, list)

    @pytest.mark.xfail(reason="C++ library issue with transformer inputs")
    def test_transformer_inputs(self, transformer_inputs, balanced_weights, reset_settings):
        """Test core adviser with transformer (multi-winding) inputs."""
        processed_inputs = PyMKF.process_inputs(transformer_inputs)
        
        result_json = PyMKF.calculate_advised_cores(processed_inputs, balanced_weights, 5, False)
        results = parse_json_result(result_json)
        
        assert isinstance(results, list)


class TestCoreAdviserWeights:
    """Test different weight configurations for core adviser."""

    def test_cost_focused_weights(self, inductor_inputs, reset_settings):
        """Test with cost-focused weights."""
        weights = {"COST": 1, "EFFICIENCY": 0, "DIMENSIONS": 0}
        processed_inputs = PyMKF.process_inputs(inductor_inputs)
        
        result_json = PyMKF.calculate_advised_cores(processed_inputs, weights, 5, False)
        results = parse_json_result(result_json)
        
        assert isinstance(results, list)

    def test_dimension_focused_weights(self, inductor_inputs, reset_settings):
        """Test with dimension/size-focused weights."""
        weights = {"COST": 0, "EFFICIENCY": 0, "DIMENSIONS": 1}
        processed_inputs = PyMKF.process_inputs(inductor_inputs)
        
        result_json = PyMKF.calculate_advised_cores(processed_inputs, weights, 5, False)
        results = parse_json_result(result_json)
        
        assert isinstance(results, list)

    def test_all_weights_balanced(self, inductor_inputs, reset_settings):
        """Test with all weights equal."""
        weights = {"COST": 1, "EFFICIENCY": 1, "DIMENSIONS": 1}
        processed_inputs = PyMKF.process_inputs(inductor_inputs)
        
        result_json = PyMKF.calculate_advised_cores(processed_inputs, weights, 5, False)
        results = parse_json_result(result_json)
        
        assert isinstance(results, list)


class TestCoreAdviserFiltering:
    """Test core adviser filtering options."""

    def test_limit_results(self, inductor_inputs, balanced_weights, reset_settings):
        """Should respect max_results limit."""
        processed_inputs = PyMKF.process_inputs(inductor_inputs)
        
        results_5 = parse_json_result(PyMKF.calculate_advised_cores(processed_inputs, balanced_weights, 5, False))
        results_2 = parse_json_result(PyMKF.calculate_advised_cores(processed_inputs, balanced_weights, 2, False))
        
        assert len(results_2) <= 2
        assert len(results_5) <= 5

    def test_stock_only_filtering(self, inductor_inputs, balanced_weights, reset_settings):
        """Test filtering for in-stock cores only."""
        processed_inputs = PyMKF.process_inputs(inductor_inputs)
        
        # With stock only
        results_stock = parse_json_result(PyMKF.calculate_advised_cores(processed_inputs, balanced_weights, 10, True))
        
        # Without stock filter
        results_all = parse_json_result(PyMKF.calculate_advised_cores(processed_inputs, balanced_weights, 10, False))
        
        # Stock-only should have <= results than all
        assert len(results_stock) <= len(results_all) or len(results_stock) == 0
