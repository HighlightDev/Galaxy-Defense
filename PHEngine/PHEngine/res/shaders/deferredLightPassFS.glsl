#version 440

#define GAMMA_CORRECTION
#define SHADING_MODEL_PBR
#define MAX_DIR_LIGHT_COUNT 2
#define MAX_POINT_LIGHT_COUNT 10
#define MAX_SPOTLIGHT_COUNT 10
#define SHADOWMAP_BIAS_DIR_LIGHT 0.005
#define SHADOWMAP_BIAS_POINT_LIGHT 0.55
#define SHADOWMAP_BIAS_SPOTLIGHT 0.25
#define PCF_SAMPLES_DIR_LIGHT 2
#define PCF_SAMPLES_POINT_LIGHT 3
#define PCF_SAMPLES_SPOTLIGHT 3
#define MAX_DIR_LIGHT_SHADOW_MAP_COUNT 2
#define MAX_POINT_LIGHT_SHADOW_MAP_COUNT 1
#define MAX_SPOTLIGHT_SHADOW_MAP_COUNT 2

const float INV_COUNT_PCF_DIR_LIGHT_SAMPLES = 1.0 / (((PCF_SAMPLES_DIR_LIGHT * 2) + 1) * ((PCF_SAMPLES_DIR_LIGHT * 2) + 1));
const float INV_COUNT_PCF_POINT_LIGHT_SAMPLES
    = 1.0 / (PCF_SAMPLES_POINT_LIGHT * PCF_SAMPLES_POINT_LIGHT * PCF_SAMPLES_POINT_LIGHT);
const float INV_COUNT_PCF_SPOT_LIGHT_SAMPLES = 1.0 / (((PCF_SAMPLES_SPOTLIGHT * 2) + 1) * ((PCF_SAMPLES_SPOTLIGHT * 2) + 1));

layout(location = 0) out vec4 FragColor;

uniform sampler2D gBuffer_Position;
uniform sampler2D gBuffer_Normal;
uniform sampler2D gBuffer_Albedo;
uniform sampler2D gBuffer_MetallicRoughness;
uniform sampler2D gBuffer_Emission;

uniform sampler2D DirLightShadowMaps[MAX_DIR_LIGHT_SHADOW_MAP_COUNT];
// IMPORTANT!! some vendors don't support array of cubemap samplers
uniform samplerCube PointLightShadowMaps[MAX_POINT_LIGHT_SHADOW_MAP_COUNT];
uniform sampler2D SpotlightShadowMaps[MAX_SPOTLIGHT_SHADOW_MAP_COUNT];

layout(std140) uniform LightData
{
    vec4 CameraWorldPosition;
    ivec4 LightsCount; // x: DL, y: PL, z: SL
    ivec4 ShadowMapsCount; // x: DL, y: PL, z: SL
    vec4 DirLightAmbientColor[MAX_DIR_LIGHT_COUNT];
    vec4 DirLightDiffuseColor[MAX_DIR_LIGHT_COUNT];
    vec4 DirLightSpecularColor[MAX_DIR_LIGHT_COUNT];
    vec4 DirLightDirection[MAX_DIR_LIGHT_COUNT];
    vec4 DirLightShadowAtlasOffset[MAX_DIR_LIGHT_COUNT];
    vec4 PointLightDiffuseColor[MAX_POINT_LIGHT_COUNT];
    vec4 PointLightSpecularColor[MAX_POINT_LIGHT_COUNT];
    vec4 PointLightAttenuation[MAX_POINT_LIGHT_COUNT];
    vec4 PointLightPositionWorld[MAX_POINT_LIGHT_COUNT];
    vec4 PointLightShadowProjectionFarPlane[MAX_POINT_LIGHT_COUNT];
    vec4 SpotlightAmbientColor[MAX_SPOTLIGHT_COUNT];
    vec4 SpotlightDiffuseColor[MAX_SPOTLIGHT_COUNT];
    vec4 SpotlightSpecularColor[MAX_SPOTLIGHT_COUNT];
    vec4 SpotlightDirection[MAX_SPOTLIGHT_COUNT];
    vec4 SpotlightPosition[MAX_SPOTLIGHT_COUNT];
    vec4 SpotlightCutoff[MAX_SPOTLIGHT_COUNT];
    vec4 SpotlightShadowAtlasOffset[MAX_SPOTLIGHT_COUNT];
    vec4 SpotlightShadowProjectionFarPlane[MAX_SPOTLIGHT_COUNT];
    mat4 DirLightShadowMatrices[MAX_DIR_LIGHT_COUNT];
    mat4 SpotlightShadowMatrices[MAX_SPOTLIGHT_COUNT];
};

