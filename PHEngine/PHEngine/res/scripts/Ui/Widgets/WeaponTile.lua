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

WeaponTile = {
    weaponBackgroundTileColor = 0xdb9427,
    missilesCountLabelColor = 0xFFFFFF
}

function WeaponTile:new(host, overlay)
    assert(host ~= nil and type(host) == "userdata" and overlay ~= nil and
               type(overlay) == "table")

    local newObj = {
        host = host,
        overlayCanvasName = "",
        parentName = "",
        weaponLabel = nil,
        backgroundTile = nil,
        weaponImage = nil,
        tileWidth = 0,
        tileHeight = 0,
        widgetName = "",
        anchorMargins = {left = 0, right = 0, bottom = 0, top = 0},
        luaProxiesReadyCallback = nil
    }

    newObj.backgroundTile = UiRectangle:new(host)
    newObj.weaponLabel = UiLabel:new(host, "Lora-VariableFont_wght")
    newObj.weaponImage = UiImage:new(host)

    overlay:addWidget(newObj.backgroundTile)
    overlay:addWidget(newObj.weaponLabel)
    overlay:addWidget(newObj.weaponImage)

    self.__index = self
    return setmetatable(newObj, self)
end

function WeaponTile:subscribeOnLuaProxiesReady(callback)
    self.luaProxiesReadyCallback = callback
end

function WeaponTile:update(host) end

function WeaponTile:setWidth(width)
    assert(width ~= nil and type(width) == "number")

    self.tileWidth = width;
    self:resizeWidgets()
end

function WeaponTile:setHeight(height)
    assert(height ~= nil and type(height) == "number")

    self.tileHeight = height;
    self:resizeWidgets()
end

function WeaponTile:setAnchor(srcAnchor, dstAnchor, dstUiItemWidgetName,
                              anchorMargin)
    assert(
        srcAnchor ~= nil and dstAnchor ~= nil and dstUiItemWidgetName ~= nil and
            srcAnchor > UiItemBase.UiAnchorType.NONE and srcAnchor <=
            UiItemBase.UiAnchorType.HORIZONTAL_CENTER)

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

    self.backgroundTile:setAnchor(srcAnchor, dstAnchor, dstUiItemWidgetName,
                                  anchorMargin)
    self:resizeWidgets()
end

function WeaponTile:addAnimation(host, onlyForTile, animationName,
                                 animationFunctionType, animationDuration,
                                 animatedPropertyName, animatedPropertyType,
                                 propertySrcValue, propertyDstValue)
    assert(host ~= nil and type(host) == "userdata")
    assert(onlyForTile ~= nil and type(onlyForTile) == "boolean")
    assert(animationName ~= nil and type(animationName) == "string" and
               animationFunctionType ~= nil and type(animationFunctionType) ==
               "number" and animationDuration ~= nil and type(animationDuration) ==
               "number" and animatedPropertyName ~= nil and
               type(animatedPropertyName) == "string" and animatedPropertyType ~=
               nil and type(animatedPropertyType) == "number")
    assert(propertySrcValue ~= nil and propertyDstValue ~= nil and
               type(propertySrcValue) == type(propertyDstValue))

    self.backgroundTile:addAnimation(host, animationName, animationFunctionType,
                                     animationDuration, animatedPropertyName,
                                     animatedPropertyType, propertySrcValue,
                                     propertyDstValue)
    if onlyForTile ~= true then
        self.weaponImage:addAnimation(host, animationName,
                                      animationFunctionType, animationDuration,
                                      animatedPropertyName,
                                      animatedPropertyType, propertySrcValue,
                                      propertyDstValue)
        self.weaponLabel:addAnimation(host, animationName,
                                      animationFunctionType, animationDuration,
                                      animatedPropertyName,
                                      animatedPropertyType, propertySrcValue,
                                      propertyDstValue)
    end
end

function WeaponTile:startAnimation(host, animationName)
    assert(host ~= nil and type(host) == "userdata")
    assert(animationName ~= nil and type(animationName) == "string")

    self.backgroundTile:startAnimation(host, animationName)
    self.weaponImage:startAnimation(host, animationName)
    self.weaponLabel:startAnimation(host, animationName)
end

function WeaponTile:setLabelText(labelText)
    assert(labelText ~= nil and type(labelText) == "string")
    self.weaponLabel:setText(labelText)
