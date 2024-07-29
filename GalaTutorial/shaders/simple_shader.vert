#version 450

// position is an attribure. It takes its value from a vertex buffer.
layout(location=0) in vec2 position;
 
void main()
{
    gl_Position = vec4(position, 0.0, 1.0);
}
