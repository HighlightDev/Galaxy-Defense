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

local UiRectangle = require("Ui/Core/uiRectangle")
local UiImage = require("Ui/Core/uiImage")
local Styles = require("Ui/Common/styles")

ImageButton = {}

function ImageButton:new(host, overlay, name)
    assert(host ~= nil and type(host) == "userdata" and overlay ~= nil and type(overlay) == "table")

    local newObj = {
        host = host,
        overlayCanvasName = "",
        parentName = "",
        backgroundTile = nil,
        image = nil,
        pressButtonStateContainer = nil,
        buttonWidth = 0,
        buttonHeight = 0,
        containerColor = 0xffffff,
        widgetName = "",
        luaProxiesReadyCallback = nil,
        buttonActiveState = {
            isButtonActive = true,
            prevContainerColorHexValue = 0xffffff
        }
    }

    local debugName = (name ~= nil and type(name) == "string" and name ~= "") and name or nil
    local containerName = debugName ~= nil and "ImageButtonContainer_" .. debugName or nil
    local imageName = debugName ~= nil and "ImageButtonImage_" .. debugName or nil
    newObj.backgroundTile = UiRectangle:new(host, containerName)
    newObj.image = UiImage:new(host, imageName)
    newObj.pressButtonStateContainer = UiRectangle:new(host)

    overlay:addWidget(newObj.backgroundTile)
    overlay:addWidget(newObj.image)
    overlay:addWidget(newObj.pressButtonStateContainer)

    self.__index = self
    return setmetatable(newObj, self)
end

function ImageButton:subscribeOnLuaProxiesReady(callback)
    self.luaProxiesReadyCallback = callback
end

function ImageButton:update(host)
end

function ImageButton:setParent(host, overlayCanvasName, parentName)
    assert(
        host ~= nil and type(host) == "userdata" and type(overlayCanvasName) == "string" and overlayCanvasName ~= "" and
            type(parentName) == "string" and parentName ~= "", debug.traceback())

    self.overlayCanvasName = overlayCanvasName
    self.parentName = parentName
end

function ImageButton:onPreCompoundWidgetInitialize()
    self.widgetName = self.backgroundTile.widgetName

    if self.luaProxiesReadyCallback ~= nil then
        self.luaProxiesReadyCallback(self.host)
    end
end

function ImageButton:onCompoundWidgetInitialize()
    local imageSize = math.min(self.buttonWidth * 0.75, self.buttonHeight)

    self.backgroundTile:setParent(self.host, self.overlayCanvasName, self.parentName)
    self.backgroundTile:setZOrder(3);
    self.backgroundTile:setHeight(self.buttonHeight);
    self.backgroundTile:setWidth(self.buttonWidth);
    self.backgroundTile:setColorHexValue(self.containerColor)
    self.backgroundTile:enableMouseInputReceiverBase(self.host)

    self.pressButtonStateContainer:setParent(self.host, self.overlayCanvasName, self.backgroundTile.widgetName)
    self.pressButtonStateContainer:setZOrder(4);
    self.pressButtonStateContainer:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
        UiItemBase.UiAnchorType.HORIZONTAL_CENTER, self.backgroundTile.widgetName);
    self.pressButtonStateContainer:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER,
        UiItemBase.UiAnchorType.VERTICAL_CENTER, self.backgroundTile.widgetName);
    self.pressButtonStateContainer:setOpacity(0.0);
    self.pressButtonStateContainer:setHeight(self.buttonHeight);
    self.pressButtonStateContainer:setWidth(self.buttonWidth);
    self.pressButtonStateContainer:setBorderRadius(10)
    self.pressButtonStateContainer:enableMouseInputReceiverBase(self.host)
    self.pressButtonStateContainer:addSequenceAnimation(self.host, "ButtonClick", {{
        animationFunctionType = UiBaseWidget.AnimationInterpolationFunctionType.LINEAR,
        animationDuration = 0.1,
        animatedPropertyName = "Scale",
        animatedPropertyType = UiBaseWidget.EnginePropertyType.Vec2,
        propertySrcValue = {
            x = 0.0,
            y = 0.0
        },
        propertyDstValue = {
            x = 1.0,
            y = 1.0
        }
    }, {
        animationFunctionType = UiBaseWidget.AnimationInterpolationFunctionType.LINEAR,
        animationDuration = 0.1,
        animatedPropertyName = "Scale",
        animatedPropertyType = UiBaseWidget.EnginePropertyType.Vec2,
        propertySrcValue = {
            x = 1.0,
            y = 1.0
        },
        propertyDstValue = {
            x = 0.0,
            y = 0.0
        }
    }});
    self.pressButtonStateContainer:addSequenceAnimation(self.host, "ButtonClick", {{
        animationFunctionType = UiBaseWidget.AnimationInterpolationFunctionType.LINEAR,
        animationDuration = 0.1,
        animatedPropertyName = "Opacity",
        animatedPropertyType = UiBaseWidget.EnginePropertyType.Float,
        propertySrcValue = 0.0,
        propertyDstValue = 1.0
    }, {
        animationFunctionType = UiBaseWidget.AnimationInterpolationFunctionType.LINEAR,
        animationDuration = 0.1,
        animatedPropertyName = "Opacity",
        animatedPropertyType = UiBaseWidget.EnginePropertyType.Float,
        propertySrcValue = 1.0,
        propertyDstValue = 0.0
    }});
    self.pressButtonStateContainer:subscribeOnMouseInputClickedCallback(function()
        self.pressButtonStateContainer:startSequenceAnimation(self.host, "ButtonClick")
    end)

    self.image:setParent(self.host, self.overlayCanvasName, self.backgroundTile.widgetName)
    self.image:setZOrder(4);
    self.image:setHeight(imageSize);
    self.image:setWidth(imageSize);
    self.image:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER, UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
        self.backgroundTile.widgetName);
    self.image:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
        self.backgroundTile.widgetName);
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
    assert(srcAnchor ~= nil and dstAnchor ~= nil and dstUiItemWidgetName ~= nil and srcAnchor >
               UiItemBase.UiAnchorType.NONE and srcAnchor <= UiItemBase.UiAnchorType.HORIZONTAL_CENTER)

    self.backgroundTile:setAnchor(srcAnchor, dstAnchor, dstUiItemWidgetName, anchorMargin)
    self:resizeWidgets()
