#include "GifGenerator.hpp"
#include "QuadTree.hpp"
#include "ImageProcessor.hpp"
#include "Utils.hpp"
#include <vector>
#include <cstring>

#define GIF_WRITER_IMPLEMENTATION
#include "gif.h"

void traverseQuadTree(const QuadTreeNode* node, std::vector<QuadTreeNode*>& nodeOrder, bool leafNodesOnly = false) {
    if (!node) return;
    if (!leafNodesOnly || node->isNodeLeaf()) nodeOrder.push_back(const_cast<QuadTreeNode*>(node));
    if (!node->isNodeLeaf()) {
        for (int i = 0; i < 4; ++i) traverseQuadTree(node->getChildAt(i), nodeOrder, leafNodesOnly);
    }
}

std::vector<std::vector<RGB>> GifGenerator::createQuadTreeFrame(const QuadTreeNode* node, int width, int height, bool showBorders) {
    std::vector<std::vector<RGB>> frame(height, std::vector<RGB>(width, {255, 255, 255}));
    drawNode(frame, node, showBorders);
    return frame;
}

bool GifGenerator::generateCompressionGif(const std::vector<std::vector<RGB>>& originalImage, const QuadTree& tree, const std::string& outputFilename, int frameDelay) {
    if (originalImage.empty() || originalImage[0].empty()) return false;

    int height = originalImage.size(), width = originalImage[0].size();
    std::string normalizedPath = Utils::normalizePath(outputFilename);
    std::string dirPath = Utils::getDirectoryPath(normalizedPath);

    if (!dirPath.empty() && !Utils::fileExists(dirPath) && !Utils::createDirectory(dirPath)) {
        normalizedPath = Utils::getFileNameFromPath(normalizedPath);
    }

    std::vector<std::vector<QuadTreeNode*>> nodesByLevel;
    collectNodesByLevel(tree.getRoot(), nodesByLevel);

    GifWriter writer = {0};
    if (!GifBegin(&writer, normalizedPath.c_str(), width, height, frameDelay)) return false;

    for (size_t level = 0; level < nodesByLevel.size(); ++level) {
        std::vector<std::vector<RGB>> frameImage(height, std::vector<RGB>(width, {255, 255, 255}));
        for (size_t l = 0; l <= level; ++l) {
            for (QuadTreeNode* node : nodesByLevel[l]) {
                if (node->isNodeLeaf() || l == level) drawNode(frameImage, node, true);
            }
        }
        uint8_t* frameData = new uint8_t[width * height * 4]();
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                int idx = (i * width + j) * 4;
                frameData[idx] = frameImage[i][j].r;
                frameData[idx + 1] = frameImage[i][j].g;
                frameData[idx + 2] = frameImage[i][j].b;
                frameData[idx + 3] = 255;
            }
        }
        if (!GifWriteFrame(&writer, frameData, width, height, frameDelay)) {
            delete[] frameData;
            GifEnd(&writer);
            return false;
        }
        delete[] frameData;
    }

    std::vector<std::vector<RGB>> finalFrame;
    tree.saveToImage(finalFrame);
    uint8_t* finalFrameData = new uint8_t[width * height * 4]();
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int idx = (i * width + j) * 4;
            if (i < finalFrame.size() && j < finalFrame[0].size()) {
                finalFrameData[idx] = finalFrame[i][j].r;
                finalFrameData[idx + 1] = finalFrame[i][j].g;
                finalFrameData[idx + 2] = finalFrame[i][j].b;
            } else {
                finalFrameData[idx] = finalFrameData[idx + 1] = finalFrameData[idx + 2] = 255;
            }
            finalFrameData[idx + 3] = 255;
        }
    }
    GifWriteFrame(&writer, finalFrameData, width, height, frameDelay * 2);
    delete[] finalFrameData;

    return GifEnd(&writer);
}

void GifGenerator::drawNode(std::vector<std::vector<RGB>>& frame, const QuadTreeNode* node, bool showBorders) {
    if (!node) return;
    int x = node->getPosX(), y = node->getPosY(), width = node->getBlockWidth(), height = node->getBlockHeight();
    RGB color = node->getAverageColor();

    for (int i = y; i < std::min(y + height, (int)frame.size()); ++i) {
        for (int j = x; j < std::min(x + width, (int)frame[0].size()); ++j) frame[i][j] = color;
    }

    if (showBorders && (width > 1 || height > 1)) {
        RGB borderColor = {static_cast<uint8_t>(color.r * 0.9), static_cast<uint8_t>(color.g * 0.9), static_cast<uint8_t>(color.b * 0.9)};
        for (int j = x; j < std::min(x + width, (int)frame[0].size()); ++j) {
            if (y < frame.size()) frame[y][j] = borderColor;
            if (y + height - 1 < frame.size()) frame[y + height - 1][j] = borderColor;
        }
        for (int i = y; i < std::min(y + height, (int)frame.size()); ++i) {
            if (x < frame[0].size()) frame[i][x] = borderColor;
            if (x + width - 1 < frame[0].size()) frame[i][x + width - 1] = borderColor;
        }
    }
}

void GifGenerator::collectNodesByLevel(const QuadTreeNode* node, std::vector<std::vector<QuadTreeNode*>>& nodesByLevel, int level) {
    if (!node) return;
    if (nodesByLevel.size() <= level) nodesByLevel.push_back({});
    nodesByLevel[level].push_back(const_cast<QuadTreeNode*>(node));
    if (!node->isNodeLeaf()) {
        for (int i = 0; i < 4; ++i) collectNodesByLevel(node->getChildAt(i), nodesByLevel, level + 1);
    }
}