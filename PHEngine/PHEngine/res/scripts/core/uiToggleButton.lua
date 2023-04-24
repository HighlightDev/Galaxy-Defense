--[[ BEGIN *** this snippet h to be inserted everywhere where your want to require custom modules *** BEGIN]]
--
local function setup()
    local str = debug.getinfo(2, "S").source:sub(2)
    local pathToCurrentScript = str:match("(.*/)")
    if pathToCurrentScript ~= nil then
        package.path = package.path .. ";" .. pathToCurrentScript .. "?.lua"
    end
end

setup()
--
--[[ END   *** this snippet has to be inserted everywhere where your want to require custom modules  ***  END]]
local UiItemBase = require("uiItemBase")
local CommonUiWidgetCreator = require("commonUiWidgetCreator")
local json = require("3rdparty/json")

UiToggleButton = UiItemBase:new()

function UiToggleButton:new(host, isStateOn)
    assert(host ~= nil and isStateOn ~= nil and type(isStateOn) == "boolean")
    print("UiToggleButton::ctor")

    local luaProxyId = CommonUiWidgetCreator:createUiWidget(host,
        CommonUiWidgetCreator.CommonUiWidgetType.UI_TOGGLE_BUTTON,
        json.encode({ is_state_on = isStateOn }))

    local toggleButtonProperties = {
        is_state_on = {
            value = isStateOn,
            dirty = false
        },
        opacity = {
            value = 1.0,
            dirty = false
        },
        toggle_on_color = {
            value = {
                r = 0.0,
                g = 0.0,
                b = 0.0
            },
            dirty = false
        },
        toggle_off_color = {
            value = {
                r = 0.0,
                g = 0.0,
                b = 0.0
            },
            dirty = false
        }
    }

    local uiToggleObj = UiToggleButton.uiItemBaseClass.new(self)
    uiToggleObj.typeName = "UiToggleButton"
    uiToggleObj.luaProxyId = luaProxyId
    uiToggleObj.toggleButtonProperties = toggleButtonProperties

    return uiToggleObj
end

local function this(ptr)
    assert(ptr.typeName == "UiToggleButton")
    return ptr
end

function UiToggleButton:updateFromReplicatorData(host)
    self:checkLuaProxyReady(host)
    if self.luaProxyReady then
        local replicatorJsonData = _GetGameThreadData(host, self.luaProxyId)
        if replicatorJsonData ~= "" then
            local parsedJson = json.decode(replicatorJsonData)
            this(self):extractUiItemBaseReplicatorData(parsedJson)

            if parsedJson["is_state_on"] ~= nil then
                this(self).toggleButtonProperties.is_state_on.value = parsedJson["is_state_on"]
            end
            if parsedJson["toggle_on_color"] ~= nil then
                local colorArray = parsedJson["toggle_on_color"]
                this(self).toggleButtonProperties.toggle_on_color.value.r = colorArray[1]
                this(self).toggleButtonProperties.toggle_on_color.value.g = colorArray[2]
                this(self).toggleButtonProperties.toggle_on_color.value.b = colorArray[3]
            end
            if parsedJson["toggle_off_color"] ~= nil then
                local colorArray = parsedJson["toggle_off_color"]
                this(self).toggleButtonProperties.toggle_off_color.value.r = colorArray[1]
                this(self).toggleButtonProperties.toggle_off_color.value.g = colorArray[2]
                this(self).toggleButtonProperties.toggle_off_color.value.b = colorArray[3]
            end
            if parsedJson["opacity"] ~= nil then
                this(self).toggleButtonProperties.opacity.value = tonumber(parsedJson["opacity"])
            end
        end
    end
end

function UiToggleButton:sendDataToReplicator(host)
    local propertiesData, basePropsDirty = UiToggleButton:getUiItemBaseDataToReplicator()

    local isPropsDirty = false
    for key, value in pairs(this(self).toggleButtonProperties) do
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

function UiToggleButton:update(host)
end

function UiToggleButton:setIsStateOn(isStateOn)
    assert(isStateOn ~= nil and type(isStateOn) == "boolean")
    if this(self).toggleButtonProperties.is_state_on.value ~= isStateOn then
        this(self).toggleButtonProperties.is_state_on.value = isStateOn
        this(self).toggleButtonProperties.is_state_on.dirty = true
    end
end

function UiToggleButton:setOpacity(opacity)
    assert(opacity ~= nil and type(opacity) == "number")
    if this(self).toggleButtonProperties.opacity.value ~= opacity then
        this(self).toggleButtonProperties.opacity.value = opacity
        this(self).toggleButtonProperties.opacity.dirty = true
    end
end

function UiToggleButton:setToggleOnColorHexValue(colorHex)
    assert(colorHex ~= nil and type(colorHex) == "number")

    local mask_b = 0xFF;
    local mask_g = 0xFF << 0x8;
    local mask_r = 0xFF << 0x10;

    local r = (mask_r & colorHex) >> 0x10;
    local g = (mask_g & colorHex) >> 0x8;
    local b = mask_b & colorHex;

    local INV_COLOR_MAX_BYTE_VALUE = 1.0 / 255.0;
    this(self):setToggleOnColor(r * INV_COLOR_MAX_BYTE_VALUE, g * INV_COLOR_MAX_BYTE_VALUE, b * INV_COLOR_MAX_BYTE_VALUE)
end

function UiToggleButton:setToggleOnColor(r, g, b)
    assert(r ~= nil and type(r) == "number" and g ~= nil and type(g) == "number" and b ~= nil and type(b) == "number" and
        r >= 0.0 and r <= 1.0 and g >= 0.0 and g <= 1.0 and b >= 0.0 and b <= 1.0)

    this(self).toggleButtonProperties.toggle_on_color.value.r = r
    this(self).toggleButtonProperties.toggle_on_color.value.g = g
    this(self).toggleButtonProperties.toggle_on_color.value.b = b

    this(self).toggleButtonProperties.toggle_on_color.dirty = true
end

function UiToggleButton:setToggleOffColorHexValue(colorHex)
    assert(colorHex ~= nil and type(colorHex) == "number")

    local mask_b = 0xFF;
    local mask_g = 0xFF << 0x8;
    local mask_r = 0xFF << 0x10;

    local r = (mask_r & colorHex) >> 0x10;
    local g = (mask_g & colorHex) >> 0x8;
    local b = mask_b & colorHex;

    local INV_COLOR_MAX_BYTE_VALUE = 1.0 / 255.0;
    this(self):setToggleOffColor(r * INV_COLOR_MAX_BYTE_VALUE, g * INV_COLOR_MAX_BYTE_VALUE, b * INV_COLOR_MAX_BYTE_VALUE)
end

function UiToggleButton:setToggleOffColor(r, g, b)
    assert(r ~= nil and type(r) == "number" and g ~= nil and type(g) == "number" and b ~= nil and type(b) == "number" and
        r >= 0.0 and r <= 1.0 and g >= 0.0 and g <= 1.0 and b >= 0.0 and b <= 1.0)

    this(self).toggleButtonProperties.toggle_off_color.value.r = r
    this(self).toggleButtonProperties.toggle_off_color.value.g = g
    this(self).toggleButtonProperties.toggle_off_color.value.b = b

    this(self).toggleButtonProperties.toggle_off_color.dirty = true
end

return UiToggleButton
