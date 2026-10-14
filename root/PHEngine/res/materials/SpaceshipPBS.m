<general>
	name: "SpaceshipPBS.m"
	shader: "SpaceshipPBSMaterial.glsl"
	material_type: "dynamic"
</general>
<properties>
    <instanced_dynamic_property>
		name: "damageEffect"
		type: "instanced_binding_float"
	</instanced_dynamic_property>
	<instanced_dynamic_property>
		name: "freezingEffect"
		type: "instanced_binding_float"
	</instanced_dynamic_property>
	<dynamic_property>
		name: "cameraPosition"
		type: "vec3"
		incremental: "false"
		<operation>
			<no_op>
				<property>
					name: "cameraPosition"
					type: "binding_vec3"
				</property>
			</no_op>
		</operation>
	</dynamic_property>
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
	<property>
		name: "albedo_ice"
		type: "texture"
	</property>
	<property>
		name: "normalMap_ice"
		type: "texture"
	</property>
	<property>
		name: "metallicMap_ice"
		type: "texture"
	</property>
	<property>
		name: "roughnessMap_ice"
		type: "texture"
	</property>
</properties>