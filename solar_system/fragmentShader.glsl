#version 330 core
out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 2
in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords; 
  
uniform vec3 viewPos;
uniform Material material;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform sampler2D texture_diffuse; 

void main()
{
    vec3 result = vec3(0.0f); 

    for (int i = 0; i < NR_POINT_LIGHTS; i++) {
        vec3 ambient = pointLights[i].ambient * material.ambient;
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(pointLights[i].position - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = pointLights[i].diffuse * (diff * material.diffuse);
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = pointLights[i].specular * (spec * material.specular);  
        result += ambient + diffuse + specular;
    }
    
    vec4 textureColor = texture(texture_diffuse, TexCoords); 
    //FragColor = mix(vec4(result, 1.0), texture(texture_diffuse, TexCoords), 1.0)
    FragColor = vec4(result, 1.0) * textureColor; 
}
