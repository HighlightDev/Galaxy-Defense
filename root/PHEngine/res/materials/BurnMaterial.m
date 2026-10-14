<general>
	name: "BurnMaterial"
	shader: "BurnMaterial.glsl"
	material_type: "dynamic"
</general>
<properties>
	<dynamic_property>
		name: "deltaTimeSec"
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