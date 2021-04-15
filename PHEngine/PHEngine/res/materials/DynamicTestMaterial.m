<general>
	name: "Dynamic test material"
	shader: "Test.glsl"
	material_type: "dynamic"
</general>
<properties>
	<dynamic_property>
		name: "distortion"
		type: "float"
		<operation>
			<mul>
				<binding_property>
					name: "deltaTime"
					type: "float"
				</binding_property>
				<add>
					<property>
						name: "test_value"
						type: "float"
					</property>
					<float_constant>
						value: "0.0"
					</float_constant>
				</add>
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