--[[ BEGIN *** this snippet h to be inserted everywhere where your want to require custom modules *** BEGIN]]
--
local function setup()
	local slash = package.config:sub(1,1)
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

function UiLabel:updateFromReplicatorData(host)
    self:checkLuaProxyReady(host)
    if self.luaProxyReady then
        local replicatorJsonData = _GetGameThreadData(host, self.luaProxyId)
        if replicatorJsonData ~= "" then
            local parsedJson = json.decode(replicatorJsonData)
            self:extractUiItemBaseReplicatorData(parsedJson)
            if parsedJson["text"] ~= nil then
                self.labelProperties.text.value = parsedJson["text"]
            end
            if parsedJson["text_color"] ~= nil then
                local colorArray = parsedJson["text_color"]
                self.labelProperties.text_color.value.r = colorArray[1]
                self.labelProperties.text_color.value.g = colorArray[2]
                self.labelProperties.text_color.value.b = colorArray[3]
            end
            if parsedJson["opacity"] ~= nil then
                self.labelProperties.opacity.value = tonumber(parsedJson["opacity"])
            end
            if parsedJson["font_size"] ~= nil then
                self.labelProperties.font_size.value = tonumber(parsedJson["font_size"])
            end
            if parsedJson["text_line_width"] ~= nil then
                self.labelProperties.text_line_width.value = tonumber(parsedJson["text_line_width"])
            end
            if parsedJson["text_horizontal_alignment"] ~= nil then
                self.labelProperties.text_horizontal_alignment.value = tonumber(parsedJson["text_horizontal_alignment"])
            end
        end
    end
end

function UiLabel:sendDataToReplicator(host)
    local propertiesData, basePropsDirty = self:getUiItemBaseDataToReplicator()

    local isPropsDirty = false
    for key, value in pairs(self.labelProperties) do
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
    if self.labelProperties.text.value ~= text then
        self.labelProperties.text.value = text
        self.labelProperties.text.dirty = true
    end
end

function UiLabel:setOpacity(opacity)
    assert(opacity ~= nil and type(opacity) == "number")
    if self.labelProperties.opacity.value ~= opacity then
        self.labelProperties.opacity.value = opacity
        self.labelProperties.opacity.dirty = true
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
    self:setTextColor(r * INV_COLOR_MAX_BYTE_VALUE, g * INV_COLOR_MAX_BYTE_VALUE, b * INV_COLOR_MAX_BYTE_VALUE)
end

function UiLabel:setTextColor(r, g, b)
    assert(r ~= nil and type(r) == "number" and g ~= nil and type(g) == "number" and b ~= nil and type(b) == "number" and
        r >= 0.0 and r <= 1.0 and g >= 0.0 and g <= 1.0 and b >= 0.0 and b <= 1.0)

    self.labelProperties.text_color.value.r = r
    self.labelProperties.text_color.value.g = g
    self.labelProperties.text_color.value.b = b

    self.labelProperties.text_color.dirty = true
end

function UiLabel:setFontSize(fontSize)
    assert(fontSize ~= nil and type(fontSize) == "number")
    if self.labelProperties.font_size.value ~= fontSize then
        self.labelProperties.font_size.value = fontSize
        self.labelProperties.font_size.dirty = true
    end
end

function UiLabel:setTextLineWidth(textLineWidth)
    assert(textLineWidth ~= nil and type(textLineWidth) == "number")
    if self.labelProperties.text_line_width.value ~= textLineWidth then
        self.labelProperties.text_line_width.value = textLineWidth
        self.labelProperties.text_line_width.dirty = true
    end
end

function UiLabel:setTextHorizontalAlignment(textHorizontalAlignment)
    assert(textHorizontalAlignment ~= nil and type(textHorizontalAlignment) == "number" and
        textHorizontalAlignment >= UiLabel.TextHorizontalAlignmentType.LEFT and
        textHorizontalAlignment <= UiLabel.TextHorizontalAlignmentType.RIGHT)
    if self.labelProperties.text_horizontal_alignment.value ~= textHorizontalAlignment then
        self.labelProperties.text_horizontal_alignment.value = textHorizontalAlignment
        self.labelProperties.text_horizontal_alignment.dirty = true
    end
end

return UiLabel
