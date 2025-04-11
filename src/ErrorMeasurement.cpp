#include "ErrorMeasurement.hpp"
#include <cmath>
#include <array>

// Basic color distance calculations
double ErrorMeasurement::colorDistance(const RGB& c1, const RGB& c2) {
    return std::sqrt(std::pow(c1.r - c2.r, 2) +
                     std::pow(c1.g - c2.g, 2) +
                     std::pow(c1.b - c2.b, 2));
}

double ErrorMeasurement::colorDistanceSquared(const RGB& c1, const RGB& c2) {
    return std::pow(c1.r - c2.r, 2) +
           std::pow(c1.g - c2.g, 2) +
           std::pow(c1.b - c2.b, 2);
}

// Variance
double ErrorMeasurement::calculateVariance(const std::vector<std::vector<RGB>>& image,
                        int x, int y, int width, int height,
                        const RGB& averageColor) {
    int N = width * height;
    double variance = 0.0;
    for (int i = y; i < y + height; ++i) {
        for (int j = x; j < x + width; ++j) {
            double diff = colorDistanceSquared(image[i][j], averageColor);
            variance += diff * diff;
        }
    }
    return variance / (3 * N);
}

// MAD (Mean Absolute Difference)
double ErrorMeasurement::calculateMAD(const std::vector<std::vector<RGB>>& image,
                    int x, int y, int width, int height,
                    const RGB& averageColor) {
    int N = width * height;
    double variance = 0.0;
    for (int i = y; i < y + height; ++i) {
        for (int j = x; j < x + width; ++j) {
            double absValR = abs(image[i][j].r - averageColor.r);
            double absValG = abs(image[i][j].g - averageColor.g);
            double absValB = abs(image[i][j].b - averageColor.b);
            variance +=  absValR + absValG + absValB;
        }
    }
    return variance / (3 * N);
}

// Max Pixel Difference
double ErrorMeasurement::calculateMaxPixelDifference(const std::vector<std::vector<RGB>>& image,
                                  int x, int y, int width, int height,
                                  const RGB& averageColor) {
    double maxR = 0.0, maxG = 0.0, maxB = 0.0;
    double minR = 255.0, minG = 255.0, minB = 255.0;

    for (int i = y; i < y + height; ++i) {
        for (int j = x; j < x + width; ++j) {
            const RGB& pixel = image[i][j];
            maxR = std::max(maxR, static_cast<double>(pixel.r));
            maxG = std::max(maxG, static_cast<double>(pixel.g));
            maxB = std::max(maxB, static_cast<double>(pixel.b));
            minR = std::min(minR, static_cast<double>(pixel.r));
            minG = std::min(minG, static_cast<double>(pixel.g));
            minB = std::min(minB, static_cast<double>(pixel.b));
        }
    }

    double maxDifferenceR = maxR - minR;
    double maxDifferenceG = maxG - minG;
    double maxDifferenceB = maxB - minB;

    return (maxDifferenceR +maxDifferenceG +maxDifferenceB)/3;
}

// Entropy
double ErrorMeasurement::calculateEntropy(const std::vector<std::vector<RGB>>& image,
                       int x, int y, int width, int height) {
    std::array<int, 256> histR = {0};
    std::array<int, 256> histG = {0};
    std::array<int, 256> histB = {0};
    int totalPixels = width * height;

    // Fill the histograms for each channel
    for (int i = y; i < y + height; ++i) {
        for (int j = x; j < x + width; ++j) {
            const RGB& pixel = image[i][j];
            histR[pixel.r]++;
            histG[pixel.g]++;
            histB[pixel.b]++;
        }
    }

    double entropyR = 0.0;
    double entropyG = 0.0;
    double entropyB = 0.0;

    // Calculate entropy for the all three channel
    for (int count : histR) {
        if (count > 0) {
            double p = static_cast<double>(count) / totalPixels;
            entropyR -= p * std::log2(p);
        }
    }
    for (int count : histG) {
        if (count > 0) {
            double p = static_cast<double>(count) / totalPixels;
            entropyG -= p * std::log2(p);
        }
    }
    for (int count : histB) {
        if (count > 0) {
            double p = static_cast<double>(count) / totalPixels;
            entropyB -= p * std::log2(p);
        }
    }

    return (entropyR + entropyG + entropyB) / 3.0;
}

