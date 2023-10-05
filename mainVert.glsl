#version 430 core
layout(location = 0) in vec3 vertexPosition;


uniform mat4 scale;
uniform mat4 translation;
uniform mat4 rotation;

void main(){
    gl_Position.xyz = /*translation * rotation * scale */ vertexPosition;
    gl_Position.w=5;
}