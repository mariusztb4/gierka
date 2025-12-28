#version 330 core
layout(location = 0) in vec2 aPos;

uniform mat4 u_proj;
uniform vec2 u_pos;
uniform vec2 u_size;

out vec2 v_localPos; // przekazujemy pozycjê lokaln¹ do fragment shadera

void main()
{
    vec2 pos = aPos * u_size + u_pos;
    gl_Position = u_proj * vec4(pos, 0.0, 1.0);

    // normalizowana lokalna wspó³rzêdna x od 0 (lewy koniec) do 1 (prawy koniec)
    v_localPos = aPos * 0.5 + 0.5; 
}