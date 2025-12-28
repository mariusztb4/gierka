#version 330 core

layout (location = 0) in vec2 aPos;      // 0..1
layout (location = 1) in vec2 aTex;      // 0..1

uniform vec2 uPosition;   // pozycja w pikselach
uniform vec2 uSize;       // rozmiar w pikselach
uniform vec2 uScreen;     // rozmiar ekranu
uniform vec4 uColor;

out vec2 TexCoord;

void main()
{
    // zamiana z pikseli na NDC
    vec2 pos = uPosition + aPos * uSize;

    vec2 ndc;
    ndc.x = (pos.x / uScreen.x) * 2.0 - 1.0;
    ndc.y = (pos.y / uScreen.y) * 2.0 - 1.0;

    gl_Position = vec4(ndc, 0.0, 1.0);
    TexCoord = aTex;
}
