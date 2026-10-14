<general>
	name: "GravityBombEnergyLine material"
	shader: "GravityBombEnergyLineMaterial.glsl"
	material_type: "dynamic"
</general>
<properties>
	<property>
		name: "opacity"
		type: "float"
	</property>
	<property>
		name: "color"
		type: "vec3"
	</property>
	<property>
		name: "glowColor"
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
