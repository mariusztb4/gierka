#version 330 core
out vec4 FragColor;

in vec2 v_localPos;

uniform vec4 u_color;
uniform float u_progress; // 0.0 - 1.0
uniform float u_radius;   // np. 0.05 dla ma³ego zaokr¹glenia

void main()
{
    // maska dla lewej i prawej strony
    float maskLeft  = smoothstep(0.0, u_radius, v_localPos.x);
    float maskRight = smoothstep(u_progress, u_progress - u_radius, v_localPos.x);

    float mask = min(maskLeft, maskRight);

    FragColor = u_color * mask;
}