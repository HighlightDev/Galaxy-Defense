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
        package.path = package.path .. ";" .. unixLikePath .. "?.lua"
    end
end

setup()
--
--[[ END   *** this snippet has to be inserted everywhere where your want to require custom modules  ***  END]]
local json = require("core/3rdparty/json")
local UiCanvas = require("core/uiCanvas")
local UiOverlay = require("core/uiOverlay")
local UiItem = require("core/uiItem")
local UiRectangle = require("core/uiRectangle")
local UiImage = require("core/uiImage")
local UiLabel = require("core/uiLabel")

PlayerStatusType = {
    NONE = 0,
    LIFE_POINTS_CHANGED = 1,
    ACTIVE_WEAPON_CHANGED = 2,
    MISSILES_COUNT_CHANGED = 3,
    AVAILABLE_MISSILES_CHANGED = 4
}

MissileType = {
    NONE = 0,
    BOMB = 1,
    FREEZING = 2,
    ELECTRO_RAY = 3,
    BLACK_HOLE = 4
}

MissileWidgetsMap = {
    BOMB = {
        image = nil,
        tile = nil,
        label = nil
    },
    FREEZING = {
        image = nil,
        tile = nil,
        label = nil
    },
    BLACK_HOLE = {
        image = nil,
        tile = nil,
        label = nil
    },
    ELECTRO_RAY = {
        image = nil,
        tile = nil,
        label = nil
    }
}

PlayerHUDOverlay = {
    testAvailableHearts = 5,
    prevSelectedMissileType = MissileType.NONE,
    weaponBackgroundTileColor = 0xdb9427,
    missilesCountLabelColor = 0xFFFFFF,
}

local function invertTable(table)
    local s = {}
    for k, v in pairs(table) do
        s[v] = k
    end
    return s
end

InvertedMissileTable = invertTable(MissileType)

local function getSelectedMissileType(host)
    return tonumber(_GetSelectedMissileType(host))
end

local function getMissileTypeNameByValue(missileTypeValue)
    assert(missileTypeValue ~= nil and type(missileTypeValue) == "number")
    local name = InvertedMissileTable[missileTypeValue]
    assert(name ~= nil)
    return name
end

local function startAnimationForMissileWidget(host, missileType, animationName)
    assert(host ~= nil and type(host) == "userdata" and missileType ~= nil and animationName ~= nil and
        type(animationName) == "string")
    local currentMissileName = InvertedMissileTable[missileType]
    if currentMissileName ~= nil then
        if MissileWidgetsMap[tostring(currentMissileName)] ~= nil then
            local activeWidgetsTable = MissileWidgetsMap[tostring(currentMissileName)]
            activeWidgetsTable.image:startAnimation(host, animationName)
            activeWidgetsTable.tile:startAnimation(host, animationName)
            activeWidgetsTable.label:startAnimation(host, animationName)
        end
    end
end

