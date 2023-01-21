#shader vertex
#version 330 core
layout(location = 0) in vec2 aPos;   // the position variable has attribute position 0

void main()
{
    gl_Position = vec4(aPos.x,aPos.y, 0.0, 1.0);
}


#shader fragment
#version 330 core

out vec4 FragColor;
void main()
{
    FragColor =vec4(1.0f, 1.0f, 1.0f, 1.0f);
}