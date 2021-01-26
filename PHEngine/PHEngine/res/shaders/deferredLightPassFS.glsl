#version 400

#define SHADING_MODEL_PBR
#define MAX_DIR_LIGHT_COUNT 5
#define MAX_POINT_LIGHT_COUNT 50
#define SHADOWMAP_BIAS_DIR_LIGHT 0.005
#define SHADOWMAP_BIAS_POINT_LIGHT 0.05
#define PCF_SAMPLES_DIR_LIGHT 2
#define PCF_SAMPLES_POINT_LIGHT 4
#define MAX_POINT_LIGHT_SHADOW_MAP_COUNT 4
#define MAX_DIR_LIGHT_SHADOW_MAP_COUNT 4

const float INV_COUNT_PCF_DIR_LIGHT_SAMPLES = 1.0 / (((PCF_SAMPLES_DIR_LIGHT * 2) + 1) * ((PCF_SAMPLES_DIR_LIGHT * 2) + 1));
const float INV_COUNT_PCF_POINT_LIGHT_SAMPLES = 1.0 / (PCF_SAMPLES_POINT_LIGHT * PCF_SAMPLES_POINT_LIGHT * PCF_SAMPLES_POINT_LIGHT);

layout (location = 0) out vec4 FragColor;

uniform vec3 CameraWorldPosition;

uniform sampler2D gBuffer_Position;
uniform sampler2D gBuffer_Normal;
uniform sampler2D gBuffer_AlbedoNSpecular;
uniform sampler2D DirLightShadowMaps[MAX_DIR_LIGHT_SHADOW_MAP_COUNT];
uniform samplerCube PointLightShadowMaps[MAX_POINT_LIGHT_SHADOW_MAP_COUNT];

uniform vec3 DirLightAmbientColor[MAX_DIR_LIGHT_COUNT];
uniform vec3 DirLightDiffuseColor[MAX_DIR_LIGHT_COUNT];
uniform vec3 DirLightSpecularColor[MAX_DIR_LIGHT_COUNT];
uniform vec3 DirLightDirection[MAX_DIR_LIGHT_COUNT];
uniform mat4 DirLightShadowMatrices[MAX_DIR_LIGHT_COUNT];
uniform vec4 DirLightShadowAtlasOffset[MAX_DIR_LIGHT_COUNT];
uniform int DirLightCount;
uniform int DirLightShadowMapCount;

uniform int PointLightCount;
uniform int PointLightShadowMapCount;
uniform vec3 PointLightDiffuseColor[MAX_POINT_LIGHT_COUNT];
uniform vec3 PointLightSpecularColor[MAX_POINT_LIGHT_COUNT];
uniform vec3 PointLightAttenuation[MAX_POINT_LIGHT_COUNT];
uniform float PointLightShadowProjectionFarPlane[MAX_POINT_LIGHT_COUNT];
uniform vec3 PointLightPositionWorld[MAX_POINT_LIGHT_COUNT];

in VS_OUT
{
	vec2 tex_coords;
} fs_in;

vec2 GetShadowTexCoords(in vec2 texCoords, in vec4 atlasOffset)
{
	vec2 texCoordsInAtlas;
	texCoordsInAtlas = (texCoords * atlasOffset.zw) + atlasOffset.xy;
	return texCoordsInAtlas;
}

float CalcLitFactorTexture2D(in sampler2D shadowmap, in vec2 shadowmapSize, in vec3 shadowTexCoord, in float shadowTransitionValue)
{
	float resultLit = 0.0;
    float actualDepth = shadowTexCoord.z - SHADOWMAP_BIAS_DIR_LIGHT;

    float SumDepth = 0.0;
    vec2 texelSize = 1.0 / shadowmapSize;

    for (int x = -PCF_SAMPLES_DIR_LIGHT; x <= PCF_SAMPLES_DIR_LIGHT; x++)
    {
       for (int y = -PCF_SAMPLES_DIR_LIGHT; y <= PCF_SAMPLES_DIR_LIGHT; y++)
       {
			vec2 offset = vec2(float(x) * texelSize.x, float(y) * texelSize.y);

            float pcfDepth = texture(shadowmap, shadowTexCoord.xy + offset).r;
            resultLit += actualDepth > pcfDepth ? 1.0 : 0.0;
       }
    }

	if (shadowTransitionValue > 0.001 && resultLit > 0.001)
	{
		resultLit = shadowTransitionValue;
	}
	else
	{
	  resultLit *= INV_COUNT_PCF_DIR_LIGHT_SAMPLES;
	  resultLit = 1 - resultLit;
	}

	return resultLit;
}