// SSIM (Structural Similarity Index)
double ErrorMeasurement::calculateSSIM(const std::vector<std::vector<RGB>>& original,
                     const std::vector<std::vector<RGB>>& compressed,
                     int x, int y, int width, int height) {
    if (y + height > original.size() || x + width > original[0].size() ||
        y + height > compressed.size() || x + width > compressed[0].size()) {
        return 0.0; // Return 0 for invalid regions (maximum error)
    }

    double meanOriginalR = 0.0, meanOriginalG = 0.0, meanOriginalB = 0.0;
    double meanCompressedR = 0.0, meanCompressedG = 0.0, meanCompressedB = 0.0;

    int N = width * height;
    if (N <= 0) return 0.0;

    for (int i = y; i < y + height; ++i) {
        for (int j = x; j < x + width; ++j) {
            const RGB& origPixel = original[i][j];
            const RGB& compPixel = compressed[i-y][j-x]; // Access compressed using relative coords

            meanOriginalR += origPixel.r;
            meanOriginalG += origPixel.g;
            meanOriginalB += origPixel.b;

            meanCompressedR += compPixel.r;
            meanCompressedG += compPixel.g;
            meanCompressedB += compPixel.b;
        }
    }
    meanOriginalR /= N;
    meanOriginalG /= N;
    meanOriginalB /= N;

    meanCompressedR /= N;
    meanCompressedG /= N;
    meanCompressedB /= N;

    double varianceOriginalR = 0.0, varianceOriginalG = 0.0, varianceOriginalB = 0.0;
    double varianceCompressedR = 0.0, varianceCompressedG = 0.0, varianceCompressedB = 0.0;
    double covarianceR = 0.0, covarianceG = 0.0, covarianceB = 0.0;

    for (int i = y; i < y + height; ++i) {
        for (int j = x; j < x + width; ++j) {
            const RGB& origPixel = original[i][j];
            const RGB& compPixel = compressed[i-y][j-x];

            varianceOriginalR += std::pow(origPixel.r - meanOriginalR, 2);
            varianceOriginalG += std::pow(origPixel.g - meanOriginalG, 2);
            varianceOriginalB += std::pow(origPixel.b - meanOriginalB, 2);

            varianceCompressedR += std::pow(compPixel.r - meanCompressedR, 2);
            varianceCompressedG += std::pow(compPixel.g - meanCompressedG, 2);
            varianceCompressedB += std::pow(compPixel.b - meanCompressedB, 2);

            covarianceR += (origPixel.r - meanOriginalR) * (compPixel.r - meanCompressedR);
            covarianceG += (origPixel.g - meanOriginalG) * (compPixel.g - meanCompressedG);
            covarianceB += (origPixel.b - meanOriginalB) * (compPixel.b - meanCompressedB);
        }
    }

    varianceOriginalR /= N;
    varianceOriginalG /= N;
    varianceOriginalB /= N;

    varianceCompressedR /= N;
    varianceCompressedG /= N;
    varianceCompressedB /= N;

    covarianceR /= N;
    covarianceG /= N;
    covarianceB /= N;

    // C1 = (K1*L)^2, C2 = (K2*L)^2 where L=255 for 8-bit images
    const double C1 = 6.5025; // (0.01 * 255)^2
    const double C2 = 58.5225; // (0.03 * 255)^2

    double ssimR = 0.0, ssimG = 0.0, ssimB = 0.0;

    double numeratorR = (2 * meanOriginalR * meanCompressedR + C1) * (2 * covarianceR + C2);
    double denominatorR = (meanOriginalR * meanOriginalR + meanCompressedR * meanCompressedR + C1) *
                          (varianceOriginalR + varianceCompressedR + C2);

    double numeratorG = (2 * meanOriginalG * meanCompressedG + C1) * (2 * covarianceG + C2);
    double denominatorG = (meanOriginalG * meanOriginalG + meanCompressedG * meanCompressedG + C1) *
                          (varianceOriginalG + varianceCompressedG + C2);

    double numeratorB = (2 * meanOriginalB * meanCompressedB + C1) * (2 * covarianceB + C2);
    double denominatorB = (meanOriginalB * meanOriginalB + meanCompressedB * meanCompressedB + C1) *
                          (varianceOriginalB + varianceCompressedB + C2);

    // Avoid division by zero
    if (denominatorR > 0.0) ssimR = numeratorR / denominatorR;
    if (denominatorG > 0.0) ssimG = numeratorG / denominatorG;
    if (denominatorB > 0.0) ssimB = numeratorB / denominatorB;

    return (ssimR + ssimG + ssimB) / 3.0;
}

// Generic error method function
double ErrorMeasurement::calculateError(const std::vector<std::vector<RGB>>& image,
                      int x, int y, int width, int height,
                      const RGB& averageColor,
                      const std::string& method) {
    if (method == "VAR") {
        return calculateVariance(image, x, y, width, height, averageColor);
    } else if (method == "MAD") {
        return calculateMAD(image, x, y, width, height, averageColor);
    } else if (method == "MPD") {
        return calculateMaxPixelDifference(image, x, y, width, height, averageColor);
    } else if (method == "ENT") {
        return calculateEntropy(image, x, y, width, height);
    } else if (method == "SSIM") {
        std::vector<std::vector<RGB>> compressedRegion(height, std::vector<RGB>(width));

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                compressedRegion[i][j] = averageColor;
            }
        }

        double ssim = calculateSSIM(image, compressedRegion, x, y, width, height);
        return 1.0 - ssim;
    } else {
        throw std::invalid_argument("Unsupported error calculation method: " + method);
    }
}

bool ErrorMeasurement::isErrorBelowThreshold(double error, double threshold, const std::string& method) {
    if (method == "SSIM") {
        return error <= (1.0 - threshold);
    } else {
        return error <= threshold;
    }
}

RGB ErrorMeasurement::calculateAverageColor(const std::vector<std::vector<RGB>>& image,
                          int x, int y, int width, int height) {
    RGB avgColor = {0, 0, 0};
    int totalPixels = width * height;

    double sumR = 0, sumG = 0, sumB = 0;

    for (int i = y; i < y + height; ++i) {
        for (int j = x; j < x + width; ++j) {
            sumR += image[i][j].r;
            sumG += image[i][j].g;
            sumB += image[i][j].b;
        }
    }

    // Convert back to uint8_t with proper rounding
    avgColor.r = static_cast<uint8_t>(sumR / totalPixels + 0.5);
    avgColor.g = static_cast<uint8_t>(sumG / totalPixels + 0.5);
    avgColor.b = static_cast<uint8_t>(sumB / totalPixels + 0.5);

    return avgColor;
}

// Calculate if a region should be subdivided based on error threshold
bool ErrorMeasurement::shouldSubdivide(const std::vector<std::vector<RGB>>& image,
                     int x, int y, int width, int height,
                     double threshold,
                     const std::string& method) {
    RGB avgColor = calculateAverageColor(image, x, y, width, height);
    double error = calculateError(image, x, y, width, height, avgColor, method);
    return !isErrorBelowThreshold(error, threshold, method);
}