end

function WeaponTile:resizeWidgets()
    local weaponImageSize = self.tileWidth * 0.75

    self.backgroundTile:setHeight(self.tileHeight);
    self.backgroundTile:setWidth(self.tileWidth);
    self.backgroundTile:setAnchorMargin(UiItemBase.UiAnchorType.LEFT,
                                        self.anchorMargins.left);
    self.backgroundTile:setAnchorMargin(UiItemBase.UiAnchorType.RIGHT,
                                        self.anchorMargins.right);
    self.backgroundTile:setAnchorMargin(UiItemBase.UiAnchorType.TOP,
                                        self.anchorMargins.top);
    self.backgroundTile:setAnchorMargin(UiItemBase.UiAnchorType.BOTTOM,
                                        self.anchorMargins.bottom);
    self.weaponImage:setHeight(weaponImageSize);
    self.weaponImage:setWidth(weaponImageSize);
    self.weaponLabel:setHeight(weaponImageSize / 15)
end

function WeaponTile:setParent(host, overlayCanvasName, parentName)
    assert(
        host ~= nil and type(host) == "userdata" and type(overlayCanvasName) ==
            "string" and overlayCanvasName ~= "" and type(parentName) ==
            "string" and parentName ~= "", debug.traceback())

    self.overlayCanvasName = overlayCanvasName
    self.parentName = parentName
end

function WeaponTile:onPreCompoundWidgetInitialize()
    self.widgetName = self.backgroundTile.widgetName

    if self.luaProxiesReadyCallback ~= nil then
        self.luaProxiesReadyCallback(self.host)
    end
end

function WeaponTile:onCompoundWidgetInitialize()
    local weaponImageSize = self.tileWidth * 0.75

    self.backgroundTile:setParent(self.host, self.overlayCanvasName,
                                  self.parentName)
    self.backgroundTile:setZOrder(3);
    self.backgroundTile:setHeight(self.tileHeight);
    self.backgroundTile:setWidth(self.tileWidth);

    self.backgroundTile:setColorHexValue(WeaponTile.weaponBackgroundTileColor)
    self.backgroundTile:setBorderRadius(8)
    self.backgroundTile:enableMouseInputReceiverBase(self.host)

    self.weaponImage:setParent(self.host, self.overlayCanvasName,
                               self.backgroundTile.widgetName)
    self.weaponImage:setTextureSource("weapon_missile.png");
    self.weaponImage:setZOrder(4);
    self.weaponImage:setRotationDegrees(180)
    self.weaponImage:setHeight(weaponImageSize);
    self.weaponImage:setWidth(weaponImageSize);
    self.weaponImage:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
                               UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
                               self.backgroundTile.widgetName);
    self.weaponImage:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER,
                               UiItemBase.UiAnchorType.VERTICAL_CENTER,
                               self.backgroundTile.widgetName);

    self.weaponLabel:setParent(self.host, self.overlayCanvasName,
                               self.backgroundTile.widgetName)
    self.weaponLabel:setAnchor(UiItemBase.UiAnchorType.LEFT,
                               UiItemBase.UiAnchorType.LEFT,
                               self.backgroundTile.widgetName, 6)
    self.weaponLabel:setAnchor(UiItemBase.UiAnchorType.RIGHT,
                               UiItemBase.UiAnchorType.RIGHT,
                               self.backgroundTile.widgetName, 0)
    self.weaponLabel:setAnchor(UiItemBase.UiAnchorType.BOTTOM,
                               UiItemBase.UiAnchorType.BOTTOM,
                               self.backgroundTile.widgetName, 6)
    self.weaponLabel:setVerticalCenterOffset(-8)
    self.weaponLabel:setHeight(weaponImageSize / 15)
    self.weaponLabel:setTextColorHexValue(WeaponTile.missilesCountLabelColor)
    self.weaponLabel:setFontSize(10.0)
    self.weaponLabel:setTextHorizontalAlignment(
        UiLabel.TextHorizontalAlignmentType.LEFT)
    self.weaponLabel:setZOrder(4)
end

function WeaponTile:subscribeOnMouseInputClickedCallback(callback)
    assert(callback ~= nil and type(callback) == "function")
    self.backgroundTile:subscribeOnMouseInputClickedCallback(callback)
end

return WeaponTile
