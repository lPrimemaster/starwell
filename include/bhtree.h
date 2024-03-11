#pragma once
#include <array>

#include "body.h"

struct BHNode
{
    std::array<BHNode*, 8> children = { nullptr };
    std::vector<const Body*> bodies;
    PVector3 centerOfMassNorm = {0.0f, 0.0f, 0.0f};
    PVector3 centerOfMassWeighted = {0.0f, 0.0f, 0.0f};
    PVector3 geometricCenter = {0.0f, 0.0f, 0.0f};
    float mass = 0.0f;
    PVector3 nodeCenter = {0.0f, 0.0f, 0.0f};
    float nodeSize = 1E12f;
    static constexpr float BHNODE_SIZE = 1.0f;
    static constexpr float BHNODE_FIELD_EPSILON_THR = 1E-8f;
    static constexpr int BHNODE_MAX_DEPTH = 10000;

    // static BHPool<BHNode> MemoryPool;
    
    // void* operator new(std::size_t n)
    // {
    //     return MemoryPool.alloc();
    // }
    //
    // void operator delete(void* p)
    // {
    //     MemoryPool.free(p);
    // }
};

class BHTree
{
public:
    BHTree();
    ~BHTree();


    void reset();
    void insertBody(const Body* body);
    PVector3 calculateFieldOnPoint(const PVector3& point, const float thr);
    void printNodes() const;
    unsigned long long computeNodeNumber() const;

private:
    unsigned long long countChildrenRecursive(BHNode* node) const;
    void printNode(const BHNode* node, int depth) const;
    void deleteNodes(BHNode* node);
    PVector3 calculateFieldOnPointDFS(const PVector3& point, const float thr, const BHNode* node);
    void calculateNodeInsertion(const Body* body, BHNode* node, unsigned long long depth);
    PVector3 calculateNodeCenter(const std::size_t nodeIndex, const BHNode* parent);
    std::size_t calculateNodeIndex(const PVector3& position, const BHNode* node);
    void spawnChildren(BHNode* parent, int depth);

private:
    BHNode* root;
};
