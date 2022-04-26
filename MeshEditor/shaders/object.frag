#version 330 core
out vec4 FragColor;

struct Map {
    sampler2D ambient;
    bool ambientEnabled;
    sampler2D diffuse;
    sampler2D specular;  
};

struct Material {
    vec3 ambient;
    bool ambientEnabled;

    vec3 diffuse;
    bool diffuseEnabled;
    
    vec3 specular;
    bool specularEnabled;

    float shininess;
    

    vec3 emission;
    bool emissionEnabled;
}; 

struct Light {
    bool enabled;
    vec3 position;

    vec3 ambient;
    bool ambientEnabled;
    vec3 diffuse;
    bool diffuseEnabled;
    vec3 specular;
    bool specularEnabled;


};

in vec3 FragPos;  
in vec3 Normal;
in vec2 TexCoords;  
  
uniform vec3 viewPos;
uniform Material material;
uniform Map map;
uniform Light light;
uniform bool isSelected;

vec3 calcAmbient()
{
    vec3 ambient = vec3(0.0, 0.0, 0.0);

    if(material.ambientEnabled)
    {
        if(light.enabled)
        {
            ambient = material.ambient;

            if(light.ambientEnabled)
            {
                ambient *= light.ambient;
            }
        }
    }

    return ambient;
}

vec3 calcDiffuse()
{
    vec3 diffuse = vec3(0.0, 0.0, 0.0);

    if(material.diffuseEnabled)
    {
        if(light.enabled)
        {
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(light.position - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            diffuse = diff * material.diffuse;

            if(light.diffuseEnabled)
            {
                diffuse *= light.diffuse;
            }
        }
        else
        {
            diffuse = material.diffuse;
        }
    }

    return diffuse;
}

vec3 calcSpecular()
{
    vec3 specular = vec3(0.0, 0.0, 0.0);

    if(material.specularEnabled)
    {
        if(light.enabled)
        {
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(light.position - FragPos);
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 reflectDir = reflect(-lightDir, norm);
       
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
            specular = spec * material.specular;

            if(light.specularEnabled)
            {
                specular *= light.specular;
            }
        }
    }

    return specular;
}

vec3 calcEmission()
{
    vec3 emission = vec3(0.0, 0.0, 0.0);

    if(material.emissionEnabled)
    {
        emission = material.emission;
    }

    return emission;
}

void main()
{
    vec3 result = vec3(0.0, 0.0, 0.0);
    
    if(material.ambientEnabled)
    {
        result += calcAmbient();
    }

    if(material.diffuseEnabled)
    {
        result += calcDiffuse();
    }

    if(material.specularEnabled)
    {
        result += calcSpecular();
    }

    if(material.emissionEnabled)
    {
        result += calcEmission();
    }

    FragColor = vec4(result,1);
}
