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
local UiLabel = require("Ui/Core/uiLabel")

UiTextBlock = UiItemBase:new()

function UiTextBlock:new(host, fontName, name)
    assert(host ~= nil and fontName ~= nil and type(fontName) == "string" and fontName ~= "")

    local jsonParameters = nil;
    if name ~= nil then
        assert(type(name) == "string" and name ~= "")
        jsonParameters = json.encode({
            font_name = fontName,
            name = name
        })
    else
        jsonParameters = json.encode({
            font_name = fontName
        })
    end

    local luaProxyId = CommonUiWidgetCreator:createUiWidget(host,
        CommonUiWidgetCreator.CommonUiWidgetType.UI_TEXT_BLOCK, jsonParameters)

    local textBlockProperties = {
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
        },
        text = {
            value = "",
            dirty = false
        },
        text_opacity = {
            value = 1.0,
            dirty = false
        },
        text_color = {
            value = {
                r = 0.0,
                g = 0.0,
                b = 0.0
            },
            dirty = false
        },
        font_size = {
            value = 5.0,
            dirty = false
        },
        text_horizontal_alignment = {
            value = UiLabel.TextHorizontalAlignmentType.LEFT,
            dirty = false
        }
    }

    local uiRectangleObj = UiTextBlock.uiItemBaseClass.new(self)
    uiRectangleObj.typeName = "UiTextBlock"
    uiRectangleObj.luaProxyId = luaProxyId
    uiRectangleObj.textBlockProperties = textBlockProperties
    uiRectangleObj.host = host

    return uiRectangleObj
end

function UiTextBlock:updateFromReplicatorData(host)
    self:checkLuaProxyReady(host)
    if self.luaProxyReady then
        local replicatorJsonData = _GetGameThreadData(host, self.luaProxyId)
        if replicatorJsonData ~= "" then
            local parsedJson = json.decode(replicatorJsonData)
            self:extractUiItemBaseReplicatorData(parsedJson)

            if parsedJson["color"] ~= nil then
                local colorArray = parsedJson["color"]
                self.textBlockProperties.color.value.r = colorArray[1]
                self.textBlockProperties.color.value.g = colorArray[2]
                self.textBlockProperties.color.value.b = colorArray[3]
            end
            if parsedJson["opacity"] ~= nil then
                self.textBlockProperties.opacity.value = parsedJson["opacity"]
            end
            if parsedJson["border_radius"] ~= nil then
                self.textBlockProperties.border_radius.value = parsedJson["border_radius"]
            end
            if parsedJson["text"] ~= nil then
                self.textBlockProperties.text.value = parsedJson["text"]
            end
            if parsedJson["text_color"] ~= nil then
                local colorArray = parsedJson["text_color"]
                self.textBlockProperties.text_color.value.r = colorArray[1]
                self.textBlockProperties.text_color.value.g = colorArray[2]
                self.textBlockProperties.text_color.value.b = colorArray[3]
            end
            if parsedJson["text_opacity"] ~= nil then
                self.textBlockProperties.text_opacity.value = tonumber(parsedJson["text_opacity"])
            end
            if parsedJson["font_size"] ~= nil then
                self.textBlockProperties.font_size.value = tonumber(parsedJson["font_size"])
            end
            if parsedJson["text_horizontal_alignment"] ~= nil then
                self.textBlockProperties.text_horizontal_alignment.value = tonumber(
                    parsedJson["text_horizontal_alignment"])
            end
        end
    end
end

function UiTextBlock:sendDataToReplicator(host)
    local propertiesData, basePropsDirty = self:getUiItemBaseDataToReplicator()

    local isPropsDirty = false
    for key, value in pairs(self.textBlockProperties) do
        if value.dirty then
            isPropsDirty = true
            propertiesData[tostring(key)] = value.value
            print("UiTextBlock:sendDataToReplicator: " .. tostring(key) .. " = " .. tostring(value.value))
            value.dirty = false
        end
    end
    if basePropsDirty or isPropsDirty then
        _OnCommonUiWidgetDataUpdated(host, self.luaProxyId, json.encode(propertiesData))
    end
end

function UiTextBlock:update(host)
end

