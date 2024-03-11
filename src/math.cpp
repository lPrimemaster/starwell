#include <cmath>
#include <iostream>

#include "../include/math.h"


    
float PVector3::DistanceSqr(const PVector3& a, const PVector3& b)
{
    return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z); 
}

float PVector3::Distance(const PVector3& a, const PVector3& b)
{
    return std::sqrt(DistanceSqr(a, b));
}

float PVector3::InnerProduct(const PVector3& a, const PVector3& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float PVector3::Magnitude(const PVector3& v)
{
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

PVector3& PVector3::operator+=(const PVector3& a)
{
    this->x += a.x;
    this->y += a.y;
    this->z += a.z;
    return *this;
}

PVector3 operator*(float s, const PVector3& v)
{
    return PVector3 { s * v.x, s * v.y, s * v.z };
}

PVector3 operator*(const PVector3& a, const PVector3& b)
{
    return PVector3 {
        a.data[1]*b.data[2] - a.data[2]*b.data[1],
        a.data[2]*b.data[0] - a.data[0]*b.data[2],
        a.data[0]*b.data[1] - a.data[1]*b.data[0]
    };
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


    
float PVector4::DistanceSqr(const PVector4& a, const PVector4& b)
{
    return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z) + (a.w - b.w) * (a.w - b.w); 
}

float PVector4::Distance(const PVector4& a, const PVector4& b)
{
    return std::sqrt(DistanceSqr(a, b));
}

float PVector4::InnerProduct(const PVector4& a, const PVector4& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

float PVector4::Magnitude(const PVector4& v)
{
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

PVector4& PVector4::operator+=(const PVector4& a)
{
    this->x += a.x;
    this->y += a.y;
    this->z += a.z;
    this->w += a.w;
    return *this;
}

PVector4 operator*(float s, const PVector4& v)
{
    return PVector4 { s * v.x, s * v.y, s * v.z, s * v.w };
}

PVector4 operator/(const PVector4& v, float s)
{
    return PVector4 { v.x / s, v.y / s, v.z / s, v.w / s };
}

PVector4 operator+(const PVector4& a, const PVector4& b)
{
    return {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}

PVector4 operator-(const PVector4& a, const PVector4& b)
{
    return {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
}

PVector4 PVector4::Normalize(const PVector4& v)
{
    return v / Magnitude(v);
}

std::ostream& operator<<(std::ostream& cout, const PVector4& v)
{
    cout << "[" << v.x << " " << v.y << " " << v.z << " " << v.w << "]";
    return cout;
}



PVector4 PMatrix4::getRow(int i) const
{
    return PVector4 { data[0][i], data[1][i], data[2][i], data[3][i] };
}

PMatrix4 PMatrix4::Identity()
{
    PMatrix4 mat;
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            mat.data[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }
    return mat;
}

PMatrix4 PMatrix4::Translate(const PVector3& v)
{
    PMatrix4 mat = PMatrix4::Identity();
    
    mat.data[3][0] = v.x;
    mat.data[3][1] = v.y;
    mat.data[3][2] = v.z;

    return mat;
}

PMatrix4 PMatrix4::Rotate(float angle, const PVector3& axis)
{
    PMatrix4 mat;
    
    const float c = cos(angle);
    const float s = sin(angle);

    // Ensure axis is normalized
    const PVector3 N = PVector3::Normalize(axis);
    const PVector3 T = (1.0f - c) * N;

    mat.data[0][0] = c + T.x * axis.x;
    mat.data[0][1] = T.x * axis.y + s * axis.z;
    mat.data[0][2] = T.x * axis.z - s * axis.y;

    mat.data[1][0] = T.y * axis.x - s * axis.z;
    mat.data[1][1] = c + T.y * axis.y;
    mat.data[1][2] = T.y * axis.z + s * axis.x;

    mat.data[2][0] = T.z * axis.x + s * axis.y;
    mat.data[2][1] = T.z * axis.y - s * axis.x;
    mat.data[2][2] = c + T.z * axis.z;

    return mat;
}

PMatrix4 PMatrix4::Perspective(float fovRad, float aspect, float near, float far)
{
    PMatrix4 mat;
    const float a = 1.0f / std::tan(fovRad / 2.0f);

    mat.data[0][0] = a / aspect;
    mat.data[0][1] = 0.0f;
    mat.data[0][2] = 0.0f;
    mat.data[0][3] = 0.0f;

    mat.data[1][0] = 0.0f;
    mat.data[1][1] = a;
    mat.data[1][2] = 0.0f;
    mat.data[1][3] = 0.0f;

    mat.data[2][0] = 0.0f;
    mat.data[2][1] = 0.0f;
    mat.data[2][2] = -((far + near) / (far - near));
    mat.data[2][3] = -1.0f;

    mat.data[3][0] = 0.0f;
    mat.data[3][1] = 0.0f;
    mat.data[3][2] = -((2.0f * far * near) / (far - near));
    mat.data[3][3] = 0.0f;

    return mat;
}

PMatrix4 PMatrix4::LookAt(const PVector3& eye, const PVector3& center, const PVector3& up)
{
    PMatrix4 mat = PMatrix4::Identity();
    PVector3 f = PVector3::Normalize(center - eye);
    PVector3 s = PVector3::Normalize(f * up);
    PVector3 t = s * f;

    mat.data[0][0] = s.x;
    mat.data[1][0] = s.y;
    mat.data[2][0] = s.z;
    mat.data[0][1] = t.x;
    mat.data[1][1] = t.y;
    mat.data[2][1] = t.z;
    mat.data[0][2] =-f.x;
    mat.data[1][2] =-f.y;
    mat.data[2][2] =-f.z;
    mat.data[3][0] =-PVector3::InnerProduct(s, eye);
    mat.data[3][1] =-PVector3::InnerProduct(t, eye);
    mat.data[3][2] = PVector3::InnerProduct(f, eye);

    return mat;
}

PMatrix4 operator*(float s, const PMatrix4& v)
{
    return PMatrix4 { 
        PVector4 { s * v.r0.x, s * v.r0.y, s * v.r0.z, s * v.r0.w },
        PVector4 { s * v.r1.x, s * v.r1.y, s * v.r1.z, s * v.r1.w },
        PVector4 { s * v.r2.x, s * v.r2.y, s * v.r2.z, s * v.r2.w },
        PVector4 { s * v.r3.x, s * v.r3.y, s * v.r3.z, s * v.r3.w }
    };
}

PMatrix4 operator*(const PMatrix4& a, const PMatrix4& b)
{
    PMatrix4 result;
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            result.data[i][j] = 0.0f;
            for(int k = 0; k < 4; k++)
            {
                result.data[i][j] += a.data[k][j] * b.data[i][k];
            }
        }
    }
    return result;
}

PVector4 operator*(const PMatrix4& a, const PVector4& b)
{
    PVector4 result;
    for(int i = 0; i < 4; i++)
    {
        result.data[i] = 0.0f;
        for(int j = 0; j < 4; j++)
        {
            result.data[i] += a.data[j][i] * b.data[j];
        }
    }
    return result;
}

std::ostream& operator<<(std::ostream& cout, const PMatrix4& v)
{
    cout << v.r0 << std::endl;
    cout << v.r1 << std::endl;
    cout << v.r2 << std::endl;
    cout << v.r3 << std::endl;
    return cout;
}

float PRadians(float degrees)
{
    static constexpr float A2R = 3.141592654f / 180;
    return degrees * A2R;
}
