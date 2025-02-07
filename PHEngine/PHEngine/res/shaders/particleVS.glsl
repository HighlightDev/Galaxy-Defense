#version 400

out vec2 vs_out_rotation_size;
out vec4 vs_out_particle_color;

out MATERIAL_VS_OUTPUT VsOutput;

void main()
{
    vs_out_particle_color = GetParticleColor();
    vs_out_rotation_size = GetParticleRotationAndSize();
    VsOutput = VertexFactoryGetMaterialOutput();
    gl_Position = viewMatrix * worldMatrix * vec4(GetParticleRelativeOffset(), 1.0);
}
