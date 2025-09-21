#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;


uniform mat4 u_model;      
uniform mat4 u_view;
uniform mat4 u_projection;


out vec3 vWorldPos;
out vec3 vNormal;

void main()
{
    
    vWorldPos = aPos;
    vNormal = aNormal;
    gl_Position = u_projection * u_view * vec4(vWorldPos, 1.0);
}

//------------------------------------------------------------------

#shader geometry
#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;


in vec3 vWorldPos[];
in vec3 vNormal[];


out vec3 gWorldPos; 
out vec3 gNormal;
out float gDepth;


uniform vec3 u_cameraPos; 

void main() {
    for (int i = 0; i < 3; i++) {
        
        gWorldPos = vWorldPos[i]; 
        gNormal = vNormal[i];     
        gDepth = distance(u_cameraPos, vWorldPos[i]); 
        gl_Position = gl_in[i].gl_Position; 
        EmitVertex();
    }
    EndPrimitive();
}



#shader fragment
#version 330 core
// Data received from the Geometry Shader
in vec3 gWorldPos;
in vec3 gNormal;
in float gDepth;

out vec4 FragColor;


uniform vec3 u_viewPos;      
uniform vec3 u_lightPos;     
uniform vec3 u_lightColor;   
float u_shininess=32;  
uniform int phongEnabled;

// Uniforms for depth coloring
uniform float minDepth;
uniform float maxDepth;


vec3 depthToColor(float normalizedDepth) {
    
    return mix(vec3(0.0, 0.0, 1.0), vec3(1.0, 0.0, 0.0), normalizedDepth);
}

void main()
{
    
   
    float range = maxDepth - minDepth;
    float normalizedDepth = clamp((gDepth - minDepth) / range, 0.0, 1.0);
    vec3 objectColor = depthToColor(normalizedDepth);
    
    
    vec3 norm = normalize(gNormal);
    vec3 lightDir = normalize(u_lightPos - gWorldPos);
    vec3 viewDir = normalize(u_viewPos - gWorldPos);
    
    
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * u_lightColor;
    
   
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * u_lightColor;
    
    
    float specularStrength = 0.5;  
    float shininess = 32.0;
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * u_lightColor;
    
    vec3 result;
    if(phongEnabled == 1)
    {
        result = (ambient + diffuse + specular) * objectColor;
    }
    else
    {
        result = objectColor;
    }
    
    FragColor = vec4(result, 1.0);
   
}