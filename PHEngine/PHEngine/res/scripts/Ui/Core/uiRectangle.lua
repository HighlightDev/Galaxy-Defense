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

UiRectangle = UiItemBase:new()

function UiRectangle:new(host, name)
    assert(host ~= nil, debug.traceback())

    local jsonParameters = nil;
    if name ~= nil then
        assert(type(name) == "string" and name ~= "", debug.traceback())
        jsonParameters = json.encode({name = name})
    end

    local luaProxyId = CommonUiWidgetCreator:createUiWidget(host, CommonUiWidgetCreator.CommonUiWidgetType.UI_RECTANGLE,
                                                            jsonParameters)

    local rectangleProperties = {
        color = {value = {r = 0.0, g = 0.0, b = 0.0}, dirty = false},
        opacity = {value = 1.0, dirty = false},
        border_radius = {value = 0.0, dirty = false},
        is_round_top = {value = false, dirty = false},
        is_round_bottom = {value = false, dirty = false},
        -- When apply_blur is true the rectangle's body colour is mixed with the
        -- PostFx Gaussian-blurred scene (sourced from IPostFxRenderTargetProvider
        -- in UiRectangleSceneProxy::Render) by blur_mix in [0,1]. Use it for
        -- frosted-glass-style panels above the 3D scene.
        apply_blur = {value = false, dirty = false},
        blur_mix = {value = 0.0, dirty = false}
    }

    local uiRectangleObj = UiRectangle.uiItemBaseClass.new(self)
    uiRectangleObj.typeName = "UiRectangle"
    uiRectangleObj.luaProxyId = luaProxyId
    uiRectangleObj.rectangleProperties = rectangleProperties
    uiRectangleObj.host = host

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
            if parsedJson["is_round_top"] ~= nil then
                self.rectangleProperties.is_round_top.value = parsedJson["is_round_top"]
            end
            if parsedJson["is_round_bottom"] ~= nil then
                self.rectangleProperties.is_round_bottom.value = parsedJson["is_round_bottom"]
            end
            if parsedJson["apply_blur"] ~= nil then
                self.rectangleProperties.apply_blur.value = parsedJson["apply_blur"]
            end
            if parsedJson["blur_mix"] ~= nil then
                self.rectangleProperties.blur_mix.value = parsedJson["blur_mix"]
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

function UiRectangle:update(host, deltaTimeSec) end

function UiRectangle:setColorHexValue(colorHex)
    assert(colorHex ~= nil and type(colorHex) == "number", debug.traceback())

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
    assert(
        r ~= nil and type(r) == "number" and g ~= nil and type(g) == "number" and b ~= nil and type(b) == "number" and r >=
            0.0 and r <= 1.0 and g >= 0.0 and g <= 1.0 and b >= 0.0 and b <= 1.0, debug.traceback())
    self.rectangleProperties.color.value.r = r
    self.rectangleProperties.color.value.g = g
    self.rectangleProperties.color.value.b = b

    self.rectangleProperties.color.dirty = true
end

function UiRectangle:setOpacity(opacity)
    assert(opacity ~= nil and type(opacity) == "number", debug.traceback())
    if self.rectangleProperties.opacity.value ~= opacity then
        self.rectangleProperties.opacity.value = opacity
        self.rectangleProperties.opacity.dirty = true
    end
end

function UiRectangle:setBorderRadius(borderRadius)
    assert(borderRadius ~= nil and type(borderRadius) == "number", debug.traceback())
    if self.rectangleProperties.border_radius.value ~= borderRadius then
        self.rectangleProperties.border_radius.value = borderRadius
        self.rectangleProperties.border_radius.dirty = true
    end
end

function UiRectangle:setIsRoundTop(bIsRoundTop)
    assert(bIsRoundTop ~= nil and type(bIsRoundTop) == "boolean", debug.traceback())
    if self.rectangleProperties.is_round_top.value ~= bIsRoundTop then
        self.rectangleProperties.is_round_top.value = bIsRoundTop
        self.rectangleProperties.is_round_top.dirty = true
    end
end

function UiRectangle:setIsRoundBottom(bIsRoundBottom)
    assert(bIsRoundBottom ~= nil and type(bIsRoundBottom) == "boolean", debug.traceback())
    if self.rectangleProperties.is_round_bottom.value ~= bIsRoundBottom then
        self.rectangleProperties.is_round_bottom.value = bIsRoundBottom
        self.rectangleProperties.is_round_bottom.dirty = true
    end
end

-- Turns on the PostFx-blur tap. When enabled, the rectangle's body colour is
-- mixed with the Gaussian-blurred scene render target by the current blur_mix.
function UiRectangle:setApplyBlur(applyBlur)
    assert(applyBlur ~= nil and type(applyBlur) == "boolean", debug.traceback())
    if self.rectangleProperties.apply_blur.value ~= applyBlur then
        self.rectangleProperties.apply_blur.value = applyBlur
        self.rectangleProperties.apply_blur.dirty = true
    end
end

-- Blend factor between the rectangle's base colour (0.0) and the sampled blur
-- (1.0). Has no visual effect unless apply_blur is also true.
function UiRectangle:setBlurMix(blurMix)
    assert(blurMix ~= nil and type(blurMix) == "number" and blurMix >= 0.0 and blurMix <= 1.0, debug.traceback())
    if self.rectangleProperties.blur_mix.value ~= blurMix then
        self.rectangleProperties.blur_mix.value = blurMix
        self.rectangleProperties.blur_mix.dirty = true
    end
end

return UiRectangle
