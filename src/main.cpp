#include <iostream>
#include <chrono>
#include <string>
#include <vector>
#include <iomanip>
#include <cstdint>
#include <cmath>
#include <fstream>

#include "QuadTree.hpp"
#include "ImageProcessor.hpp"
#include "QuadTreeCompressor.hpp"
#include "Utils.hpp"
// #include "GifGenerator.hpp"

// Helper function to display program usage
void showUsage(const char* programName) {
    std::cout << "QuadTree Image Compression\n";
    std::cout << "==========================\n\n";
    std::cout << "Usage: " << programName << " [options]\n\n";
    std::cout << "Options:\n";
    std::cout << "  --input <file>          : Input image file path\n";
    std::cout << "  --error-method <method> : Error measurement method (VAR, MAD, MPD, ENT, SSIM)\n";
    std::cout << "  --threshold <value>     : Error threshold value\n";
    std::cout << "  --min-block <size>      : Minimum block size\n";
    std::cout << "  --target-ratio <ratio>  : Target compression ratio (0.0-1.0, 0 to disable)\n";
    std::cout << "  --output <file>         : Output compressed image file path\n";
    std::cout << "  --gif <file>            : Output GIF animation file path (optional)\n";
    std::cout << "  --help                  : Display this help message\n\n";
    std::cout << "Example:\n";
    std::cout << "  " << programName << " --input image.jpg --error-method VAR --threshold 10.0 --min-block 4 --output output.jpg\n";
}

// calculate maximum depth of the quadtree
int calculateTreeDepth(const QuadTreeNode* node, int currentDepth = 0) {
    if (!node) return currentDepth - 1;

    if (node->isNodeLeaf()) {
        return currentDepth;
    }

    int maxDepth = currentDepth;
    for (int i = 0; i < 4; ++i) {
        int childDepth = calculateTreeDepth(node->getChildAt(i), currentDepth + 1);
        maxDepth = std::max(maxDepth, childDepth);
    }

    return maxDepth;
}

// Function to adjust threshold to achieve target compression ratio (not finished)
double adjustThresholdForTargetRatio(
    const std::vector<std::vector<RGB>>& image,
    double targetRatio,
    double initialThreshold,
    const std::string& errorMethod,
    int minBlockSize) {

    // Initial threshold guesses
    double lowThreshold = 0.1;
    double highThreshold = 1000.0;
    double currentThreshold = initialThreshold;
    double bestThreshold = currentThreshold;
    double bestRatioDifference = 1.0; // Start with a large difference

    // Binary search for the threshold that gives closest to the target ratio
    const int MAX_ITERATIONS = 15; // Usually converges in <15 iterations

    for (int i = 0; i < MAX_ITERATIONS; i++) {
        // Set the current threshold for compression
        Utils::ProgramOptions::threshold = currentThreshold;
        Utils::ProgramOptions::minBlockSize = minBlockSize;
        Utils::ProgramOptions::errorMethod = errorMethod;

        // Test compression with current threshold
        QuadTreeCompressor compressor;
        QuadTree tree = compressor.compressImage(image, currentThreshold, errorMethod);

        // Calculate actual compression ratio
        double actualRatio = compressor.getCompressionRatio();
        double ratioDifference = std::fabs(actualRatio - targetRatio);

        // Store the best threshold found so far
        if (ratioDifference < bestRatioDifference) {
            bestRatioDifference = ratioDifference;
            bestThreshold = currentThreshold;

            // If we're very close to the target, we can stop
            if (ratioDifference < 0.005) { // Within 0.5%
                break;
            }
        }

        // Binary search adjustment
        if (actualRatio < targetRatio) {
            highThreshold = currentThreshold;
            currentThreshold = (lowThreshold + currentThreshold) / 2;
        } else {
            lowThreshold = currentThreshold;
            currentThreshold = (highThreshold + currentThreshold) / 2;
        }
    }

    return bestThreshold;
}

