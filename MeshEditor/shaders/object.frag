#version 330 core
out vec4 FragColor;
/*
struct Map {
    sampler2D ambient;
    bool ambientEnabled;
    sampler2D diffuse;
    sampler2D specular;  
};*/

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

    sampler2D diffuseMap;
    bool diffuseMapEnabled;

    sampler2D specularMap;
    bool specularMapEnabled;

    sampler2D normalMap;
    bool normalMapEnabled;
}; 

struct PointLight {
    bool enabled;
    vec3 position;

    vec3 ambient;
    bool ambientEnabled;
    vec3 diffuse;
    bool diffuseEnabled;
    vec3 specular;
    bool specularEnabled;
};

#define NR_POINT_LIGHTS 2 
uniform int pointLightCount;
uniform PointLight pointLights[NR_POINT_LIGHTS];

in vec3 FragPos;  
in vec3 Normal;
in vec2 TexCoords;  
  
uniform vec3 viewPos;
uniform Material material;
//uniform Map map;
uniform bool lightingEnabled;
uniform bool isSelected;

vec3 calcAmbient(PointLight light)
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

vec3 calcDiffuse(PointLight light, vec3 norm, vec3 lightDir)
{
    vec3 diffuse = vec3(0.0, 0.0, 0.0);

    if(material.diffuseEnabled)
    {
        if(light.enabled)
        {
            float diff = max(dot(norm, lightDir), 0.0);
            diffuse = diff * material.diffuse;

            if(material.diffuseMapEnabled)
            {
                diffuse = diff * material.diffuse * texture(material.diffuseMap, TexCoords).rgb;
            }

            if(light.diffuseEnabled)
            {
                diffuse *= light.diffuse;
            }
        }
        else
        {
            //diffuse = material.diffuse;
        }
    }
    

    return diffuse;
}

vec3 calcSpecular(PointLight light, vec3 norm, vec3 lightDir)
{
    vec3 specular = vec3(0.0, 0.0, 0.0);

    if(material.specularEnabled)
    {
        if(light.enabled)
        {
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 reflectDir = reflect(-lightDir, norm);
       
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
            specular = spec * material.specular;

            if(material.specularMapEnabled)
            {
                specular = spec * material.specular * texture(material.specularMap, TexCoords).rgb;
            }

            if(light.specularEnabled)
            {
                specular *= light.specular;
            }
        }
    }
    

    return specular;
}

vec3 calcEmission(PointLight light)
{
    vec3 emission = vec3(0.0, 0.0, 0.0);

    if(material.emissionEnabled)
    {
        emission = material.emission;
    }

    return emission;
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos)
{
    vec3 lightDir = normalize(light.position - fragPos);

    vec3 result = vec3(0.0, 0.0, 0.0);
    result += calcAmbient(light);
    result += calcDiffuse(light, normal, lightDir);
    result += calcSpecular(light, normal, lightDir);
    result += calcEmission(light);

    return result;//(ambient + diffuse + specular);
}

void main()
{
    vec3 result = vec3(0.0, 0.0, 0.0);

    vec3 norm = vec3(0.0, 0.0, 0.0);
    if(material.normalMapEnabled)
    {
        norm = texture(material.normalMap, TexCoords).rgb;
        norm = normalize(norm * 2.0 - 1.0);
    }
    else
    {
        norm = normalize(Normal);
    }
    
if(lightingEnabled)
{
    for(int i = 0; i < pointLightCount; i++)
    {
        if(i < NR_POINT_LIGHTS)
        result += CalcPointLight(pointLights[i], norm, FragPos);
    }
}
else
{
    if(material.diffuseEnabled)
    {
        result =material.diffuse;
    }

    if(material.diffuseMapEnabled)
    {
        result =texture(material.diffuseMap, TexCoords).rgb;
    }
}

    
    if(isSelected)
    {
        FragColor = vec4(0.972, 0.650, 0.407,1);
    }
    else
    {
        FragColor = vec4(result,1);
    }
}
