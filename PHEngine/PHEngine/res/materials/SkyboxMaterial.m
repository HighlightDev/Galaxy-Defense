<general>
	name: "SkyboxDynamicMaterial"
	shader: "SkyboxDayCycleMaterial.glsl"
	material_type: "dynamic"
</general>
<properties>
	<property>
		name: "dayTexture"
		type: "texture"
	</property>
	<property>
		name: "nightTexture"
		type: "texture"
	</property>
	<dynamic_property>
		name: "dayTimeElapsed"
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
					name: "deltaTime"
					type: "binding_float"
				</property>
			</mul>
		</operation>
	</dynamic_property>
</properties>