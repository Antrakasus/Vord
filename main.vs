#version 430 core
layout(location = 0) in vec3 vertexPosition;

/*
in mat4 scale;
in mat4 translation;
in mat4 cameraTranslation;
in mat4 rotation;
in mat4 cameraRotation
*/
void main(){
    gl_Position = /*translation * rotation * scale */ vec4(vertexPosition, 1);
}