float CalcLitFactorCubemap(in samplerCube shadowmap, in vec3 worldPos, in vec3 pointLightWorldPos, in float shadowmapProjectionfarPlane)
{
	float resultLit = 0.0f;

	vec3 LightToFragVec = worldPos - pointLightWorldPos;
	float actualDepth = length(LightToFragVec);

	float shadow  = 0.0;
	const float offset  = 0.1;
	const float offsetStep = offset / (PCF_SAMPLES_POINT_LIGHT * 0.5);

	for (float x = -offset; x < offset; x += offsetStep)
	{
		for (float y = -offset; y < offset; y += offsetStep)
		{
			for (float z = -offset; z < offset; z += offsetStep)
			{
				float shadowmapDepth = texture(shadowmap, LightToFragVec + vec3(x, y, z)).r; // depth is in range [0 ; 1]
				shadowmapDepth *= shadowmapProjectionfarPlane; // now depth is linear in world space in range [0 ; Far Plane]
				shadow += actualDepth - SHADOWMAP_BIAS_POINT_LIGHT > shadowmapDepth ? 1.0f : 0.0f;
			}
		}
	}

	shadow *= INV_COUNT_PCF_POINT_LIGHT_SAMPLES;
	resultLit = 1 - shadow;
	return resultLit;
}

float GetShadowTransitionValue(in vec2 shadowTexCoords, in vec2 shadowmapAtlasSize)
{
	float aspectRatioWidthToHeight = shadowmapAtlasSize.x / shadowmapAtlasSize.y;
	float radius = length(vec2(0.5 - shadowTexCoords.x, (0.5 - shadowTexCoords.y) * aspectRatioWidthToHeight));
	return smoothstep(0.4, 0.5, radius);
}

