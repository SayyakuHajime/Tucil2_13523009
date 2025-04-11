#include "ImageProcessor.hpp"
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include "Utils.hpp"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


std::vector<std::vector<RGB>> ImageProcessor::loadImage(const std::string& filename) {

    std::string normalizedPath = Utils::normalizePath(filename);

    int width, height, channels;

    unsigned char* data = stbi_load(normalizedPath.c_str(), &width, &height, &channels, 0);
    if (!data) {
        std::cerr << "Error: Could not load image file: " << normalizedPath << std::endl;
        std::cerr << "Make sure the file exists and the path is correct." << std::endl;
        throw std::runtime_error("Failed to load image: " + normalizedPath);
    }

    std::vector<std::vector<RGB>> image(height, std::vector<RGB>(width));

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = (y * width + x) * channels;
            // Handle grayscale (1 channel) or RGB (3+ channels)
            image[y][x].r = data[index];
            image[y][x].g = channels > 1 ? data[index + 1] : data[index];
            image[y][x].b = channels > 2 ? data[index + 2] : data[index];
        }
    }

    stbi_image_free(data);

    std::cout << "Successfully loaded image: " << normalizedPath << std::endl;
    std::cout << "Image dimensions: " << width << "x" << height << " with " << channels << " channels" << std::endl;

    return image;
}

bool ImageProcessor::saveImage(const std::vector<std::vector<RGB>>& image, const std::string& filename) {
    if (image.empty() || image[0].empty()) {
        std::cerr << "Error: Cannot save empty image" << std::endl;
        return false;
    }

    // Normalize the path
    std::string normalizedPath = Utils::normalizePath(filename);

    // Ensure the directory exists
    std::string dirPath = Utils::getDirectoryPath(normalizedPath);
    if (!dirPath.empty() && !Utils::fileExists(dirPath)) {
        std::cout << "Creating directory: " << dirPath << std::endl;
        if (!Utils::createDirectory(dirPath)) {
            std::cerr << "Error: Failed to create directory: " << dirPath << std::endl;
            return false;
        }
    }

    int height = static_cast<int>(image.size());
    int width = static_cast<int>(image[0].size());

    // Create a buffer for pixel data (RGB format)
    unsigned char* data = new unsigned char[width * height * 3];

    // Fill the buffer with RGB values
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = (y * width + x) * 3;
            data[index] = static_cast<unsigned char>(image[y][x].r);
            data[index + 1] = static_cast<unsigned char>(image[y][x].g);
            data[index + 2] = static_cast<unsigned char>(image[y][x].b);
        }
    }

    // Get file extension
    std::string extension = Utils::getFileExtension(normalizedPath);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

    bool success = false;
    if (extension == "png") {
        success = stbi_write_png(normalizedPath.c_str(), width, height, 3, data, width * 3) != 0;
    } else if (extension == "jpg" || extension == "jpeg") {
        success = stbi_write_jpg(normalizedPath.c_str(), width, height, 3, data, 90) != 0;
    } else {
        // Default to PNG if extension is unsupported
        std::string newPath = normalizedPath + ".png";
        std::cout << "Unsupported extension, saving as PNG: " << newPath << std::endl;
        success = stbi_write_png(newPath.c_str(), width, height, 3, data, width * 3) != 0;
    }

    delete[] data;

    if (success) {
        std::cout << "Successfully saved image to: " << normalizedPath << std::endl;
    } else {
        std::cerr << "Error: Failed to save image to: " << normalizedPath << std::endl;
    }

    return success;
}

// Extract a region from an image
std::vector<std::vector<RGB>> ImageProcessor::extractRegion(
    const std::vector<std::vector<RGB>>& image,
    int x, int y, int width, int height) {

    // Check if image is valid
    if (image.empty() || image[0].empty()) {
        return std::vector<std::vector<RGB>>();
    }

    // Ensure the region is within image bounds
    x = std::max(0, x);
    y = std::max(0, y);
    width = std::min(width, static_cast<int>(image[0].size()) - x);
    height = std::min(height, static_cast<int>(image.size()) - y);

    // Return empty region if dimensions are invalid
    if (width <= 0 || height <= 0) {
        return std::vector<std::vector<RGB>>();
    }

    // Create the region and copy pixel data
    std::vector<std::vector<RGB>> region(height, std::vector<RGB>(width));
    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
            region[j][i] = image[y + j][x + i];
        }
    }

    return region;
}

// Apply a color to a region of an image
void ImageProcessor::applyColorToRegion(
    std::vector<std::vector<RGB>>& image,
    int x, int y, int width, int height,
    const RGB& color) {

    // Check if image is valid
    if (image.empty() || image[0].empty()) {
        return;
    }

    // Ensure the region is within image bounds
    x = std::max(0, x);
    y = std::max(0, y);
    width = std::min(width, static_cast<int>(image[0].size()) - x);
    height = std::min(height, static_cast<int>(image.size()) - y);

    // Do nothing if dimensions are invalid
    if (width <= 0 || height <= 0) {
        return;
    }

    // Apply the color to the region
    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
            image[y + j][x + i] = color;
        }
    }
}

// Get the color of a pixel
RGB ImageProcessor::getPixel(const std::vector<std::vector<RGB>>& image, int x, int y) {
    // Check if coordinates are within bounds
    if (x < 0 || y < 0 ||
        y >= static_cast<int>(image.size()) ||
        x >= static_cast<int>(image[0].size())) {
        // Return black for out-of-bounds pixels
        return {0, 0, 0};
    }

    return image[y][x];
}

// Set the color of a pixel
void ImageProcessor::setPixel(std::vector<std::vector<RGB>>& image, int x, int y, const RGB& color) {
    // Check if coordinates are within bounds
    if (x < 0 || y < 0 ||
        y >= static_cast<int>(image.size()) ||
        x >= static_cast<int>(image[0].size())) {
        // Ignore out-of-bounds pixels
        return;
    }

    image[y][x] = color;
}