int main(int argc, char* argv[]) {
    try {
        // If no arguments provided or help requested, show usage
        if (argc == 1 || (argc == 2 && (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h"))) {
            showUsage(argv[0]);
            return 0;
        }

        // Parse command line arguments
        Utils::ProgramOptions options = Utils::parseCommandLineArgs(argc, argv);

        // Validate inputs
        if (options.inputFile.empty()) {
            std::cerr << "Error: Input file path is required. Use --input <file>\n";
            return 1;
        }

        if (options.outputFile.empty()) {
            // Generate output filename based on input if not provided
            std::string ext = Utils::getFileExtension(options.inputFile);
            options.outputFile = options.inputFile.substr(0, options.inputFile.length() - ext.length() - 1)
                               + "_compressed." + ext;
            std::cout << "Output file not specified. Using: " << options.outputFile << std::endl;
        }

        if (!Utils::fileExists(options.inputFile)) {
            std::cerr << "Error: Input file does not exist: " << options.inputFile << "\n";
            return 1;
        }

        std::cout << "=== QuadTree Image Compression ===\n\n";
        std::cout << "Input file: " << options.inputFile << "\n";
        std::cout << "Error method: " << options.errorMethod << "\n";
        std::cout << "Threshold: " << options.threshold << "\n";
        std::cout << "Min block size: " << options.minBlockSize << "\n";
        if (options.targetCompressionRatio > 0) {
            std::cout << "Target compression ratio: " << options.targetCompressionRatio * 100 << "%\n";
        }
        std::cout << "Output file: " << options.outputFile << "\n";
        if (!options.gifFile.empty()) {
            std::cout << "GIF output file: " << options.gifFile << "\n";
        }
        std::cout << "\n";

        // Start timing
        auto startTime = std::chrono::high_resolution_clock::now();

        // Load the image
        std::cout << "Loading image...\n";
        std::vector<std::vector<RGB>> image = ImageProcessor::loadImage(options.inputFile);

        if (image.empty() || image[0].empty()) {
            std::cerr << "Error: Failed to load image or image is empty.\n";
            return 1;
        }

        int imageWidth = image[0].size();
        int imageHeight = image.size();

        std::cout << "Image loaded: " << imageWidth << "x" << imageHeight << " pixels\n";

        // Set static program options for use in the compression process
        Utils::ProgramOptions::errorMethod = options.errorMethod;
        Utils::ProgramOptions::threshold = options.threshold;
        Utils::ProgramOptions::minBlockSize = options.minBlockSize;

        // Adjust threshold if target compression ratio is specified
        if (options.targetCompressionRatio > 0) {
            std::cout << "Adjusting threshold to achieve target compression ratio...\n";
            double adjustedThreshold = adjustThresholdForTargetRatio(
                image,
                options.targetCompressionRatio,
                options.threshold,
                options.errorMethod,
                options.minBlockSize
            );

            std::cout << "Adjusted threshold: " << adjustedThreshold << "\n";
            Utils::ProgramOptions::threshold = adjustedThreshold;
        }

        // Create compressor
        QuadTreeCompressor compressor;

        // Compress the image
        std::cout << "Compressing image...\n";
        QuadTree tree = compressor.compressImage(image, Utils::ProgramOptions::threshold, options.errorMethod);

        // Calculate compression statistics
        double compressionRatio = compressor.getCompressionRatio() * 100.0;
        int originalSize = compressor.getOriginalSize();
        int compressedSize = compressor.getCompressedSize();

        // Generate the output image
        std::cout << "Generating output image...\n";
        std::vector<std::vector<RGB>> outputImage;
        tree.saveToImage(outputImage);

        // Check output image dimensions
        if (!outputImage.empty()) {
            std::cout << "Output image dimensions: " << outputImage[0].size() << " x " << outputImage.size() << std::endl;
        } else {
            std::cerr << "Error: Output image is empty\n";
            return 1;
        }

        // Save the output image
        std::cout << "Saving image to: " << options.outputFile << "...\n";
        bool saved = ImageProcessor::saveImage(outputImage, options.outputFile);
        if (!saved) {
            std::cerr << "Error: Failed to save output image.\n";

            std::string fileName = options.outputFile.substr(options.outputFile.find_last_of('/') + 1);
            std::cout << "Trying to save to current directory instead: " << fileName << std::endl;

            saved = ImageProcessor::saveImage(outputImage, fileName);
            if (saved) {
                options.outputFile = fileName;
                std::cout << "Image saved to current directory successfully.\n";
            } else {
                std::cerr << "Error: Failed to save image to current directory as well.\n";
                return 1;
            }
        }

        // // Generate GIF animation
        // if (!options.gifFile.empty()) {
        //     std::cout << "Generating GIF animation...\n";
        //     if (GifGenerator::generateCompressionGif(image, tree, options.gifFile)) {
        //         std::cout << "GIF animation saved to: " << options.gifFile << "\n";
        //     } else {
        //         std::cerr << "Error: Failed to generate GIF animation.\n";
        //     }
        // }

        // Calculate tree depth
        int treeDepth = calculateTreeDepth(tree.getRoot());

        // End timing
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

        // Display results
        std::cout << "\n=== Compression Results ===\n";
        std::cout << "Execution time: " << duration.count() << " ms\n";

        // Get actual file sizes
        std::ifstream inputFile(options.inputFile, std::ios::binary | std::ios::ate);
        std::ifstream outputFile(options.outputFile, std::ios::binary | std::ios::ate);

        long actualOriginalSize = 0;
        long actualCompressedSize = 0;

        if (inputFile.is_open()) {
            actualOriginalSize = inputFile.tellg();
            inputFile.close();
        }

        if (outputFile.is_open()) {
            actualCompressedSize = outputFile.tellg();
            outputFile.close();
        }

        // Display both theoretical and actual sizes
        std::cout << "Theoretical original image size: " << originalSize << " bytes\n";
        std::cout << "Theoretical compressed image size: " << compressedSize << " bytes\n";
        std::cout << "Theoretical compression ratio: " << std::fixed << std::setprecision(2) << compressionRatio << "%\n";

        if (actualOriginalSize > 0 && actualCompressedSize > 0) {
            double actualCompressionRatio = 100.0 * (1.0 - (double)actualCompressedSize / actualOriginalSize);
            std::cout << "\nActual file sizes:\n";
            std::cout << "Original file size: " << actualOriginalSize << " bytes ("
                      << std::fixed << std::setprecision(2) << (actualOriginalSize / 1024.0) << " KB)\n";
            std::cout << "Compressed file size: " << actualCompressedSize << " bytes ("
                      << std::fixed << std::setprecision(2) << (actualCompressedSize / 1024.0) << " KB)\n";

            if (actualCompressionRatio < 0) {
                std::cout << "File size increased by: " << std::fixed << std::setprecision(2)
                          << -actualCompressionRatio << "%\n";
            } else {
                std::cout << "Actual compression ratio: " << std::fixed << std::setprecision(2)
                          << actualCompressionRatio << "%\n";
            }
        }
        std::cout << "Tree depth: " << treeDepth << "\n";
        std::cout << "Total nodes: " << tree.countNodes() << "\n";
        std::cout << "Leaf nodes: " << tree.countLeafNodes() << "\n";
        std::cout << "Output image saved to: " << options.outputFile << "\n";
        if (!options.gifFile.empty()) {
            std::cout << "GIF animation saved to: " << options.gifFile << "\n";
        }

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}