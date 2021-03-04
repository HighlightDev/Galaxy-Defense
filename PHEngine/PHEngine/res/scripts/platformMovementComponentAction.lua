StartRoute = "A1"

function OnStart(host)
	_AddRoutePoint(host, "A1", 0, 0, 0 , 0, 90, 0, 1, 1, 1, 1.8)	
	_AddRoutePoint(host, "A2", 0, 0, 0 , 0, 90, 0, 1, 1, 1, 1.8)
	_AddRoutePoint(host, "A3", 0, 0, 0 , 0, 90, 0, 1, 1, 1, 1.8)
	_AddRoutePoint(host, "A4", 0, 0, 0 , 0, 90, 0, 1, 1, 1, 1.8)
end

HasOnStart = (_G["OnStart"] ~= nil and 1 or 0)
HasOnUpdate = (_G["OnUpdate"] ~= nil and 1 or 0)
