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

function LabelButton:new(host, overlay, labelFontName, name)
    assert(host ~= nil and type(host) == "userdata" and overlay ~= nil and type(overlay) == "table")

    local newObj = {
        host = host,
        overlayCanvasName = "",
        parentName = "",
        backgroundTile = nil,
        label = nil,
        pressButtonStateContainer = nil,
        buttonWidth = 0,
        buttonHeight = 0,
        containerColor = 0xffffff,
        widgetName = "",
        luaProxiesReadyCallback = nil
    }

    local debugName = (name ~= nil and type(name) == "string" and name ~= "") and name or nil
    local containerName = debugName ~= nil and "LabelButton_" .. debugName or nil
    local labelName = debugName ~= nil and "LabelButton_" .. debugName or nil
    newObj.backgroundTile = UiRectangle:new(host, containerName)
    newObj.label = UiLabel:new(host, labelFontName, labelName)
    newObj.pressButtonStateContainer = UiRectangle:new(host)

    overlay:addWidget(newObj.backgroundTile)
    overlay:addWidget(newObj.label)
    overlay:addWidget(newObj.pressButtonStateContainer)

    self.__index = self
    return setmetatable(newObj, self)
end

function LabelButton:subscribeOnLuaProxiesReady(callback)
    self.luaProxiesReadyCallback = callback
end

function LabelButton:update(host)
end

function LabelButton:setParent(host, overlayCanvasName, parentName)
    assert(
        host ~= nil and type(host) == "userdata" and type(overlayCanvasName) == "string" and overlayCanvasName ~= "" and
        type(parentName) == "string" and parentName ~= "", debug.traceback())

    self.overlayCanvasName = overlayCanvasName
    self.parentName = parentName
end

function LabelButton:onPreCompoundWidgetInitialize()
    self.widgetName = self.backgroundTile.widgetName

    if self.luaProxiesReadyCallback ~= nil then
        self.luaProxiesReadyCallback(self.host)
    end
end

function LabelButton:onCompoundWidgetInitialize()
    self.backgroundTile:setParent(self.host, self.overlayCanvasName, self.parentName)
    self.backgroundTile:setZOrder(3);
    self.backgroundTile:setHeight(self.buttonHeight);
    self.backgroundTile:setWidth(self.buttonWidth);
    self.backgroundTile:setColorHexValue(self.containerColor)
    self.backgroundTile:enableMouseInputReceiverBase(self.host)

    self.pressButtonStateContainer:setParent(self.host, self.overlayCanvasName, self.backgroundTile.widgetName)
    self.pressButtonStateContainer:setZOrder(4);
    self.pressButtonStateContainer:fill(self.backgroundTile.widgetName)
    self.pressButtonStateContainer:setOpacity(0.0);
    self.pressButtonStateContainer:enableMouseInputReceiverBase(self.host)
    self.pressButtonStateContainer:addSequenceAnimation(self.host, "ButtonClick",
        {
            {
                animationFunctionType = UiBaseWidget.AnimationInterpolationFunctionType.LINEAR,
                animationDuration = 0.1,
                animatedPropertyName = "Scale",
                animatedPropertyType = UiBaseWidget.EnginePropertyType.Float,
                propertySrcValue = 0.0,
                propertyDstValue = 1.0
            },
            {
                animationFunctionType = UiBaseWidget.AnimationInterpolationFunctionType.LINEAR,
                animationDuration = 0.1,
                animatedPropertyName = "Scale",
                animatedPropertyType = UiBaseWidget.EnginePropertyType.Float,
                propertySrcValue = 1.0,
                propertyDstValue = 0.0
            },
        });
    self.pressButtonStateContainer:addSequenceAnimation(self.host, "ButtonClick",
        {
            {
                animationFunctionType = UiBaseWidget.AnimationInterpolationFunctionType.LINEAR,
                animationDuration = 0.1,
                animatedPropertyName = "Opacity",
                animatedPropertyType = UiBaseWidget.EnginePropertyType.Float,
                propertySrcValue = 0.0,
                propertyDstValue = 1.0
            },
            {
                animationFunctionType = UiBaseWidget.AnimationInterpolationFunctionType.LINEAR,
                animationDuration = 0.1,
                animatedPropertyName = "Opacity",
                animatedPropertyType = UiBaseWidget.EnginePropertyType.Float,
                propertySrcValue = 1.0,
                propertyDstValue = 0.0
            },
        });
    self.pressButtonStateContainer:subscribeOnMouseInputClickedCallback(function()
        self.pressButtonStateContainer:startSequenceAnimation(self.host, "ButtonClick")
    end)

    self.label:setParent(self.host, self.overlayCanvasName, self.backgroundTile.widgetName)
    self.label:setZOrder(4);
    self.label:fill(self.backgroundTile.widgetName)
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

    self.backgroundTile:setAnchor(srcAnchor, dstAnchor, dstUiItemWidgetName, anchorMargin)
    self:resizeWidgets()
