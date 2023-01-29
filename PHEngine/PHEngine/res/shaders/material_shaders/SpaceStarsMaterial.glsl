#version 400

#include "materialCommon.incl"

uniform float timeSec;
uniform vec2 resolution;

#define iterations 17
#define formuparam 0.53

#define volsteps 10
#define stepsize 0.1

#define zoom   3.900
#define tile   0.850
#define speed  0.005 

#define brightness 0.0015
#define darkmatter 0.300
#define distfading 0.730
#define saturation 0.850

vec2 iMouse = vec2(0.5, 0.5);

// this shader was taken from https://www.shadertoy.com/view/XlfGRj#
vec3 getSpaceStarsColor(in MATERIAL_VS_OUTPUT materialIn)
{
	//get coords and direction
    vec2 texCoords = materialIn.TextureCoordinates.xy;
	vec2 uv=texCoords-.5;
	uv.y*=resolution.y / resolution.x;
	vec3 dir=vec3(uv*zoom,1.);
	float time=timeSec*speed+.25;

	//mouse rotation
	float a1=.5+iMouse.x;
	float a2=.8+iMouse.y;
	mat2 rot1=mat2(cos(a1),sin(a1),-sin(a1),cos(a1));
	mat2 rot2=mat2(cos(a2),sin(a2),-sin(a2),cos(a2));
	dir.xz*=rot1;
	dir.xy*=rot2;
	vec3 from=vec3(1.,.5,0.5);
	from+=vec3(time*2.,time,-2.);
	from.xz*=rot1;
	from.xy*=rot2;
	
	//volumetric rendering
	float s=0.1,fade=1.;
	vec3 v=vec3(0.);
	for (int r=0; r<volsteps; r++) {
		vec3 p=from+s*dir*.5;
		p = abs(vec3(tile)-mod(p,vec3(tile*2.))); // tiling fold
		float pa,a=pa=0.;
		for (int i=0; i<iterations; i++) { 
			p=abs(p)/dot(p,p)-formuparam; // the magic formula
			a+=abs(length(p)-pa); // absolute sum of average change
			pa=length(p);
		}
		float dm=max(0.,darkmatter-a*a*.001); //dark matter
		a*=a*a; // add contrast
		if (r>6) fade*=1.-dm; // dark matter, don't render near
		//v+=vec3(dm,dm*.5,0.);
		v+=fade;
		v+=vec3(s,s*s,s*s*s*s)*a*brightness*fade; // coloring based on distance
		fade*=distfading; // distance fading
		s+=stepsize;
	}
	v=mix(vec3(length(v)),v,saturation); //color adjust
	
    return vec3(v*.01);	
}

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn)
{
	return getSpaceStarsColor(materialIn);
}

vec2 GetMaterialMetallicRoughness(in MATERIAL_VS_OUTPUT materialIn)
{
    return vec2(0);
}

float GetMaterialAmbientOcclusion(in MATERIAL_VS_OUTPUT materialIn)
{
	return 0.0;
}

float GetMaterialAlphaMask(in MATERIAL_VS_OUTPUT materialIn)
{
	return 1.0;
};

vec3 GetMaterialNormalMapNormal(in MATERIAL_VS_OUTPUT materialIn)
{
    return vec3(0);
}