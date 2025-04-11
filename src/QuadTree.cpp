#include "QuadTree.hpp"
#include "Utils.hpp"
#include "ErrorMeasurement.hpp"
// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------
// QuadTreeNode Class
QuadTreeNode::QuadTreeNode(int x, int y, int width, int height) {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->isLeaf = true;
    this->error = 0.0;
    this->avgColor = {0, 0, 0};
    for (int i = 0; i < 4; ++i) {
        children[i] = nullptr;
    }
}
QuadTreeNode::~QuadTreeNode() {
    for (int i = 0; i < 4; ++i) {
        if (children[i] != nullptr) {
            delete children[i];
        }
    }
}

int QuadTreeNode::getPosX() const {
    return x;
}

int QuadTreeNode::getPosY() const {
    return y;
}

std::pair<int, int> QuadTreeNode::getPosition() const {
    return std::make_pair(x, y);
}

int QuadTreeNode::getBlockWidth() const {
    return width;
}

int QuadTreeNode::getBlockHeight() const {
    return height;
}

std::pair<int, int> QuadTreeNode::getDimensions() const {
    return std::make_pair(width, height);
}

bool QuadTreeNode::isNodeLeaf() const {
    return isLeaf;
}
RGB QuadTreeNode::getAverageColor() const {
    return avgColor;
}

double QuadTreeNode::getError() const {
    return error;
}

QuadTreeNode* QuadTreeNode::getChildAt(int index) const {
    if (index < 0 || index >= 4) {
        return nullptr;
    }
    return children[index];
}

void QuadTreeNode::setAverageColor(const RGB& color) {
    avgColor.r = color.r;
    avgColor.g = color.g;
    avgColor.b = color.b;
}

void QuadTreeNode::setError(double errorValue) {
    if (errorValue < 0) {
        errorValue = 0;
    }
    if (errorValue > 1) {
        errorValue = 1;
    }
    this->error = errorValue;
}

void QuadTreeNode::markAsLeaf(bool leafStatus) {
    this->isLeaf = leafStatus;
    if (leafStatus) {
        for (int i = 0; i < 4; ++i) {
            delete children[i];
            children[i] = nullptr;
        }
    }
}

void QuadTreeNode::setChildNode(int index, QuadTreeNode* node) {
    this->children[index] = node;
    this->isLeaf = false;
}

void QuadTreeNode::subdivide() {
    int halfWidth = width / 2;
    int halfHeight = height / 2;

    // Calculate remaining width and height for odd dimensions
    int remWidth = width - halfWidth;
    int remHeight = height - halfHeight;

    // Create four child nodes with adjusted dimensions
    children[0] = new QuadTreeNode(x, y, halfWidth, halfHeight);
    children[1] = new QuadTreeNode(x + halfWidth, y, remWidth, halfHeight);
    children[2] = new QuadTreeNode(x, y + halfHeight, halfWidth, remHeight);
    children[3] = new QuadTreeNode(x + halfWidth, y + halfHeight, remWidth, remHeight);

    isLeaf = false;
}

void QuadTreeNode::normalize() {
    for (int i = 0; i < 4; ++i) {
        if (children[i] != nullptr) {
            children[i]->setAverageColor(avgColor);
            children[i]->normalize();
        }
    }
}
// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------



// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------
// QuadTree Class
QuadTree::QuadTree(int width, int height) {
    root = new QuadTreeNode(0, 0, width, height);
}

QuadTree::~QuadTree() {
    clear();
    delete root;
}

void QuadTree::buildFromImage(const std::vector<std::vector<RGB>>& image, double threshold) {
    clear();
    buildNodeRecursive(root, image, threshold);
}

void QuadTree::compress(double threshold) {
    if (root) {
        buildNodeRecursive(root, {}, threshold);
    }
}

double QuadTree::getTotalError() const {
    return root ? root->getError() : 0.0;
}

QuadTreeNode* QuadTree::getRoot() const {
    return root;
}

int QuadTree::countNodes() const {
    return countNodesRecursive(root);
}

int QuadTree::countLeafNodes() const {
    return countLeafNodesRecursive(root);
}

void QuadTree::saveToImage(std::vector<std::vector<RGB>>& outputImage) const {
    if (!root) return;

    int width = root->getBlockWidth();
    int height = root->getBlockHeight();

    outputImage.resize(height, std::vector<RGB>(width));

    saveNodeToImage(root, outputImage);
}

void QuadTree::clear() {
    if (root) {
        for (int i = 0; i < 4; ++i) {
            delete root->getChildAt(i);
            root->setChildNode(i, nullptr);
        }
    }

}

void QuadTree::buildNodeRecursive(QuadTreeNode* node, const std::vector<std::vector<RGB>>& image, double threshold) {
    if (!node) return;

    int x = node->getPosX();
    int y = node->getPosY();
    int width = node->getBlockWidth();
    int height = node->getBlockHeight();

    // Calculate average color for the node
    RGB avgColor = ErrorMeasurement::calculateAverageColor(image, x, y, width, height);
    node->setAverageColor(avgColor);

    // Calculate error using the appropriate method
    double error = ErrorMeasurement::calculateError(image, x, y, width, height, avgColor, Utils::ProgramOptions::errorMethod);

    // Set error value on the node
    node->setError(error);

    // Check if we should subdivide based on threshold
    if (error > Utils::ProgramOptions::threshold &&
        width >= Utils::ProgramOptions::minBlockSize &&
        height >= Utils::ProgramOptions::minBlockSize &&
        width / 2 >= Utils::ProgramOptions::minBlockSize &&
        height / 2 >= Utils::ProgramOptions::minBlockSize) {
        node->subdivide();
        for (int i = 0; i < 4; ++i) {
            buildNodeRecursive(node->getChildAt(i), image, Utils::ProgramOptions::threshold);
        }
        }
}

int QuadTree::countNodesRecursive(const QuadTreeNode* node) const {
    if (!node) return 0;

    int count = 1;
    for (int i = 0; i < 4; ++i) {
        count += countNodesRecursive(node->getChildAt(i));
    }
    return count;
}

int QuadTree::countLeafNodesRecursive(const QuadTreeNode* node) const {
    if (!node) return 0;

    if (node->isNodeLeaf()) {
        return 1;
    }

    int count = 0;
    for (int i = 0; i < 4; ++i) {
        count += countLeafNodesRecursive(node->getChildAt(i));
    }
    return count;
}

void QuadTree::saveNodeToImage(const QuadTreeNode* node, std::vector<std::vector<RGB>>& outputImage) const {
    if (!node) return;

    if (node->isNodeLeaf()) {
        for (int y = node->getPosY(); y < node->getPosY() + node->getBlockHeight(); ++y) {
            for (int x = node->getPosX(); x < node->getPosX() + node->getBlockWidth(); ++x) {
                // Add bounds checking to avoid segmentation faults
                if (y < outputImage.size() && x < outputImage[0].size()) {
                    outputImage[y][x] = node->getAverageColor();
                }
            }
        }
    } else {
        for (int i = 0; i < 4; ++i) {
            saveNodeToImage(node->getChildAt(i), outputImage);
        }
    }
}
// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------