function UiTextBlock:setRectangleColorHexValue(colorHex)
    assert(colorHex ~= nil and type(colorHex) == "number")

    local mask_b = 0xFF;
    local mask_g = 0xFF << 0x8;
    local mask_r = 0xFF << 0x10;

    local r = (mask_r & colorHex) >> 0x10;
    local g = (mask_g & colorHex) >> 0x8;
    local b = mask_b & colorHex;

    local INV_COLOR_MAX_BYTE_VALUE = 1.0 / 255.0;
    self:setRectangleColor(r * INV_COLOR_MAX_BYTE_VALUE, g * INV_COLOR_MAX_BYTE_VALUE, b * INV_COLOR_MAX_BYTE_VALUE)
end

function UiTextBlock:setRectangleColor(r, g, b)
    assert(
        r ~= nil and type(r) == "number" and g ~= nil and type(g) == "number" and b ~= nil and type(b) == "number" and r >=
            0.0 and r <= 1.0 and g >= 0.0 and g <= 1.0 and b >= 0.0 and b <= 1.0)
    self.textBlockProperties.color.value.r = r
    self.textBlockProperties.color.value.g = g
    self.textBlockProperties.color.value.b = b

    self.textBlockProperties.color.dirty = true
end

function UiTextBlock:setOpacity(opacity)
    assert(opacity ~= nil and type(opacity) == "number")
    if self.textBlockProperties.opacity.value ~= opacity then
        self.textBlockProperties.opacity.value = opacity
        self.textBlockProperties.opacity.dirty = true
    end
end

function UiTextBlock:setRectangleBorderRadius(borderRadius)
    assert(borderRadius ~= nil and type(borderRadius) == "number")
    if self.textBlockProperties.border_radius.value ~= borderRadius then
        self.textBlockProperties.border_radius.value = borderRadius
        self.textBlockProperties.border_radius.dirty = true
    end
end

function UiTextBlock:setText(text)
    assert(text ~= nil and type(text) == "string")
    if self.textBlockProperties.text.value ~= text then
        self.textBlockProperties.text.value = text
        self.textBlockProperties.text.dirty = true
    end
end

function UiTextBlock:setOpacity(opacity)
    assert(opacity ~= nil and type(opacity) == "number")
    if self.textBlockProperties.text_opacity.value ~= opacity then
        self.textBlockProperties.text_opacity.value = opacity
        self.textBlockProperties.text_opacity.dirty = true
    end
end

function UiTextBlock:setTextColorHexValue(colorHex)
    assert(colorHex ~= nil and type(colorHex) == "number")

    local mask_b = 0xFF;
    local mask_g = 0xFF << 0x8;
    local mask_r = 0xFF << 0x10;

    local r = (mask_r & colorHex) >> 0x10;
    local g = (mask_g & colorHex) >> 0x8;
    local b = mask_b & colorHex;

    local INV_COLOR_MAX_BYTE_VALUE = 1.0 / 255.0;
    self:setTextColor(r * INV_COLOR_MAX_BYTE_VALUE, g * INV_COLOR_MAX_BYTE_VALUE, b * INV_COLOR_MAX_BYTE_VALUE)
end

function UiTextBlock:setTextColor(r, g, b)
    assert(
        r ~= nil and type(r) == "number" and g ~= nil and type(g) == "number" and b ~= nil and type(b) == "number" and r >=
            0.0 and r <= 1.0 and g >= 0.0 and g <= 1.0 and b >= 0.0 and b <= 1.0)

    self.textBlockProperties.text_color.value.r = r
    self.textBlockProperties.text_color.value.g = g
    self.textBlockProperties.text_color.value.b = b

    self.textBlockProperties.text_color.dirty = true
end

function UiTextBlock:setFontSize(fontSize)
    assert(fontSize ~= nil and type(fontSize) == "number")
    if self.textBlockProperties.font_size.value ~= fontSize then
        self.textBlockProperties.font_size.value = fontSize
        self.textBlockProperties.font_size.dirty = true
    end
end

function UiTextBlock:setTextHorizontalAlignment(textHorizontalAlignment)
    assert(textHorizontalAlignment ~= nil and type(textHorizontalAlignment) == "number" and textHorizontalAlignment >=
               UiLabel.TextHorizontalAlignmentType.LEFT and textHorizontalAlignment <=
               UiLabel.TextHorizontalAlignmentType.RIGHT)
    if self.textBlockProperties.text_horizontal_alignment.value ~= textHorizontalAlignment then
        self.textBlockProperties.text_horizontal_alignment.value = textHorizontalAlignment
        self.textBlockProperties.text_horizontal_alignment.dirty = true
    end
end

return UiTextBlock
