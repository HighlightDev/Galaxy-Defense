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

local UiRectangle = require("Ui/Core/uiRectangle")
local UiImage = require("Ui/Core/uiImage")

ImageButton = {
}

function ImageButton:new(host, overlay, name)
    assert(host ~= nil and type(host) == "userdata" and overlay ~= nil and type(overlay) == "table")

    local newObj = {
        host = host,
        overlayCanvasName = "",
        parentName = "",
        buttonContainer = nil,
        image = nil,
        pressButtonStateContainer = nil,
        buttonWidth = 0,
        buttonHeight = 0,
        containerColor = 0xffffff,
        widgetName = "",
    }

    local debugName = (name ~= nil and type(name) == "string" and name ~= "") and name or nil
    local containerName = debugName ~= nil and "ImageButton_" .. debugName or nil
    local imageName = debugName ~= nil and "ImageButton_" .. debugName or nil
    newObj.buttonContainer = UiRectangle:new(host, containerName)
    newObj.image = UiImage:new(host, imageName)
    newObj.pressButtonStateContainer = UiRectangle:new(host)

    overlay:addWidget(newObj.buttonContainer)
    overlay:addWidget(newObj.image)
    overlay:addWidget(newObj.pressButtonStateContainer)

    self.__index = self
    return setmetatable(newObj, self)
end

function ImageButton:setParent(host, overlayCanvasName, parentName)
    assert(
        host ~= nil and type(host) == "userdata" and type(overlayCanvasName) == "string" and overlayCanvasName ~= "" and
        type(parentName) == "string" and parentName ~= "", debug.traceback())

    self.overlayCanvasName = overlayCanvasName
    self.parentName = parentName
end

function ImageButton:onPreCompoundWidgetInitialize()
    self.widgetName = self.buttonContainer.widgetName
end

function ImageButton:onCompoundWidgetInitialize()
    local imageSize = math.min(self.buttonWidth * 0.75, self.buttonHeight)

    self.buttonContainer:setParent(self.host, self.overlayCanvasName, self.parentName)
    self.buttonContainer:setZOrder(3);
    self.buttonContainer:setHeight(self.buttonHeight);
    self.buttonContainer:setWidth(self.buttonWidth);
    self.buttonContainer:setColorHexValue(self.containerColor)
    self.buttonContainer:enableMouseInputReceiverBase(self.host)

    self.pressButtonStateContainer:setParent(self.host, self.overlayCanvasName, self.buttonContainer.widgetName)
    self.pressButtonStateContainer:setZOrder(4);
    self.pressButtonStateContainer:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
        UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
        self.buttonContainer.widgetName);
    self.pressButtonStateContainer:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER,
        UiItemBase.UiAnchorType.VERTICAL_CENTER,
        self.buttonContainer.widgetName);
    self.pressButtonStateContainer:setOpacity(0.0);
    self.pressButtonStateContainer:setHeight(self.buttonHeight);
    self.pressButtonStateContainer:setWidth(self.buttonWidth);
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
    self.pressButtonStateContainer:subscriveOnMouseInputClickedCallback(function()
        self.pressButtonStateContainer:startSequenceAnimation(self.host, "ButtonClick")
    end)

    self.image:setParent(self.host, self.overlayCanvasName, self.buttonContainer.widgetName)
    self.image:setZOrder(4);
    self.image:setHeight(imageSize);
    self.image:setWidth(imageSize);
    self.image:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER, UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
        self.buttonContainer.widgetName);
    self.image:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
        self.buttonContainer.widgetName);
end

function ImageButton:setWidth(width)
    assert(width ~= nil and type(width) == "number")

    self.buttonWidth = width;
    self:resizeWidgets()
end

function ImageButton:setHeight(height)
    assert(height ~= nil and type(height) == "number")

    self.buttonHeight = height;
    self:resizeWidgets()
end

function ImageButton:setAnchor(srcAnchor, dstAnchor, dstUiItemWidgetName, anchorMargin)
    assert(srcAnchor ~= nil and dstAnchor ~= nil and dstUiItemWidgetName ~= nil and
        srcAnchor > UiItemBase.UiAnchorType.NONE and srcAnchor <= UiItemBase.UiAnchorType.HORIZONTAL_CENTER)

    self.buttonContainer:setAnchor(srcAnchor, dstAnchor, dstUiItemWidgetName, anchorMargin)
    self:resizeWidgets()
end

function ImageButton:subscriveOnMouseInputClickedCallback(callback)
    assert(callback ~= nil and type(callback) == "function")
    self.buttonContainer:subscriveOnMouseInputClickedCallback(callback)
end

function ImageButton:addAnimation(host, animationName, animationFunctionType, animationDuration, animatedPropertyName,
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
    self.image:addAnimation(host, animationName, animationFunctionType, animationDuration,
        animatedPropertyName,
        animatedPropertyType, propertySrcValue, propertyDstValue)
end

function ImageButton:startAnimation(host, animationName)
    assert(host ~= nil and type(host) == "userdata")
    assert(animationName ~= nil and type(animationName) == "string")

    self.buttonContainer:startAnimation(host, animationName)
    self.image:startAnimation(host, animationName)
end

function ImageButton:setImageTextureSource(textureSource)
    assert(textureSource ~= nil and type(textureSource) == "string")
    self.image:setTextureSource(textureSource)
end

function ImageButton:setButtonBorderRadius(radius)
    assert(radius ~= nil and type(radius) == "number")
    self.buttonContainer:setBorderRadius(radius)
end

function ImageButton:setImageRotationDegrees(angleDegrees)
    assert(angleDegrees ~= nil and type(angleDegrees) == "number")
    self.image:setRotationDegrees(angleDegrees)
end

function ImageButton:getImageRotationDegrees()
    return self.image:getRotationDegrees()
end

function ImageButton:resizeWidgets()
    local imageSize = math.min(self.buttonWidth * 0.75, self.buttonHeight)

    self.buttonContainer:setHeight(self.buttonHeight);
    self.buttonContainer:setWidth(self.buttonWidth);
    self.image:setHeight(imageSize);
    self.image:setWidth(imageSize);
end

function ImageButton:setButtonColorHexValue(colorHex)
    assert(colorHex ~= nil and type(colorHex) == "number")

    self.containerColor = colorHex
    self.buttonContainer:setColorHexValue(colorHex)
end

function ImageButton:setUseImageCustomColor(isUsed)
    assert(isUsed ~= nil and type(isUsed) == "boolean")

    self.image:setUseImageCustomColor(isUsed)
end

function ImageButton:setImageColorHexValue(colorHex)
    assert(colorHex ~= nil and type(colorHex) == "number")

    self.image:setColorHexValue(colorHex)
end

function ImageButton:setZOrder(zOrder)
    assert(zOrder ~= nil and type(zOrder) == "number")
    self.buttonContainer:setZOrder(zOrder)
    self.image:setZOrder(zOrder + 1)
end

function ImageButton:setIsVisible(isVisible)
    assert(isVisible ~= nil and type(isVisible) == "boolean")
    self.buttonContainer:setIsVisible(isVisible)
    self.image:setIsVisible(isVisible)
end

return ImageButton