end

function LabelButton:subscribeOnMouseInputClickedCallback(callback)
    assert(callback ~= nil and type(callback) == "function")
    self.backgroundTile:subscribeOnMouseInputClickedCallback(callback)
end

function LabelButton:subscribeOnMouseInputCursorHoverStateChangedCallback(callback)
    assert(callback ~= nil and type(callback) == "function")
    self.backgroundTile:subscribeOnMouseInputCursorHoverStateChangedCallback(callback)
end

function LabelButton:addAnimation(host, animationName, animationFunctionType, animationDuration, animatedPropertyName,
                                  animatedPropertyType, propertySrcValue, propertyDstValue)
    assert(host ~= nil and type(host) == "userdata")
    assert(animationName ~= nil and type(animationName) == "string" and animationFunctionType ~= nil and
        type(animationFunctionType) == "number" and animationDuration ~= nil and
        type(animationDuration) == "number" and animatedPropertyName ~= nil and type(animatedPropertyName) == "string" and
        animatedPropertyType ~= nil and type(animatedPropertyType) == "number")
    assert(propertySrcValue ~= nil and propertyDstValue ~= nil and type(propertySrcValue) == type(propertyDstValue))

    self.backgroundTile:addAnimation(host, animationName, animationFunctionType, animationDuration,
        animatedPropertyName,
        animatedPropertyType, propertySrcValue, propertyDstValue)
    self.label:addAnimation(host, animationName, animationFunctionType, animationDuration,
        animatedPropertyName,
        animatedPropertyType, propertySrcValue, propertyDstValue)
end

function LabelButton:startAnimation(host, animationName)
    assert(host ~= nil and type(host) == "userdata")
    assert(animationName ~= nil and type(animationName) == "string")

    self.backgroundTile:startAnimation(host, animationName)
    self.label:startAnimation(host, animationName)
end

function LabelButton:setLabelText(labelText)
    assert(labelText ~= nil and type(labelText) == "string")
    self.label:setText(labelText)
end

function LabelButton:setButtonBorderRadius(radius)
    assert(radius ~= nil and type(radius) == "number")
    self.backgroundTile:setBorderRadius(radius)
end

function LabelButton:setLabelOpacity(opacity)
    assert(opacity ~= nil and type(opacity) == "number")
    self.label:setOpacity(opacity)
end

function LabelButton:resizeWidgets()
    local labelHeight = self.buttonHeight * 0.75

    self.backgroundTile:setHeight(self.buttonHeight);
    self.backgroundTile:setWidth(self.buttonWidth);
    self.label:setHeight(labelHeight);
end

function LabelButton:setButtonColorHexValue(colorHex)
    assert(colorHex ~= nil and type(colorHex) == "number")

    self.containerColor = colorHex
    self.backgroundTile:setColorHexValue(colorHex)
end

function LabelButton:setLabelTextColorHexValue(colorHex)
    assert(colorHex ~= nil and type(colorHex) == "number")

    self.label:setTextColorHexValue(colorHex)
end

function LabelButton:setZOrder(zOrder)
    assert(zOrder ~= nil and type(zOrder) == "number")
    self.backgroundTile:setZOrder(zOrder)
    self.label:setZOrder(zOrder + 1)
end

function LabelButton:setIsVisible(isVisible)
    assert(isVisible ~= nil and type(isVisible) == "boolean")
    self.backgroundTile:setIsVisible(isVisible)
    self.label:setIsVisible(isVisible)
end

function LabelButton:setLabelFontSize(fontSize)
    self.label:setFontSize(fontSize)
end

function LabelButton:setPressStateButtonColorHexValues(colorHex)
    self.pressButtonStateContainer:setColorHexValue(colorHex)
end

return LabelButton
