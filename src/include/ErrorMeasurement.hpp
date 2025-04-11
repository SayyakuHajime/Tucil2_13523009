#ifndef ERROR_MEASUREMENT_HPP
#define ERROR_MEASUREMENT_HPP

#include <vector>
#include <string>
#include "QuadTree.hpp"

namespace ErrorMeasurement {
    // Basic color distance calculations
    double colorDistance(const RGB& c1, const RGB& c2);
    double colorDistanceSquared(const RGB& c1, const RGB& c2);

    // Variance
    double calculateVariance(const std::vector<std::vector<RGB>>& image,
                            int x, int y, int width, int height,
                            const RGB& averageColor);

    // MAD (Mean Absolute Difference)
    double calculateMAD(const std::vector<std::vector<RGB>>& image,
                        int x, int y, int width, int height,
                        const RGB& averageColor);

    // Max Pixel Difference
    double calculateMaxPixelDifference(const std::vector<std::vector<RGB>>& image,
                                      int x, int y, int width, int height,
                                      const RGB& averageColor);

    // Entropy
    double calculateEntropy(const std::vector<std::vector<RGB>>& image,
                           int x, int y, int width, int height);

    // SSIM (Structural Similarity Index)
    double calculateSSIM(const std::vector<std::vector<RGB>>& original,
                         const std::vector<std::vector<RGB>>& compressed,
                         int x, int y, int width, int height);


    // Generic error calculation function that uses the specified method
    double calculateError(const std::vector<std::vector<RGB>>& image,
                          int x, int y, int width, int height,
                          const RGB& averageColor,
                          const std::string& method = "VAR");

    // Determine if error is below threshold
    bool isErrorBelowThreshold(double error, double threshold, const std::string& method = "VAR");

    // Calculate average color of a region
    RGB calculateAverageColor(const std::vector<std::vector<RGB>>& image,
                              int x, int y, int width, int height);

    // Calculate if a region should be subdivided based on error threshold
    bool shouldSubdivide(const std::vector<std::vector<RGB>>& image,
                         int x, int y, int width, int height,
                         double threshold,
                         const std::string& method = "VAR");
};

#endif // ERROR_MEASUREMENT_HPP