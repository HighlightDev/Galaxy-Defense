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
local GameUiWidgetType = require("Ui/Common/gameUiWidgetType")
local json = require("Ui/Core/3rdparty/json")

UiUpgradeIcon = UiItemBase:new()

function UiUpgradeIcon:new(host, name)
    assert(host ~= nil)

    local jsonParameters = nil
    if name ~= nil then
        assert(type(name) == "string" and name ~= "")
        jsonParameters = json.encode({name = name})
    end

    local luaProxyId = CommonUiWidgetCreator:createUiWidget(host, GameUiWidgetType.UI_UPGRADE_ICON, jsonParameters)

    local iconProperties = {
        texture_source = {value = "", dirty = false},
        is_custom_color = {value = false, dirty = false},
        color = {value = {r = 1.0, g = 1.0, b = 1.0}, dirty = false},
        border_color = {value = {r = 0.91, g = 0.36, b = 0.02}, dirty = false},
        glow_color = {value = {r = 1.0, g = 0.55, b = 0.22}, dirty = false},
        fill_color = {value = {r = 0.91, g = 0.36, b = 0.02}, dirty = false},
        fill_strength = {value = 0.55, dirty = false},
        border_thickness_px = {value = 2.0, dirty = false},
        glow_size_px = {value = 10.0, dirty = false},
        opacity = {value = 1.0, dirty = false},
        rotation_degrees = {value = 0.0, dirty = false},
        is_flipped = {value = false, dirty = false},
        glow_visible = {value = false, dirty = false}
    }

    local uiUpgradeIconObj = UiUpgradeIcon.uiItemBaseClass.new(self)
    uiUpgradeIconObj.host = host
    uiUpgradeIconObj.typeName = "UiUpgradeIcon"
    uiUpgradeIconObj.luaProxyId = luaProxyId
    uiUpgradeIconObj.iconProperties = iconProperties

    return uiUpgradeIconObj
end

local function readColor(parsedJson, key, target)
    if parsedJson[key] ~= nil then
        local colorArray = parsedJson[key]
        target.r = colorArray[1]
        target.g = colorArray[2]
        target.b = colorArray[3]
    end
end

function UiUpgradeIcon:updateFromReplicatorData(host)
    self:checkLuaProxyReady(host)
    if self.luaProxyReady then
        local replicatorJsonData = _GetGameThreadData(host, self.luaProxyId)
        if replicatorJsonData ~= "" then
            local parsedJson = json.decode(replicatorJsonData)
            self:extractUiItemBaseReplicatorData(parsedJson)
            if parsedJson["texture_source"] ~= nil then
                self.iconProperties.texture_source.value = parsedJson["texture_source"]
            end
            if parsedJson["opacity"] ~= nil then self.iconProperties.opacity.value = parsedJson["opacity"] end
            if parsedJson["is_custom_color"] ~= nil then
                self.iconProperties.is_custom_color.value = parsedJson["is_custom_color"]
            end
            readColor(parsedJson, "color", self.iconProperties.color.value)
            readColor(parsedJson, "border_color", self.iconProperties.border_color.value)
            readColor(parsedJson, "glow_color", self.iconProperties.glow_color.value)
            readColor(parsedJson, "fill_color", self.iconProperties.fill_color.value)
            if parsedJson["fill_strength"] ~= nil then
                self.iconProperties.fill_strength.value = parsedJson["fill_strength"]
            end
            if parsedJson["border_thickness_px"] ~= nil then
                self.iconProperties.border_thickness_px.value = parsedJson["border_thickness_px"]
            end
            if parsedJson["glow_size_px"] ~= nil then
                self.iconProperties.glow_size_px.value = parsedJson["glow_size_px"]
            end
            if parsedJson["rotation_degrees"] ~= nil then
                self.iconProperties.rotation_degrees.value = parsedJson["rotation_degrees"]
            end
            if parsedJson["is_flipped"] ~= nil then
                self.iconProperties.is_flipped.value = parsedJson["is_flipped"]
            end
            if parsedJson["glow_visible"] ~= nil then
                self.iconProperties.glow_visible.value = parsedJson["glow_visible"]
            end
        end
    end
end

function UiUpgradeIcon:sendDataToReplicator(host)
    local propertiesData, basePropsDirty = self:getUiItemBaseDataToReplicator()

    local isPropsDirty = false
    for key, value in pairs(self.iconProperties) do
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

function UiUpgradeIcon:update(host, deltaTimeSec) end

function UiUpgradeIcon:setTextureSource(textureSource)
    assert(textureSource ~= nil and type(textureSource) == "string")
    if self.iconProperties.texture_source.value ~= textureSource then
        self.iconProperties.texture_source.value = textureSource
        self.iconProperties.texture_source.dirty = true
    end
end

function UiUpgradeIcon:setUseImageCustomColor(isUsed)
    assert(isUsed ~= nil and type(isUsed) == "boolean")
    if self.iconProperties.is_custom_color.value ~= isUsed then
        self.iconProperties.is_custom_color.value = isUsed
        self.iconProperties.is_custom_color.dirty = true
    end
end

