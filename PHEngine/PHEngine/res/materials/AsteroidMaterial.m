<general>
	name: "Asteroid material"
	shader: "AsteroidMaterial.glsl"
	material_type: "dynamic"
</general>
<properties>
    <instanced_dynamic_property>
		name: "freezingBlendValue"
		type: "instanced_binding_float"
	</instanced_dynamic_property>
	<property>
		name: "albedo"
		type: "texture"
	</property>
	<property>
		name: "normalMap"
		type: "texture"
	</property>
	<property>
		name: "metallicMap"
		type: "texture"
	</property>
	<property>
		name: "roughnessMap"
		type: "texture"
	</property>
	<property>
		name: "ambientOcclusionMap"
		type: "texture"
	</property>
	<property>
		name: "uvScale"
		type: "float"
	</property>
</properties>