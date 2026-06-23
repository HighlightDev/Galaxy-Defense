<general>
	name: "RepairBarrier material"
	shader: "RepairBarrierMaterial.glsl"
	material_type: "dynamic"
</general>
<properties>
	<property>
		name: "wallColorLow"
		type: "vec3"
	</property>
	<property>
		name: "wallColorHigh"
		type: "vec3"
	</property>
	<property>
		name: "fillColor"
		type: "vec3"
	</property>
	<property>
		name: "outlineColorLow"
		type: "vec3"
	</property>
	<property>
		name: "outlineColorHigh"
		type: "vec3"
	</property>
	<property>
		name: "opacity"
		type: "float"
	</property>
	<property>
		name: "healPeriod"
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
