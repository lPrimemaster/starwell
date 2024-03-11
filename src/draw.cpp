#include <cmath>
#include <fstream>

#include "../include/draw.h"

GenShader::GenShader() : activeProgram(0)
{
    loadShaderCache();
}

GenShader::~GenShader()
{
    for(auto& shader : shaderCache)
    {
        glDeleteProgram(shader.second);
    }
}

bool GenShader::swap(const std::string& name) const
{
    auto shader = shaderCache.find(name);

    if(shader == shaderCache.end())
    {
        return false;
    }

    activeProgram = shader->second;
    glUseProgram(activeProgram);
    return true;
}

void GenShader::setRenderTarget()
{

}

void GenShader::loadShaderCache()
{
    std::ifstream input("shaders.glsl");
    for(std::string line; std::getline(input, line);)
    {
        if(line.substr(0, 3) == "---")
        {
            std::size_t colon = line.find(":");
            std::string shaderName = line.substr(4, colon - 4);
            std::string shaderType = line.substr(colon+1);

            std::string shaderBuffer;
            bool isVertexShader = (shaderType == "VTX");
            while(std::getline(input, line) && !line.starts_with('-'))
            {
                shaderBuffer += line;
                shaderBuffer += '\n';
            }
            GLuint shaderid = glCreateShader(isVertexShader ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
            const char* shaderSource = shaderBuffer.c_str();
            glShaderSource(shaderid, 1, &shaderSource, nullptr);
            glCompileShader(shaderid);

            GLint status;
            glGetShaderiv(shaderid, GL_COMPILE_STATUS, &status);
            if(status == GL_FALSE)
            {
                GLint bufLen;
                glGetShaderiv(shaderid, GL_INFO_LOG_LENGTH, &bufLen);
                if (bufLen > 0)
                {
                    std::vector<char> buffer(bufLen+1);
                    glGetShaderInfoLog(shaderid, bufLen, NULL, &buffer[0]);
                    std::cerr << "Shader [@" << shaderName << ":" << shaderType << "]: COMPILE ERROR." << std::endl;
                    std::cerr << std::string(&buffer[0]) << std::endl;
                }
            }

            shaderCache.emplace(shaderName + "_" + shaderType, shaderid);
        }
    }
    
    std::vector<std::pair<std::string, GLuint>> tempPrograms;
    tempPrograms.reserve(shaderCache.size() / 2);
    for(auto& shaderKVP : shaderCache)
    {
        if(shaderKVP.first.find("VTX") != std::string::npos)
        {
            std::string shaderName = shaderKVP.first.substr(0, shaderKVP.first.find("_"));
            GLuint vshader = shaderKVP.second;
            GLuint fshader = shaderCache[shaderName + "_FRG"];
            GLuint program = glCreateProgram();
            glAttachShader(program, vshader);
            glAttachShader(program, fshader);
            glLinkProgram(program);

            glDeleteShader(vshader);
            glDeleteShader(fshader);

            GLint status;
            glGetProgramiv(program, GL_LINK_STATUS, &status);
            if(status == GL_FALSE)
            {
                GLint bufLen;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLen);
                if (bufLen > 0)
                {
                    std::vector<char> buffer(bufLen+1);
                    glGetProgramInfoLog(program, bufLen, NULL, &buffer[0]);
                    std::cerr << "Shader [@" << shaderName << "]: LINK ERROR." << std::endl;
                    std::cerr << std::string(&buffer[0]) << std::endl;
                    continue;
                }
            }

            std::cout << "Shader [@" << shaderName << "]: OK." << std::endl;
            tempPrograms.push_back(std::make_pair(shaderName, program));
        }
    }
    
    shaderCache.clear();
    for(auto& program : tempPrograms)
    {
        shaderCache.emplace(program.first, program.second);
    }
}


InstanceState::InstanceState()
{
    // Default billboard vertex buffer
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &billboard);
    glBindBuffer(GL_ARRAY_BUFFER, billboard);

    static const GLfloat vertex_data[] = {
         -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         -0.5f, 0.5f, 0.0f,
         0.5f, 0.5f, 0.0f,
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);

    glGenBuffers(1, &position);
    glBindBuffer(GL_ARRAY_BUFFER, position);
    glBufferData(GL_ARRAY_BUFFER, MAX_INSTANCE_PARTICLES * 3 * sizeof(GLfloat), nullptr, GL_STREAM_DRAW);

    glGenBuffers(1, &color);
    glBindBuffer(GL_ARRAY_BUFFER, color);
    glBufferData(GL_ARRAY_BUFFER, MAX_INSTANCE_PARTICLES * 4 * sizeof(GLubyte), nullptr, GL_STREAM_DRAW);

    setAttribute(0, billboard, 3, GL_FLOAT, false);
    setAttribute(1, position, 3, GL_FLOAT, true);
    setAttribute(2, color, 4, GL_UNSIGNED_BYTE, true);
}

InstanceState::~InstanceState()
{
    glDeleteBuffers(1, &billboard);
    glDeleteBuffers(1, &position);
    glDeleteBuffers(1, &color);
    glDeleteVertexArrays(1, &vao);
}

void InstanceState::updatePositions(const std::vector<PVector3>* p)
{
    glBindBuffer(GL_ARRAY_BUFFER, position);
    glBufferData(GL_ARRAY_BUFFER, MAX_INSTANCE_PARTICLES * 3 * sizeof(GLfloat), nullptr, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, p->size() * 3 * sizeof(GLfloat), p->data());
    particleCount = p->size();
}

void InstanceState::updateColors(const std::vector<UVector4>* c)
{
    glBindBuffer(GL_ARRAY_BUFFER, color);
    glBufferData(GL_ARRAY_BUFFER, MAX_INSTANCE_PARTICLES * 4 * sizeof(GLubyte), nullptr, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, c->size() * 4 * sizeof(GLubyte), c->data());
}

void InstanceState::draw() const
{
    if(particleCount < 1) return;

    glBindVertexArray(vao);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particleCount);
}

void InstanceState::setAttribute(int location, GLuint buffer, int size, GLenum type, bool perInstance) const
{
    glBindVertexArray(vao);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribPointer(location, size, type, (type == GL_FLOAT) ? GL_FALSE : GL_TRUE, 0, (void*)0);
    glVertexAttribDivisor(location, perInstance ? 1 : 0);
}
