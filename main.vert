#version 330 core
layout(location = 0) in vec3 vertexPosition_modelspace;
loyout(location = 1) uniform vec3 cameraPosition;

void main(){
    gl_Position.xyz = vertexPosition_modelspace-cameraPos;
    gl_Position.w = 2.0;
}