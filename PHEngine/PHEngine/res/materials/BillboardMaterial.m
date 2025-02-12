<general>
	name: "BillboardMaterial material"
	shader: "BillboardMaterial.glsl"
	material_type: "dynamic"
</general>
<properties>
	<property>
		name: "albedo"
		type: "texture"
	</property>
	<property>
		name: "mask"
		type: "texture"
	</property>
	<property>
		name: "inverse_y"
		type: "integer"
	</property>
	<property>
		name: "use_mask"
		type: "integer"
	</property>
	<property>
		name: "fill_albedo_transparency_with_color"
		type: "integer"
	</property>
	<property>
		name: "use_custom_color_for_albedo"
		type: "integer"
	</property>
	<property>
		name: "albedo_custom_color"
		type: "vec3"
	</property>
	<dynamic_property>
		name: "transparency_color_filler"
		type: "vec3"
		incremental: "false"
		<operation>
			<no_op>
				<property>
					name: "b_transparency_color_filler"
					type: "binding_vec3"
				</property>
			</no_op>
		</operation>
	</dynamic_property>
</properties>