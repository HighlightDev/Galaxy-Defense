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

UiLabel = UiItemBase:new()
UiLabel.TextHorizontalAlignmentType = {
    LEFT = 0,
    CENTER = 1,
    RIGHT = 2
}

function UiLabel:new(host, fontName)
    assert(host ~= nil and fontName ~= nil and type(fontName) == "string" and fontName ~= "")
    print("UiLabel::ctor")

    local luaProxyId = CommonUiWidgetCreator:createUiWidget(host, CommonUiWidgetCreator.CommonUiWidgetType.UI_LABEL,
        json.encode({ font_name = fontName }))

    local labelProperties = {
        text = {
            value = "",
            dirty = false
        },
        opacity = {
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
        text_line_width = {
            value = 1.0,
            dirty = false
        },
        text_horizontal_alignment = {
            value = UiLabel.TextHorizontalAlignmentType.LEFT,
            dirty = false
        }
    }

    local uiLabelObj = UiLabel.uiItemBaseClass.new(self)
    uiLabelObj.typeName = "UiLabel"
    uiLabelObj.luaProxyId = luaProxyId
    uiLabelObj.labelProperties = labelProperties

    return uiLabelObj
end

local function this(ptr)
    assert(ptr.typeName == "UiLabel")
    return ptr
end

function UiLabel:updateFromReplicatorData(host)
    self:checkLuaProxyReady(host)
    if self.luaProxyReady then
        local replicatorJsonData = _GetGameThreadData(host, self.luaProxyId)
        if replicatorJsonData ~= "" then
            local parsedJson = json.decode(replicatorJsonData)
            this(self):extractUiItemBaseReplicatorData(parsedJson)
            if parsedJson["text"] ~= nil then
                this(self).labelProperties.text.value = parsedJson["text"]
            end
            if parsedJson["text_color"] ~= nil then
                local colorArray = parsedJson["text_color"]
                this(self).labelProperties.text_color.value.r = colorArray[1]
                this(self).labelProperties.text_color.value.g = colorArray[2]
                this(self).labelProperties.text_color.value.b = colorArray[3]
            end
            if parsedJson["opacity"] ~= nil then
                this(self).labelProperties.opacity.value = tonumber(parsedJson["opacity"])
            end
            if parsedJson["font_size"] ~= nil then
                this(self).labelProperties.font_size.value = tonumber(parsedJson["font_size"])
            end
            if parsedJson["text_line_width"] ~= nil then
                this(self).labelProperties.text_line_width.value = tonumber(parsedJson["text_line_width"])
            end
            if parsedJson["text_horizontal_alignment"] ~= nil then
                this(self).labelProperties.text_horizontal_alignment.value = tonumber(parsedJson["text_horizontal_alignment"])
            end
        end
    end
end

function UiLabel:sendDataToReplicator(host)
    local propertiesData, basePropsDirty = UiLabel:getUiItemBaseDataToReplicator()

    local isPropsDirty = false
    for key, value in pairs(this(self).labelProperties) do
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

function UiLabel:update(host)
end

function UiLabel:setText(text)
    assert(text ~= nil and type(text) == "string")
    if this(self).labelProperties.text.value ~= text then
        this(self).labelProperties.text.value = text
        this(self).labelProperties.text.dirty = true
    end
end

function UiLabel:setOpacity(opacity)
    assert(opacity ~= nil and type(opacity) == "number")
    if this(self).labelProperties.opacity.value ~= opacity then
        this(self).labelProperties.opacity.value = opacity
        this(self).labelProperties.opacity.dirty = true
    end
end

function UiLabel:setTextColorHexValue(colorHex)
    assert(colorHex ~= nil and type(colorHex) == "number")

    local mask_b = 0xFF;
    local mask_g = 0xFF << 0x8;
    local mask_r = 0xFF << 0x10;

    local r = (mask_r & colorHex) >> 0x10;
    local g = (mask_g & colorHex) >> 0x8;
    local b = mask_b & colorHex;

    local INV_COLOR_MAX_BYTE_VALUE = 1.0 / 255.0;
    this(self):setTextColor(r * INV_COLOR_MAX_BYTE_VALUE, g * INV_COLOR_MAX_BYTE_VALUE, b * INV_COLOR_MAX_BYTE_VALUE)
end

function UiLabel:setTextColor(r, g, b)
    assert(r ~= nil and type(r) == "number" and g ~= nil and type(g) == "number" and b ~= nil and type(b) == "number" and
        r >= 0.0 and r <= 1.0 and g >= 0.0 and g <= 1.0 and b >= 0.0 and b <= 1.0)

    this(self).labelProperties.text_color.value.r = r
    this(self).labelProperties.text_color.value.g = g
    this(self).labelProperties.text_color.value.b = b

    this(self).labelProperties.text_color.dirty = true
end

function UiLabel:setFontSize(fontSize)
    assert(fontSize ~= nil and type(fontSize) == "number")
    if this(self).labelProperties.font_size.value ~= fontSize then
        this(self).labelProperties.font_size.value = fontSize
        this(self).labelProperties.font_size.dirty = true
    end
end

function UiLabel:setTextLineWidth(textLineWidth)
    assert(textLineWidth ~= nil and type(textLineWidth) == "number")
    if this(self).labelProperties.text_line_width.value ~= textLineWidth then
        this(self).labelProperties.text_line_width.value = textLineWidth
        this(self).labelProperties.text_line_width.dirty = true
    end
end

function UiLabel:setTextHorizontalAlignment(textHorizontalAlignment)
    assert(textHorizontalAlignment ~= nil and type(textHorizontalAlignment) == "number" and
        textHorizontalAlignment >= UiLabel.TextHorizontalAlignmentType.LEFT and
        textHorizontalAlignment <= UiLabel.TextHorizontalAlignmentType.RIGHT)
    if this(self).labelProperties.text_horizontal_alignment.value ~= textHorizontalAlignment then
        this(self).labelProperties.text_horizontal_alignment.value = textHorizontalAlignment
        this(self).labelProperties.text_horizontal_alignment.dirty = true
    end
end

return UiLabel
