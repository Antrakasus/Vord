#version 430 core
layout(location = 0) in vec3 vertexPosition;

uniform mat4 scale;
uniform mat4 translation;
uniform mat4 rotation;

in vec4 vertexColor;

out vec4 fragColor;

void main(){
    fragColor=vertexColor;
    gl_Position = translation * /*rotation * scale */ vec4(vertexPosition,10);
}