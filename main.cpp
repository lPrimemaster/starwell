// This is a very simple attempt to start some simulation code using the Barnes-Hut algorithm for n-body interactions
// It features a O(n log n) complexity

#include <array>
#include <cstddef>
#include <fstream>
#include <random>
#include <vector>
#include <cmath>
#include <iostream>

struct Vector3
{
    union
    {
        struct
        {
            float x;
            float y;
            float z;
        };
        float data[3];
    };
    
    static float DistanceSqr(const Vector3& a, const Vector3& b)
    {
        return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z); 
    }

    static float Distance(const Vector3& a, const Vector3& b)
    {
        return std::sqrt(DistanceSqr(a, b));
    }

    static float Magnitude(const Vector3& v)
    {
        return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    }

    Vector3& operator+=(const Vector3& a)
    {
        this->x += a.x;
        this->y += a.y;
        this->z += a.z;
        return *this;
    }

    static Vector3 Normalize(const Vector3& v);
};

Vector3 operator*(float s, const Vector3& v)
{
    return Vector3 { s * v.x, s * v.y, s * v.z };
}

Vector3 operator/(const Vector3& v, float s)
{
    return Vector3 { v.x / s, v.y / s, v.z / s };
}

Vector3 operator+(const Vector3& a, const Vector3& b)
{
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

Vector3 operator-(const Vector3& a, const Vector3& b)
{
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

Vector3 Vector3::Normalize(const Vector3& v)
{
    return v / Magnitude(v);
}

std::ostream& operator<<(std::ostream& cout, const Vector3& v)
{
    cout << "[" << v.x << " " << v.y << " " << v.z << "]";
    return cout;
}

// Make this a simple body
class Body
{
public:
    Body(const Vector3& position, const Vector3& velocity = {0, 0, 0}) : position(position), velocity(velocity), mass(1.0f)
    {
        
    }

    Body(const Body& body) = default;
    Body(Body&& body) = default;
    ~Body() = default;

    void move(const Vector3& field)
    {
        Vector3 force = field;
        velocity += 0.01f * 2 * force;
        position += 0.01f * velocity;
    }

    float getMass() const
    {
        return mass;
    }

    Vector3 getPosition() const
    {
        return position;
    }

private:
    Vector3 position;
    Vector3 velocity;
    float mass;
};

struct BHNode
{
    std::array<BHNode*, 8> children = { nullptr };
    std::vector<const Body*> bodies;
    Vector3 centerOfMassNorm = {0.0f, 0.0f, 0.0f};
    Vector3 centerOfMassWeighted = {0.0f, 0.0f, 0.0f};
    float mass = 0.0f;
    Vector3 nodeCenter = {0.0f, 0.0f, 0.0f};
    float nodeSize = 100.0f;
    static constexpr float BHNODE_SIZE = 1.0f;
    static constexpr float BHNODE_FIELD_EPSILON_THR = 1E-8f;
};

// The BHTree is centered on the origin
class BHTree
{
public:
    BHTree()
    {
        root = new BHNode();
    }
    
    ~BHTree()
    {
        deleteNodes(root);
    }

    void reset()
    {
        // HACK: (César) : This is very unefficient
        deleteNodes(root);
        root = new BHNode();
    }

    void insertBody(const Body* body)
    {
        calculateNodeInsertion(body, root);
    }
    
    Vector3 calculateFieldOnPoint(const Vector3& point, const float thr)
    {
        // Traverse the tree with dfs and use a threshold of thr
        return calculateFieldOnPointDFS(point, thr, root);
    }

    void printNodes()
    {
        printNode(root, 0);
    }

private:
    void printNode(const BHNode* node, int depth)
    {
        for(int i = 0; i < depth; i++) std::cout << "\t";

        std::cout << "Node " << depth << ":" << std::endl;

        for(int i = 0; i < depth + 1; i++) std::cout << "\t";
        std::cout << "- Mass           : " << node->mass << std::endl;

        for(int i = 0; i < depth + 1; i++) std::cout << "\t";
        std::cout << "- Bodies         : " << node->bodies.size() << std::endl;

        for(int i = 0; i < depth + 1; i++) std::cout << "\t";
        std::cout << "- Center of Mass : " << node->centerOfMassNorm << std::endl;

        if(node->children[0])
            for(int i = 0; i < 8; i++) printNode(node->children[i], depth + 1);
    }

    void deleteNodes(BHNode* node)
    {
        if(!node) return;
        if(node->children[0])
        {
            for(std::size_t i = 0; i < 8; i++)
            {
                deleteNodes(node->children[i]);
            }
        }
        delete node;
    }

    Vector3 calculateFieldOnPointDFS(const Vector3& point, const float thr, const BHNode* node)
    {
        Vector3 field = {0.0f, 0.0f, 0.0f};
        constexpr float K = 0.1f;

        if(node->bodies.empty())
        {
            return field;
        }

        // Check thr
        // TODO: (César) : Change to DistanceSqr
        float distance = Vector3::Distance(point, node->centerOfMassNorm);

        // Exclude self (for now just use this distance approach)
        // There might be better ways
        if(distance < BHNode::BHNODE_FIELD_EPSILON_THR && node->bodies.size() == 1)
        {
            return field;
        }

        bool useCM = ((node->nodeSize / distance) < thr) || (node->bodies.size() == 1);

        if(useCM)
        {
            field = (K * node->mass / distance) * Vector3::Normalize(node->centerOfMassNorm - point);
            // std::cout << "Norm: " << Vector3::Normalize(node->centerOfMassNorm - point) << std::endl;
        }
        else
        {
            // Continue the traversal
            for(std::size_t i = 0; i < 8; i++)
            {
                field += calculateFieldOnPointDFS(point, thr, node->children[i]);
            }
        }
        // std::cout << "Field: " << field << std::endl;
        return field;
    }

    void calculateNodeInsertion(const Body* body, BHNode* node)
    {
        // node->bodies.push_back(&body);
        // node->mass += body.getMass();

        if(node->bodies.empty())
        {
            node->bodies.push_back(body);
            node->mass += body->getMass();
            node->centerOfMassWeighted = body->getPosition();
            node->centerOfMassNorm = node->centerOfMassWeighted;
            return;
        }
        const Vector3& bposition = body->getPosition();
        node->centerOfMassWeighted += body->getMass() * bposition;
        node->mass += body->getMass();
        node->centerOfMassNorm = node->centerOfMassWeighted / node->mass;

        node->bodies.push_back(body);

        // Do we not have children ?
        if(!node->children[0])
        {
            spawnChildren(node);
        }
        else
        {
            std::size_t cindex = calculateNodeIndex(bposition, node);
            calculateNodeInsertion(body, node->children[cindex]);
        }

        // node->bodies.push_back(&body);
        
        // Were does this new body fall ?
        // std::size_t cindex = calculateNodeIndex(bposition, node);
        // calculateNodeInsertion(body, node->children[cindex]);
    }

    Vector3 calculateNodeCenter(const std::size_t nodeIndex, const BHNode* parent)
    {
        constexpr std::array<Vector3, 8> offsets = {
            Vector3{ -BHNode::BHNODE_SIZE, -BHNode::BHNODE_SIZE, -BHNode::BHNODE_SIZE },
            Vector3{ -BHNode::BHNODE_SIZE, -BHNode::BHNODE_SIZE,  BHNode::BHNODE_SIZE },
            Vector3{ -BHNode::BHNODE_SIZE,  BHNode::BHNODE_SIZE, -BHNode::BHNODE_SIZE },
            Vector3{ -BHNode::BHNODE_SIZE,  BHNode::BHNODE_SIZE,  BHNode::BHNODE_SIZE },
            Vector3{  BHNode::BHNODE_SIZE, -BHNode::BHNODE_SIZE, -BHNode::BHNODE_SIZE },
            Vector3{  BHNode::BHNODE_SIZE, -BHNode::BHNODE_SIZE,  BHNode::BHNODE_SIZE },
            Vector3{  BHNode::BHNODE_SIZE,  BHNode::BHNODE_SIZE, -BHNode::BHNODE_SIZE },
            Vector3{  BHNode::BHNODE_SIZE,  BHNode::BHNODE_SIZE,  BHNode::BHNODE_SIZE }
        };
        return parent->nodeCenter + 0.25f * parent->nodeSize * offsets[nodeIndex];
    }

    std::size_t calculateNodeIndex(const Vector3& position, const BHNode* node)
    {
        std::size_t index = 0;
        index |= static_cast<std::size_t>(position.x >= node->nodeCenter.x) << 2;
        index |= static_cast<std::size_t>(position.y >= node->nodeCenter.y) << 1;
        index |= static_cast<std::size_t>(position.z >= node->nodeCenter.z);
        return index;
    }

    void spawnChildren(BHNode* parent)
    {
        for(std::size_t i = 0; i < parent->children.size(); i++)
        {
            parent->children[i] = new BHNode();
            parent->children[i]->nodeCenter = calculateNodeCenter(i, parent);
            parent->children[i]->nodeSize = 0.5f * parent->nodeSize;
        }

        for(const Body* body : parent->bodies)
        {
            std::size_t cindex = calculateNodeIndex(body->getPosition(), parent);
            calculateNodeInsertion(body, parent->children[cindex]);
            // parent->children[cindex]->bodies.push_back(body);
            // parent->children[cindex]->mass += body->getMass();
            // parent->children[cindex]->centerOfMassWeighted += body->getMass() * body->getPosition();
            // parent->children[cindex]->centerOfMassNorm = parent->children[cindex]->centerOfMassWeighted / parent->children[cindex]->mass;
        }
    }

private:
    BHNode* root;
};

int main(void)
{
    std::vector<Body> bodies;
    BHTree tree;

    // std::random_device dev;
    // std::mt19937 mt(dev());
    // std::normal_distribution<float> distX(-20.0f, 5.0f);
    // std::normal_distribution<float> distY(0.0f, 0.1f);
    // std::normal_distribution<float> distZ(0.0f, 5.0f);
    //
    // for(int i = 0 ; i < 1'000; i++)
    // {
    //     bodies.push_back(Body({
    //         distX(mt),
    //         distY(mt),
    //         distZ(mt)
    //     }, {0.0f, 50.0f, 0.0f}));
    // }
    //
    // std::normal_distribution<float> distX1(20.0f, 5.0f);
    // std::normal_distribution<float> distY1(0.0f, 0.1f);
    // std::normal_distribution<float> distZ1(0.0f, 5.0f);
    //
    // for(int i = 0 ; i < 1'000; i++)
    // {
    //     bodies.push_back(Body({
    //         distX1(mt),
    //         distY1(mt),
    //         distZ1(mt)
    //     }, {0.0f, -50.0f, 0.0f}));
    // }
    
    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 10; j++)
        {
            for(int k = 0; k < 10; k++)
            {
                float x = (i / 10.0f) * 100.0f - 50.0f;
                float y = (j / 10.0f) * 100.0f - 50.0f;
                float z = (k / 10.0f) * 100.0f - 50.0f;
                bodies.push_back(Body({x, y, z}));
            }
        }
    }

    std::ofstream file("output.csv");
    file << "x, y, z, frame\n";
    
    for(int i = 0; i < 10'000; i++)
    {
        for(auto& body : bodies)
        {
            tree.insertBody(&body);
            Vector3 pos = body.getPosition();
            file << pos.x << "," << pos.y << "," << pos.z << "," << i << "\n";
        }

        for(auto& body : bodies)
        {
            Vector3 field = tree.calculateFieldOnPoint(body.getPosition(), 0.5f);
            body.move(field);
            // std::cout << "field = " << field << std::endl;
        }
        tree.reset();
        std::cout << "Frame " << i << "..." << std::endl;
    }
    
    return 0;
}
