<general>
	name: "BlackHoleMisile material"
	shader: "BlackHoleMissileMaterial.glsl"
	material_type: "dynamic"
</general>
<properties>
	<dynamic_property>
		name: "distortion"
		type: "float"
		incremental: "true"
		range: "[0.0 ; 1.0]"
		<operation>
			<mul>
				<property>
					name: "mul_coef"
					type: "float"
				</property>
				<property>
					name: "deltaTimeSec"
					type: "binding_float"
				</property>
			</mul>
		</operation>
	</dynamic_property>
	<property>
		name: "albedo"
		type: "texture"
	</property>
	<property>
		name: "dudv"
		type: "texture"
	</property>
</properties>