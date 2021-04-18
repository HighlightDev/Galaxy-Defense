<general>
	name: "Dynamic test material"
	shader: "Test.glsl"
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
					name: "deltaTime"
					type: "binding_float"
				</property>
				<float_constant>
					"1"
				</float_constant>
			</mul>
		</operation>
	</dynamic_property>
	<property>
		name: "reflectionTexture"
		type: "texture"
	</property>
	<property>
		name: "refractionTexture"
		type: "texture"
	</property>
</properties>