#ifdef SHADING_MODEL_PBR

	const float Metallic = 0.1;
	const float Roughness = 0.9;
	const float Epsilon = 0.00001;
	uniform float ao;

	const float PI = 3.14159265359;

	float ndfGGX(float cosLh, float roughness)
	{
		float alpha   = roughness * roughness;
		float alphaSq = alpha * alpha;

		float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
		return alphaSq / (PI * denom * denom);
	}

	// Single term for separable Schlick-GGX below.
	float gaSchlickG1(float cosTheta, float k)
	{
		return cosTheta / (cosTheta * (1.0 - k) + k);
	}

	// Schlick-GGX approximation of geometric attenuation function using Smith's method.
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

	vec3 GetPBRContribution(in vec3 nWorldNormal, in vec3 albedoColor, in vec3 F0, in float cosLo, in vec3 Li, in vec3 Lo, in vec3 lightRadiance)
	{
		// Half-vector between Li and Lo.
		vec3 Lh = normalize(Li + Lo);

		// Calculate angles between surface normal and various light vectors.
		float cosLi = max(0.0, dot(nWorldNormal, Li));
		float cosLh = max(0.0, dot(nWorldNormal, Lh));

		// Calculate Fresnel term for direct lighting.
		vec3 F  = fresnelSchlick(F0, max(0.0, dot(Lh, Lo)));
		// Calculate normal distribution for specular BRDF.
		float D = ndfGGX(cosLh, Roughness);
		// Calculate geometric attenuation for specular BRDF.
		float G = gaSchlickGGX(cosLi, cosLo, Roughness);

		// Diffuse scattering happens due to light being refracted multiple times by a dielectric medium.
		// Metals on the other hand either reflect or absorb energy, so diffuse contribution is always zero.
		// To be energy conserving we must scale diffuse BRDF contribution based on Fresnel factor & metalness.
		vec3 kd = mix(vec3(1.0) - F, vec3(0.0), Metallic);

		// Lambert diffuse BRDF.
		vec3 diffuseBRDF = kd * albedoColor;

		// Cook-Torrance specular microfacet BRDF.
		vec3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * cosLo);

		// Total contribution for this light.
		return (diffuseBRDF + specularBRDF) * lightRadiance * cosLi;
	}

	vec3 GetPBRLightColor(in vec3 worldPos, in vec3 nWorldNormal, in vec3 albedoColor)
	{
		// General data
		vec3 F0 = mix(vec3(0.04), albedoColor, Metallic);
		vec3 Lo = normalize(CameraWorldPosition - worldPos);
		// Angle between surface normal and camera position.
		float cosLo = max(0.0, dot(nWorldNormal, Lo));
		// Specular reflection vector.
		vec3 Lr = 2.0 * cosLo * nWorldNormal - Lo;

		vec3 pointLighting = vec3(0);
		{
			for (int pointLightIndex = 0; pointLightIndex < PointLightCount; ++pointLightIndex)
			{
				// calculate per-light radiance
				vec3 toLVec = PointLightPositionWorld[pointLightIndex] - worldPos;
				float lSrcDstSquared = dot(toLVec, toLVec);
				float lSrcDist = sqrt(lSrcDstSquared);
				vec3 Li = toLVec / lSrcDist;

				float attenuation = 1.0; // for now
				// (lSrcDstSquared);

				vec3 LRadiance = PointLightDiffuseColor[pointLightIndex] * attenuation;

				vec3 pbrRadiance = GetPBRContribution(nWorldNormal, albedoColor, F0, cosLo, Li, Lo, LRadiance);

				float litFactor = 1.0;
				// Calculating shadow
				if (PointLightShadowMapCount > pointLightIndex)
				{
					litFactor = CalcLitFactorCubemap(PointLightShadowMaps[pointLightIndex], worldPos, PointLightPositionWorld[pointLightIndex], PointLightShadowProjectionFarPlane[pointLightIndex]);
				}

				pointLighting += pbrRadiance * litFactor;
			}
		}

		vec3 directLighting = vec3(0);
		{
			for (int directLightIndex = 0; directLightIndex < DirLightCount; ++directLightIndex)
			{
				// calculate per-light radiance
				vec3 Li = -normalize(DirLightDirection[directLightIndex]);

				vec3 lightRadiance = DirLightDiffuseColor[directLightIndex]; // for now

				vec3 LRadiance = lightRadiance;

				vec3 pbrRadiance = GetPBRContribution(nWorldNormal, albedoColor, F0, cosLo, Li, Lo, LRadiance);

				// Calculating shadow
				float litFactor = 1.0f;
				if (DirLightShadowMapCount > directLightIndex)
				{
					vec4 atlasOffset = DirLightShadowAtlasOffset[directLightIndex];
					mat4 shadowMatrix = DirLightShadowMatrices[directLightIndex];

					vec4 shadowProjectedPosition = (shadowMatrix * vec4(worldPos, 1.0));
					vec3 shadowFragCoords = shadowProjectedPosition.xyz / shadowProjectedPosition.w;

					vec2 shadowmapAtlasSize = textureSize(DirLightShadowMaps[directLightIndex], 0);
					vec2 shadowCoordinates = GetShadowTexCoords(shadowFragCoords.xy, atlasOffset);
					float depth = clamp(shadowFragCoords.z, 0.0, 1.0);
					vec3 shadowCoordinatesAndDepth = vec3(shadowCoordinates, depth);

					float shadowTransitionValue = GetShadowTransitionValue(shadowFragCoords.xy, shadowmapAtlasSize);

				    litFactor = CalcLitFactorTexture2D(DirLightShadowMaps[directLightIndex], shadowmapAtlasSize, shadowCoordinatesAndDepth, shadowTransitionValue);
				}

				// Total contribution for this light.
				directLighting += pbrRadiance * litFactor;
			}
		}

		return pointLighting + directLighting;
	}

#endif

#ifndef SHADING_MODEL_PBR

