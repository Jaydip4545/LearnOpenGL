#shader vertex
#version 330 core
layout(location = 0) in vec2 aPos;   // the position variable has attribute position 0
layout(location = 1) in vec2 tCoords;

out vec2 v_tCoords;
        
void main()
{
    gl_Position = vec4(aPos.x,aPos.y,0.0,1.0);
    v_tCoords = tCoords;
   
}


#shader fragment
#version 330 core

in vec2 v_tCoords;
out vec4 FragColor;

uniform vec4 in_color;
uniform sampler2D u_Texture;

void main()
{
    FragColor = texture(u_Texture,v_tCoords);// vec4(1.0f, 0.0f, 0.0f, 1.0f);
}