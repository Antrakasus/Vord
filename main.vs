#version 430 core
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) uniform vec3 cameraPosition;

void main(){
    gl_Position.xyz = vertexPosition_modelspace-cameraPosition;
    gl_Position.w = gl_Position.z;
}