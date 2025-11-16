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

//        float linearFalloff = clamp(1.0 - dist / uLights[i].radius, 0.0, 1.0);
//        float attenuation = linearFalloff * linearFalloff;

        float attenuation = smoothstep(uLights[i].radius, 0.0, dist);
        lighting += uLights[i].color * uLights[i].intensity * attenuation;
    }

    outColor = vec4(baseColor * lighting, finalAlpha);
}
