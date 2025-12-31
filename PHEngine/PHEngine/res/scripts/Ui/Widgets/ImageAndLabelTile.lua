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
local UiLabel = require("Ui/Core/uiLabel")

ImageAndLabelTile = {}

function ImageAndLabelTile:new(host, overlay)
    assert(host ~= nil and type(host) == "userdata" and overlay ~= nil and type(overlay) == "table")

    local newObj = {
        host = host,
        overlayCanvasName = "",
        parentName = "",
        label = nil,
        backgroundTile = nil,
        image = nil,
        tileWidth = 0,
        tileHeight = 0,
        widgetName = "",
        containerColor = 0xffffff,
        anchorMargins = {left = 0, right = 0, bottom = 0, top = 0},
        luaProxiesReadyCallback = nil
    }

    newObj.backgroundTile = UiRectangle:new(host)
    newObj.label = UiLabel:new(host, "Lora-VariableFont_wght")
    newObj.image = UiImage:new(host)

    overlay:addWidget(newObj.backgroundTile)
    overlay:addWidget(newObj.label)
    overlay:addWidget(newObj.image)

    self.__index = self
    return setmetatable(newObj, self)
end

function ImageAndLabelTile:subscribeOnLuaProxiesReady(callback)
    print("subscribeOnLuaProxiesReady: " .. self.backgroundTile.luaProxyId)
    self.luaProxiesReadyCallback = callback
end

function ImageAndLabelTile:update(host) end

function ImageAndLabelTile:setWidth(width)
    assert(width ~= nil and type(width) == "number")

    self.tileWidth = width;
    self:resizeWidgets()
end

function ImageAndLabelTile:setHeight(height)
    assert(height ~= nil and type(height) == "number")

    self.tileHeight = height;
    self:resizeWidgets()
end

function ImageAndLabelTile:setAnchor(srcAnchor, dstAnchor, dstUiItemWidgetName, anchorMargin)
    assert(srcAnchor ~= nil and dstAnchor ~= nil and dstUiItemWidgetName ~= nil and srcAnchor >
               UiItemBase.UiAnchorType.NONE and srcAnchor <= UiItemBase.UiAnchorType.HORIZONTAL_CENTER)

    anchorMargin = nil == anchorMargin and 0 or anchorMargin
    if srcAnchor == UiItemBase.UiAnchorType.LEFT then
        self.anchorMargins.left = anchorMargin
    elseif srcAnchor == UiItemBase.UiAnchorType.RIGHT then
        self.anchorMargins.right = anchorMargin
    elseif srcAnchor == UiItemBase.UiAnchorType.TOP then
        self.anchorMargins.top = anchorMargin
    elseif srcAnchor == UiItemBase.UiAnchorType.BOTTOM then
        self.anchorMargins.bottom = anchorMargin
    end

    self.backgroundTile:setAnchor(srcAnchor, dstAnchor, dstUiItemWidgetName, anchorMargin)
    self:resizeWidgets()
end

function ImageAndLabelTile:addAnimation(host, onlyForTile, animationName, animationFunctionType, animationDuration,
                                        animatedPropertyName, animatedPropertyType, propertySrcValue, propertyDstValue)
    assert(host ~= nil and type(host) == "userdata")
    assert(onlyForTile ~= nil and type(onlyForTile) == "boolean")
    assert(animationName ~= nil and type(animationName) == "string" and animationFunctionType ~= nil and
               type(animationFunctionType) == "number" and animationDuration ~= nil and type(animationDuration) ==
               "number" and animatedPropertyName ~= nil and type(animatedPropertyName) == "string" and
               animatedPropertyType ~= nil and type(animatedPropertyType) == "number")
    assert(propertySrcValue ~= nil and propertyDstValue ~= nil and type(propertySrcValue) == type(propertyDstValue))

    self.backgroundTile:addAnimation(host, animationName, animationFunctionType, animationDuration,
                                     animatedPropertyName, animatedPropertyType, propertySrcValue, propertyDstValue)
    if onlyForTile ~= true then
        self.image:addAnimation(host, animationName, animationFunctionType, animationDuration, animatedPropertyName,
                                animatedPropertyType, propertySrcValue, propertyDstValue)
        self.label:addAnimation(host, animationName, animationFunctionType, animationDuration, animatedPropertyName,
                                animatedPropertyType, propertySrcValue, propertyDstValue)
    end
end

function ImageAndLabelTile:startAnimation(host, animationName)
    assert(host ~= nil and type(host) == "userdata")
    assert(animationName ~= nil and type(animationName) == "string")

    self.backgroundTile:startAnimation(host, animationName)
    self.image:startAnimation(host, animationName)
    self.label:startAnimation(host, animationName)
end

function ImageAndLabelTile:setIsVisible(isVisible)
    self.backgroundTile:setIsVisible(isVisible)
    self.image:setIsVisible(isVisible)
    self.label:setIsVisible(isVisible)
end

function ImageAndLabelTile:setLabelText(labelText)
    assert(labelText ~= nil and type(labelText) == "string")
    self.label:setText(labelText)
