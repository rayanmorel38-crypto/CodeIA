#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

// Simple WAV writer (PCM 16-bit little endian)
static void write_wav_pcm16(const std::string &path, const std::vector<int16_t> &pcm, int sample_rate, int channels) {
    fs::create_directories(fs::path(path).parent_path());
    std::ofstream out(path, std::ios::binary);
    if (!out) throw std::runtime_error("Failed to open " + path);

    int32_t subchunk2_size = static_cast<int32_t>(pcm.size() * sizeof(int16_t));
    int32_t chunk_size = 36 + subchunk2_size;

    // RIFF header
    out.write("RIFF", 4);
    out.write(reinterpret_cast<const char*>(&chunk_size), 4);
    out.write("WAVE", 4);

    // fmt subchunk
    out.write("fmt ", 4);
    int32_t subchunk1_size = 16;
    int16_t audio_format = 1; // PCM
    out.write(reinterpret_cast<const char*>(&subchunk1_size), 4);
    out.write(reinterpret_cast<const char*>(&audio_format), 2);
    out.write(reinterpret_cast<const char*>(&channels), 2);
    out.write(reinterpret_cast<const char*>(&sample_rate), 4);
    int32_t byte_rate = sample_rate * channels * sizeof(int16_t);
    out.write(reinterpret_cast<const char*>(&byte_rate), 4);
    int16_t block_align = channels * sizeof(int16_t);
    out.write(reinterpret_cast<const char*>(&block_align), 2);
    int16_t bits_per_sample = 16;
    out.write(reinterpret_cast<const char*>(&bits_per_sample), 2);

    // data subchunk
    out.write("data", 4);
    out.write(reinterpret_cast<const char*>(&subchunk2_size), 4);
    out.write(reinterpret_cast<const char*>(pcm.data()), subchunk2_size);
}

namespace cpp_engine { namespace modules { namespace generators {

class AudioGenerator {
public:
    static std::string name() { return "audio_generator"; }

    // Generates a short test tone WAV and returns the path
    static std::string generate_wav(const std::string &output_path, double duration_s = 2.0, int sample_rate = 22050, double freq = 440.0) {
        int channels = 1;
        int total_samples = static_cast<int>(duration_s * sample_rate);
        std::vector<int16_t> pcm;
        pcm.reserve(total_samples);

        double amplitude = 0.5 * 32767.0;
        for (int i = 0; i < total_samples; ++i) {
            double t = static_cast<double>(i) / sample_rate;
            double sample = amplitude * std::sin(2.0 * M_PI * freq * t);
            pcm.push_back(static_cast<int16_t>(sample));
        }

        write_wav_pcm16(output_path, pcm, sample_rate, channels);
        std::cout << "[AudioGenerator] Saved WAV to: " << output_path << std::endl;
        return output_path;
    }
};

} } }

extern "C" void register_module() {
    // create a tiny example file in ./data/generated_audio
    cpp_engine::modules::generators::AudioGenerator::generate_wav("data/generated_audio/example_tone.wav", 1.0, 22050, 440.0);
}
