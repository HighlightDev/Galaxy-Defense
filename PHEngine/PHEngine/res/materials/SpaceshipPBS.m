<general>
	name: "SpaceshipPBS.m"
	shader: "SpaceshipPBSMaterial.glsl"
	material_type: "dynamic"
</general>
<properties>
    <dynamic_property>
		name: "damageEffect"
		type: "float"
		incremental: "false"
		range: "[0.0 ; 1.0]"
		<operation>
			<no_op>
				<property>
					name: "damageTime"
					type: "binding_float"
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
</properties>