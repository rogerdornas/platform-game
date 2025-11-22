// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
//
// Released under the BSD License
// See LICENSE.txt for full details.
// ----------------------------------------------------------------

// Request GLSL 3.3
#version 330

// This corresponds to the output color to the color buffer
out vec4 outColor;
uniform vec3 uColor;

// This is used for the texture sampling
uniform sampler2D uTexture;

// This is used for texture blending
uniform float uTextureFactor;

// Transparência global
uniform float uAlpha;

// Nível de congelamento (0.0 = normal, 1.0 = totalmente congelado)
uniform float uFreezeLevel;

// Iluminação
uniform vec3 uAmbientColor;
uniform float uAmbientIntensity;
uniform bool uEnableLighting;

#define MAX_LIGHTS 80
uniform int uNumLights;

struct Light {
    vec2 position;  // posição 2D da luz
    vec3 color;     // cor da luz
    float intensity; // força da luz
    float radius;   // raio de influência
};
uniform Light uLights[MAX_LIGHTS];

// Tex coord input from vertex shader
in vec2 fragTexCoord;
in vec2 fragWorldPos;

void main()
{
    // Sample color from texture
    vec4 texColor = texture(uTexture, fragTexCoord);
    vec3 baseColor = mix(uColor, texColor.rgb, uTextureFactor);

    // Congelamento
    if (uFreezeLevel > 0.0)
    {
        // Converte a cor atual para escala de cinza (Luminosidade)
        float gray = dot(baseColor, vec3(0.299, 0.587, 0.114));

        // Define a cor do "Gelo" (Ciano Claro).
        // Multiplicamos gray por 1.5 para aumentar o brilho/reflexo do gelo.
        vec3 iceTint = vec3(0.5, 0.8, 1.0) * (gray * 1.5);

        // 3. Mistura a cor original com a cor de gelo baseada no nível (0.0 a 1.0)
        baseColor = mix(baseColor, iceTint, uFreezeLevel);
    }

    // Combina alpha global com alpha da textura
    float finalAlpha = texColor.a * uAlpha;

    if (!uEnableLighting)
    {
        outColor = vec4(baseColor, finalAlpha);
        return;
    }

    // Iluminação ambiente
    vec3 lighting = uAmbientColor * uAmbientIntensity;

    // Iluminação pontual (distância 2D)
    for (int i = 0; i < uNumLights; ++i)
    {
        float dist = length(fragWorldPos - uLights[i].position);

        float attenuation = smoothstep(uLights[i].radius, 0.0, dist);

        lighting += uLights[i].color * uLights[i].intensity * attenuation;
    }

    lighting = clamp(lighting, vec3(0.0), vec3(1.3));
    outColor = vec4(baseColor * lighting, finalAlpha);
}
