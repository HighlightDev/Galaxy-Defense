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
local json = require("Ui/Core/3rdparty/json")
local UiCanvas = require("Ui/Core/uiCanvas")
local UiOverlay = require("Ui/Core/uiOverlay")
local UiItem = require("Ui/Core/uiItem")
local UiRectangle = require("Ui/Core/uiRectangle")
local UiImage = require("Ui/Core/uiImage")
local WeaponTile = require("Ui/Widgets/WeaponTile")

PlayerStatusType = {
    NONE = 0,
    LIFE_POINTS_CHANGED = 1,
    ACTIVE_WEAPON_CHANGED = 2,
    MISSILES_COUNT_CHANGED = 3,
    AVAILABLE_MISSILES_CHANGED = 4,
    DESTROYED_ENEMY_SPACESHIPS_COUNT_CHANGED = 5
}

GameModeType = {
    IDLE = 0,
    COMBAT = 1,
    SPACE_STATION_PLACEMENT = 2
}

MissileType = {
    NONE = 0,
    BOMB = 1,
    FREEZING = 2,
    ELECTRO_RAY = 3,
    BLACK_HOLE = 4
}

MissileWidgetsMap = {
    BOMB = nil,
    FREEZING = nil,
    BLACK_HOLE = nil,
    ELECTRO_RAY = nil
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

local function getEnemySpaceshipsCountDestroyedByPlayer(host)
    return tonumber(_GetEnemySpaceshipsCountDestroyedByPlayer(host))
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
            activeWidgetsTable:startAnimation(host, animationName)
        end
    end
end

function PlayerHUDOverlay:new(host)
    local windowWidth = _GetWindowWidth(host)
    local windowHeight = _GetWindowHeight(host)

    local playerHUDOverlayCanvas = UiCanvas:new(host, 0, 0, windowWidth, windowHeight)
    playerHUDOverlayCanvas:subscribeOnLuaProxyReady(function(host)
        _InitializeCanvasInputSystem(host, playerHUDOverlayCanvas.luaProxyId)
    end)
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

    local enemyCountTile = UiRectangle:new(host)
    playerHUDOverlay:addWidget(enemyCountTile)

    enemyCountTile:subscriveOnMouseInputClickedCallback(function()
        EventsHelper:sendChangeGameModeGameThreadEvent(host,
            EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH, GameModeType.SPACE_STATION_PLACEMENT)
    end)

    local stationImg = UiImage:new(host)
    playerHUDOverlay:addWidget(stationImg)

    local lifeRootContainer = UiItem:new(host)
    playerHUDOverlay:addWidget(lifeRootContainer)

    local weaponRootContainer = UiItem:new(host)
    playerHUDOverlay:addWidget(weaponRootContainer)

    local weaponTile1 = WeaponTile:new(host, playerHUDOverlay)
    playerHUDOverlay:addCompoundWidget(weaponTile1)
    local weaponTile2 = WeaponTile:new(host, playerHUDOverlay)
    playerHUDOverlay:addCompoundWidget(weaponTile2)
    local weaponTile3 = WeaponTile:new(host, playerHUDOverlay)
    playerHUDOverlay:addCompoundWidget(weaponTile3)
    local weaponTile4 = WeaponTile:new(host, playerHUDOverlay)
    playerHUDOverlay:addCompoundWidget(weaponTile4)

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

    MissileWidgetsMap.BOMB = weaponTile1
    MissileWidgetsMap.FREEZING = weaponTile2
    MissileWidgetsMap.ELECTRO_RAY = weaponTile3
    MissileWidgetsMap.BLACK_HOLE = weaponTile4

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
        if false and PlayerHUDOverlay.prevSelectedMissileType ~= currentMissileType then
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
                activeWidgetsTable:setLabelText(tostring(missilesCount))
            end
        end
    end

    playerHUDOverlay.onWindowSizeChanged = function(width, height)
        assert(width ~= nil and type(width) == "number" and height ~= nil and type(height) == "number")
        local lifeRootContainerWidth = width / 3.0;
        local lifeRootContainerHeight = height / 4.0;
        local weaponRootContainerWidth = width / 3.0;
        local heartWidth = lifeRootContainerWidth / 10.0
        local heartInterval = heartWidth * 0.5
        local weaponCount = 4.0
        local weaponWidth = weaponRootContainerWidth / weaponCount
        local weaponInterval = weaponWidth / 8.0
        weaponWidth = (weaponRootContainerWidth - (weaponInterval * (weaponCount + 1))) / weaponCount

        lifeRootContainer:setWidth(lifeRootContainerWidth)
        lifeRootContainer:setHeight(lifeRootContainerHeight)
        weaponRootContainer:setWidth(weaponRootContainerWidth)
        weaponRootContainer:setHeight(lifeRootContainerHeight)

        weaponTile1:setWidth(weaponWidth)
        weaponTile1:setHeight(weaponWidth)

        weaponTile2:setHeight(weaponWidth)
        weaponTile2:setWidth(weaponWidth)

        weaponTile3:setHeight(weaponWidth);
        weaponTile3:setWidth(weaponWidth);

        weaponTile4:setHeight(weaponWidth);
        weaponTile4:setWidth(weaponWidth);

        lifeImage1:setHeight(heartWidth);
        lifeImage1:setWidth(heartWidth);
        lifeImage2:setHeight(heartWidth);
        lifeImage2:setWidth(heartWidth);
        lifeImage3:setHeight(heartWidth);
        lifeImage3:setWidth(heartWidth);
        lifeImage3:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT, lifeImage2.widgetName,
            heartInterval);
        lifeImage4:setHeight(heartWidth);
        lifeImage4:setWidth(heartWidth);
        lifeImage4:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT, lifeImage3.widgetName,
            heartInterval);
        lifeImage5:setHeight(heartWidth);
        lifeImage5:setWidth(heartWidth);
        lifeImage5:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT, lifeImage4.widgetName,
            heartInterval);
    end

    playerHUDOverlay:subscribeOnAllWidgetLuaProxiesReady(function()
        enemyCountTile:setParent(host, playerHUDOverlayCanvas.widgetName, playerHUDOverlayCanvas.widgetName)
        enemyCountTile:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
            playerHUDOverlayCanvas.widgetName, 30)
        enemyCountTile:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP,
            playerHUDOverlayCanvas.widgetName, 30)
        enemyCountTile:setWidth(150)
        enemyCountTile:setHeight(150)
        enemyCountTile:setZOrder(3);
        enemyCountTile:setColorHexValue(PlayerHUDOverlay.weaponBackgroundTileColor)
        enemyCountTile:setBorderRadius(8)
        enemyCountTile:enableMouseInputReceiverBase(host)

        stationImg:setParent(host, playerHUDOverlayCanvas.widgetName, enemyCountTile.widgetName)
        stationImg:setTextureSource("space_station_img.png");
        stationImg:setZOrder(4);
        stationImg:setRotationDegrees(180)
        stationImg:setHeight(130);
        stationImg:setWidth(130);
        stationImg:setAnchor(UiItemBase.UiAnchorType.HORIZONTAL_CENTER, UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
            enemyCountTile.widgetName);
        stationImg:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER, UiItemBase.UiAnchorType.VERTICAL_CENTER,
            enemyCountTile.widgetName);

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

        weaponTile1:setParent(host, playerHUDOverlayCanvas.widgetName, weaponRootContainer.widgetName)
        weaponTile1:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, weaponRootContainer.widgetName,
            weaponInterval);
        weaponTile1:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
            weaponRootContainer.widgetName, weaponTopBottomMargin);
        weaponTile1:setWidth(weaponWidth)
        weaponTile1:setHeight(weaponWidth)
        weaponTile1:addAnimation(host, "FocusIn", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "Scale", UiBaseWidget.EnginePropertyType.Float, 1.0, 1.25)
        weaponTile1:addAnimation(host, "FocusOut", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "Scale", UiBaseWidget.EnginePropertyType.Float, 1.25, 1.0)
        weaponTile1:addAnimation(host, "FocusIn", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "VerticalCenterOffset", UiBaseWidget.EnginePropertyType.Integer, 0, 40)
        weaponTile1:addAnimation(host, "FocusOut", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "VerticalCenterOffset", UiBaseWidget.EnginePropertyType.Integer, 40, 0)

        weaponTile2:setParent(host, playerHUDOverlayCanvas.widgetName, weaponRootContainer.widgetName)
        weaponTile2:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT,
            weaponTile1.widgetName, weaponInterval);
        weaponTile2:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
            weaponRootContainer.widgetName, weaponTopBottomMargin)
        weaponTile2:setHeight(weaponWidth);
        weaponTile2:setWidth(weaponWidth);
        weaponTile2:addAnimation(host, "FocusIn", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "Scale", UiBaseWidget.EnginePropertyType.Float, 1.0, 1.25)
        weaponTile2:addAnimation(host, "FocusOut", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "Scale", UiBaseWidget.EnginePropertyType.Float, 1.25, 1.0)
        weaponTile2:addAnimation(host, "FocusIn", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "VerticalCenterOffset", UiBaseWidget.EnginePropertyType.Integer, 0, 40)
        weaponTile2:addAnimation(host, "FocusOut", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "VerticalCenterOffset", UiBaseWidget.EnginePropertyType.Integer, 40, 0)

        weaponTile3:setParent(host, playerHUDOverlayCanvas.widgetName, weaponRootContainer.widgetName)
        weaponTile3:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT,
            weaponTile2.widgetName, weaponInterval);
        weaponTile3:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
            weaponRootContainer.widgetName, weaponTopBottomMargin)
        weaponTile3:setHeight(weaponWidth);
        weaponTile3:setWidth(weaponWidth);
        weaponTile3:addAnimation(host, "FocusIn", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "Scale", UiBaseWidget.EnginePropertyType.Float, 1.0, 1.25)
        weaponTile3:addAnimation(host, "FocusOut", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "Scale", UiBaseWidget.EnginePropertyType.Float, 1.25, 1.0)
        weaponTile3:addAnimation(host, "FocusIn", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "VerticalCenterOffset", UiBaseWidget.EnginePropertyType.Integer, 0, 40)
        weaponTile3:addAnimation(host, "FocusOut", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "VerticalCenterOffset", UiBaseWidget.EnginePropertyType.Integer, 40, 0)

        weaponTile4:setParent(host, playerHUDOverlayCanvas.widgetName, weaponRootContainer.widgetName)
        weaponTile4:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT,
            weaponTile3.widgetName, weaponInterval);
        weaponTile4:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
            weaponRootContainer.widgetName, weaponTopBottomMargin)
        weaponTile4:setHeight(weaponWidth);
        weaponTile4:setWidth(weaponWidth);
        weaponTile4:addAnimation(host, "FocusIn", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "Scale", UiBaseWidget.EnginePropertyType.Float, 1.0, 1.25)
        weaponTile4:addAnimation(host, "FocusOut", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "Scale", UiBaseWidget.EnginePropertyType.Float, 1.25, 1.0)
        weaponTile4:addAnimation(host, "FocusIn", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "VerticalCenterOffset", UiBaseWidget.EnginePropertyType.Integer, 0, 40)
        weaponTile4:addAnimation(host, "FocusOut", UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.2,
            "VerticalCenterOffset", UiBaseWidget.EnginePropertyType.Integer, 40, 0)

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
                    playerHUDOverlay.onMissilesDataChanged()
                elseif statusType == PlayerStatusType.DESTROYED_ENEMY_SPACESHIPS_COUNT_CHANGED then
                end
            end
        end
    end

    playerHUDOverlay.onEngineEventTriggered = function(eventName, jsonArgs)
        if "WindowSizeChanged" == eventName and playerHUDOverlay.allWidgetLuaProxiesReady == true then
            assert(jsonArgs ~= nil and type(jsonArgs) == "string")
            local parsedJson = json.decode(jsonArgs)
            local windowSize = {}
            if parsedJson["width"] ~= nil then
                windowSize.width = tonumber(parsedJson["width"])
            end
            if parsedJson["height"] ~= nil then
                windowSize.height = tonumber(parsedJson["height"])
            end
            playerHUDOverlay.onWindowSizeChanged(windowSize.width, windowSize.height)
        end
    end

    return playerHUDOverlay
end

return PlayerHUDOverlay
