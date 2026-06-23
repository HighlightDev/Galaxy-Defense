<general>
	name: "RepairBurst material"
	shader: "RepairBurstMaterial.glsl"
	material_type: "dynamic"
</general>
<properties>
	<property>
		name: "ringColor"
		type: "vec3"
	</property>
	<property>
		name: "opacity"
		type: "float"
	</property>
	<property>
		name: "cyclePeriod"
		type: "float"
	</property>
	<property>
		name: "burstStart"
		type: "float"
	</property>
	<property>
		name: "ringDelay"
		type: "float"
	</property>
	<property>
		name: "ringLife"
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
