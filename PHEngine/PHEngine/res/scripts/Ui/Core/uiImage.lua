--[[ BEGIN *** this snippet h to be inserted everywhere where your want to require custom modules *** BEGIN]]
--
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

UiImage = UiItemBase:new()

function UiImage:new(host, name)
    assert(host ~= nil)

    local jsonParameters = nil;
    if name ~= nil then
        assert(type(name) == "string" and name ~= "")
        jsonParameters = json.encode({ name = name })
    end

    local luaProxyId = CommonUiWidgetCreator:createUiWidget(host, CommonUiWidgetCreator.CommonUiWidgetType.UI_IMAGE,
        jsonParameters)

    local imageProperties = {
        texture_source = {
            value = "",
            dirty = false
        },
        is_custom_color = {
            value = false,
            dirty = false
        },
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
        rotation_degrees = {
            value = 0.0,
            dirty = false
        },
        is_flipped = {
            value = false,
            dirty = false
        }
    }

    local uiImageObj = UiImage.uiItemBaseClass.new(self)
    uiImageObj.host = host
    uiImageObj.typeName = "UiImage"
    uiImageObj.luaProxyId = luaProxyId
    uiImageObj.imageProperties = imageProperties

    return uiImageObj
end

function UiImage:updateFromReplicatorData(host)
    self:checkLuaProxyReady(host)
    if self.luaProxyReady then
        local replicatorJsonData = _GetGameThreadData(host, self.luaProxyId)
        if replicatorJsonData ~= "" then
            local parsedJson = json.decode(replicatorJsonData)
            self:extractUiItemBaseReplicatorData(parsedJson)
            if parsedJson["texture_source"] ~= nil then
                self.imageProperties.texture_source.value = parsedJson["texture_source"]
            end
            if parsedJson["opacity"] ~= nil then
                self.imageProperties.opacity.value = parsedJson["opacity"]
            end
            if parsedJson["is_custom_color"] ~= nil then
                self.imageProperties.is_custom_color.value = parsedJson["is_custom_color"]
            end
            if parsedJson["color"] ~= nil then
                local colorArray = parsedJson["color"]
                self.imageProperties.color.value.r = colorArray[1]
                self.imageProperties.color.value.g = colorArray[2]
                self.imageProperties.color.value.b = colorArray[3]
            end
            if parsedJson["rotation_degrees"] ~= nil then
                self.imageProperties.rotation_degrees.value = parsedJson["rotation_degrees"]
            end
            if parsedJson["is_flipped"] ~= nil then
                self.imageProperties.is_flipped.value = parsedJson["is_flipped"]
            end
        end
    end
end

function UiImage:sendDataToReplicator(host)
    local propertiesData, basePropsDirty = self:getUiItemBaseDataToReplicator()

    local isPropsDirty = false
    for key, value in pairs(self.imageProperties) do
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

function UiImage:update(host)
end

function UiImage:setTextureSource(textureSource)
    assert(textureSource ~= nil and type(textureSource) == "string")
    if self.imageProperties.texture_source.value ~= textureSource then
        self.imageProperties.texture_source.value = textureSource
        self.imageProperties.texture_source.dirty = true
    end
end

function UiImage:setUseImageCustomColor(isUsed)
    assert(isUsed ~= nil and type(isUsed) == "boolean")
    if self.imageProperties.is_custom_color.value ~= isUsed then
        self.imageProperties.is_custom_color.value = isUsed
        self.imageProperties.is_custom_color.dirty = true
    end
end

function UiImage:setColorHexValue(colorHex)
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

function UiImage:setColor(r, g, b)
    assert(r ~= nil and type(r) == "number" and g ~= nil and type(g) == "number" and b ~= nil and type(b) == "number" and
        r >= 0.0 and r <= 1.0 and g >= 0.0 and g <= 1.0 and b >= 0.0 and b <= 1.0)
    self.imageProperties.color.value.r = r
    self.imageProperties.color.value.g = g
    self.imageProperties.color.value.b = b

    self.imageProperties.color.dirty = true
end

function UiImage:setOpacity(opacity)
    assert(opacity ~= nil and type(opacity) == "number")
    if self.imageProperties.opacity.value ~= opacity then
        self.imageProperties.opacity.value = opacity
        self.imageProperties.opacity.dirty = true
    end
end

function UiImage:setRotationDegrees(rotationDegrees)
    assert(rotationDegrees ~= nil and type(rotationDegrees) == "number")
    if self.imageProperties.rotation_degrees.value ~= rotationDegrees then
        self.imageProperties.rotation_degrees.value = rotationDegrees
        self.imageProperties.rotation_degrees.dirty = true
    end
end

function UiImage:getRotationDegrees()
    return self.imageProperties.rotation_degrees.value
end

function UiImage:setIsFlipped(isFlipped)
    assert(isFlipped ~= nil and type(isFlipped) == "boolean")
    if self.imageProperties.is_flipped.value ~= isFlipped then
        self.imageProperties.is_flipped.value = isFlipped
        self.imageProperties.is_flipped.dirty = true
    end
end

return UiImage
