--[[ BEGIN *** this snippet has to be inserted everywhere where your want to require custom modules *** BEGIN]]
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

local UiRectangle = require("Ui/Core/uiRectangle")
local UiLabel = require("Ui/Core/uiLabel")

LabelButton = {
}

function LabelButton:new(host, overlay, labelFontName)
    assert(host ~= nil and type(host) == "userdata" and overlay ~= nil and type(overlay) == "table")

    local newObj = {
        host = host,
        overlayCanvasName = "",
        parentName = "",
        buttonContainer = nil,
        label = nil,
        buttonWidth = 0,
        buttonHeight = 0,
        containerColor = 0xffffff,
        widgetName = ""
    }

    newObj.buttonContainer = UiRectangle:new(host)
    newObj.label = UiLabel:new(host, labelFontName)

    overlay:addWidget(newObj.buttonContainer)
    overlay:addWidget(newObj.label)

    self.__index = self
    return setmetatable(newObj, self)
end

function LabelButton:setParent(host, overlayCanvasName, parentName)
    assert(
        host ~= nil and type(host) == "userdata" and type(overlayCanvasName) == "string" and overlayCanvasName ~= "" and
        type(parentName) == "string" and parentName ~= "", debug.traceback())

    self.overlayCanvasName = overlayCanvasName
    self.parentName = parentName
end

function LabelButton:onPreCompoundWidgetInitialize()
    self.widgetName = self.buttonContainer.widgetName
end

function LabelButton:onCompoundWidgetInitialize()
    local labelHeight = self.buttonHeight * 0.75
    local labelTopMargin = self.buttonHeight * 0.15
    local labelSideMargin = self.buttonWidth * 0.1

    self.buttonContainer:setParent(self.host, self.overlayCanvasName, self.parentName)
    self.buttonContainer:setZOrder(3);
    self.buttonContainer:setHeight(self.buttonHeight);
    self.buttonContainer:setWidth(self.buttonWidth);
    self.buttonContainer:setColorHexValue(self.containerColor)
    self.buttonContainer:enableMouseInputReceiverBase(self.host)

    self.label:setParent(self.host, self.overlayCanvasName, self.buttonContainer.widgetName)
    self.label:setZOrder(4);
    self.label:setHeight(labelHeight);
    self.label:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, self.buttonContainer.widgetName,
        labelSideMargin)
    self.label:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, self.buttonContainer.widgetName,
        labelSideMargin)
    self.label:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, self.buttonContainer.widgetName,
        labelTopMargin)
end

function LabelButton:setLabelTextHorizontalAlignment(textHorizontalAlignment)
    assert(textHorizontalAlignment ~= nil and type(textHorizontalAlignment) == "number" and
        textHorizontalAlignment >= UiLabel.TextHorizontalAlignmentType.LEFT and
        textHorizontalAlignment <= UiLabel.TextHorizontalAlignmentType.RIGHT)

    self.label:setTextHorizontalAlignment(textHorizontalAlignment)
end

function LabelButton:setWidth(width)
    assert(width ~= nil and type(width) == "number")

    self.buttonWidth = width;
    self:resizeWidgets()
end

function LabelButton:setHeight(height)
    assert(height ~= nil and type(height) == "number")

    self.buttonHeight = height;
    self:resizeWidgets()
end

function LabelButton:setAnchor(srcAnchor, dstAnchor, dstUiItemWidgetName, anchorMargin)
    assert(srcAnchor ~= nil and dstAnchor ~= nil and dstUiItemWidgetName ~= nil and
        srcAnchor > UiItemBase.UiAnchorType.NONE and srcAnchor <= UiItemBase.UiAnchorType.HORIZONTAL_CENTER)

    self.buttonContainer:setAnchor(srcAnchor, dstAnchor, dstUiItemWidgetName, anchorMargin)
    self:resizeWidgets()
end

function LabelButton:subscriveOnMouseInputClickedCallback(callback)
    assert(callback ~= nil and type(callback) == "function")
    self.buttonContainer:subscriveOnMouseInputClickedCallback(callback)
end

function LabelButton:addAnimation(host, animationName, animationFunctionType, animationDuration, animatedPropertyName,
                                  animatedPropertyType, propertySrcValue, propertyDstValue)
    assert(host ~= nil and type(host) == "userdata")
    assert(animationName ~= nil and type(animationName) == "string" and animationFunctionType ~= nil and
        type(animationFunctionType) == "number" and animationDuration ~= nil and
        type(animationDuration) == "number" and animatedPropertyName ~= nil and type(animatedPropertyName) == "string" and
        animatedPropertyType ~= nil and type(animatedPropertyType) == "number")
    assert(propertySrcValue ~= nil and propertyDstValue ~= nil and type(propertySrcValue) == type(propertyDstValue))

    self.buttonContainer:addAnimation(host, animationName, animationFunctionType, animationDuration,
        animatedPropertyName,
        animatedPropertyType, propertySrcValue, propertyDstValue)
    self.label:addAnimation(host, animationName, animationFunctionType, animationDuration,
        animatedPropertyName,
        animatedPropertyType, propertySrcValue, propertyDstValue)
end

function LabelButton:startAnimation(host, animationName)
    assert(host ~= nil and type(host) == "userdata")
    assert(animationName ~= nil and type(animationName) == "string")

    self.buttonContainer:startAnimation(host, animationName)
    self.label:startAnimation(host, animationName)
end

function LabelButton:setLabelText(labelText)
    assert(labelText ~= nil and type(labelText) == "string")
    self.label:setText(labelText)
end

function LabelButton:setButtonBorderRadius(radius)
    assert(radius ~= nil and type(radius) == "number")
    self.buttonContainer:setBorderRadius(radius)
end

function LabelButton:setLabelOpacity(opacity)
    assert(opacity ~= nil and type(opacity) == "number")
    self.label:setOpacity(opacity)
end

function LabelButton:resizeWidgets()
    local labelHeight = self.buttonHeight * 0.75

    self.buttonContainer:setHeight(self.buttonHeight);
    self.buttonContainer:setWidth(self.buttonWidth);
    self.label:setHeight(labelHeight);
end

function LabelButton:setButtonColorHexValue(colorHex)
    assert(colorHex ~= nil and type(colorHex) == "number")

    self.containerColor = colorHex
    self.buttonContainer:setColorHexValue(colorHex)
end

function LabelButton:setLabelTextColorHexValue(colorHex)
    assert(colorHex ~= nil and type(colorHex) == "number")

    self.label:setTextColorHexValue(colorHex)
end

function LabelButton:setZOrder(zOrder)
    assert(zOrder ~= nil and type(zOrder) == "number")
    self.buttonContainer:setZOrder(zOrder)
    self.label:setZOrder(zOrder + 1)
end

function LabelButton:setIsVisible(isVisible)
    assert(isVisible ~= nil and type(isVisible) == "boolean")
    self.buttonContainer:setIsVisible(isVisible)
    self.label:setIsVisible(isVisible)
end

return LabelButton
