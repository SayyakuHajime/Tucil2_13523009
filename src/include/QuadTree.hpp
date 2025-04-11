#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include <iostream>
#include <vector>
#include <utility>
#include <cstdint>

// RGB color structure
struct RGB {
    uint8_t r, g, b; // component (0-255)
};

class QuadTreeNode {
private:
    int x, y;
    int width, height;
    bool isLeaf;
    RGB avgColor;
    double error;
    QuadTreeNode* children[4];  // NW, NE, SW, SE corners

public:
    // Constructor and destructor
    QuadTreeNode(int x, int y, int width, int height);
    ~QuadTreeNode();

    // Position and dimension getters
    int getPosX() const;
    int getPosY() const;
    std::pair<int, int> getPosition() const;
    int getBlockWidth() const;
    int getBlockHeight() const;
    std::pair<int, int> getDimensions() const;

    // Node property access
    bool isNodeLeaf() const;
    RGB getAverageColor() const;
    double getError() const;
    QuadTreeNode* getChildAt(int index) const;

    // Node property setters
    void setAverageColor(const RGB& color);
    void setError(double errorValue);
    void markAsLeaf(bool leafStatus);
    void setChildNode(int index, QuadTreeNode* node);

    // Basic node operations
    void subdivide();  // Divide node into four children
    void normalize();  // Apply average color to entire block

    // Friend class to allow QuadTree to access private members
    friend class QuadTree;
};

class QuadTree {
private:
    QuadTreeNode* root;
    int imageWidth;
    int imageHeight;
    double totalError;

public:
    // Constructor and destructor
    QuadTree(int width, int height);
    ~QuadTree();

    // Building tree from image
    void buildFromImage(const std::vector<std::vector<RGB>>& image, double threshold);

    // Compression
    void compress(double threshold);
    double getTotalError() const;

    // Access and traversal
    QuadTreeNode* getRoot() const;
    int countNodes() const;
    int countLeafNodes() const;

    // Output
    void saveToImage(std::vector<std::vector<RGB>>& outputImage) const;

    // Memory management
    void clear();

private:
    // Helper methods for recursive operations
    void buildNodeRecursive(QuadTreeNode* node, const std::vector<std::vector<RGB>>& image, double threshold);
    int countNodesRecursive(const QuadTreeNode* node) const;
    int countLeafNodesRecursive(const QuadTreeNode* node) const;
    void saveNodeToImage(const QuadTreeNode* node, std::vector<std::vector<RGB>>& outputImage) const;
};

#endif // QUADTREE_HPP