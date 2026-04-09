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
        jsonParameters = json.encode({font_name = fontName, name = name})
    else
        jsonParameters = json.encode({font_name = fontName})
    end

    local luaProxyId = CommonUiWidgetCreator:createUiWidget(host,
                                                            CommonUiWidgetCreator.CommonUiWidgetType.UI_TEXT_BLOCK,
                                                            jsonParameters)

    local textBlockProperties = {
        rectangle_color = {value = {r = 0.0, g = 0.0, b = 0.0}, dirty = false},
        rectangle_opacity = {value = 1.0, dirty = false},
        rectangle_radius = {value = 0.0, dirty = false},
        border_color = {value = {r = 0.0, g = 0.0, b = 0.0}, dirty = false},
        border_opacity = {value = 1.0, dirty = false},
        border_radius = {value = 0.0, dirty = false},
        text = {value = "", dirty = false},
        text_opacity = {value = 1.0, dirty = false},
        text_color = {value = {r = 0.0, g = 0.0, b = 0.0}, dirty = false},
        font_size = {value = 5.0, dirty = false},
        text_horizontal_alignment = {value = UiLabel.TextHorizontalAlignmentType.LEFT, dirty = false},
        text_vertical_alignment = {value = UiLabel.TextVerticalAlignmentType.TOP, dirty = false},
        attach_target_ui_item_name = {value = "", dirty = false},
        border_thickness = {value = 1, dirty = false}
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

            if parsedJson["rectangle_color"] ~= nil then
                local colorArray = parsedJson["rectangle_color"]
                self.textBlockProperties.rectangle_color.value.r = colorArray[1]
                self.textBlockProperties.rectangle_color.value.g = colorArray[2]
                self.textBlockProperties.rectangle_color.value.b = colorArray[3]
            end
            if parsedJson["rectangle_opacity"] ~= nil then
                self.textBlockProperties.rectangle_opacity.value = parsedJson["rectangle_opacity"]
            end
            if parsedJson["rectangle_radius"] ~= nil then
                self.textBlockProperties.rectangle_radius.value = parsedJson["rectangle_radius"]
            end
            if parsedJson["border_color"] ~= nil then
                local colorArray = parsedJson["border_color"]
                self.textBlockProperties.border_color.value.r = colorArray[1]
                self.textBlockProperties.border_color.value.g = colorArray[2]
                self.textBlockProperties.border_color.value.b = colorArray[3]
            end
            if parsedJson["border_opacity"] ~= nil then
                self.textBlockProperties.border_opacity.value = parsedJson["border_opacity"]
            end
            if parsedJson["border_radius"] ~= nil then
                self.textBlockProperties.border_radius.value = parsedJson["border_radius"]
            end
            if parsedJson["text"] ~= nil then self.textBlockProperties.text.value = parsedJson["text"] end
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
            if parsedJson["text_vertical_alignment"] ~= nil then
                self.textBlockProperties.text_vertical_alignment.value = tonumber(parsedJson["text_vertical_alignment"])
            end
            if parsedJson["attach_target_ui_item_name"] ~= nil then
                self.textBlockProperties.attach_target_ui_item_name.value = tostring(
                                                                                parsedJson["attach_target_ui_item_name"])
            end
            if parsedJson["border_thickness"] ~= nil then
                self.textBlockProperties.border_thickness.value = tonumber(parsedJson["border_thickness"])
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
            value.dirty = false
        end
    end
    if basePropsDirty or isPropsDirty then
        _OnCommonUiWidgetDataUpdated(host, self.luaProxyId, json.encode(propertiesData))
    end
end

function UiTextBlock:update(host, deltaTimeSec) end

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
    self.textBlockProperties.rectangle_color.value.r = r
    self.textBlockProperties.rectangle_color.value.g = g
    self.textBlockProperties.rectangle_color.value.b = b

    self.textBlockProperties.rectangle_color.dirty = true
end

function UiTextBlock:setRectangleOpacity(opacity)
    assert(opacity ~= nil and type(opacity) == "number")
    if self.textBlockProperties.rectangle_opacity.value ~= opacity then
        self.textBlockProperties.rectangle_opacity.value = opacity
        self.textBlockProperties.rectangle_opacity.dirty = true
    end
end

function UiTextBlock:setRectangleRadius(borderRadius)
    assert(borderRadius ~= nil and type(borderRadius) == "number")
    if self.textBlockProperties.rectangle_radius.value ~= borderRadius then
        self.textBlockProperties.rectangle_radius.value = borderRadius
        self.textBlockProperties.rectangle_radius.dirty = true
    end
end

function UiTextBlock:setBorderColor(r, g, b)
    assert(
        r ~= nil and type(r) == "number" and g ~= nil and type(g) == "number" and b ~= nil and type(b) == "number" and r >=
            0.0 and r <= 1.0 and g >= 0.0 and g <= 1.0 and b >= 0.0 and b <= 1.0)

    self.textBlockProperties.border_color.value.r = r
    self.textBlockProperties.border_color.value.g = g
    self.textBlockProperties.border_color.value.b = b

    self.textBlockProperties.border_color.dirty = true
end

function UiTextBlock:setBorderColorHexValue(colorHex)
    assert(colorHex ~= nil and type(colorHex) == "number")

    local mask_b = 0xFF;
    local mask_g = 0xFF << 0x8;
    local mask_r = 0xFF << 0x10;

    local r = (mask_r & colorHex) >> 0x10;
    local g = (mask_g & colorHex) >> 0x8;
    local b = mask_b & colorHex;

    local INV_COLOR_MAX_BYTE_VALUE = 1.0 / 255.0;
    self:setBorderColor(r * INV_COLOR_MAX_BYTE_VALUE, g * INV_COLOR_MAX_BYTE_VALUE, b * INV_COLOR_MAX_BYTE_VALUE)
end

function UiTextBlock:setBorderRadius(borderRadius)
    assert(borderRadius ~= nil and type(borderRadius) == "number")
    if self.textBlockProperties.border_radius.value ~= borderRadius then
        self.textBlockProperties.border_radius.value = borderRadius
        self.textBlockProperties.border_radius.dirty = true
    end
end

function UiTextBlock:setBorderOpacity(opacity)
    assert(opacity ~= nil and type(opacity) == "number")
    if self.textBlockProperties.border_opacity.value ~= opacity then
        self.textBlockProperties.border_opacity.value = opacity
        self.textBlockProperties.border_opacity.dirty = true
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

function UiTextBlock:setTextVerticalAlignment(textVerticalAlignment)
    assert(textVerticalAlignment ~= nil and type(textVerticalAlignment) == "number" and textVerticalAlignment >=
               UiLabel.TextVerticalAlignmentType.TOP and textVerticalAlignment <=
               UiLabel.TextVerticalAlignmentType.BOTTOM)
    if self.textBlockProperties.text_vertical_alignment.value ~= textVerticalAlignment then
        self.textBlockProperties.text_vertical_alignment.value = textVerticalAlignment
        self.textBlockProperties.text_vertical_alignment.dirty = true
    end
end

function UiTextBlock:setAttachTargetUiItemName(uiItemName)
    assert(uiItemName ~= nil and type(uiItemName) == "string")
    if self.textBlockProperties.attach_target_ui_item_name.value ~= uiItemName then
        self.textBlockProperties.attach_target_ui_item_name.value = uiItemName
        self.textBlockProperties.attach_target_ui_item_name.dirty = true
    end
end

function UiTextBlock:setBorderThickness(thickness)
    assert(thickness ~= nil and type(thickness) == "number")
    if self.textBlockProperties.border_thickness.value ~= thickness then
        self.textBlockProperties.border_thickness.value = thickness
        self.textBlockProperties.border_thickness.dirty = true
    end
end

return UiTextBlock
