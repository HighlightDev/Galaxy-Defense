<general>
	name: "BombMissile material"
	shader: "BombMissileMaterial.glsl"
	material_type: "dynamic"
</general>
<properties>
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
</properties>