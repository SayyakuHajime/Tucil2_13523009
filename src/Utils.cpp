#include "Utils.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <iostream>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(dir) _mkdir(dir)
#else
#include <unistd.h>
#define MKDIR(dir) mkdir(dir, 0755)
#endif

namespace Utils {
    std::string ProgramOptions::errorMethod = "VAR"; // Replace 0 with the desired default value
    int ProgramOptions::minBlockSize = 1;
    double ProgramOptions::threshold = 100.0;
    double ProgramOptions::targetCompressionRatio = 0.0;

    // File operations
    bool fileExists(const std::string& filename) {
        struct stat buffer;
        return (stat(filename.c_str(), &buffer) == 0);
    }

    std::string getFileExtension(const std::string& filename) {
        size_t dotPos = filename.find_last_of('.');
        if (dotPos == std::string::npos) {
            return ""; // No extension found
        }
        return filename.substr(dotPos + 1);
    }

    std::string normalizePath(const std::string& path) {
        std::string normalized = path;
        std::replace(normalized.begin(), normalized.end(), '\\', '/');
        return normalized;
    }

    std::string getDirectoryPath(const std::string& filePath) {
        std::string normalized = normalizePath(filePath);
        size_t lastSlash = normalized.find_last_of('/');
        if (lastSlash != std::string::npos) {
            return normalized.substr(0, lastSlash);
        }
        return "";
    }

    std::string getFileNameFromPath(const std::string& filePath) {
        std::string normalized = normalizePath(filePath);
        size_t lastSlash = normalized.find_last_of('/');
        if (lastSlash != std::string::npos) {
            return normalized.substr(lastSlash + 1);
        }
        return normalized;
    }

    bool createDirectory(const std::string& path) {
        std::string normalizedPath = normalizePath(path);

        // Skip if empty
        if (normalizedPath.empty()) {
            return true;
        }

        // Check if directory already exists
        if (fileExists(normalizedPath)) {
            struct stat buffer;
            stat(normalizedPath.c_str(), &buffer);
            return (buffer.st_mode & S_IFDIR) != 0;
        }

        // Create parent directories first
        size_t pos = normalizedPath.find_last_of('/');
        if (pos != std::string::npos) {
            std::string parentDir = normalizedPath.substr(0, pos);
            if (!parentDir.empty() && !createDirectory(parentDir)) {
                return false;
            }
        }

        // Create the directory
        int result = MKDIR(normalizedPath.c_str());
        return (result == 0 || errno == EEXIST);
    }

    ProgramOptions parseCommandLineArgs(int argc, char* argv[]) {
        ProgramOptions options;

        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--input" && i + 1 < argc) {
                options.inputFile = normalizePath(argv[++i]);
            } else if (arg == "--output" && i + 1 < argc) {
                options.outputFile = normalizePath(argv[++i]);
            } else if (arg == "--gif" && i + 1 < argc) {
                options.gifFile = normalizePath(argv[++i]);
            } else if (arg == "--error-method" && i + 1 < argc) {
                options.errorMethod = argv[++i];
            } else if (arg == "--threshold" && i + 1 < argc) {
                options.threshold = std::stod(argv[++i]);
            } else if (arg == "--min-block" && i + 1 < argc) {
                options.minBlockSize = std::stoi(argv[++i]);
            } else if (arg == "--target-ratio" && i + 1 < argc) {
                options.targetCompressionRatio = std::stod(argv[++i]);
            } else if (arg == "--generate-gif") {
                options.generateGif = true;
            }
        }

        return options;
    }

    // Color conversion utilities
    RGB hexToRGB(const std::string& hexColor) {
        RGB color = {0, 0, 0};
        if (hexColor.size() == 7 && hexColor[0] == '#') {
            color.r = std::stoi(hexColor.substr(1, 2), nullptr, 16);
            color.g = std::stoi(hexColor.substr(3, 2), nullptr, 16);
            color.b = std::stoi(hexColor.substr(5, 2), nullptr, 16);
        }
        return color;
    }

    std::string RGBToHex(const RGB& color) {
        std::ostringstream oss;
        oss << "#" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(color.r)
            << std::setw(2) << std::setfill('0') << static_cast<int>(color.g)
            << std::setw(2) << std::setfill('0') << static_cast<int>(color.b);
        return oss.str();
    }

    // Math utilities
    int clamp(int value, int min, int max) {
        if (value < min) return min;
        if (value > max) return max;
        return value;
    }

    // Template function implementation for clamping any numeric type
    template<typename T>
    T clampT(T value, T min, T max) {
        if (value < min) return min;
        if (value > max) return max;
        return value;
    }

    // Explicit instantiation of the template for uint8_t
    template uint8_t clampT<uint8_t>(uint8_t value, uint8_t min, uint8_t max);

}