#pragma once
#include <cmath>
#include <iostream>

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
    
    static float DistanceSqr(const PVector3& a, const PVector3& b);
    static float Distance(const PVector3& a, const PVector3& b);
    static float InnerProduct(const PVector3& a, const PVector3& b);
    static float Magnitude(const PVector3& v);
    static PVector3 Normalize(const PVector3& v);

    PVector3& operator+=(const PVector3& a);
};

PVector3 operator*(float s, const PVector3& v);
PVector3 operator*(const PVector3& a, const PVector3& b);
PVector3 operator/(const PVector3& v, float s);
PVector3 operator+(const PVector3& a, const PVector3& b);
PVector3 operator-(const PVector3& a, const PVector3& b);

std::ostream& operator<<(std::ostream& cout, const PVector3& v);


struct PVector4
{
    union
    {
        struct
        {
            float x;
            float y;
            float z;
            float w;
        };
        float data[4];
    };
    
    static float DistanceSqr(const PVector4& a, const PVector4& b);
    static float Distance(const PVector4& a, const PVector4& b);
    static float InnerProduct(const PVector4& a, const PVector4& b);
    static float Magnitude(const PVector4& v);
    static PVector4 Normalize(const PVector4& v);

    PVector4& operator+=(const PVector4& a);
};

PVector4 operator*(float s, const PVector4& v);
PVector4 operator/(const PVector4& v, float s);
PVector4 operator+(const PVector4& a, const PVector4& b);
PVector4 operator-(const PVector4& a, const PVector4& b);

std::ostream& operator<<(std::ostream& cout, const PVector4& v);

struct PMatrix4
{
    union
    {
        struct
        {
            PVector4 r0;
            PVector4 r1;
            PVector4 r2;
            PVector4 r3;
        };
        float data[4][4];
    };

    PVector4 getRow(int i) const;

    static PMatrix4 Identity();
    static PMatrix4 Translate(const PVector3& v);
    static PMatrix4 Rotate(float angle, const PVector3& axis);
    static PMatrix4 Perspective(float fovRad, float aspect, float near, float far);
    static PMatrix4 LookAt(const PVector3& eye, const PVector3& center, const PVector3& up = PVector3{0.0f, 1.0f, 0.0f});

};

PMatrix4 operator*(float s, const PMatrix4& v);
PMatrix4 operator*(const PMatrix4& a, const PMatrix4& b);
PVector4 operator*(const PMatrix4& a, const PVector4& b);

std::ostream& operator<<(std::ostream& cout, const PMatrix4& v);

float PRadians(float degrees);
