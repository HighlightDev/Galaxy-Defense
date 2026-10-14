<general>
	name: "RepairNanobot material"
	shader: "RepairNanobotMaterial.glsl"
	material_type: "dynamic"
</general>
<properties>
	<property>
		name: "coreColor"
		type: "vec3"
	</property>
	<property>
		name: "haloColor"
		type: "vec3"
	</property>
	<property>
		name: "opacity"
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
