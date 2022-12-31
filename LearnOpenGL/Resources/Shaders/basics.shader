#shader vertex
#version 330 core
layout(location = 0) in vec2 aPos;   // the position variable has attribute position 0
void main()
{
    gl_Position = vec4(aPos,0.0,1.0);
    
}


#shader fragment
#version 330 core
out vec4 FragColor;
uniform vec4 in_color;
void main()
{
    FragColor = in_color;// vec4(1.0f, 0.0f, 0.0f, 1.0f);
}