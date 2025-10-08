<general>
	name: "SpaceStationRadiusMarker material"
	shader: "SpaceStationRadiusMarkerMaterial.glsl"
	material_type: "dynamic"
</general>
<properties>
	<property>
		name: "color"
		type: "vec3"
	</property>
	<dynamic_property>
		name: "radius"
		type: "float"
		<operation>
			<no_op>
				<property>
					name: "b_ShootRadius"
					type: "binding_float"
				</property>
			</no_op>
		</operation>
	</dynamic_property>
	<dynamic_property>
		name: "world_spacestation_position"
		type: "vec3"
		<operation>
			<no_op>
				<property>
					name: "p_spacestation_translation"
					type: "binding_vec3"
				</property>
			</no_op>
		</operation>
	</dynamic_property>
</properties>