function PlayerHUDOverlay:new(host)
    local windowWidth = _GetWindowWidth(host)
    local windowHeight = _GetWindowHeight(host)

    local playerHUDOverlayCanvas = UiCanvas:new(host, 0, 0, windowWidth, windowHeight)
    local playerHUDOverlay = UiOverlay:createBackgroundOverlay(host, "PlayerHUDOverlay", playerHUDOverlayCanvas)

    local lifeRootContainerWidth = windowWidth / 3.0;
    local lifeRootContainerHeight = windowHeight / 4.0;
    local weaponRootContainerWidth = windowWidth / 3.0;
    local heartWidth = lifeRootContainerWidth / 10.0
    local heartInterval = heartWidth * 0.5
    local weaponCount = 4.0
    local weaponWidth = weaponRootContainerWidth / weaponCount
    local weaponRootContainerHeight = weaponWidth + 10
    local weaponInterval = weaponWidth / 8.0
    weaponWidth = (weaponRootContainerWidth - (weaponInterval * (weaponCount + 1))) / weaponCount
    local weaponTopBottomMargin = (weaponRootContainerHeight - weaponWidth) * 0.5
    local weaponImageSize = weaponWidth * 0.75

    local lifeRootContainer = UiItem:new(host)
    playerHUDOverlay:addWidget(lifeRootContainer)

    local weaponRootContainer = UiItem:new(host)
    playerHUDOverlay:addWidget(weaponRootContainer)

    local weaponBackgroundTile1 = UiRectangle:new(host)
    playerHUDOverlay:addWidget(weaponBackgroundTile1)

    local weaponTile1Label = UiLabel:new(host, "nimbus_mono")
    playerHUDOverlay:addWidget(weaponTile1Label)

    local weaponBackgroundTile2 = UiRectangle:new(host)
    playerHUDOverlay:addWidget(weaponBackgroundTile2)

    local weaponTile2Label = UiLabel:new(host, "nimbus_mono")
    playerHUDOverlay:addWidget(weaponTile2Label)

    local weaponBackgroundTile3 = UiRectangle:new(host)
    playerHUDOverlay:addWidget(weaponBackgroundTile3)

    local weaponTile3Label = UiLabel:new(host, "nimbus_mono")
    playerHUDOverlay:addWidget(weaponTile3Label)

    local weaponBackgroundTile4 = UiRectangle:new(host)
    playerHUDOverlay:addWidget(weaponBackgroundTile4)

    local weaponTile4Label = UiLabel:new(host, "nimbus_mono")
    playerHUDOverlay:addWidget(weaponTile4Label)

    local weaponImage1 = UiImage:new(host)
    playerHUDOverlay:addWidget(weaponImage1)

    local weaponImage2 = UiImage:new(host)
    playerHUDOverlay:addWidget(weaponImage2)

    local weaponImage3 = UiImage:new(host)
    playerHUDOverlay:addWidget(weaponImage3)

    local weaponImage4 = UiImage:new(host)
    playerHUDOverlay:addWidget(weaponImage4)

    local lifeImage1 = UiImage:new(host)
    playerHUDOverlay:addWidget(lifeImage1)

    local lifeImage2 = UiImage:new(host)
    playerHUDOverlay:addWidget(lifeImage2)

    local lifeImage3 = UiImage:new(host)
    playerHUDOverlay:addWidget(lifeImage3)

    local lifeImage4 = UiImage:new(host)
    playerHUDOverlay:addWidget(lifeImage4)

    local lifeImage5 = UiImage:new(host)
    playerHUDOverlay:addWidget(lifeImage5)

    MissileWidgetsMap.BOMB.image = weaponImage1
    MissileWidgetsMap.BOMB.tile = weaponBackgroundTile1
    MissileWidgetsMap.BOMB.label = weaponTile1Label

    MissileWidgetsMap.FREEZING.image = weaponImage2
    MissileWidgetsMap.FREEZING.tile = weaponBackgroundTile2
    MissileWidgetsMap.FREEZING.label = weaponTile2Label

    MissileWidgetsMap.ELECTRO_RAY.image = weaponImage3
    MissileWidgetsMap.ELECTRO_RAY.tile = weaponBackgroundTile3
    MissileWidgetsMap.ELECTRO_RAY.label = weaponTile3Label

    MissileWidgetsMap.BLACK_HOLE.image = weaponImage4
    MissileWidgetsMap.BLACK_HOLE.tile = weaponBackgroundTile4
    MissileWidgetsMap.BLACK_HOLE.label = weaponTile4Label

    playerHUDOverlay.testDamage = function()
        PlayerHUDOverlay.testAvailableHearts = PlayerHUDOverlay.testAvailableHearts - 1
        if PlayerHUDOverlay.testAvailableHearts <= 0 then
            PlayerHUDOverlay.testAvailableHearts = 5
        end

        if PlayerHUDOverlay.testAvailableHearts == 1 then
            lifeImage1:setOpacity(1.0)
            lifeImage2:setOpacity(0.5)
            lifeImage3:setOpacity(0.5)
            lifeImage4:setOpacity(0.5)
            lifeImage5:setOpacity(0.5)
        elseif PlayerHUDOverlay.testAvailableHearts == 2 then
            lifeImage1:setOpacity(1.0)
            lifeImage2:setOpacity(1.0)
            lifeImage3:setOpacity(0.5)
            lifeImage4:setOpacity(0.5)
            lifeImage5:setOpacity(0.5)
        elseif PlayerHUDOverlay.testAvailableHearts == 3 then
            lifeImage1:setOpacity(1.0)
            lifeImage2:setOpacity(1.0)
            lifeImage3:setOpacity(1.0)
            lifeImage4:setOpacity(0.5)
            lifeImage5:setOpacity(0.5)
        elseif PlayerHUDOverlay.testAvailableHearts == 4 then
            lifeImage1:setOpacity(1.0)
            lifeImage2:setOpacity(1.0)
            lifeImage3:setOpacity(1.0)
            lifeImage4:setOpacity(1.0)
            lifeImage5:setOpacity(0.5)
        elseif PlayerHUDOverlay.testAvailableHearts == 5 then
            lifeImage1:setOpacity(1.0)
            lifeImage2:setOpacity(1.0)
            lifeImage3:setOpacity(1.0)
            lifeImage4:setOpacity(1.0)
            lifeImage5:setOpacity(1.0)
        end
    end

    playerHUDOverlay.onCurrentMissileChanged = function()
        local currentMissileType = getSelectedMissileType(host)
        if PlayerHUDOverlay.prevSelectedMissileType ~= currentMissileType then
            startAnimationForMissileWidget(host, PlayerHUDOverlay.prevSelectedMissileType, "FocusOut")
            PlayerHUDOverlay.prevSelectedMissileType = currentMissileType
            startAnimationForMissileWidget(host, PlayerHUDOverlay.prevSelectedMissileType, "FocusIn")
        end
    end

    playerHUDOverlay.onMissilesDataChanged = function()
        local missileDataJsonStr = _GetAllMissilesData(host)
        local parsedDataJson = json.decode(missileDataJsonStr)
        if parsedDataJson["all_missiles_data"] ~= nil then
            local missilesDataTable = parsedDataJson["all_missiles_data"]
            for _, value in pairs(missilesDataTable) do
                local missileTypeName = getMissileTypeNameByValue(tonumber(value[1]))
                local missilesCount = tonumber(value[2])
                local activeWidgetsTable = MissileWidgetsMap[missileTypeName]
                activeWidgetsTable.label:setText(tostring(missilesCount))
                print("onMissilesDataChanged => missileName: " .. missileTypeName .. ", count missiles: " .. tostring(missilesCount))
            end
        end
    end

    playerHUDOverlay:subscribeOnAllWidgetLuaProxiesReady(function()
        lifeRootContainer:setParent(host, playerHUDOverlayCanvas.widgetName, playerHUDOverlayCanvas.widgetName)
        lifeRootContainer:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
            playerHUDOverlayCanvas.widgetName, 30)
        lifeRootContainer:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
            playerHUDOverlayCanvas.widgetName, 30)
        lifeRootContainer:setWidth(lifeRootContainerWidth)
        lifeRootContainer:setHeight(lifeRootContainerHeight)

        weaponRootContainer:setParent(host, playerHUDOverlayCanvas.widgetName, playerHUDOverlayCanvas.widgetName)
        weaponRootContainer:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
            playerHUDOverlayCanvas.widgetName, 30)
        weaponRootContainer:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
            playerHUDOverlayCanvas.widgetName, 30)
        weaponRootContainer:setWidth(weaponRootContainerWidth)
        weaponRootContainer:setHeight(lifeRootContainerHeight)

        weaponBackgroundTile1:setParent(host, playerHUDOverlayCanvas.widgetName, weaponRootContainer.widgetName)
        weaponBackgroundTile1:setZOrder(3);
        weaponBackgroundTile1:setHeight(weaponWidth);
        weaponBackgroundTile1:setWidth(weaponWidth);
        weaponBackgroundTile1:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
            weaponRootContainer.widgetName, weaponInterval);
        weaponBackgroundTile1:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
            weaponRootContainer.widgetName, weaponTopBottomMargin);
        weaponBackgroundTile1:setColorHexValue(PlayerHUDOverlay.weaponBackgroundTileColor)
        weaponBackgroundTile1:setBorderRadius(8)
        weaponBackgroundTile1:addAnimation(host, "FocusIn", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "Scale",
            UiBaseWidget.EnginePropertyType.Float, 1.0, 1.25)
        weaponBackgroundTile1:addAnimation(host, "FocusOut", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "Scale",
            UiBaseWidget.EnginePropertyType.Float, 1.25, 1.0)
        weaponBackgroundTile1:addAnimation(host, "FocusIn", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "VerticalCenterOffset",
            UiBaseWidget.EnginePropertyType.Integer, 0, 40)
        weaponBackgroundTile1:addAnimation(host, "FocusOut", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "VerticalCenterOffset",
            UiBaseWidget.EnginePropertyType.Integer, 40, 0)

        weaponImage1:setParent(host, playerHUDOverlayCanvas.widgetName, weaponBackgroundTile1.widgetName)
        weaponImage1:setTextureSource("weapon_missile.png");
        weaponImage1:setZOrder(4);
        weaponImage1:setRotationDegrees(180)
        weaponImage1:setHeight(weaponImageSize);
        weaponImage1:setWidth(weaponImageSize);
        weaponImage1:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER, UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
            weaponBackgroundTile1.widgetName);
        weaponImage1:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
            weaponBackgroundTile1.widgetName);
        weaponImage1:addAnimation(host, "FocusIn", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2, "Scale",
            UiBaseWidget.EnginePropertyType.Float, 1.0, 1.25)
        weaponImage1:addAnimation(host, "FocusOut", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2, "Scale",
            UiBaseWidget.EnginePropertyType.Float, 1.25, 1.0)
        weaponImage1:addAnimation(host, "FocusIn", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "VerticalCenterOffset",
            UiBaseWidget.EnginePropertyType.Integer, 0, 40)
        weaponImage1:addAnimation(host, "FocusOut", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "VerticalCenterOffset",
            UiBaseWidget.EnginePropertyType.Integer, 40, 0)

        weaponTile1Label:setParent(host, playerHUDOverlayCanvas.widgetName, weaponBackgroundTile1.widgetName)
        weaponTile1Label:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
            weaponBackgroundTile1.widgetName, 6)
        weaponTile1Label:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
            weaponBackgroundTile1.widgetName, 0)
        weaponTile1Label:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
            weaponBackgroundTile1.widgetName, 6)
        weaponTile1Label:setVerticalCenterOffset(-8)
        weaponTile1Label:setHeight(weaponImageSize / 15)
        weaponTile1Label:setTextColorHexValue(PlayerHUDOverlay.missilesCountLabelColor)
        weaponTile1Label:setFontSize(10.0)
        weaponTile1Label:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.LEFT)
        weaponTile1Label:setZOrder(4)
        weaponTile1Label:addAnimation(host, "FocusIn", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "VerticalCenterOffset",
            UiBaseWidget.EnginePropertyType.Integer, 0, 30)
        weaponTile1Label:addAnimation(host, "FocusOut", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "VerticalCenterOffset",
            UiBaseWidget.EnginePropertyType.Integer, 30, 0)

        weaponBackgroundTile2:setParent(host, playerHUDOverlayCanvas.widgetName, weaponRootContainer.widgetName)
        weaponBackgroundTile2:setZOrder(3);
        weaponBackgroundTile2:setHeight(weaponWidth);
        weaponBackgroundTile2:setWidth(weaponWidth);
        weaponBackgroundTile2:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT,
            weaponBackgroundTile1.widgetName, weaponInterval);
        weaponBackgroundTile2:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
            weaponRootContainer.widgetName, weaponTopBottomMargin)
        weaponBackgroundTile2:setColorHexValue(PlayerHUDOverlay.weaponBackgroundTileColor)
        weaponBackgroundTile2:setBorderRadius(8)
        weaponBackgroundTile2:addAnimation(host, "FocusIn", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "Scale",
            UiBaseWidget.EnginePropertyType.Float, 1.0, 1.25)
        weaponBackgroundTile2:addAnimation(host, "FocusOut", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "Scale",
            UiBaseWidget.EnginePropertyType.Float, 1.25, 1.0)
        weaponBackgroundTile2:addAnimation(host, "FocusIn", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "VerticalCenterOffset",
            UiBaseWidget.EnginePropertyType.Integer, 0, 40)
        weaponBackgroundTile2:addAnimation(host, "FocusOut", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "VerticalCenterOffset",
            UiBaseWidget.EnginePropertyType.Integer, 40, 0)

        weaponImage2:setParent(host, playerHUDOverlayCanvas.widgetName, weaponBackgroundTile2.widgetName)
        weaponImage2:setTextureSource("weapon_missile.png");
        weaponImage2:setZOrder(4);
        weaponImage2:setRotationDegrees(180)
        weaponImage2:setHeight(weaponImageSize);
        weaponImage2:setWidth(weaponImageSize);
        weaponImage2:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER, UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
            weaponBackgroundTile2.widgetName);
        weaponImage2:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
            weaponBackgroundTile2.widgetName);
        weaponImage2:addAnimation(host, "FocusIn", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2, "Scale",
            UiBaseWidget.EnginePropertyType.Float, 1.0, 1.25)
        weaponImage2:addAnimation(host, "FocusOut", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2, "Scale",
            UiBaseWidget.EnginePropertyType.Float, 1.25, 1.0)
        weaponImage2:addAnimation(host, "FocusIn", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "VerticalCenterOffset",
            UiBaseWidget.EnginePropertyType.Integer, 0, 40)
        weaponImage2:addAnimation(host, "FocusOut", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "VerticalCenterOffset",
            UiBaseWidget.EnginePropertyType.Integer, 40, 0)

        weaponTile2Label:setParent(host, playerHUDOverlayCanvas.widgetName, weaponBackgroundTile2.widgetName)
        weaponTile2Label:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
            weaponBackgroundTile2.widgetName, 6)
        weaponTile2Label:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
            weaponBackgroundTile2.widgetName, 0)
        weaponTile2Label:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
            weaponBackgroundTile2.widgetName, 6)
        weaponTile2Label:setVerticalCenterOffset(-8)
        weaponTile2Label:setHeight(weaponImageSize / 15)
        weaponTile2Label:setTextColorHexValue(PlayerHUDOverlay.missilesCountLabelColor)
        weaponTile2Label:setFontSize(10.0)
        weaponTile2Label:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.LEFT)
        weaponTile2Label:setZOrder(4)
        weaponTile2Label:addAnimation(host, "FocusIn", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "VerticalCenterOffset",
            UiBaseWidget.EnginePropertyType.Integer, 0, 30)
        weaponTile2Label:addAnimation(host, "FocusOut", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "VerticalCenterOffset",
            UiBaseWidget.EnginePropertyType.Integer, 30, 0)

        weaponBackgroundTile3:setParent(host, playerHUDOverlayCanvas.widgetName, weaponRootContainer.widgetName)
        weaponBackgroundTile3:setZOrder(3);
        weaponBackgroundTile3:setHeight(weaponWidth);
        weaponBackgroundTile3:setWidth(weaponWidth);
        weaponBackgroundTile3:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT,
            weaponBackgroundTile2.widgetName, weaponInterval);
        weaponBackgroundTile3:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
            weaponRootContainer.widgetName, weaponTopBottomMargin)
        weaponBackgroundTile3:setColorHexValue(PlayerHUDOverlay.weaponBackgroundTileColor)
        weaponBackgroundTile3:setBorderRadius(8)
        weaponBackgroundTile3:addAnimation(host, "FocusIn", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "Scale",
            UiBaseWidget.EnginePropertyType.Float, 1.0, 1.25)
        weaponBackgroundTile3:addAnimation(host, "FocusOut", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "Scale",
            UiBaseWidget.EnginePropertyType.Float, 1.25, 1.0)
        weaponBackgroundTile3:addAnimation(host, "FocusIn", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "VerticalCenterOffset",
            UiBaseWidget.EnginePropertyType.Integer, 0, 40)
        weaponBackgroundTile3:addAnimation(host, "FocusOut", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "VerticalCenterOffset",
            UiBaseWidget.EnginePropertyType.Integer, 40, 0)

        weaponImage3:setParent(host, playerHUDOverlayCanvas.widgetName, weaponBackgroundTile3.widgetName)
        weaponImage3:setTextureSource("weapon_missile.png");
        weaponImage3:setZOrder(4);
        weaponImage3:setRotationDegrees(180)
        weaponImage3:setHeight(weaponImageSize);
        weaponImage3:setWidth(weaponImageSize);
        weaponImage3:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER, UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
            weaponBackgroundTile3.widgetName);
        weaponImage3:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
            weaponBackgroundTile3.widgetName);
        weaponImage3:addAnimation(host, "FocusIn", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2, "Scale",
            UiBaseWidget.EnginePropertyType.Float, 1.0, 1.25)
        weaponImage3:addAnimation(host, "FocusOut", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2, "Scale",
            UiBaseWidget.EnginePropertyType.Float, 1.25, 1.0)
        weaponImage3:addAnimation(host, "FocusIn", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "VerticalCenterOffset",
            UiBaseWidget.EnginePropertyType.Integer, 0, 40)
        weaponImage3:addAnimation(host, "FocusOut", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "VerticalCenterOffset",
            UiBaseWidget.EnginePropertyType.Integer, 40, 0)

        weaponTile3Label:setParent(host, playerHUDOverlayCanvas.widgetName, weaponBackgroundTile3.widgetName)
        weaponTile3Label:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
            weaponBackgroundTile3.widgetName, 6)
        weaponTile3Label:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
            weaponBackgroundTile3.widgetName, 0)
        weaponTile3Label:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
            weaponBackgroundTile3.widgetName, 6)
        weaponTile3Label:setVerticalCenterOffset(-8)
        weaponTile3Label:setHeight(weaponImageSize / 15)
        weaponTile3Label:setTextColorHexValue(PlayerHUDOverlay.missilesCountLabelColor)
        weaponTile3Label:setFontSize(10.0)
        weaponTile3Label:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.LEFT)
        weaponTile3Label:setZOrder(4)
        weaponTile3Label:addAnimation(host, "FocusIn", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "VerticalCenterOffset",
            UiBaseWidget.EnginePropertyType.Integer, 0, 30)
        weaponTile3Label:addAnimation(host, "FocusOut", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "VerticalCenterOffset",
            UiBaseWidget.EnginePropertyType.Integer, 30, 0)

        weaponBackgroundTile4:setParent(host, playerHUDOverlayCanvas.widgetName, weaponRootContainer.widgetName)
        weaponBackgroundTile4:setZOrder(3);
        weaponBackgroundTile4:setHeight(weaponWidth);
        weaponBackgroundTile4:setWidth(weaponWidth);
        weaponBackgroundTile4:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT,
            weaponBackgroundTile3.widgetName, weaponInterval);
        weaponBackgroundTile4:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
            weaponRootContainer.widgetName, weaponTopBottomMargin)
        weaponBackgroundTile4:setColorHexValue(PlayerHUDOverlay.weaponBackgroundTileColor)
        weaponBackgroundTile4:setBorderRadius(8)
        weaponBackgroundTile4:addAnimation(host, "FocusIn", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "Scale",
            UiBaseWidget.EnginePropertyType.Float, 1.0, 1.25)
        weaponBackgroundTile4:addAnimation(host, "FocusOut", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "Scale",
            UiBaseWidget.EnginePropertyType.Float, 1.25, 1.0)
        weaponBackgroundTile4:addAnimation(host, "FocusIn", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "VerticalCenterOffset",
            UiBaseWidget.EnginePropertyType.Integer, 0, 40)
        weaponBackgroundTile4:addAnimation(host, "FocusOut", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "VerticalCenterOffset",
            UiBaseWidget.EnginePropertyType.Integer, 40, 0)

        weaponImage4:setParent(host, playerHUDOverlayCanvas.widgetName, weaponBackgroundTile4.widgetName)
        weaponImage4:setTextureSource("weapon_missile.png");
        weaponImage4:setZOrder(4);
        weaponImage4:setRotationDegrees(180)
        weaponImage4:setHeight(weaponImageSize);
        weaponImage4:setWidth(weaponImageSize);
        weaponImage4:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER, UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
            weaponBackgroundTile4.widgetName);
        weaponImage4:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
            weaponBackgroundTile4.widgetName);
        weaponImage4:addAnimation(host, "FocusIn", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "VerticalCenterOffset",
            UiBaseWidget.EnginePropertyType.Integer, 0, 40)
        weaponImage4:addAnimation(host, "FocusOut", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "VerticalCenterOffset",
            UiBaseWidget.EnginePropertyType.Integer, 40, 0)

        weaponImage4:addAnimation(host, "FocusIn", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2, "Scale",
            UiBaseWidget.EnginePropertyType.Float, 1.0, 1.25)
        weaponImage4:addAnimation(host, "FocusOut", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2, "Scale",
            UiBaseWidget.EnginePropertyType.Float, 1.25, 1.0)

        weaponTile4Label:setParent(host, playerHUDOverlayCanvas.widgetName, weaponBackgroundTile4.widgetName)
        weaponTile4Label:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
            weaponBackgroundTile4.widgetName, 6)
        weaponTile4Label:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
            weaponBackgroundTile4.widgetName, 0)
        weaponTile4Label:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
            weaponBackgroundTile4.widgetName, 6)
        weaponTile4Label:setVerticalCenterOffset(-8)
        weaponTile4Label:setHeight(weaponImageSize / 15)
        weaponTile4Label:setTextColorHexValue(PlayerHUDOverlay.missilesCountLabelColor)
        weaponTile4Label:setFontSize(10.0)
        weaponTile4Label:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.LEFT)
        weaponTile4Label:setZOrder(4)
        weaponTile4Label:addAnimation(host, "FocusIn", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "VerticalCenterOffset",
            UiBaseWidget.EnginePropertyType.Integer, 0, 30)
        weaponTile4Label:addAnimation(host, "FocusOut", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "VerticalCenterOffset",
            UiBaseWidget.EnginePropertyType.Integer, 30, 0)

        lifeImage1:setParent(host, playerHUDOverlayCanvas.widgetName, lifeRootContainer.widgetName);
        lifeImage1:setTextureSource("scaled_down_heart.png");
        lifeImage1:setZOrder(2);
        lifeImage1:setRotationDegrees(180)
        lifeImage1:setHeight(heartWidth);
        lifeImage1:setWidth(heartWidth);
        lifeImage1:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, lifeRootContainer.widgetName);
        lifeImage1:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM, lifeRootContainer
            .widgetName);

        lifeImage2:setParent(host, playerHUDOverlayCanvas.widgetName, lifeRootContainer.widgetName);
        lifeImage2:setTextureSource("scaled_down_heart.png");
        lifeImage2:setZOrder(2);
        lifeImage2:setRotationDegrees(180)
        lifeImage2:setHeight(heartWidth);
        lifeImage2:setWidth(heartWidth);
        lifeImage2:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT, lifeImage1.widgetName,
            heartInterval);
        lifeImage2:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM, lifeRootContainer
            .widgetName);

        lifeImage3:setParent(host, playerHUDOverlayCanvas.widgetName, lifeRootContainer.widgetName);
        lifeImage3:setTextureSource("scaled_down_heart.png");
        lifeImage3:setZOrder(2);
        lifeImage3:setRotationDegrees(180)
        lifeImage3:setHeight(heartWidth);
        lifeImage3:setWidth(heartWidth);
        lifeImage3:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT, lifeImage2.widgetName,
            heartInterval);
        lifeImage3:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM, lifeRootContainer
            .widgetName);

        lifeImage4:setParent(host, playerHUDOverlayCanvas.widgetName, lifeRootContainer.widgetName);
        lifeImage4:setTextureSource("scaled_down_heart.png");
        lifeImage4:setZOrder(2);
        lifeImage4:setRotationDegrees(180)
        lifeImage4:setHeight(heartWidth);
        lifeImage4:setWidth(heartWidth);
        lifeImage4:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT, lifeImage3.widgetName,
            heartInterval);
        lifeImage4:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM, lifeRootContainer
            .widgetName);

        lifeImage5:setParent(host, playerHUDOverlayCanvas.widgetName, lifeRootContainer.widgetName);
        lifeImage5:setTextureSource("scaled_down_heart.png");
        lifeImage5:setZOrder(2);
        lifeImage5:setRotationDegrees(180)
        lifeImage5:setHeight(heartWidth);
        lifeImage5:setWidth(heartWidth);
        lifeImage5:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT, lifeImage4.widgetName,
            heartInterval);
        lifeImage5:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM, lifeRootContainer
            .widgetName);

        playerHUDOverlay:onMissilesDataChanged()   -- initialize all data missiles widgets
        playerHUDOverlay:onCurrentMissileChanged() -- initialize current missile widget
    end)

    playerHUDOverlay.onGameEventTriggered = function(eventName, jsonArgs)
        if "PlayerStatusChanged" == eventName then
            assert(jsonArgs ~= nil and type(jsonArgs) == "string")
            local parsedJson = json.decode(jsonArgs)
            if parsedJson["player_status_type"] ~= nil then
                local statusType = tonumber(parsedJson["player_status_type"])
                if statusType == PlayerStatusType.LIFE_POINTS_CHANGED then
                    playerHUDOverlay.testDamage()
                elseif statusType == PlayerStatusType.ACTIVE_WEAPON_CHANGED then
                    playerHUDOverlay.onCurrentMissileChanged()
                elseif statusType == PlayerStatusType.MISSILES_COUNT_CHANGED then
                    print("onGameEventTriggered => MISSILES_COUNT_CHANGED");
                    playerHUDOverlay.onMissilesDataChanged()
                end
            end
        end
    end

    return playerHUDOverlay
end

return PlayerHUDOverlay