/* nWorldNormal - n means that normal has to be normalized */
vec3 GetDiffuseColor(in vec3 worldPos, in vec3 nWorldNormal)
{
	vec3 resultDiffuseColor = vec3(0);

	/* POINT LIGHTS */
	for (int pointLightIndex = 0; pointLightIndex < PointLightCount; ++pointLightIndex)
	{
		vec3 pointLightPositionWorld = PointLightPositionWorld[pointLightIndex];
		vec3 nToLightVec = normalize(pointLightPositionWorld - worldPos);
		float nDotP = dot(nToLightVec, nWorldNormal);
		float diffuseFactor = max(nDotP, 0.0);
		float litFactor = 1.0f;

		// Calculating shadow
		if (PointLightShadowMapCount > pointLightIndex)
		{
			litFactor = CalcLitFactorCubemap(PointLightShadowMaps[pointLightIndex], worldPos, pointLightPositionWorld, PointLightShadowProjectionFarPlane[pointLightIndex]);
		}

		resultDiffuseColor += PointLightDiffuseColor[pointLightIndex] * diffuseFactor * litFactor;
	}

	/* DIRECTIONAL LIGHTS */
	for (int dirLightIndex = 0; dirLightIndex < DirLightCount; ++dirLightIndex)
	{
		vec3 direction = -normalize(DirLightDirection[dirLightIndex]);
		float nDotD = dot(direction, nWorldNormal);
		float diffuseFactor = max(nDotD, 0.0);
		float litFactor = 1.0f;

		// Calculating shadow
		if (DirLightShadowMapCount > dirLightIndex)
		{
			// Common
			vec4 atlasOffset = DirLightShadowAtlasOffset[dirLightIndex];
			mat4 shadowMatrix = DirLightShadowMatrices[dirLightIndex];
			//

			vec4 shadowProjectedPosition = (shadowMatrix * vec4(worldPos, 1.0));
			vec3 shadowFragCoords = shadowProjectedPosition.xyz / shadowProjectedPosition.w;
			vec2 shadowCoordinates = GetShadowTexCoords(shadowFragCoords.xy, atlasOffset);
			vec3 shadowCoordinatesAndDepth = vec3(shadowCoordinates, shadowFragCoords.z);

		 	vec2 shadowmapAtlasSize = textureSize(DirLightShadowMaps[dirLightIndex], 0);

			float shadowTransitionValue = GetShadowTransitionValue(shadowFragCoords.xy, shadowmapAtlasSize);

		    litFactor = CalcLitFactorTexture2D(DirLightShadowMaps[dirLightIndex], shadowmapAtlasSize, shadowCoordinatesAndDepth, shadowTransitionValue);
		}

		resultDiffuseColor += DirLightDiffuseColor[dirLightIndex] * diffuseFactor * litFactor;
	}
	/* SPOT LIGHTS */
	// TODO: SPOT LIGHTS

	return resultDiffuseColor;
}

#endif

vec3 GetAmbientColor()
{
	return DirLightAmbientColor[0];
}

void main()
{
	vec4 worldPos = texture(gBuffer_Position, fs_in.tex_coords);
	vec3 worldNormal = texture(gBuffer_Normal, fs_in.tex_coords).xyz;
	vec4 albedoAndSpecular = texture(gBuffer_AlbedoNSpecular, fs_in.tex_coords);

	// Lighting
	#ifdef SHADING_MODEL_PBR
		vec4 totalColor = vec4(GetPBRLightColor(worldPos.xyz, worldNormal, albedoAndSpecular.xyz), 1.0);
	#else
		#ifdef NO_LIT
			vec4 totalColor = albedoAndSpecular;
		#endif
	#endif

	#ifndef SHADING_MODEL_PBR
		#ifndef NO_LIT
		vec3 diffuseColor = GetDiffuseColor(worldPos, worldNormal, shadowTransitionValue);
		vec3 ambientColor = GetAmbientColor();
		vec4 totalColor = vec4(albedoAndSpecular.rgb * (diffuseColor + ambientColor), 1);
		#endif
	#endif

	FragColor = vec4(totalColor);
}