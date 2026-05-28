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
local UiItemBase = require("Ui/Core/uiItemBase")
local CommonUiWidgetCreator = require("Ui/Core/commonUiWidgetCreator")
local json = require("Ui/Core/3rdparty/json")

UiProgressBar = UiItemBase:new()

function UiProgressBar:new(host, name)
    assert(host ~= nil, debug.traceback())

    local jsonParameters = nil;
    if name ~= nil then
        assert(type(name) == "string" and name ~= "", debug.traceback())
        jsonParameters = json.encode({name = name})
    end

    local luaProxyId = CommonUiWidgetCreator:createUiWidget(host,
                                                            CommonUiWidgetCreator.CommonUiWidgetType.UI_PROGRESS_BAR,
                                                            jsonParameters)

    local progressBarProperties = {
        empty_color = {value = {r = 0.0, g = 0.0, b = 0.0}, dirty = false},
        filled_color = {value = {r = 0.0, g = 0.0, b = 0.0}, dirty = false},
        opacity = {value = 1.0, dirty = false},
        fill_percent_value = {value = 0.0, dirty = false},
        border_radius = {value = 0.0, dirty = false}
    }

    local uiProgressBarObj = UiProgressBar.uiItemBaseClass.new(self)
    uiProgressBarObj.typeName = "UiProgressBar"
    uiProgressBarObj.luaProxyId = luaProxyId
    uiProgressBarObj.progressBarProperties = progressBarProperties
    uiProgressBarObj.host = host

    return uiProgressBarObj
end

function UiProgressBar:updateFromReplicatorData(host)
    self:checkLuaProxyReady(host)
    if self.luaProxyReady then
        local replicatorJsonData = _GetGameThreadData(host, self.luaProxyId)
        if replicatorJsonData ~= "" then
            local parsedJson = json.decode(replicatorJsonData)
            self:extractUiItemBaseReplicatorData(parsedJson)

            if parsedJson["empty_color"] ~= nil then
                local colorArray = parsedJson["empty_color"]
                self.progressBarProperties.empty_color.value.r = colorArray[1]
                self.progressBarProperties.empty_color.value.g = colorArray[2]
                self.progressBarProperties.empty_color.value.b = colorArray[3]
            end
            if parsedJson["filled_color"] ~= nil then
                local colorArray = parsedJson["filled_color"]
                self.progressBarProperties.filled_color.value.r = colorArray[1]
                self.progressBarProperties.filled_color.value.g = colorArray[2]
                self.progressBarProperties.filled_color.value.b = colorArray[3]
            end
            if parsedJson["opacity"] ~= nil then
                self.progressBarProperties.opacity.value = parsedJson["opacity"]
            end
            if parsedJson["fill_percent_value"] ~= nil then
                self.progressBarProperties.fill_percent_value.value = parsedJson["fill_percent_value"]
            end
            if parsedJson["border_radius"] ~= nil then
                self.progressBarProperties.border_radius.value = parsedJson["border_radius"]
            end
        end
    end
end

function UiProgressBar:sendDataToReplicator(host)
    local propertiesData, basePropsDirty = self:getUiItemBaseDataToReplicator()

    local isPropsDirty = false
    for key, value in pairs(self.progressBarProperties) do
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

function UiProgressBar:update(host, deltaTimeSec) end

function UiProgressBar:setEmptyColorHexValue(colorHex)
    assert(colorHex ~= nil and type(colorHex) == "number", debug.traceback())

    local mask_b = 0xFF;
    local mask_g = 0xFF << 0x8;
    local mask_r = 0xFF << 0x10;

    local r = (mask_r & colorHex) >> 0x10;
    local g = (mask_g & colorHex) >> 0x8;
    local b = mask_b & colorHex;

    local INV_COLOR_MAX_BYTE_VALUE = 1.0 / 255.0;
    self:setEmptyColor(r * INV_COLOR_MAX_BYTE_VALUE, g * INV_COLOR_MAX_BYTE_VALUE, b * INV_COLOR_MAX_BYTE_VALUE)
end

function UiProgressBar:setEmptyColor(r, g, b)
    assert(
        r ~= nil and type(r) == "number" and g ~= nil and type(g) == "number" and b ~= nil and type(b) == "number" and r >=
            0.0 and r <= 1.0 and g >= 0.0 and g <= 1.0 and b >= 0.0 and b <= 1.0)
    self.progressBarProperties.empty_color.value.r = r
    self.progressBarProperties.empty_color.value.g = g
    self.progressBarProperties.empty_color.value.b = b

    self.progressBarProperties.empty_color.dirty = true
end

function UiProgressBar:setFilledColorHexValue(colorHex)
    assert(colorHex ~= nil and type(colorHex) == "number", debug.traceback())

    local mask_b = 0xFF;
    local mask_g = 0xFF << 0x8;
    local mask_r = 0xFF << 0x10;

    local r = (mask_r & colorHex) >> 0x10;
    local g = (mask_g & colorHex) >> 0x8;
    local b = mask_b & colorHex;

    local INV_COLOR_MAX_BYTE_VALUE = 1.0 / 255.0;
    self:setFilledColor(r * INV_COLOR_MAX_BYTE_VALUE, g * INV_COLOR_MAX_BYTE_VALUE, b * INV_COLOR_MAX_BYTE_VALUE)
end

function UiProgressBar:setFilledColor(r, g, b)
    assert(
        r ~= nil and type(r) == "number" and g ~= nil and type(g) == "number" and b ~= nil and type(b) == "number" and r >=
            0.0 and r <= 1.0 and g >= 0.0 and g <= 1.0 and b >= 0.0 and b <= 1.0, debug.traceback())
    self.progressBarProperties.filled_color.value.r = r
    self.progressBarProperties.filled_color.value.g = g
    self.progressBarProperties.filled_color.value.b = b

    self.progressBarProperties.filled_color.dirty = true
end

function UiProgressBar:setOpacity(opacity)
    assert(opacity ~= nil and type(opacity) == "number", debug.traceback())
    if self.progressBarProperties.opacity.value ~= opacity then
        self.progressBarProperties.opacity.value = opacity
        self.progressBarProperties.opacity.dirty = true
    end
end

function UiProgressBar:setFillPercentValue(fillPercentValue)
    assert(fillPercentValue ~= nil and type(fillPercentValue) == "number", debug.traceback())
    if self.progressBarProperties.fill_percent_value.value ~= fillPercentValue then
        self.progressBarProperties.fill_percent_value.value = fillPercentValue
        self.progressBarProperties.fill_percent_value.dirty = true
    end
end

function UiProgressBar:setBorderRadius(borderRadius)
    assert(borderRadius ~= nil and type(borderRadius) == "number", debug.traceback())
    if self.progressBarProperties.border_radius.value ~= borderRadius then
        self.progressBarProperties.border_radius.value = borderRadius
        self.progressBarProperties.border_radius.dirty = true
    end
end

return UiProgressBar
