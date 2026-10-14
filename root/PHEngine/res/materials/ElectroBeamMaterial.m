<general>
	name: "ElectroBeamMaterial"
	shader: "ElectroBeamMaterial.glsl"
	material_type: "dynamic"
</general>
<properties>
	<property>
		name: "noise"
		type: "texture"
	</property>
	<property>
		name: "beamMainColor"
		type: "vec3"
	</property>
		<property>
		name: "beamGlowColor"
		type: "vec3"
	</property>
	<dynamic_property>
		name: "timeSec"
		type: "float"
		incremental: "true"
		<operation>
			<no_op>
				<property>
					name: "gt_timeSec"
					type: "binding_float"
				</property>
			</no_op>
		</operation>
	</dynamic_property>
</properties>