#version 330 core

out vec4 FragColor;

uniform vec4 uColor;   // kolor t³a konsoli (RGBA)

void main()
{
    FragColor = uColor;
}
