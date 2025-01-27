#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec2 aTexCoords; 

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords; 
out vec3 LightingColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

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
uniform vec3 viewPos;
uniform Material material;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform int sceltaShader;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    TexCoords = aTexCoords;    
    LightingColor = vec3(0.0f); 

    //Gourad shading
    if (sceltaShader == 2 || sceltaShader == 3) {
        vec3 result = vec3(0.0);
        vec3 norm = normalize(Normal);
        vec3 viewDir = normalize(viewPos - FragPos);

        for (int i = 0; i < NR_POINT_LIGHTS; i++) {
            vec3 lightDir = normalize(pointLights[i].position - FragPos);

            vec3 ambient = pointLights[i].ambient * material.ambient;

            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = pointLights[i].diffuse * (diff * material.diffuse);

            float spec;
            if (sceltaShader == 2) // Phong-Gouraud
            { 
                vec3 reflectDir = reflect(-lightDir, norm);  
                spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
            } 
            else if (sceltaShader == 3) // Blinn-Phong-Gouraud
            { 
                vec3 halfwayDir = normalize(lightDir + viewDir);
                spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
            }
            vec3 specular = pointLights[i].specular * (spec * material.specular);

            result += ambient + diffuse + specular;
        }
        LightingColor = result; 
    }
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
