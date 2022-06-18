StartRoute = "A1"

function OnStart(host)
	_AddRoutePoint(host, "A1", 0, 0, 0 , 0, 90, 0, 1, 1, 1, 1.8)	
	_AddRoutePoint(host, "A2", 0, 0, 0 , 0, 90, 0, 1, 1, 1, 1.8)
	_AddRoutePoint(host, "A3", 0, 0, 0 , 0, 90, 0, 1, 1, 1, 1.8)
	_AddRoutePoint(host, "A4", 0, 0, 0 , 0, 90, 0, 1, 1, 1, 1.8)
end

function OnUpdate()
	while true do
		
		coroutine.yield()
	end
end

local OnUpdateCoroutine;

function System_OnStart(host)
	--OnUpdateCoroutine = coroutine.create(OnUpdate)
	OnStart(host)
end

--function System_OnUpdate(host, deltaTime)
--end

--function System_OnUpdate(host, deltaTime)
--local status = coroutine.status(OnUpdateCoroutine)
--	if coroutine.status(OnUpdateCoroutine) ~= 'dead' then
--		coroutine.resume(OnUpdateCoroutine, host)
--	end
--end

HasOnStart = (_G["System_OnStart"] ~= nil and 1 or 0)
HasOnUpdate = (_G["System_OnUpdate"] ~= nil and 1 or 0)