in VS_OUT
{
    vec2 tex_coords;
}
fs_in;

vec2 GetShadowTexCoords(in vec2 texCoords, in vec4 atlasOffset)
{
    return (texCoords * atlasOffset.zw) + atlasOffset.xy;
}

float CalcLitFactorDirectionalLight(
    in sampler2D shadowmap, in vec2 shadowmapSize, in vec3 shadowTexCoord, in float shadowTransitionValue)
{
    float resultLit = 0.0;
    float actualDepth = shadowTexCoord.z - SHADOWMAP_BIAS_DIR_LIGHT;

    float SumDepth = 0.0;
    vec2 texelSize = 1.0 / shadowmapSize;

    for (int x = -PCF_SAMPLES_DIR_LIGHT; x <= PCF_SAMPLES_DIR_LIGHT; x++) {
        for (int y = -PCF_SAMPLES_DIR_LIGHT; y <= PCF_SAMPLES_DIR_LIGHT; y++) {
            vec2 offset = vec2(float(x) * texelSize.x, float(y) * texelSize.y);

            float pcfDepth = texture(shadowmap, shadowTexCoord.xy + offset).r;
            resultLit += step(pcfDepth, actualDepth);
        }
    }

    if (shadowTransitionValue > 0.001 && resultLit > 0.001) {
        resultLit = shadowTransitionValue;
    } else {
        resultLit *= INV_COUNT_PCF_DIR_LIGHT_SAMPLES;
        resultLit = 1 - resultLit;
    }

    return resultLit;
}

float CalcLitFactorPointLight(
    in int pointLightIndex, in vec3 pixelWorldPos, in vec3 pointLightWorldPos, in float shadowmapProjectionFarPlane)
{
    vec3 LightToFragVec = pixelWorldPos - pointLightWorldPos;
    float actualDepth = length(LightToFragVec);

    float shadow = 0.0;
    const float offset = 0.1;
    const float offsetStep = offset / (PCF_SAMPLES_POINT_LIGHT * 0.5);

    for (float x = -offset; x < offset; x += offsetStep) {
        for (float y = -offset; y < offset; y += offsetStep) {
            for (float z = -offset; z < offset; z += offsetStep) {
                float shadowmapDepth = texture(PointLightShadowMaps[pointLightIndex], LightToFragVec + vec3(x, y, z))
                                           .r; // depth is in range [0 ; 1]
                shadowmapDepth *= shadowmapProjectionFarPlane; // now depth is linear in world space
                                                               // in range [0 ; Far Plane]
                shadow += step(shadowmapDepth, actualDepth - SHADOWMAP_BIAS_POINT_LIGHT);
            }
        }
    }

    shadow *= INV_COUNT_PCF_POINT_LIGHT_SAMPLES;
    return 1.0 - shadow;
}

float CalcLitFactorSpotlight(
    in sampler2D shadowmap,
    in vec2 shadowMapTexCoords,
    in vec2 shadowmapSize,
    in vec3 pixelWorldPos,
    in vec3 spotlightWorldPos,
    in float shadowmapProjectionFarPlane)
{
    float actualDepth = length(pixelWorldPos - spotlightWorldPos);

    float shadow = 0;
    vec2 texelSize = 1.0 / shadowmapSize;

    for (float x = -PCF_SAMPLES_SPOTLIGHT; x < PCF_SAMPLES_SPOTLIGHT; ++x) {
        for (float y = -PCF_SAMPLES_SPOTLIGHT; y < PCF_SAMPLES_SPOTLIGHT; ++y) {
            vec2 offset = vec2(float(x) * texelSize.x, float(y) * texelSize.y);
            float shadowmapDepth = texture(shadowmap, shadowMapTexCoords + offset).r; // depth is in range [0 ; 1]
            shadowmapDepth *= shadowmapProjectionFarPlane; // now depth is linear in world space in
                                                           // range [0 ; Far Plane]
            shadow += step(shadowmapDepth, actualDepth - SHADOWMAP_BIAS_SPOTLIGHT);
        }
    }

    shadow *= INV_COUNT_PCF_SPOT_LIGHT_SAMPLES;
    return 1 - shadow;
}

