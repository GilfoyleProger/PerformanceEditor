#version 330 core
out vec4 FragColor;

uniform vec4 objectColor;

void main()
{
    FragColor = objectColor;// vec4(1.0,0,0,1); // set alle 4 vector values to 1.0
}