local function hexToRgb(colorHex)
    assert(colorHex ~= nil and type(colorHex) == "number")
    local mask_b = 0xFF
    local mask_g = 0xFF << 0x8
    local mask_r = 0xFF << 0x10
    local r = (mask_r & colorHex) >> 0x10
    local g = (mask_g & colorHex) >> 0x8
    local b = mask_b & colorHex
    local INV = 1.0 / 255.0
    return r * INV, g * INV, b * INV
end

local function assertNormalizedColor(r, g, b)
    assert(
        r ~= nil and type(r) == "number" and g ~= nil and type(g) == "number" and b ~= nil and type(b) == "number" and r >=
            0.0 and r <= 1.0 and g >= 0.0 and g <= 1.0 and b >= 0.0 and b <= 1.0)
end

function UiUpgradeIcon:setColor(r, g, b)
    assertNormalizedColor(r, g, b)
    self.iconProperties.color.value.r = r
    self.iconProperties.color.value.g = g
    self.iconProperties.color.value.b = b
    self.iconProperties.color.dirty = true
end

function UiUpgradeIcon:setColorHexValue(colorHex)
    local r, g, b = hexToRgb(colorHex)
    self:setColor(r, g, b)
end

function UiUpgradeIcon:setBorderColor(r, g, b)
    assertNormalizedColor(r, g, b)
    self.iconProperties.border_color.value.r = r
    self.iconProperties.border_color.value.g = g
    self.iconProperties.border_color.value.b = b
    self.iconProperties.border_color.dirty = true
end

function UiUpgradeIcon:setBorderColorHexValue(colorHex)
    local r, g, b = hexToRgb(colorHex)
    self:setBorderColor(r, g, b)
end

function UiUpgradeIcon:setGlowColor(r, g, b)
    assertNormalizedColor(r, g, b)
    self.iconProperties.glow_color.value.r = r
    self.iconProperties.glow_color.value.g = g
    self.iconProperties.glow_color.value.b = b
    self.iconProperties.glow_color.dirty = true
end

function UiUpgradeIcon:setGlowColorHexValue(colorHex)
    local r, g, b = hexToRgb(colorHex)
    self:setGlowColor(r, g, b)
end

function UiUpgradeIcon:setFillColor(r, g, b)
    assertNormalizedColor(r, g, b)
    self.iconProperties.fill_color.value.r = r
    self.iconProperties.fill_color.value.g = g
    self.iconProperties.fill_color.value.b = b
    self.iconProperties.fill_color.dirty = true
end

function UiUpgradeIcon:setFillColorHexValue(colorHex)
    local r, g, b = hexToRgb(colorHex)
    self:setFillColor(r, g, b)
end

function UiUpgradeIcon:setFillStrength(fillStrength)
    assert(fillStrength ~= nil and type(fillStrength) == "number" and fillStrength >= 0.0 and fillStrength <= 1.0)
    if self.iconProperties.fill_strength.value ~= fillStrength then
        self.iconProperties.fill_strength.value = fillStrength
        self.iconProperties.fill_strength.dirty = true
    end
end

function UiUpgradeIcon:setBorderThicknessPx(borderThicknessPx)
    assert(borderThicknessPx ~= nil and type(borderThicknessPx) == "number" and borderThicknessPx >= 0.0)
    if self.iconProperties.border_thickness_px.value ~= borderThicknessPx then
        self.iconProperties.border_thickness_px.value = borderThicknessPx
        self.iconProperties.border_thickness_px.dirty = true
    end
end

function UiUpgradeIcon:setGlowSizePx(glowSizePx)
    assert(glowSizePx ~= nil and type(glowSizePx) == "number" and glowSizePx >= 0.0)
    if self.iconProperties.glow_size_px.value ~= glowSizePx then
        self.iconProperties.glow_size_px.value = glowSizePx
        self.iconProperties.glow_size_px.dirty = true
    end
end

function UiUpgradeIcon:setOpacity(opacity)
    assert(opacity ~= nil and type(opacity) == "number")
    if self.iconProperties.opacity.value ~= opacity then
        self.iconProperties.opacity.value = opacity
        self.iconProperties.opacity.dirty = true
    end
end

function UiUpgradeIcon:setRotationDegrees(rotationDegrees)
    assert(rotationDegrees ~= nil and type(rotationDegrees) == "number")
    if self.iconProperties.rotation_degrees.value ~= rotationDegrees then
        self.iconProperties.rotation_degrees.value = rotationDegrees
        self.iconProperties.rotation_degrees.dirty = true
    end
end

function UiUpgradeIcon:getRotationDegrees() return self.iconProperties.rotation_degrees.value end

function UiUpgradeIcon:setIsFlipped(isFlipped)
    assert(isFlipped ~= nil and type(isFlipped) == "boolean")
    if self.iconProperties.is_flipped.value ~= isFlipped then
        self.iconProperties.is_flipped.value = isFlipped
        self.iconProperties.is_flipped.dirty = true
    end
end

function UiUpgradeIcon:setGlowVisible(isGlowVisible)
    assert(isGlowVisible ~= nil and type(isGlowVisible) == "boolean")
    if self.iconProperties.glow_visible.value ~= isGlowVisible then
        self.iconProperties.glow_visible.value = isGlowVisible
        self.iconProperties.glow_visible.dirty = true
    end
end

function UiUpgradeIcon:isGlowVisible() return self.iconProperties.glow_visible.value end

return UiUpgradeIcon
