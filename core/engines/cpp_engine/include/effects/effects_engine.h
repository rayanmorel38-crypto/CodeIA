#ifndef CPP_ENGINE_EFFECTS_EFFECTS_ENGINE_H
#define CPP_ENGINE_EFFECTS_EFFECTS_ENGINE_H

#include <string>
#include <vector>

namespace cppengine {
namespace effects {

/**
 * EffectsEngine - Advanced visual effects
 * Supports: Lighting, shadows, particles, distortions, chromatic aberration
 */
class EffectsEngine {
public:
    EffectsEngine();
    ~EffectsEngine();
    
    // Lighting effects
    bool apply_lighting(const std::string& input_file, const std::string& output_file,
                      float light_x, float light_y, float light_z);
    
    // Shadow rendering
    bool apply_shadows(const std::string& input_file, const std::string& output_file,
                     float shadow_intensity);
    
    // Particle effects
    bool add_particles(const std::string& input_file, const std::string& output_file,
                      int particle_count, const std::string& particle_type);
    
    // Distortion effects
    bool apply_wave_distortion(const std::string& input_file, const std::string& output_file,
                             float amplitude, float frequency);
    
    bool apply_radial_distortion(const std::string& input_file, const std::string& output_file,
                                float distortion_factor);
    
    // Color effects
    bool apply_chromatic_aberration(const std::string& input_file, const std::string& output_file,
                                   float red_shift, float blue_shift);
    
    bool apply_bloom(const std::string& input_file, const std::string& output_file,
                    float threshold, float intensity);
    
private:
    int effect_quality_;
};

} // namespace effects
} // namespace cppengine

#endif // CPP_ENGINE_EFFECTS_EFFECTS_ENGINE_H
