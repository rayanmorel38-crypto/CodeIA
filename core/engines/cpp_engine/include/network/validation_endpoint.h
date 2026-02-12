#ifndef CPP_ENGINE_VALIDATION_ENDPOINT_H
#define CPP_ENGINE_VALIDATION_ENDPOINT_H

#include <string>
#include <nlohmann/json.hpp>
#include "sandbox/comparator.h"

using json = nlohmann::json;

namespace cppengine {
namespace network {

/**
 * HTTP Endpoint for sandbox validation
 * Provides POST /validate and POST /validate/batch endpoints
 */
class ValidationEndpoint {
public:
    ValidationEndpoint();
    ~ValidationEndpoint();
    
    /**
     * Handle validation request
     * @param request JSON body with validation parameters
     * @return JSON response with validation result
     */
    json handle_validate_request(const json& request);
    
    /**
     * Handle batch validation request
     * @param request JSON body with array of validation tasks
     * @return JSON response with array of results
     */
    json handle_batch_validate_request(const json& request);
    
private:
    sandbox::SandboxComparator comparator_;
    
    json validate_filter(const json& params);
    json validate_effect(const json& params);
    json validate_video(const json& params);
    json validate_images(const json& params);
};

}  // namespace network
}  // namespace cppengine

#endif // CPP_ENGINE_VALIDATION_ENDPOINT_H
