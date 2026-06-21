#version 440 core

out vec2 vs_out_rotation_size;
out vec3 vs_out_particle_color;

out MATERIAL_VS_OUTPUT VsOutput;
out FLAT_MATERIAL_VS_OUTPUT FlatVsOutput;

void main()
{
    vs_out_particle_color = GetParticleColor();
    vs_out_rotation_size = GetParticleRotationAndSize();
    VsOutput = VertexFactoryGetMaterialOutput();
    FlatVsOutput = VertexFactoryGetFlatMaterialOutput();
    gl_Position = viewMatrix * worldMatrix * vec4(GetParticleRelativeOffset(), 1.0);
}
