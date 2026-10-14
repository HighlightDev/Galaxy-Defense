<general>
	name: "SpaceBackground material"
	shader: "SpaceBackgroundMaterial.glsl"
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
	<property>
		name: "aspectRatio"
		type: "float"
	</property>
	<property>
		name: "randomSeed"
		type: "float"
	</property>
	<property>
		name: "planetCenter"
		type: "vec3"
	</property>
	<property>
		name: "planetRadius"
		type: "float"
	</property>
	<property>
		name: "planetHue"
		type: "vec3"
	</property>
	<property>
		name: "planetHighlight"
		type: "vec3"
	</property>
	<property>
		name: "nebulaHue"
		type: "vec3"
	</property>
	<property>
		name: "nebulaTintB"
		type: "vec3"
	</property>
	<property>
		name: "nebulaTintC"
		type: "vec3"
	</property>
	<property>
		name: "planetEnabled"
		type: "integer"
	</property>
	<property>
		name: "ringEnabled"
		type: "integer"
	</property>
</properties>
