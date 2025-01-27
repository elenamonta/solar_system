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
in vec3 LightingColor;

uniform vec3 viewPos;
uniform Material material;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform sampler2D texture_diffuse; 
uniform float mixFactor; 
uniform int sceltaShader;

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
        float spec;

        if(sceltaShader==2 || sceltaShader==3) //Gourad shading
        {
            result = LightingColor; 
        }
        else if(sceltaShader==0) //shading Phong
        {
            vec3 reflectDir = reflect(-lightDir, norm);  
            spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        }
        else if(sceltaShader==1) //shading Phong
        {
            vec3 halfwayDir = normalize(lightDir + viewDir);
            spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
        }
        
        vec3 specular = pointLights[i].specular * (spec * material.specular);  
        result += ambient + diffuse + specular;
    }
    
    vec4 textureColor = texture(texture_diffuse, TexCoords); 
    //FragColor = textureColor; 
    //FragColor = vec4(result, 1.0);
    //FragColor = mix(vec4(result, 1.0), textureColor, mixFactor);

    if (mixFactor == 1.0){
        FragColor = textureColor;
    }else{
        FragColor = vec4(result, 1.0);
    }

    
}
