--[[ BEGIN *** this snippet h to be inserted everywhere where your want to require custom modules *** BEGIN]]
--
local function setup()
	local slash = package.config:sub(1,1)
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
local UiItemBase = require("Ui/Core/uiItemBase")
local CommonUiWidgetCreator = require("Ui/Core/commonUiWidgetCreator")
local json = require("Ui/Core/3rdparty/json")

UiRectangle = UiItemBase:new()

function UiRectangle:new(host)
    assert(host ~= nil)
    print("UiRectangle::ctor")

    local luaProxyId = CommonUiWidgetCreator:createUiWidget(host, CommonUiWidgetCreator.CommonUiWidgetType.UI_RECTANGLE)

    local rectangleProperties = {
        color = {
            value = {
                r = 0.0,
                g = 0.0,
                b = 0.0
            },
            dirty = false
        },
        opacity = {
            value = 1.0,
            dirty = false
        },
        border_radius = {
            value = 0.0,
            dirty = false
        }
    }

    local uiRectangleObj = UiRectangle.uiItemBaseClass.new(self)
    uiRectangleObj.typeName = "UiRectangle"
    uiRectangleObj.luaProxyId = luaProxyId
    uiRectangleObj.rectangleProperties = rectangleProperties

    return uiRectangleObj
end

function UiRectangle:updateFromReplicatorData(host)
    self:checkLuaProxyReady(host)
    if self.luaProxyReady then
        local replicatorJsonData = _GetGameThreadData(host, self.luaProxyId)
        if replicatorJsonData ~= "" then
            local parsedJson = json.decode(replicatorJsonData)
            self:extractUiItemBaseReplicatorData(parsedJson)

            if parsedJson["color"] ~= nil then
                local colorArray = parsedJson["color"]
                self.rectangleProperties.color.value.r = colorArray[1]
                self.rectangleProperties.color.value.g = colorArray[2]
                self.rectangleProperties.color.value.b = colorArray[3]
            end
            if parsedJson["opacity"] ~= nil then
                self.rectangleProperties.opacity.value = parsedJson["opacity"]
            end
            if parsedJson["border_radius"] ~= nil then
                self.rectangleProperties.border_radius.value = parsedJson["border_radius"]
            end
        end
    end
end

function UiRectangle:sendDataToReplicator(host)
    local propertiesData, basePropsDirty = self:getUiItemBaseDataToReplicator()

    local isPropsDirty = false
    for key, value in pairs(self.rectangleProperties) do
        if value.dirty then
            isPropsDirty = true
            propertiesData[tostring(key)] = value.value
            value.dirty = false
        end
    end
    if basePropsDirty or isPropsDirty then
        _OnCommonUiWidgetDataUpdated(host, self.luaProxyId, json.encode(propertiesData))
    end
end

function UiRectangle:update(host)
end

function UiRectangle:setColorHexValue(colorHex)
    assert(colorHex ~= nil and type(colorHex) == "number")

    local mask_b = 0xFF;
    local mask_g = 0xFF << 0x8;
    local mask_r = 0xFF << 0x10;

    local r = (mask_r & colorHex) >> 0x10;
    local g = (mask_g & colorHex) >> 0x8;
    local b = mask_b & colorHex;

    local INV_COLOR_MAX_BYTE_VALUE = 1.0 / 255.0;
    self:setColor(r * INV_COLOR_MAX_BYTE_VALUE, g * INV_COLOR_MAX_BYTE_VALUE, b * INV_COLOR_MAX_BYTE_VALUE)
end

function UiRectangle:setColor(r, g, b)
    assert(r ~= nil and type(r) == "number" and g ~= nil and type(g) == "number" and b ~= nil and type(b) == "number" and
        r >= 0.0 and r <= 1.0 and g >= 0.0 and g <= 1.0 and b >= 0.0 and b <= 1.0)
    self.rectangleProperties.color.value.r = r
    self.rectangleProperties.color.value.g = g
    self.rectangleProperties.color.value.b = b

    self.rectangleProperties.color.dirty = true
end

function UiRectangle:setOpacity(opacity)
    assert(opacity ~= nil and type(opacity) == "number")
    if self.rectangleProperties.opacity.value ~= opacity then
        self.rectangleProperties.opacity.value = opacity
        self.rectangleProperties.opacity.dirty = true
    end
end

function UiRectangle:setBorderRadius(borderRadius)
    assert(borderRadius ~= nil and type(borderRadius) == "number")
    if self.rectangleProperties.border_radius.value ~= borderRadius then
        self.rectangleProperties.border_radius.value = borderRadius
        self.rectangleProperties.border_radius.dirty = true
    end
end

return UiRectangle
