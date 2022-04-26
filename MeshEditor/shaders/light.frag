#version 330 core
out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 

in vec3 FragPos;  
in vec3 Normal;  
  
uniform vec3 viewPos;
uniform Material material;
uniform vec3 inColor;
uniform bool isSelected;

void main()
{
    

    if(isSelected){
    FragColor = vec4(0.972, 0.650, 0.407,1);
    }
    else
    {
    FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
}
