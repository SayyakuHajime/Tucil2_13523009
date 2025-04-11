#ifndef GIF_GENERATOR_HPP
#define GIF_GENERATOR_HPP

#include <string>
#include <vector>
#include "QuadTree.hpp"

class GifGenerator {
public:
    // Creates a GIF showing progressive compression steps
    static bool generateCompressionGif(
        const std::vector<std::vector<RGB>>& originalImage,
        const QuadTree& tree,
        const std::string& outputFilename,
        int frameDelay = 10); // Frame delay in 1/100 seconds

    // Creates a single frame representing the current state of the quadtree
    static std::vector<std::vector<RGB>> createQuadTreeFrame(
        const QuadTreeNode* node,
        int width, int height,
        bool showBorders = true);

private:
    // Helper method to recursively draw tree nodes
    static void drawNode(
        std::vector<std::vector<RGB>>& frame,
        const QuadTreeNode* node,
        bool showBorders);
    static void collectNodesByLevel(
        const QuadTreeNode* node,
        std::vector<std::vector<QuadTreeNode*>>& nodesByLevel,
        int level = 0);
};

#endif // GIF_GENERATOR_HPP