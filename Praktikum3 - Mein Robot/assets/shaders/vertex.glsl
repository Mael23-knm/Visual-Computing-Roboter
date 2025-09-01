#version 330 core

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 vertex_normal;

out vec3 colorVS;
out float animation;
out vec3 Normal, FragPos, LightDir, ViewDir;

uniform mat4 model;
uniform mat4 rotation;
uniform mat4 projection;
uniform mat4 view;
uniform vec3 vertexColor;

// Beleuchtung
uniform vec3 lightPos;

void main() {
    // Transform the vertex position
    vec4 v = vec4(vertex, 1.0);
    gl_Position = projection * view * model * rotation * v;

    // Pass data to the fragment shader
    colorVS = vertexColor;
    animation = vertex.x + vertex.y; // or any other animation-related value

    // Compute normal in view space
    vec4 n = vec4(vertex_normal, 1.0);
    mat4 normalMat = transpose(inverse(view * model));
    Normal = (normalMat * n).xyz;

    // Compute light direction in view space
    vec4 lp = view * vec4(lightPos, 1.0);
    vec4 P = (view * model * vec4(vertex, 1.0));
    LightDir = (lp - P).xyz;

    // Compute view direction in view space
    ViewDir = -P.xyz;

    // Pass fragment position to the fragment shader
    FragPos = P.xyz;
}
