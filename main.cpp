// This is a very simple attempt to start some simulation code using the Barnes-Hut algorithm for n-body interactions
// It features a O(n log n) complexity

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <new>
#include <random>
#include <vector>
#include <cmath>
#include <iostream>

#include <stperf.h>
#include <glad/gl.h>
#include <glfw3.h>

struct PVector3
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
    
    static float DistanceSqr(const PVector3& a, const PVector3& b)
    {
        return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z); 
    }

    static float Distance(const PVector3& a, const PVector3& b)
    {
        return std::sqrt(DistanceSqr(a, b));
    }

    static float Magnitude(const PVector3& v)
    {
        return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    }

    PVector3& operator+=(const PVector3& a)
    {
        this->x += a.x;
        this->y += a.y;
        this->z += a.z;
        return *this;
    }

    static PVector3 Normalize(const PVector3& v);
};

PVector3 operator*(float s, const PVector3& v)
{
    return PVector3 { s * v.x, s * v.y, s * v.z };
}

PVector3 operator/(const PVector3& v, float s)
{
    return PVector3 { v.x / s, v.y / s, v.z / s };
}

PVector3 operator+(const PVector3& a, const PVector3& b)
{
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

PVector3 operator-(const PVector3& a, const PVector3& b)
{
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

PVector3 PVector3::Normalize(const PVector3& v)
{
    return v / Magnitude(v);
}

std::ostream& operator<<(std::ostream& cout, const PVector3& v)
{
    cout << "[" << v.x << " " << v.y << " " << v.z << "]";
    return cout;
}

// Make this a simple body
class Body
{
public:
    Body(const PVector3& position, const PVector3& velocity = {0, 0, 0}) : position(position), velocity(velocity), mass(1.0f)
    {
        
    }

    Body(const Body& body) = default;
    Body(Body&& body) = default;
    ~Body() = default;

    void move(const PVector3& field)
    {
        PVector3 force = field;
        velocity += 0.01f * 2 * force;
        position += 0.01f * velocity;
    }

    float getMass() const
    {
        return mass;
    }

    PVector3 getPosition() const
    {
        return position;
    }

private:
    PVector3 position;
    PVector3 velocity;
    float mass;
};

template<typename T>
class BHPool
{
public:
    BHPool(std::size_t maxInstances) : N(maxInstances)
    {
        buffer = new unsigned char[sizeof(T) * N];
        state = new bool[N]{ false };
    }

    ~BHPool() { if(buffer) delete[] buffer; }

    void* alloc()
    {
        for(std::size_t i = 0; i < N; i++)
        {
            if(!state[i])
            {
                state[i] = true;
                return &buffer[sizeof(T) * i];
            }
        }
        throw std::bad_alloc();
    }

    void free(void* p)
    {
        for(std::size_t i = 0; i < N; i++)
        {
            if(&buffer[sizeof(T) * i] == p)
            {
                state[i] = false;
                return;
            }
        }
    }

private:
    unsigned char* buffer = nullptr;
    bool* state = nullptr;
    const std::size_t N;
};

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

// BHPool<BHNode> BHNode::MemoryPool(10000);

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
        ST_PROF;
        calculateNodeInsertion(body, root, 0);
    }
    
    PVector3 calculateFieldOnPoint(const PVector3& point, const float thr)
    {
        ST_PROF;
        // Traverse the tree with dfs and use a threshold of thr
        return calculateFieldOnPointDFS(point, thr, root);
    }

    void printNodes() const
    {
        printNode(root, 0);
    }

    unsigned long long computeNodeNumber() const
    {
        return countChildrenRecursive(root);
    }

private:
    unsigned long long countChildrenRecursive(BHNode* node) const
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

    void printNode(const BHNode* node, int depth) const
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

    void deleteNodes(BHNode* node)
    {
        for(std::size_t i = 0; i < 8; i++)
        {
            if(node->children[i]) deleteNodes(node->children[i]);
        }
        delete node;
    }

    PVector3 calculateFieldOnPointDFS(const PVector3& point, const float thr, const BHNode* node)
    {
        PVector3 field = {0.0f, 0.0f, 0.0f};
        constexpr float K = 30.0f;

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
            // field = (K * node->mass / (distance * distance)) * Vector3::Normalize(node->centerOfMassNorm - point);
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

    void calculateNodeInsertion(const Body* body, BHNode* node, unsigned long long depth)
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

    PVector3 calculateNodeCenter(const std::size_t nodeIndex, const BHNode* parent)
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

    std::size_t calculateNodeIndex(const PVector3& position, const BHNode* node)
    {
        std::size_t index = 0;
        index |= static_cast<std::size_t>(position.x >= node->nodeCenter.x) << 2;
        index |= static_cast<std::size_t>(position.y >= node->nodeCenter.y) << 1;
        index |= static_cast<std::size_t>(position.z >= node->nodeCenter.z);
        return index;
    }

    void spawnChildren(BHNode* parent, int depth)
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

private:
    BHNode* root;
};

class RenderWindow
{
public:
    RenderWindow(const std::string& name, bool maximized = true) : glfwOK(false), window(nullptr), width(1920), height(1080)
    {
        if(!glfwInit())
        {
            std::cerr << "Failed to init glfw." << std::endl;
            return;
        }

        glfwOK = true;

        glfwSetErrorCallback([](int error, const char* msg) {
            std::cerr << "glfw Error : " << error << " - " << msg << std::endl;
        });

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

        window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);

        if(!window)
        {
            std::cerr << "Failed to create window." << std::endl;
            return;
        }

        glfwMakeContextCurrent(window);
        gladLoadGL(glfwGetProcAddress);
        glfwSwapInterval(1);

        if(maximized)
        {
            glfwMaximizeWindow(window);
        }
    }

    RenderWindow(const RenderWindow& window) = delete;
    RenderWindow(RenderWindow&& window) = delete;

    ~RenderWindow()
    {
        if(window)
        {
            glfwDestroyWindow(window);
        }

        if(glfwOK)
        {
            glfwTerminate();
        }
    }

    bool initOK() const
    {
        return (window != nullptr) && glfwOK;
    }

    bool windowOpen() const
    {
        return !glfwWindowShouldClose(window);
    }

    void render()
    {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

private:
    bool glfwOK;
    GLFWwindow* window;
    int width;
    int height;
};

int main(void)
{
    RenderWindow rwindow("starwell v0.1");
    
    if(rwindow.initOK())
    {
        while(rwindow.windowOpen())
        {
            rwindow.render();
        }
    }

    std::vector<Body> bodies;
    BHTree tree;

    std::random_device dev;
    std::mt19937 mt(dev());
    std::normal_distribution<float> distX(-20.0f, 5.0f);
    std::normal_distribution<float> distY(0.0f, 0.1f);
    std::normal_distribution<float> distZ(0.0f, 5.0f);

    for(int i = 0 ; i < 1000; i++)
    {
        bodies.push_back(Body({
            distX(mt),
            distY(mt),
            distZ(mt)
        }, {0.0f, 50.0f, 0.0f}));
    }

    std::normal_distribution<float> distX1(20.0f, 5.0f);
    std::normal_distribution<float> distY1(0.0f, 0.1f);
    std::normal_distribution<float> distZ1(0.0f, 5.0f);

    for(int i = 0 ; i < 1000; i++)
    {
        bodies.push_back(Body({
            distX1(mt),
            distY1(mt),
            distZ1(mt)
        }, {0.0f, -50.0f, 0.0f}));
    }
    
    // for(int i = 0; i < 10; i++)
    // {
    //     for(int j = 0; j < 10; j++)
    //     {
    //         for(int k = 0; k < 10; k++)
    //         {
    //             float x = (i / 10.0f) * 100.0f - 50.0f;
    //             float y = (j / 10.0f) * 100.0f - 50.0f;
    //             float z = (k / 10.0f) * 100.0f - 50.0f;
    //             bodies.push_back(Body({x, y, z}));
    //         }
    //     }
    // }

    std::ofstream file("output.csv");
    file << "x, y, z, frame\n";
    
    for(int i = 0; i < 100; i++)
    {
        for(auto& body : bodies)
        {
            tree.insertBody(&body);
            PVector3 pos = body.getPosition();
            file << pos.x << "," << pos.y << "," << pos.z << "," << i << "\n";
        }

        // tree.printNodes();
        // std::cout << tree.computeNodeNumber() << std::endl;
        // if(i == 0) break; 
        for(auto& body : bodies)
        {
            PVector3 field = tree.calculateFieldOnPoint(body.getPosition(), 0.5f);
            body.move(field);
            // std::cout << "field = " << field << std::endl;
        }
        tree.reset();
        std::cout << "Frame " << i << "..." << std::endl;
    }

    std::cout << cag::PerfTimer::GetCallTreeString(cag::PerfTimer::GetCallTree()) << std::endl;
    
    return 0;
}
