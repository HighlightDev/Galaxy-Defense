<general>
	name: "SpaceStars material"
	shader: "SpaceStarsMaterial.glsl"
	material_type: "dynamic"
</general>
<properties>
	<property>
		name: "resolution"
		type: "vec2"
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