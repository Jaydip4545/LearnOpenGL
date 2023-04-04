#shader vertex
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;// the position variable has attribute position 0
layout(location = 2) in vec2 aTexCoords;
out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
void main()
{
    gl_Position = u_projection * u_view * u_model * vec4(aPos,1.0);
    Normal = mat3(transpose(inverse(u_model))) * aNormal;
    FragPos = vec3(u_model * vec4(aPos, 1.0));
    TexCoords = aTexCoords;
    
}


#shader fragment
#version 330 core

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
out vec4 FragColor;

struct Material {
   
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
uniform Material material;

//light properties 

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;




uniform vec3 lightpos;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 viewPos;
void main()
{
    float ambientStrength = 1;
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightpos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff* vec3(texture(material.diffuse, TexCoords)) * light.diffuse;
    float specularStrength = 3.0;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = spec * light.specular * vec3(texture(material.specular, TexCoords));

    vec3 result = (ambient  + diffuse + specular);
    FragColor = vec4(result, 1.0);
    
}