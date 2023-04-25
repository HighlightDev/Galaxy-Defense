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

UiImage = UiItemBase:new()

function UiImage:new(host)
    assert(host ~= nil)
    print("UiImage::ctor")

    local luaProxyId = CommonUiWidgetCreator:createUiWidget(host, CommonUiWidgetCreator.CommonUiWidgetType.UI_IMAGE)

    local imageProperties = {
        texture_source = {
            value = "",
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

function UiImage:setIsFlipped(isFlipped)
    assert(isFlipped ~= nil and type(isFlipped) == "boolean")
    if self.imageProperties.is_flipped.value ~= isFlipped then
        self.imageProperties.is_flipped.value = isFlipped
        self.imageProperties.is_flipped.dirty = true
    end
end

return UiImage
