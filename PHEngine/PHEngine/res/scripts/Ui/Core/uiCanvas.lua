--[[ BEGIN *** this snippet h to be inserted everywhere where your want to require custom modules *** BEGIN]] --
local function setup()
    local slash = package.config:sub(1, 1)
    assert(slash ~= nil and type(slash) == "string" and slash ~= "")
    local pattern = ""
    if slash == "/" then
        pattern = "(.*/)"
    elseif slash == "\\" then
        pattern = "(.*\\)"
    end
    local str = debug.getinfo(2, "S").source:sub(2)
    local pathToCurrentScript = str:match(pattern)
    if pathToCurrentScript ~= nil then
        local unixLikePath = pathToCurrentScript:gsub("\\", "/")
        unixLikePath = unixLikePath:gsub("//", "/")
        local _, endindex = string.find(unixLikePath, "scripts/")
        unixLikePath = string.sub(unixLikePath, 1, endindex)
        package.path = package.path .. ";" .. unixLikePath .. "?.lua"
    end
end

setup()
--
--[[ END   *** this snippet has to be inserted everywhere where your want to require custom modules  ***  END]]
local UiBaseWidget = require("Ui/Core/uiBaseWidget")
local CommonUiWidgetCreator = require("Ui/Core/commonUiWidgetCreator")
local json = require("Ui/Core/3rdparty/json")

UiCanvas = UiBaseWidget:new()

function UiCanvas:new(host, originX, originY, width, height, name)
    assert(host ~= nil and originX ~= nil and originY ~= nil and width ~= nil and height ~= nil)
    local canvasJsonParameters;
    if name ~= nil then
        assert(type(name) == "string" and name ~= "")
        canvasJsonParameters = json.encode({
            originX = originX,
            originY = originY,
            width = width,
            height = height,
            name = name
        })
    else
        canvasJsonParameters = json.encode({originX = originX, originY = originY, width = width, height = height})
    end

    local luaProxyId = CommonUiWidgetCreator:createUiWidget(host, CommonUiWidgetCreator.CommonUiWidgetType.UI_CANVAS,
                                                            canvasJsonParameters)

    local canvasProperties = {
        visible = {value = false, dirty = false},
        intercept_mouse_input_event = {value = false, dirty = false},
        canvas_z_order = {value = 0, dirty = false}
    }

    local canvasObj = UiCanvas.parentClass.new(self)
    canvasObj.luaProxyId = luaProxyId
    canvasObj.originX = originX
    canvasObj.originY = originY
    canvasObj.width = width
    canvasObj.height = height
    canvasObj.properties = canvasProperties
    canvasObj.host = host
    return canvasObj
end

function UiCanvas:updateFromReplicatorData(host)
    local replicatorJsonData = _GetGameThreadData(host, self.luaProxyId)
    if replicatorJsonData ~= "" then
        local parsedJson = json.decode(replicatorJsonData)
        if parsedJson["visible"] ~= nil then self.properties.visible.value = parsedJson["visible"] end
        if parsedJson["intercept_mouse_input_event"] ~= nil then
            self.properties.intercept_mouse_input_event.value = parsedJson["intercept_mouse_input_event"]
        end
        if parsedJson["canvas_z_order"] ~= nil then
            self.properties.canvas_z_order.value = parsedJson["canvas_z_order"];
        end
    end
    self:checkLuaProxyReady(host)
end

function UiCanvas:sendDataToReplicator(host)
    local propertiesData = {}
    local propDataDirty = false
    for key, value in pairs(self.properties) do
        if value.dirty then
            propDataDirty = true
            propertiesData[tostring(key)] = value.value
            value.dirty = false
        end
    end

    if propDataDirty then _OnCommonUiWidgetDataUpdated(host, self.luaProxyId, json.encode(propertiesData)) end
end

function UiCanvas:setIsVisible(isVisible)
    if self.properties.visible.value ~= isVisible then
        self.properties.visible.value = isVisible
        self.properties.visible.dirty = true
    end
end

function UiCanvas:setIfCanInterceptMouseInputEvent(intercept)
    if self.properties.intercept_mouse_input_event.value ~= intercept then
        self.properties.intercept_mouse_input_event.value = intercept
        self.properties.intercept_mouse_input_event.dirty = true
    end
end

function UiCanvas:getIfCanInterceptMouseInputEvent() return self.properties.intercept_mouse_input_event.value end

function UiCanvas:setCanvasZOrder(zOrder)
    if self.properties.canvas_z_order.value ~= zOrder then
        self.properties.canvas_z_order.value = zOrder
        self.properties.canvas_z_order.dirty = true
    end
end

function UiCanvas:addFadeInAnimation(host, animationFunctionType, animationDuration, animatedPropertyName,
                                     animatedPropertyType, propertySrcValue, propertyDstValue)
    assert(host ~= nil and type(host) == "userdata" and self.luaProxyReady == true)
    assert(animationFunctionType ~= nil and type(animationFunctionType) == "number" and animationDuration ~= nil and
               type(animationDuration) == "number" and animatedPropertyName ~= nil and type(animatedPropertyName) ==
               "string" and animatedPropertyType ~= nil and type(animatedPropertyType) == "number")
    assert(propertySrcValue ~= nil and propertyDstValue ~= nil and type(propertySrcValue) == type(propertyDstValue))
    local animationJsonData = json.encode({
        animatedPropertyType = animatedPropertyType,
        animationFunctionType = animationFunctionType,
        animationDuration = animationDuration,
        animatedPropertyName = animatedPropertyName,
        propertySrcValue = propertySrcValue,
        propertyDstValue = propertyDstValue
    })
    _AddCanvasFadeAnimation(host, self.luaProxyId, 0, animationJsonData)
end

function UiCanvas:addFadeOutAnimation(host, animationFunctionType, animationDuration, animatedPropertyName,
                                      animatedPropertyType, propertySrcValue, propertyDstValue)
    assert(host ~= nil and type(host) == "userdata" and self.luaProxyReady == true)
    assert(animationFunctionType ~= nil and type(animationFunctionType) == "number" and animationDuration ~= nil and
               type(animationDuration) == "number" and animatedPropertyName ~= nil and type(animatedPropertyName) ==
               "string" and animatedPropertyType ~= nil and type(animatedPropertyType) == "number")
    assert(propertySrcValue ~= nil and propertyDstValue ~= nil and type(propertySrcValue) == type(propertyDstValue))
    local animationJsonData = json.encode({
        animatedPropertyType = animatedPropertyType,
        animationFunctionType = animationFunctionType,
        animationDuration = animationDuration,
        animatedPropertyName = animatedPropertyName,
        propertySrcValue = propertySrcValue,
        propertyDstValue = propertyDstValue
    })
    _AddCanvasFadeAnimation(host, self.luaProxyId, 1, animationJsonData)
end

function UiCanvas:update(host, deltaTimeSec) end

return UiCanvas
