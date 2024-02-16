<general>
	name: "EditorPicker material"
	shader: "EditorPickerMaterial.glsl"
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
	<dynamic_property>
		name: "pulse_animation_time"
		type: "float"
		incremental: "true"
		range: "[0.0 ; 2.0]"
		<operation>
			<mul>
				<float_constant>
					value: "0.8"
				</float_constant>
				<property>
					name: "gt_timeSec"
					type: "binding_float"
				</property>
			</mul>
		</operation>
	</dynamic_property>
</properties>