float GetShadowTransitionValue(in vec2 shadowTexCoords, in vec2 shadowmapAtlasSize)
{
    float aspectRatioWidthToHeight = shadowmapAtlasSize.x / shadowmapAtlasSize.y;
    float radius = length(vec2(0.5 - shadowTexCoords.x, (0.5 - shadowTexCoords.y) * aspectRatioWidthToHeight));
    return smoothstep(0.4, 0.5, radius);
}

#ifdef SHADING_MODEL_PBR

const float Metallic = 0.4;
const float Roughness = 0.8;
const float Epsilon = 0.00001;

const float PI = 3.14159265359;

float ndfGGX(float cosLh, float roughness)
{
    float alpha = roughness * roughness;
    float alphaSq = alpha * alpha;

    float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
    return alphaSq / (PI * denom * denom);
}

// Single term for separable Schlick-GGX below.
float gaSchlickG1(float cosTheta, float k)
{
    return cosTheta / (cosTheta * (1.0 - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's
// method.
float gaSchlickGGX(float cosLi, float cosLo, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
    return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}

// Shlick's approximation of the Fresnel factor.
vec3 fresnelSchlick(vec3 F0, float cosTheta)
{
    return F0 + (vec3(1.0) - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 GetPBRContribution(
    in vec3 nWorldNormal,
    in vec3 albedoColor,
    in vec3 F0,
    in float cosLo,
    in vec3 Li,
    in vec3 Lo,
    in vec3 lightRadiance,
    in vec2 metallicRoughness)
{
    // Half-vector between Li and Lo.
    vec3 Lh = normalize(Li + Lo);

    // Calculate angles between surface normal and various light vectors.
    float cosLi = max(0.0, dot(nWorldNormal, Li));
    float cosLh = max(0.0, dot(nWorldNormal, Lh));

    // Calculate Fresnel term for direct lighting.
    vec3 F = fresnelSchlick(F0, max(0.0, dot(Lh, Lo)));
    // Calculate normal distribution for specular BRDF.
    float D = ndfGGX(cosLh, metallicRoughness.g);
    // Calculate geometric attenuation for specular BRDF.
    float G = gaSchlickGGX(cosLi, cosLo, metallicRoughness.g);

    // Diffuse scattering happens due to light being refracted multiple times by a
    // dielectric medium. Metals on the other hand either reflect or absorb
    // energy, so diffuse contribution is always zero. To be energy conserving we
    // must scale diffuse BRDF contribution based on Fresnel factor & metalness.
    vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metallicRoughness.r);

    // Lambert diffuse BRDF.
    vec3 diffuseBRDF = kd * albedoColor;

    // Cook-Torrance specular microfacet BRDF.
    vec3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * cosLo);

    // Total contribution for this light.
    return (diffuseBRDF + specularBRDF) * lightRadiance * cosLi;
}

vec3 CalculatePointLightLitColor(in vec3 radiance, in int pointLightIndex, in vec3 pixelWorldPos)
{
    float litFactor = 1.0;
    if (ShadowMapsCount.y > pointLightIndex && ShadowMapsCount.y <= MAX_POINT_LIGHT_SHADOW_MAP_COUNT) {
        litFactor = CalcLitFactorPointLight(
            pointLightIndex,
            pixelWorldPos,
            PointLightPositionWorld[pointLightIndex].xyz,
            PointLightShadowProjectionFarPlane[pointLightIndex].x);
    }
    return litFactor * radiance;
}

vec3 GetPBRLightColor(in vec3 pixelWorldPos, in vec3 nWorldNormal, in vec3 albedoColor, in vec2 metallicRoughness)
{
    // General data
    vec3 F0 = mix(vec3(0.04), albedoColor, metallicRoughness.r);
    vec3 Lo = normalize(CameraWorldPosition.xyz - pixelWorldPos);
    // Angle between surface normal and camera position.
    float cosLo = max(0.0, dot(nWorldNormal, Lo));
    // Specular reflection vector.
    vec3 Lr = 2.0 * cosLo * nWorldNormal - Lo;

    vec3 directLighting = vec3(0);
    {
        for (int directLightIndex = 0; directLightIndex < LightsCount.x; ++directLightIndex) {
            // calculate per-light radiance
            vec3 Li = -normalize(DirLightDirection[directLightIndex].xyz);

            vec3 lightRadiance = DirLightDiffuseColor[directLightIndex].rgb;

            vec3 LRadiance = lightRadiance;

            vec3 pbrRadiance = GetPBRContribution(nWorldNormal, albedoColor, F0, cosLo, Li, Lo, LRadiance, metallicRoughness);

            // Calculating shadow
            float litFactor = 1.0f;
#ifdef ENABLE_SHADOWS
            if (ShadowMapsCount.x > directLightIndex && ShadowMapsCount.x <= MAX_DIR_LIGHT_SHADOW_MAP_COUNT) {
                vec4 atlasOffset = DirLightShadowAtlasOffset[directLightIndex];
                mat4 shadowMatrix = DirLightShadowMatrices[directLightIndex];

                vec4 shadowProjectedPosition = (shadowMatrix * vec4(pixelWorldPos, 1.0));
                vec3 shadowFragCoords = shadowProjectedPosition.xyz / shadowProjectedPosition.w;

                vec2 shadowmapAtlasSize = textureSize(DirLightShadowMaps[directLightIndex], 0);
                vec2 shadowCoordinates = GetShadowTexCoords(shadowFragCoords.xy, atlasOffset);
                float depth = clamp(shadowFragCoords.z, 0.0, 1.0);
                vec3 shadowCoordinatesAndDepth = vec3(shadowCoordinates, depth);

                float shadowTransitionValue = GetShadowTransitionValue(shadowFragCoords.xy, shadowmapAtlasSize);

                litFactor = CalcLitFactorDirectionalLight(
                    DirLightShadowMaps[directLightIndex], shadowmapAtlasSize, shadowCoordinatesAndDepth, shadowTransitionValue);
            }
#endif

            // Total contribution for this light.
            directLighting += pbrRadiance * litFactor;
        }
    }

    vec3 pointLighting = vec3(0);
#ifdef ENABLE_POINT_LIGHTING
    {
        for (int pointLightIndex = 0; pointLightIndex < LightsCount.y; ++pointLightIndex) {
            // calculate per-light radiance
            vec3 toLVec = PointLightPositionWorld[pointLightIndex].xyz - pixelWorldPos;
            float lSrcDstSquared = dot(toLVec, toLVec);
            float lSrcDist = sqrt(lSrcDstSquared);
            vec3 Li = toLVec / lSrcDist;

            float attenuation = clamp(10.0 / lSrcDist, 0.0, 1.0);

            vec3 LRadiance = PointLightDiffuseColor[pointLightIndex].rgb * attenuation;

            vec3 pbrRadiance = GetPBRContribution(nWorldNormal, albedoColor, F0, cosLo, Li, Lo, LRadiance, metallicRoughness);
#ifdef ENABLE_SHADOWS
            pointLighting += CalculatePointLightLitColor(pbrRadiance, pointLightIndex, pixelWorldPos);
#else
            pointLighting += pbrRadiance;
#endif
        }
    }
#endif

    vec3 spotlightColor = vec3(0);
    {
        for (int spotlightIndex = 0; spotlightIndex < LightsCount.z; ++spotlightIndex) {
            // calculate per-light radiance
            vec3 Li = normalize(SpotlightDirection[spotlightIndex].xyz);

            vec3 nLtoPixel = normalize(pixelWorldPos - SpotlightPosition[spotlightIndex].xyz);
            float spotlightFactor = max(dot(Li, nLtoPixel), 0.0);

            vec3 lightRadiance = SpotlightDiffuseColor[spotlightIndex].rgb; // for now

            vec3 LRadiance = lightRadiance;

            vec3 pbrRadiance = GetPBRContribution(nWorldNormal, albedoColor, F0, cosLo, -Li, Lo, LRadiance, metallicRoughness);
            pbrRadiance *= smoothstep(SpotlightCutoff[spotlightIndex].x, 1.0, spotlightFactor);

            // Calculating shadow
            float litFactor = 1.0f;
#ifdef ENABLE_SHADOWS
            if (ShadowMapsCount.z > spotlightIndex && spotlightFactor > SpotlightCutoff[spotlightIndex].x
                && ShadowMapsCount.z <= MAX_SPOTLIGHT_SHADOW_MAP_COUNT) {
                vec4 atlasOffset = SpotlightShadowAtlasOffset[spotlightIndex];
                mat4 shadowMatrix = SpotlightShadowMatrices[spotlightIndex];

                vec4 shadowProjectedPosition = (shadowMatrix * vec4(pixelWorldPos, 1.0));
                vec2 shadowFragCoords = shadowProjectedPosition.xy / shadowProjectedPosition.w;

                vec2 shadowmapAtlasSize = textureSize(SpotlightShadowMaps[spotlightIndex], 0);
                vec2 shadowCoordinates = GetShadowTexCoords(shadowFragCoords, atlasOffset);

                litFactor = CalcLitFactorSpotlight(
                    SpotlightShadowMaps[spotlightIndex],
                    shadowCoordinates,
                    shadowmapAtlasSize,
                    pixelWorldPos,
                    SpotlightPosition[spotlightIndex].xyz,
                    SpotlightShadowProjectionFarPlane[spotlightIndex].x);
            }
#endif

            // Total contribution for this light.
            spotlightColor += pbrRadiance * litFactor;
        }
    }

    return directLighting + pointLighting + spotlightColor;
}

#endif

#ifndef SHADING_MODEL_PBR

/* nWorldNormal - n means that normal has to be normalized */
vec3 GetDiffuseColor(in vec3 pixelWorldPos, in vec3 nWorldNormal)
{
    vec3 resultDiffuseColor = vec3(0);

    /* POINT LIGHTS */
    for (int pointLightIndex = 0; pointLightIndex < LightsCount.y; ++pointLightIndex) {
        vec3 pointLightPositionWorld = PointLightPositionWorld[pointLightIndex].xyz;
        vec3 nToLightVec = normalize(pointLightPositionWorld - pixelWorldPos);
        float nDotP = dot(nToLightVec, nWorldNormal);
        float diffuseFactor = max(nDotP, 0.0);
        float lSrcDist = length(pointLightPositionWorld - pixelWorldPos);
        float attenuation = clamp(1.0 / lSrcDist, 0.0, 1.0);
        float litFactor = 1.0f;

        // Calculating shadow
        if (ShadowMapsCount.y > pointLightIndex && ShadowMapsCount.y <= MAX_POINT_LIGHT_SHADOW_MAP_COUNT) {
            litFactor = CalcLitFactorPointLight(
                pointLightIndex, pixelWorldPos, pointLightPositionWorld, PointLightShadowProjectionFarPlane[pointLightIndex].x);
        }

        resultDiffuseColor += PointLightDiffuseColor[pointLightIndex].rgb * diffuseFactor * attenuation * litFactor;
    }

    /* DIRECTIONAL LIGHTS */
    for (int dirLightIndex = 0; dirLightIndex < LightsCount.x; ++dirLightIndex) {
        vec3 direction = -normalize(DirLightDirection[dirLightIndex].xyz);
        float nDotD = dot(direction, nWorldNormal);
        float diffuseFactor = max(nDotD, 0.0);
        float litFactor = 1.0f;

        // Calculating shadow
        if (ShadowMapsCount.x > dirLightIndex && ShadowMapsCount.x <= MAX_DIR_LIGHT_SHADOW_MAP_COUNT) {
            // Common
            vec4 atlasOffset = DirLightShadowAtlasOffset[dirLightIndex];
            mat4 shadowMatrix = DirLightShadowMatrices[dirLightIndex];

            vec4 shadowProjectedPosition = (shadowMatrix * vec4(pixelWorldPos, 1.0));
            vec3 shadowFragCoords = shadowProjectedPosition.xyz / shadowProjectedPosition.w;
            vec2 shadowCoordinates = GetShadowTexCoords(shadowFragCoords.xy, atlasOffset);
            vec3 shadowCoordinatesAndDepth = vec3(shadowCoordinates, shadowFragCoords.z);

            vec2 shadowmapAtlasSize = textureSize(DirLightShadowMaps[dirLightIndex], 0);

            float shadowTransitionValue = GetShadowTransitionValue(shadowFragCoords.xy, shadowmapAtlasSize);

            litFactor = CalcLitFactorDirectionalLight(
                DirLightShadowMaps[dirLightIndex], shadowmapAtlasSize, shadowCoordinatesAndDepth, shadowTransitionValue);
        }

        resultDiffuseColor += DirLightDiffuseColor[dirLightIndex].rgb * diffuseFactor * litFactor;
    }

    /* SPOT LIGHTS */
    for (int spotlightIndex = 0; spotlightIndex < LightsCount.z; ++spotlightIndex) {
        vec3 nDirection = normalize(SpotlightDirection[spotlightIndex].xyz);
        float diffuseFactor = max(dot(-nDirection, nWorldNormal), 0.0);
        vec3 nLtoPixel = normalize(pixelWorldPos - SpotlightPosition[spotlightIndex].xyz);
        float spotlightFactor = max(dot(nDirection, nLtoPixel), 0.0);

        diffuseFactor *= smoothstep(SpotlightCutoff[spotlightIndex].x, 1.0, spotlightFactor);

        float litFactor = 1.0f;
        if (ShadowMapsCount.z > spotlightIndex && spotlightFactor > SpotlightCutoff[spotlightIndex].x
            && ShadowMapsCount.z <= MAX_SPOTLIGHT_SHADOW_MAP_COUNT) {
            vec4 atlasOffset = SpotlightShadowAtlasOffset[spotlightIndex];
            mat4 shadowMatrix = SpotlightShadowMatrices[spotlightIndex];

            vec4 shadowProjectedPosition = (shadowMatrix * vec4(pixelWorldPos, 1.0));
            vec2 shadowFragCoords = shadowProjectedPosition.xy / shadowProjectedPosition.w;

            vec2 shadowmapAtlasSize = textureSize(SpotlightShadowMaps[spotlightIndex], 0);
            vec2 shadowCoordinates = GetShadowTexCoords(shadowFragCoords, atlasOffset);

            litFactor = CalcLitFactorSpotlight(
                SpotlightShadowMaps[spotlightIndex],
                shadowCoordinates,
                shadowmapAtlasSize,
                pixelWorldPos,
                SpotlightPosition[spotlightIndex].xyz,
                SpotlightShadowProjectionFarPlane[spotlightIndex].x);
        }

        resultDiffuseColor += SpotlightDiffuseColor[spotlightIndex].rgb * diffuseFactor * litFactor;
    }

    return resultDiffuseColor;
}

#endif

vec3 GetAmbientColor()
{
    return vec3(0.1);
}

void main()
{
    vec3 pixelWorldPos = texture(gBuffer_Position, fs_in.tex_coords).xyz;
    vec3 worldNormal = texture(gBuffer_Normal, fs_in.tex_coords).xyz;
    vec3 albedo = texture(gBuffer_Albedo, fs_in.tex_coords).rgb;
    vec2 metallicRoughness = texture(gBuffer_MetallicRoughness, fs_in.tex_coords).rg;
    vec4 emissionColor = texture(gBuffer_Emission, fs_in.tex_coords);

#ifdef SHADING_MODEL_PBR
    vec3 ambientColor = (1.0 - step(1, LightsCount.x)) * GetAmbientColor();
    vec3 ambientAlbedo = albedo * ambientColor;
    vec4 totalColor
        = vec4(GetPBRLightColor(pixelWorldPos, worldNormal, albedo, metallicRoughness), 1.0) + vec4(ambientAlbedo, 1.0);
#else
#ifdef NO_LIT
    vec4 totalColor = vec4(albedo, 1.0);
#else
    vec3 diffuseColor = GetDiffuseColor(pixelWorldPos, worldNormal);
    vec3 ambientColor = GetAmbientColor();
    vec4 totalColor = vec4(albedo * (diffuseColor + ambientColor), 1.0);
#endif
#endif

#ifdef GAMMA_CORRECTION
    const float gammaCorrection = 1.0 / 2.2;
    FragColor = vec4(mix(pow(totalColor.rgb, vec3(gammaCorrection)), emissionColor.rgb, emissionColor.a), 1.0);
#else
    FragColor = vec4(mix(totalColor.rgb, emissionColor.rgb, emissionColor.a), 1.0);
#endif
}