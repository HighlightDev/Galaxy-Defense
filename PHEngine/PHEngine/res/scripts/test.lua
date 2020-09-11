function LoadLevel(host)
	translation = {}
	translation["x"] = 0
	translation["y"] = 0
	translation["z"] = 0
	CreateActor(host, "TestActor", translation, translation, translation)
end

function CreateActor(host, name, translation, eulerRot, scale)
	actor = { }
	actor["Name"] = name
	actor["translationX"] = translation["x"]
	actor["translationY"] = translation["y"]
	actor["translationZ"] = translation["z"]

	actor["rotationX"] = eulerRot["x"]
	actor["rotationY"] = eulerRot["y"]
	actor["rotationZ"] = eulerRot["z"]

	actor["scaleX"] = scale["x"]
	actor["scaleY"] = scale["y"]
	actor["scaleZ"] = scale["z"]

	_CreateActor(host, actor)
end