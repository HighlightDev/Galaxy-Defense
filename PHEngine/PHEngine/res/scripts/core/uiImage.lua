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
        textureSource = {
            value = "",
            dirty = false
        },
        opacity = {
            value = 1.0,
            dirty = false
        },
        rotationDegrees = {
            value = 0.0,
            dirty = false
        },
        isFlipped = {
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

local function this(ptr)
    assert(ptr.typeName == "UiImage")
    return ptr
end

function UiImage:updateFromReplicatorData(host)
    self:checkLuaProxyReady(host)
    if self.luaProxyReady then
        local replicatorJsonData = _GetGameThreadData(host, self.luaProxyId)
        if replicatorJsonData ~= "" then
            local parsedJson = json.decode(replicatorJsonData)
            this(self):extractUiItemBaseReplicatorData(parsedJson)
            if parsedJson["texture_source"] ~= nil then
                this(self).imageProperties.textureSource.value = parsedJson["texture_source"]
            end
            if parsedJson["opacity"] ~= nil then
                this(self).imageProperties.opacity.value = parsedJson["opacity"]
            end
            if parsedJson["rotation_degrees"] ~= nil then
                this(self).imageProperties.rotationDegrees.value = parsedJson["rotation_degrees"]
            end
            if parsedJson["is_flipped"] ~= nil then
                this(self).imageProperties.isFlipped.value = parsedJson["is_flipped"]
            end
        end
    end
end

function UiImage:sendDataToReplicator(host)
    local propertiesData, basePropsDirty = UiImage:getUiItemBaseDataToReplicator()

    local isPropsDirty = false
    for key, value in pairs(this(self).imageProperties) do
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
    if this(self).imageProperties.textureSource ~= textureSource then
        this(self).imageProperties.textureSource = textureSource
        this(self).imageProperties.textureSource.dirty = true
    end
end

function UiImage:setOpacity(opacity)
    assert(opacity ~= nil and type(opacity) == "number")
    if this(self).imageProperties.opacity.value ~= opacity then
        this(self).imageProperties.opacity.value = opacity
        this(self).imageProperties.opacity.dirty = true
    end
end

function UiImage:setRotationDegrees(rotationDegrees)
    assert(rotationDegrees ~= nil and type(rotationDegrees) == "number")
    if this(self).imageProperties.rotationDegrees.value ~= rotationDegrees then
        this(self).imageProperties.rotationDegrees.value = rotationDegrees
        this(self).imageProperties.rotationDegrees.dirty = true
    end
end

function UiImage:setIsFlipped(isFlipped)
    assert(isFlipped ~= nil and type(isFlipped) == "boolean")
    if this(self).imageProperties.isFlipped.value ~= isFlipped then
        this(self).imageProperties.isFlipped.value = isFlipped
        this(self).imageProperties.isFlipped.dirty = true
    end
end

return UiImage
