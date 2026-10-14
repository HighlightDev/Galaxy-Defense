<general>
	name: "GhostTowerMaterial material"
	shader: "GhostTowerMaterial.glsl"
	material_type: "dynamic"
</general>
<properties>
	<property>
		name: "opacity"
		type: "float"
	</property>
	<property>
		name: "albedo"
		type: "texture"
	</property>
    <property>
        name: "uvScale"
        type: "float"
    </property>
	<property>
		name: "blendFactor"
		type: "float"
	</property>
	<dynamic_property>
		name: "blendColor"
		type: "vec3"
		incremental: "false"
		<operation>
			<no_op>
				<property>
					name: "b_blendColor"
					type: "binding_vec3"
				</property>
			</no_op>
		</operation>
	</dynamic_property>
</properties>