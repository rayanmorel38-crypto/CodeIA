#include "network/validation_endpoint.h"
#include <iostream>

namespace cppengine {
namespace network {

ValidationEndpoint::ValidationEndpoint() {
    comparator_.set_validation_mode(sandbox::SandboxComparator::ValidationMode::FUZZY);
}

ValidationEndpoint::~ValidationEndpoint() = default;

json ValidationEndpoint::handle_validate_request(const json& request) {
    json response;
    
    try {
        std::string validation_type = request.value("type", "");
        
        if (validation_type == "filter") {
            return validate_filter(request);
        } else if (validation_type == "effect") {
            return validate_effect(request);
        } else if (validation_type == "video") {
            return validate_video(request);
        } else if (validation_type == "images") {
            return validate_images(request);
        } else {
            response["error"] = "Unknown validation type: " + validation_type;
            response["success"] = false;
        }
    } catch (const std::exception& e) {
        response["error"] = e.what();
        response["success"] = false;
    }
    
    return response;
}

json ValidationEndpoint::handle_batch_validate_request(const json& request) {
    json response;
    json results = json::array();
    
    try {
        if (!request.contains("validations") || !request["validations"].is_array()) {
            response["error"] = "Missing or invalid 'validations' array";
            response["success"] = false;
            return response;
        }
        
        for (const auto& validation : request["validations"]) {
            json result = handle_validate_request(validation);
            results.push_back(result);
        }
        
        response["success"] = true;
        response["results"] = results;
        response["count"] = results.size();
        
    } catch (const std::exception& e) {
        response["error"] = e.what();
        response["success"] = false;
    }
    
    return response;
}

json ValidationEndpoint::validate_filter(const json& params) {
    json response;
    
    try {
        std::string filter_type = params.value("filter_type", "");
        std::string input_file = params.value("input_file", "");
        std::string output_file = params.value("output_file", "");
        
        if (filter_type.empty() || input_file.empty() || output_file.empty()) {
            response["error"] = "Missing required parameters: filter_type, input_file, output_file";
            response["success"] = false;
            return response;
        }
        
        auto result = comparator_.validate_filter_output(filter_type, input_file, output_file);
        
        response["success"] = result.matches;
        response["validation_type"] = "filter";
        response["filter_type"] = filter_type;
        response["similarity"] = result.similarity;
        response["matches"] = result.matches;
        response["report"] = result.diff_report;
        response["warnings"] = result.warnings;
        response["errors"] = result.errors;
        
    } catch (const std::exception& e) {
        response["error"] = e.what();
        response["success"] = false;
    }
    
    return response;
}

json ValidationEndpoint::validate_effect(const json& params) {
    json response;
    
    try {
        std::string effect_type = params.value("effect_type", "");
        std::string input_file = params.value("input_file", "");
        std::string output_file = params.value("output_file", "");
        
        if (effect_type.empty() || input_file.empty() || output_file.empty()) {
            response["error"] = "Missing required parameters: effect_type, input_file, output_file";
            response["success"] = false;
            return response;
        }
        
        auto result = comparator_.validate_effect_output(effect_type, input_file, output_file);
        
        response["success"] = result.matches;
        response["validation_type"] = "effect";
        response["effect_type"] = effect_type;
        response["similarity"] = result.similarity;
        response["matches"] = result.matches;
        response["report"] = result.diff_report;
        response["warnings"] = result.warnings;
        response["errors"] = result.errors;
        
    } catch (const std::exception& e) {
        response["error"] = e.what();
        response["success"] = false;
    }
    
    return response;
}

json ValidationEndpoint::validate_video(const json& params) {
    json response;
    
    try {
        std::string input_file = params.value("input_file", "");
        std::string output_file = params.value("output_file", "");
        json expected_config = params.value("expected_config", json::object());
        
        if (input_file.empty() || output_file.empty()) {
            response["error"] = "Missing required parameters: input_file, output_file";
            response["success"] = false;
            return response;
        }
        
        auto result = comparator_.validate_video_output(input_file, output_file, expected_config);
        
        response["success"] = result.matches;
        response["validation_type"] = "video";
        response["similarity"] = result.similarity;
        response["matches"] = result.matches;
        response["report"] = result.diff_report;
        response["warnings"] = result.warnings;
        response["errors"] = result.errors;
        
    } catch (const std::exception& e) {
        response["error"] = e.what();
        response["success"] = false;
    }
    
    return response;
}

json ValidationEndpoint::validate_images(const json& params) {
    json response;
    
    try {
        std::string expected_path = params.value("expected_path", "");
        std::string actual_path = params.value("actual_path", "");
        bool perceptual = params.value("perceptual", false);
        
        if (expected_path.empty() || actual_path.empty()) {
            response["error"] = "Missing required parameters: expected_path, actual_path";
            response["success"] = false;
            return response;
        }
        
        auto result = comparator_.compare_images(expected_path, actual_path, perceptual);
        
        response["success"] = result.matches;
        response["validation_type"] = "image_comparison";
        response["similarity"] = result.similarity;
        response["matches"] = result.matches;
        response["report"] = result.diff_report;
        response["warnings"] = result.warnings;
        response["errors"] = result.errors;
        
    } catch (const std::exception& e) {
        response["error"] = e.what();
        response["success"] = false;
    }
    
    return response;
}

}  // namespace network
}  // namespace cppengine