end

function ImageButton:subscribeOnMouseInputClickedCallback(callback)
    assert(callback ~= nil and type(callback) == "function")
    self.backgroundTile:subscribeOnMouseInputClickedCallback(callback)
end

function ImageButton:subscribeOnMouseInputCursorHoverStateChangedCallback(callback)
    assert(callback ~= nil and type(callback) == "function")
    self.backgroundTile:subscribeOnMouseInputCursorHoverStateChangedCallback(callback)
end

function ImageButton:addAnimation(host, animationName, animationFunctionType, animationDuration, animatedPropertyName,
    animatedPropertyType, propertySrcValue, propertyDstValue)
    assert(host ~= nil and type(host) == "userdata")
    assert(animationName ~= nil and type(animationName) == "string" and animationFunctionType ~= nil and
               type(animationFunctionType) == "number" and animationDuration ~= nil and type(animationDuration) ==
               "number" and animatedPropertyName ~= nil and type(animatedPropertyName) == "string" and
               animatedPropertyType ~= nil and type(animatedPropertyType) == "number")
    assert(propertySrcValue ~= nil and propertyDstValue ~= nil and type(propertySrcValue) == type(propertyDstValue))

    self.backgroundTile:addAnimation(host, animationName, animationFunctionType, animationDuration,
        animatedPropertyName, animatedPropertyType, propertySrcValue, propertyDstValue)
    self.image:addAnimation(host, animationName, animationFunctionType, animationDuration, animatedPropertyName,
        animatedPropertyType, propertySrcValue, propertyDstValue)
end

function ImageButton:startAnimation(host, animationName)
    assert(host ~= nil and type(host) == "userdata")
    assert(animationName ~= nil and type(animationName) == "string")

    self.backgroundTile:startAnimation(host, animationName)
    self.image:startAnimation(host, animationName)
end

function ImageButton:setImageTextureSource(textureSource)
    assert(textureSource ~= nil and type(textureSource) == "string")
    self.image:setTextureSource(textureSource)
end

function ImageButton:setButtonBorderRadius(radius)
    assert(radius ~= nil and type(radius) == "number")
    self.backgroundTile:setBorderRadius(radius)
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

    self.backgroundTile:setHeight(self.buttonHeight);
    self.backgroundTile:setWidth(self.buttonWidth);
    self.image:setHeight(imageSize);
    self.image:setWidth(imageSize);
end

function ImageButton:setButtonColorHexValue(colorHex)
    assert(colorHex ~= nil and type(colorHex) == "number")

    self.containerColor = colorHex
    self.backgroundTile:setColorHexValue(colorHex)
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
    self.backgroundTile:setZOrder(zOrder)
    self.image:setZOrder(zOrder + 1)
end

function ImageButton:setIsVisible(isVisible)
    assert(isVisible ~= nil and type(isVisible) == "boolean")
    self.backgroundTile:setIsVisible(isVisible)
    self.image:setIsVisible(isVisible)
end

function ImageButton:setIsButtonActive(isButtonActive)
    assert(isButtonActive ~= nil and type(isButtonActive) == "boolean")
    if self.buttonActiveState.isButtonActive ~= isButtonActive then
        self.buttonActiveState.isButtonActive = isButtonActive
        if isButtonActive then
            self.backgroundTile:setIsUiInputEnabled(true)
            self.pressButtonStateContainer:setIsUiInputEnabled(true)
            self:setButtonColorHexValue(self.buttonActiveState.prevContainerColorHexValue)
            self.buttonActiveState.prevContainerColorHexValue = 0xffffff
        else
            self.backgroundTile:setIsUiInputEnabled(false)
            self.pressButtonStateContainer:setIsUiInputEnabled(false)
            self.buttonActiveState.prevContainerColorHexValue = self.containerColor
            self:setButtonColorHexValue(Styles.Colors.notActiveButtonColor)
        end
    end
end

return ImageButton
