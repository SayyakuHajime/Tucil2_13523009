#include "QuadTreeCompressor.hpp"
#include <iostream>

#include "Utils.hpp"

// Constructor
QuadTreeCompressor::QuadTreeCompressor()
    : originalSize(0), compressedSize(0) {
}

// Main compression function
QuadTree QuadTreeCompressor::compressImage(
    const std::vector<std::vector<RGB>>& image,
    double threshold,
    const std::string& errorMethod) {

    if (image.empty() || image[0].empty()) {
        throw std::invalid_argument("Empty image provided for compression");
    }

    // Calculate the original size (3 bytes per pixel for RGB)
    int width = image[0].size();
    int height = image.size();
    originalSize = width * height * 3;

    // Note: This is the theoretical memory size, not the file size
    // Actual file size depends on the image format (JPEG, PNG, etc.)
    // which already includes compression

    QuadTree tree(width, height);

    tree.buildFromImage(image, threshold);

    int leafNodes = tree.countLeafNodes();
    compressedSize = leafNodes * (4*sizeof(int) + 3);

    return tree;
}

double QuadTreeCompressor::getCompressionRatio() const {
    if (originalSize == 0) return 0.0;
    return 1.0 - (static_cast<double>(compressedSize) / originalSize);
}

int QuadTreeCompressor::getOriginalSize() const {
    return originalSize;
}

int QuadTreeCompressor::getCompressedSize() const {
    return compressedSize;
}