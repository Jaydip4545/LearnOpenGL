#shader vertex
#version 330 core
layout (location = 0) in vec4 aPos;
void main()
{
   gl_Position = aPos;
};


#shader fragment
#version 330 core
out vec4 color;
uniform vec4 in_color;
void main(void)
{
    color =in_color;
}