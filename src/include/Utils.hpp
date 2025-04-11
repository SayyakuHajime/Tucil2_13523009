#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <cstdint>
#include "QuadTree.hpp"

namespace Utils {
    // File operations
    bool fileExists(const std::string& filename);
    std::string getFileExtension(const std::string& filename);
    std::string normalizePath(const std::string& path);
    std::string getDirectoryPath(const std::string& filePath);
    std::string getFileNameFromPath(const std::string& filePath);
    bool createDirectory(const std::string& path);

    // Program options structure
    struct ProgramOptions {
        std::string inputFile;
        std::string outputFile;
        std::string gifFile;
        static std::string errorMethod;
        static double threshold;
        static int minBlockSize;
        static double targetCompressionRatio;
        bool generateGif = false;
    };

    ProgramOptions parseCommandLineArgs(int argc, char* argv[]);

    // Color conversion utilities
    RGB hexToRGB(const std::string& hexColor);
    std::string RGBToHex(const RGB& color);

    // Math utilities
    int clamp(int value, int min, int max);

    // Template function for clamping any numeric type
    template<typename T>
    T clampT(T value, T min, T max);

}
#endif // UTILS_HPP