#version 330 core

in vec2 TexCoord;
in vec3 WorldPos;
in vec4 FragPosLightSpace;  // DODANE: dla cieni

out vec4 FragColor;

uniform sampler2D terrainTexture;
uniform sampler2D shadowMap;  // DODANE: mapa cieni
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 viewPos;

// DODANE: Funkcja obliczaj¹ca cieñ
float ShadowCalculation(vec4 fragPosLightSpace)
{
    // Perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    
    // Jeœli poza shadow map, nie ma cienia
    if(projCoords.z > 1.0)
        return 0.0;
    
    // Pobierz najbli¿sz¹ g³êbokoœæ z shadow map
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    
    // Pobierz g³êbokoœæ obecnego fragmentu
    float currentDepth = projCoords.z;
    
    // SprawdŸ czy fragment jest w cieniu (z bias aby unikn¹æ shadow acne)
    float bias = 0.005;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    
    // PCF (Percentage Closer Filtering) dla miêkkiego cienia
    shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    
    return shadow;
}

void main()
{
    // Tekstura bazowa
    vec3 baseColor = texture(terrainTexture, TexCoord * 0.1).rgb;
    
    // Normalna skierowana w górê dla p³askiego terenu
    vec3 normal = vec3(0.0, 1.0, 0.0);
    
    // Ambient
    float ambientStrength = 0.4;
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse
    vec3 lightDirection = normalize(-lightDir);
    float diff = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor * 0.6;
    
    // DODANE: Oblicz cieñ
    float shadow = ShadowCalculation(FragPosLightSpace);
    
    // £¹czny wynik - ZMIENIONE: odjêcie cienia od diffuse
    vec3 result = (ambient + (1.0 - shadow) * diffuse) * baseColor;
    
    FragColor = vec4(result, 1.0);
}