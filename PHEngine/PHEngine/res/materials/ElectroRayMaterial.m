<general>
	name: "ElectroRay material"
	shader: "ElectroRayMaterial.glsl"
	material_type: "dynamic"
</general>
<properties>
	<property>
		name: "noise"
		type: "texture"
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
	<dynamic_property>
		name: "opacity"
		type: "float"
		incremental: "false"
		range: "[0.0 ; 1.0]"
		<operation>
			<no_op>
				<property>
					name: "b_opacity"
					type: "binding_float"
				</property>
			</no_op>
		</operation>
	</dynamic_property>
</properties>