end

function ImageAndLabelTile:setLabelVisibility(isVisible)
    assert(isVisible ~= nil and type(isVisible) == "boolean")
    self.label:setIsVisible(isVisible)
end

function ImageAndLabelTile:rotateImage(rotateDegrees)
    assert(rotateDegrees ~= nil and type(rotateDegrees) == "number")
    self.image:setRotationDegrees(rotateDegrees)
end

function ImageAndLabelTile:setFlipImage(isFlipped) self.image:setIsFlipped(isFlipped) end

function ImageAndLabelTile:setTextureSource(texSource)
    assert(texSource ~= nil and type(texSource) == "string")
    self.image:setTextureSource(texSource)
end

function ImageAndLabelTile:setBackgroundTileColorHexValue(colorHex)
    self.containerColor = colorHex
    self.backgroundTile:setColorHexValue(colorHex)
end

function ImageAndLabelTile:setBackgroundTileOpacity(opacity) self.backgroundTile:setOpacity(opacity) end

function ImageAndLabelTile:setImageColorHexValue(colorHex) self.image:setColorHexValue(colorHex) end

function ImageAndLabelTile:setUseImageCustomColor(isCustom) self.image:setUseImageCustomColor(isCustom) end

function ImageAndLabelTile:resizeWidgets()
    local imageSize = self.tileWidth * 0.75

    self.backgroundTile:setHeight(self.tileHeight);
    self.backgroundTile:setWidth(self.tileWidth);
    self.backgroundTile:setAnchorMargin(UiItemBase.UiAnchorType.LEFT, self.anchorMargins.left);
    self.backgroundTile:setAnchorMargin(UiItemBase.UiAnchorType.RIGHT, self.anchorMargins.right);
    self.backgroundTile:setAnchorMargin(UiItemBase.UiAnchorType.TOP, self.anchorMargins.top);
    self.backgroundTile:setAnchorMargin(UiItemBase.UiAnchorType.BOTTOM, self.anchorMargins.bottom);
    self.image:setHeight(imageSize);
    self.image:setWidth(imageSize);
    self.label:setHeight(imageSize / 15)
end

function ImageAndLabelTile:setParent(host, overlayCanvasName, parentName)
    assert(
        host ~= nil and type(host) == "userdata" and type(overlayCanvasName) == "string" and overlayCanvasName ~= "" and
            type(parentName) == "string" and parentName ~= "", debug.traceback())

    self.overlayCanvasName = overlayCanvasName
    self.parentName = parentName
end

function ImageAndLabelTile:onPreCompoundWidgetInitialize()
    self.widgetName = self.backgroundTile.widgetName

    if self.luaProxiesReadyCallback ~= nil then self.luaProxiesReadyCallback(self.host) end
end

function ImageAndLabelTile:onCompoundWidgetInitialize()
    local imageSize = self.tileWidth * 0.75

    self.backgroundTile:setParent(self.host, self.overlayCanvasName, self.parentName)
    self.backgroundTile:setZOrder(3);
    self.backgroundTile:setHeight(self.tileHeight);
    self.backgroundTile:setWidth(self.tileWidth);

    self.backgroundTile:setColorHexValue(self.containerColor)
    self.backgroundTile:setBorderRadius(8)
    self.backgroundTile:enableMouseInputReceiverBase(self.host)
    self.backgroundTile:setIfCanInterceptMouseInputEvent(true)

    self.image:setParent(self.host, self.overlayCanvasName, self.backgroundTile.widgetName)
    self.image:setZOrder(4);
    self.image:setRotationDegrees(180)
    self.image:setHeight(imageSize);
    self.image:setWidth(imageSize);
    self.image:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER, UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
                         self.backgroundTile.widgetName);
    self.image:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
                         self.backgroundTile.widgetName);

    self.label:setParent(self.host, self.overlayCanvasName, self.backgroundTile.widgetName)
    self.label:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, self.backgroundTile.widgetName, 6)
    self.label:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER, UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
                         self.backgroundTile.widgetName)
    self.label:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM, self.backgroundTile.widgetName)
    self.label:setVerticalCenterOffset(0)
    self.label:setHeight(imageSize)
    self.label:setWidth(self.tileWidth)
    self.label:setTextColorHexValue(0xFFFFFF)
    self.label:setFontSize(14.0)
    self.label:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)
    self.label:setZOrder(5)
end

function ImageAndLabelTile:subscribeOnMouseInputClickedCallback(callback)
    assert(callback ~= nil and type(callback) == "function")
    self.backgroundTile:subscribeOnMouseInputClickedCallback(callback)
end

function ImageAndLabelTile:subscribeOnMouseInputCursorHoverStateChangedCallback(callback)
    assert(callback ~= nil and type(callback) == "function")
    self.backgroundTile:subscribeOnMouseInputCursorHoverStateChangedCallback(callback)
end

function ImageAndLabelTile:setIfCanInterceptMouseInputEvent(intercept)
    self.backgroundTile:setIfCanInterceptMouseInputEvent(intercept)
end

return ImageAndLabelTile
