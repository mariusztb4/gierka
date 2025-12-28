#version 330 core

in vec2 TexCoord;
in vec3 WorldPos;

out vec4 FragColor;

uniform sampler2D terrainTexture;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 viewPos;

void main()
{
    // Tekstura bazowa
    vec3 baseColor = texture(terrainTexture, TexCoord * 0.1).rgb;
    
    // Normalna skierowana w górê dla p³askiego terenu
    vec3 normal = vec3(0.0, 1.0, 0.0);
    
    // Ambient (œwiat³o otaczaj¹ce)
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse (œwiat³o rozproszone)
    vec3 lightDirection = normalize(-lightDir);
    float diff = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular (odbicie lustrzane) - s³abe dla trawy
    vec3 viewDir = normalize(viewPos - WorldPos);
    vec3 reflectDir = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0);
    vec3 specular = 0.1 * spec * lightColor;
    
    // £¹czny wynik
    vec3 result = (ambient + diffuse + specular) * baseColor;
    
    FragColor = vec4(result, 1.0);
}