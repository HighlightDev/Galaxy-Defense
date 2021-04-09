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
			<increment>
				<mul>
					<float_value>
						value: "0.5"
					</float_value>
					<add>
						<float_value>
							value: "0.1"
						</float_value>
						<float_value>
							value: "123"
						</float_value>
					</add>
				</mul>
			</increment>
		</operation>
	</dynamic_property>

	<property>
		name: "reflectionTexture"
		type: "deferred_texture"
	</property>
	<property>
		name: "distortion"
		type: "texture"
	</property>
	<property>
		name: "refractionTexture"
		type: "texture"
	</property>
</properties>