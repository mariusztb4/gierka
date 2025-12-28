#version 330 core
in vec2 fragCoord;
out vec4 FragColor;

uniform vec4 u_color;
uniform vec2 u_size;
uniform float u_radius;

void main()
{
    vec2 coord = fragCoord * u_size;
    vec2 halfSize = u_size * 0.5;
    
    // Odleg³oœæ od œrodka prostok¹ta
    vec2 dist = abs(coord - halfSize) - (halfSize - u_radius);
    
    // Wyg³adzona krawêdŸ (antialiasing)
    float d = length(max(dist, 0.0)) - u_radius;
    float alpha = 1.0 - smoothstep(-1.0, 1.0, d);
    
    FragColor = vec4(u_color.rgb, u_color.a * alpha);
}