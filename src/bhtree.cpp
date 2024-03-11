#include "../include/bhtree.h"
#include <algorithm>


BHTree::BHTree()
{
    root = new BHNode();
}
    
BHTree::~BHTree()
{
    deleteNodes(root);
}

void BHTree::reset()
{
    // HACK: (César) : This is very unefficient
    deleteNodes(root);
    root = new BHNode();
}

void BHTree::insertBody(const Body* body)
{
    // ST_PROF;
    calculateNodeInsertion(body, root, 0);
}

PVector3 BHTree::calculateFieldOnPoint(const PVector3& point, const float thr)
{
    // ST_PROF;
    // Traverse the tree with dfs and use a threshold of thr
    return calculateFieldOnPointDFS(point, thr, root);
}

void BHTree::printNodes() const
{
    printNode(root, 0);
}

unsigned long long BHTree::computeNodeNumber() const
{
    return countChildrenRecursive(root);
}

unsigned long long BHTree::countChildrenRecursive(BHNode* node) const
{
    unsigned long long count = 1ULL;

    for(std::size_t i = 0; i < node->children.size(); i++)
    {
        if(node->children[i])
        {
            count += countChildrenRecursive(node->children[i]);
        }
    }

    return count;
}

void BHTree::printNode(const BHNode* node, int depth) const
{
    for(int i = 0; i < depth; i++) std::cout << "\t";

    std::cout << "Node " << depth << ":" << std::endl;

    for(int i = 0; i < depth + 1; i++) std::cout << "\t";
    std::cout << "- Mass           : " << node->mass << std::endl;

    for(int i = 0; i < depth + 1; i++) std::cout << "\t";
    std::cout << "- Bodies         : " << node->bodies.size() << std::endl;

    for(int i = 0; i < depth + 1; i++) std::cout << "\t";
    std::cout << "- Center of Mass : " << node->centerOfMassNorm << std::endl;

    for(int i = 0; i < depth + 1; i++) std::cout << "\t";
    std::cout << "- Child Count    : " << std::count_if(node->children.begin(), node->children.end(), [](auto* child) { return child != nullptr; }) << std::endl;

    for(int i = 0; i < 8; i++) if(node->children[i]) printNode(node->children[i], depth + 1);
}

void BHTree::deleteNodes(BHNode* node)
{
    for(std::size_t i = 0; i < 8; i++)
    {
        if(node->children[i]) deleteNodes(node->children[i]);
    }
    delete node;
}

PVector3 BHTree::calculateFieldOnPointDFS(const PVector3& point, const float thr, const BHNode* node)
{
    PVector3 field = {0.0f, 0.0f, 0.0f};
    // constexpr float K = 1E3;
    constexpr float K = 10.0f;

    if(node->bodies.empty())
    {
        return field;
    }

    // Check thr
    // TODO: (César) : Change to DistanceSqr
    float distance = PVector3::Distance(point, node->centerOfMassNorm);

    // Exclude self (for now just use this distance approach)
    // There might be better ways
    if(distance < BHNode::BHNODE_FIELD_EPSILON_THR && node->bodies.size() == 1)
    {
        return field;
    }

    bool useCM = ((node->nodeSize / distance) < thr) || (node->bodies.size() == 1);

    if(useCM)
    {
        // field = (K * node->mass / (distance * distance)) * PVector3::Normalize(node->centerOfMassNorm - point);
        field = (K * node->mass / distance) * PVector3::Normalize(node->centerOfMassNorm - point);
    }
    else
    {
        // Continue the traversal
        for(std::size_t i = 0; i < 8; i++)
        {
            if(!node->children[i]) continue;

            field += calculateFieldOnPointDFS(point, thr, node->children[i]);
        }
    }
    return field;
}

void BHTree::calculateNodeInsertion(const Body* body, BHNode* node, unsigned long long depth)
{
    if(node->bodies.empty())
    {
        node->bodies.push_back(body);
        node->mass += body->getMass();
        node->centerOfMassWeighted = body->getPosition();
        node->centerOfMassNorm = node->centerOfMassWeighted;
        node->geometricCenter = body->getPosition();
        return;
    }
    const PVector3& bposition = body->getPosition();
    node->geometricCenter += (bposition - node->geometricCenter) / (node->bodies.size() + 1);
    node->centerOfMassWeighted += body->getMass() * bposition;
    node->mass += body->getMass();
    node->centerOfMassNorm = node->centerOfMassWeighted / node->mass;

    node->bodies.push_back(body);
    
    std::size_t cindex = calculateNodeIndex(bposition, node);
    if(!node->children[cindex])
    {
        spawnChildren(node, depth);
    }
    else
    {
        calculateNodeInsertion(body, node->children[cindex], depth + 1);
    }
}

PVector3 BHTree::calculateNodeCenter(const std::size_t nodeIndex, const BHNode* parent)
{
    constexpr std::array<PVector3, 8> offsets = {
        PVector3{ -BHNode::BHNODE_SIZE, -BHNode::BHNODE_SIZE, -BHNode::BHNODE_SIZE },
        PVector3{ -BHNode::BHNODE_SIZE, -BHNode::BHNODE_SIZE,  BHNode::BHNODE_SIZE },
        PVector3{ -BHNode::BHNODE_SIZE,  BHNode::BHNODE_SIZE, -BHNode::BHNODE_SIZE },
        PVector3{ -BHNode::BHNODE_SIZE,  BHNode::BHNODE_SIZE,  BHNode::BHNODE_SIZE },
        PVector3{  BHNode::BHNODE_SIZE, -BHNode::BHNODE_SIZE, -BHNode::BHNODE_SIZE },
        PVector3{  BHNode::BHNODE_SIZE, -BHNode::BHNODE_SIZE,  BHNode::BHNODE_SIZE },
        PVector3{  BHNode::BHNODE_SIZE,  BHNode::BHNODE_SIZE, -BHNode::BHNODE_SIZE },
        PVector3{  BHNode::BHNODE_SIZE,  BHNode::BHNODE_SIZE,  BHNode::BHNODE_SIZE }
    };
    // return parent->geometricCenter + 0.25f * parent->nodeSize * offsets[nodeIndex];
    return parent->nodeCenter + 0.25f * parent->nodeSize * offsets[nodeIndex];
}

std::size_t BHTree::calculateNodeIndex(const PVector3& position, const BHNode* node)
{
    std::size_t index = 0;
    index |= static_cast<std::size_t>(position.x >= node->nodeCenter.x) << 2;
    index |= static_cast<std::size_t>(position.y >= node->nodeCenter.y) << 1;
    index |= static_cast<std::size_t>(position.z >= node->nodeCenter.z);
    return index;
}

void BHTree::spawnChildren(BHNode* parent, int depth)
{
    std::array<bool, 8> needRecalculation = { false };
    for(const Body* body : parent->bodies)
    {
        std::size_t cindex = calculateNodeIndex(body->getPosition(), parent);
        if(!parent->children[cindex])
        {
            needRecalculation[cindex] = true;
            parent->children[cindex] = new BHNode();
            parent->children[cindex]->nodeCenter = calculateNodeCenter(cindex, parent);
            parent->children[cindex]->nodeSize = 0.5f * parent->nodeSize;
        }

        if(needRecalculation[cindex])
        {
            calculateNodeInsertion(body, parent->children[cindex], depth + 1);
        }
    }
}
