<general>
	name: "Portal material"
	shader: "PortalMaterial.glsl"
	material_type: "dynamic"
</general>
<properties>
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
	<dynamic_property>
		name: "resolution"
		type: "vec2"
		incremental: "false"
		<operation>
			<no_op>
				<property>
					name: "screenResolution"
					type: "binding_vec2"
				</property>
			</no_op>
		</operation>
	</dynamic_property>
	<dynamic_property>
		name: "colorIntensity"
		type: "vec3"
		incremental: "false"
		<operation>
			<no_op>
				<property>
					name: "b_colorIntensity"
					type: "binding_vec3"
				</property>
			</no_op>
		</operation>
	</dynamic_property>
</properties>