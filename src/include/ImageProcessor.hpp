#ifndef IMAGE_PROCESSOR_HPP
#define IMAGE_PROCESSOR_HPP

#include <vector>
#include <string>
#include "QuadTree.hpp"

class ImageProcessor {
public:
    // Image loading and saving
    static std::vector<std::vector<RGB>> loadImage(const std::string& filename);
    static bool saveImage(const std::vector<std::vector<RGB>>& image, const std::string& filename);

    // Image conversion and manipulation
    static std::vector<std::vector<RGB>> extractRegion(
        const std::vector<std::vector<RGB>>& image,
        int x, int y, int width, int height);

    static void applyColorToRegion(
        std::vector<std::vector<RGB>>& image,
        int x, int y, int width, int height,
        const RGB& color);

    // Utility functions for pixel operations
    static RGB getPixel(const std::vector<std::vector<RGB>>& image, int x, int y);
    static void setPixel(std::vector<std::vector<RGB>>& image, int x, int y, const RGB& color);
};

#endif // IMAGE_PROCESSOR_HPP