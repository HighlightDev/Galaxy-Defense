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

UiToggleButton = UiItemBase:new()

function UiToggleButton:new(host, isStateOn, name)
    assert(host ~= nil and isStateOn ~= nil and type(isStateOn) == "boolean", debug.traceback())

    local jsonParameters = nil;
    if name ~= nil then
        assert(type(name) == "string" and name ~= "", debug.traceback())
        jsonParameters = json.encode({is_state_on = isStateOn, name = name})
    else
        jsonParameters = json.encode({is_state_on = isStateOn})
    end

    local luaProxyId = CommonUiWidgetCreator:createUiWidget(host,
                                                            CommonUiWidgetCreator.CommonUiWidgetType.UI_TOGGLE_BUTTON,
                                                            jsonParameters)

    local toggleButtonProperties = {
        is_state_on = {value = isStateOn, dirty = false},
        opacity = {value = 1.0, dirty = false},
        toggle_on_color = {value = {r = 0.0, g = 0.0, b = 0.0}, dirty = false},
        toggle_off_color = {value = {r = 0.0, g = 0.0, b = 0.0}, dirty = false}
    }

    local uiToggleObj = UiToggleButton.uiItemBaseClass.new(self)
    uiToggleObj.typeName = "UiToggleButton"
    uiToggleObj.luaProxyId = luaProxyId
    uiToggleObj.toggleButtonProperties = toggleButtonProperties
    uiToggleObj.onStateChangedCallback = nil
    uiToggleObj.host = host

    return uiToggleObj
end

function UiToggleButton:enableToggleButtonMouseInputReceiver(host)
    assert(host ~= nil and type(host) == "userdata", debug.traceback())
    _EnableToggleButtonMouseInputReceiver(host, self.luaProxyId)
end

function UiToggleButton:updateFromReplicatorData(host)
    self:checkLuaProxyReady(host)
    if self.luaProxyReady then
        local replicatorJsonData = _GetGameThreadData(host, self.luaProxyId)
        if replicatorJsonData ~= "" then
            local parsedJson = json.decode(replicatorJsonData)
            self:extractUiItemBaseReplicatorData(parsedJson)

            if parsedJson["is_state_on"] ~= nil then
                local newState = parsedJson["is_state_on"]
                if self.toggleButtonProperties.is_state_on.value ~= newState then
                    self.toggleButtonProperties.is_state_on.value = newState
                    if self.onStateChangedCallback ~= nil then self.onStateChangedCallback(newState) end
                end
            end
            if parsedJson["toggle_on_color"] ~= nil then
                local colorArray = parsedJson["toggle_on_color"]
                self.toggleButtonProperties.toggle_on_color.value.r = colorArray[1]
                self.toggleButtonProperties.toggle_on_color.value.g = colorArray[2]
                self.toggleButtonProperties.toggle_on_color.value.b = colorArray[3]
            end
            if parsedJson["toggle_off_color"] ~= nil then
                local colorArray = parsedJson["toggle_off_color"]
                self.toggleButtonProperties.toggle_off_color.value.r = colorArray[1]
                self.toggleButtonProperties.toggle_off_color.value.g = colorArray[2]
                self.toggleButtonProperties.toggle_off_color.value.b = colorArray[3]
            end
            if parsedJson["opacity"] ~= nil then
                self.toggleButtonProperties.opacity.value = tonumber(parsedJson["opacity"])
            end
        end
    end
end

function UiToggleButton:sendDataToReplicator(host)
    local propertiesData, basePropsDirty = self:getUiItemBaseDataToReplicator()

    local isPropsDirty = false
    for key, value in pairs(self.toggleButtonProperties) do
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

function UiToggleButton:update(host, deltaTimeSec) end

function UiToggleButton:setIsStateOn(isStateOn)
    assert(isStateOn ~= nil and type(isStateOn) == "boolean", debug.traceback())
    if self.toggleButtonProperties.is_state_on.value ~= isStateOn then
        self.toggleButtonProperties.is_state_on.value = isStateOn
        self.toggleButtonProperties.is_state_on.dirty = true
    end
end

function UiToggleButton:setOpacity(opacity)
    assert(opacity ~= nil and type(opacity) == "number", debug.traceback())
    if self.toggleButtonProperties.opacity.value ~= opacity then
        self.toggleButtonProperties.opacity.value = opacity
        self.toggleButtonProperties.opacity.dirty = true
    end
end

function UiToggleButton:setToggleOnColorHexValue(colorHex)
    assert(colorHex ~= nil and type(colorHex) == "number", debug.traceback())

    local mask_b = 0xFF;
    local mask_g = 0xFF << 0x8;
    local mask_r = 0xFF << 0x10;

    local r = (mask_r & colorHex) >> 0x10;
    local g = (mask_g & colorHex) >> 0x8;
    local b = mask_b & colorHex;

    local INV_COLOR_MAX_BYTE_VALUE = 1.0 / 255.0;
    self:setToggleOnColor(r * INV_COLOR_MAX_BYTE_VALUE, g * INV_COLOR_MAX_BYTE_VALUE, b * INV_COLOR_MAX_BYTE_VALUE)
end

function UiToggleButton:setToggleOnColor(r, g, b)
    assert(
        r ~= nil and type(r) == "number" and g ~= nil and type(g) == "number" and b ~= nil and type(b) == "number" and r >=
            0.0 and r <= 1.0 and g >= 0.0 and g <= 1.0 and b >= 0.0 and b <= 1.0, debug.traceback())

    self.toggleButtonProperties.toggle_on_color.value.r = r
    self.toggleButtonProperties.toggle_on_color.value.g = g
    self.toggleButtonProperties.toggle_on_color.value.b = b

    self.toggleButtonProperties.toggle_on_color.dirty = true
end

function UiToggleButton:setToggleOffColorHexValue(colorHex)
    assert(colorHex ~= nil and type(colorHex) == "number", debug.traceback())

    local mask_b = 0xFF;
    local mask_g = 0xFF << 0x8;
    local mask_r = 0xFF << 0x10;

    local r = (mask_r & colorHex) >> 0x10;
    local g = (mask_g & colorHex) >> 0x8;
    local b = mask_b & colorHex;

    local INV_COLOR_MAX_BYTE_VALUE = 1.0 / 255.0;
    self:setToggleOffColor(r * INV_COLOR_MAX_BYTE_VALUE, g * INV_COLOR_MAX_BYTE_VALUE, b * INV_COLOR_MAX_BYTE_VALUE)
end

function UiToggleButton:setToggleOffColor(r, g, b)
    assert(
        r ~= nil and type(r) == "number" and g ~= nil and type(g) == "number" and b ~= nil and type(b) == "number" and r >=
            0.0 and r <= 1.0 and g >= 0.0 and g <= 1.0 and b >= 0.0 and b <= 1.0, debug.traceback())

    self.toggleButtonProperties.toggle_off_color.value.r = r
    self.toggleButtonProperties.toggle_off_color.value.g = g
    self.toggleButtonProperties.toggle_off_color.value.b = b

    self.toggleButtonProperties.toggle_off_color.dirty = true
end

function UiToggleButton:setOnIsStateChangedCallback(callback)
    assert(callback ~= nil and type(callback) == "function", debug.traceback())
    self.onStateChangedCallback = callback
end

return UiToggleButton
