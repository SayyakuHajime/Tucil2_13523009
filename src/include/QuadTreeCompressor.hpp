#ifndef QUADTREE_COMPRESSOR_HPP
#define QUADTREE_COMPRESSOR_HPP

#include <string>
#include <vector>
#include "QuadTree.hpp"
#include "ErrorMeasurement.hpp"

class QuadTreeCompressor {
public:
    // Constructor
    QuadTreeCompressor();

    // Main compression function
    QuadTree compressImage(
        const std::vector<std::vector<RGB>>& image,
        double threshold,
        const std::string& errorMethod = "VAR");

    // Compression with target ratio
    QuadTree compressImageWithTargetRatio(
        const std::vector<std::vector<RGB>>& image,
        double targetRatio,
        const std::string& errorMethod = "VAR",
        int maxIterations = 10);

    // Compression statistics
    double getCompressionRatio() const;
    int getOriginalSize() const;
    int getCompressedSize() const;

private:
    int originalSize;
    int compressedSize;
};

#endif // QUADTREE_COMPRESSOR_HPP