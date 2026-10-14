<general>
	name: "ForceShield material"
	shader: "ForceShieldMaterial.glsl"
	material_type: "dynamic"
</general>
<properties>
	<property>
		name: "amberColor"
		type: "vec3"
	</property>
	<property>
		name: "brightColor"
		type: "vec3"
	</property>
	<property>
		name: "opacity"
		type: "float"
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
