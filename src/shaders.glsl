--- classic:VTX
#version 450

uniform mat4 MVP;
uniform float sf;
layout(location = 0) in vec3 meshPos;
layout(location = 1) in vec3 partPos;
layout(location = 2) in vec4 partCol;

out vec4 color;

void main()
{
    gl_Position = MVP * vec4(sf * meshPos + partPos, 1.0);
    // gl_Position = MVP * vec4(sf * meshPos + partPos, 1.0);
    color = partCol;
}
---

--- classic:FRG
#version 450
in vec4 color;
out vec4 fcolor;

void main()
{
    fcolor = color;
}
---
