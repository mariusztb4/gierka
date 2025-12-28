#version 330 core

in vec2 TexCoord;
in vec3 LocalPos;

out vec4 FragColor;

uniform sampler2D bodyTexture;
uniform int useObjectColor;
uniform vec3 objectColor;

void main()
{
    if (useObjectColor == 1)
    {
        // Przeciwnicy/obiekty - czysty kolor
        FragColor = vec4(objectColor, 1.0);
    }
    else
    {
        // Gracz - tekstura + czarna głowa
        if (LocalPos.y > 3.1)
            FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        else
            FragColor = texture(bodyTexture, TexCoord);
    }
}