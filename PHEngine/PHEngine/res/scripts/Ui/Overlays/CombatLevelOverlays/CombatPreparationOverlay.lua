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
        package.path = package.path .. "" .. unixLikePath .. "?.lua"
    end
end

setup()
--
--[[ END   *** this snippet has to be inserted everywhere where your want to require custom modules  ***  END]]
local json = require("Ui/Core/3rdparty/json")
local UiCanvas = require("Ui/Core/uiCanvas")
local UiOverlay = require("Ui/Core/uiOverlay")
local ImageButton = require("Ui/Widgets/ImageButton")
local EventsHelper = require("Ui/Core/eventsHelper")
local Styles = require("Ui/Common/styles")
local TowerGridPanel = require("Ui/Widgets/TowerGridPanel")
local TowerUpgradesPanel = require("Ui/Widgets/TowerUpgradesPanel")

CombatPreparationOverlay = {buttonRadius = 6, localTimerManager = nil}

local GameModeType = {INIT = 0, COMBAT = 1, SPACE_STATION_PLACEMENT = 2}

function CombatPreparationOverlay:new(host)
    local windowWidth = _GetWindowWidth(host)
    local windowHeight = _GetWindowHeight(host)

    local combatPreparationOverlayCanvas = UiCanvas:new(host, 0, 0, windowWidth, windowHeight,
                                                        "CombatPreparationOverlayCanvas")
    combatPreparationOverlayCanvas:subscribeOnLuaProxyReady(function(host)
        _InitializeCanvasInputSystem(host, combatPreparationOverlayCanvas.luaProxyId)
    end)
    local combatPreparationOverlay = UiOverlay:createOverlay(host, "CombatPreparationOverlay",
                                                             combatPreparationOverlayCanvas)

    combatPreparationOverlay.localTimerManager = combatPreparationOverlay:getTimerManager()

    local towerUpgradesPanel = TowerUpgradesPanel:new(host, combatPreparationOverlay)
    combatPreparationOverlay:addCompoundWidget(towerUpgradesPanel)

    local TileSize = 80

    local towerGridPanel = TowerGridPanel:new(host, combatPreparationOverlay, {
        buttonRadius = CombatPreparationOverlay.buttonRadius,
        mainButtonSize = TileSize,
        onTowerUpgradesButtonClicked = function() towerUpgradesPanel:setIsVisible(true) end
    })
    combatPreparationOverlay:addCompoundWidget(towerGridPanel)

    local mainButtonSize = towerGridPanel.mainButtonSize

    local completeStageButton = ImageButton:new(host, combatPreparationOverlay, "CompleteStageButton")
    combatPreparationOverlay:addCompoundWidget(completeStageButton)

    local canCompletePreparationStage = false

    completeStageButton:subscribeOnMouseInputClickedCallback(function()
        towerGridPanel.hideCreatePanel()
        EventsHelper:sendChangeGameModeGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
                                                       GameModeType.COMBAT)
    end)

    completeStageButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if canCompletePreparationStage then
            if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
                completeStageButton:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
            else
                completeStageButton:setButtonColorHexValue(Styles.Colors.buttonColor)
            end
        end
    end)

    combatPreparationOverlay:subscribeOnAllWidgetLuaProxiesReady(function(host, sender)
        print("combatPreparationOverlay:OnAllWidgetLuaProxiesReady: name: " .. tostring(sender.overlayName))

        towerGridPanel:setupLayout(combatPreparationOverlayCanvas.widgetName)
        towerUpgradesPanel:setupLayout(combatPreparationOverlayCanvas.widgetName, windowWidth, windowHeight)

        completeStageButton:setParent(host, combatPreparationOverlayCanvas.widgetName,
                                      combatPreparationOverlayCanvas.widgetName)
        completeStageButton:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP,
                                      combatPreparationOverlayCanvas.widgetName, 20)
        completeStageButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
                                      combatPreparationOverlayCanvas.widgetName, 20)
        completeStageButton:setWidth(mainButtonSize)
        completeStageButton:setHeight(mainButtonSize)
        completeStageButton:setButtonBorderRadius(CombatPreparationOverlay.buttonRadius)
        completeStageButton:setImageTextureSource("flag-banner-fold.png")
        completeStageButton:setRotationDegrees(180)
        completeStageButton:setButtonColorHexValue(Styles.Colors.buttonColor)
    end)

    combatPreparationOverlay.onGameEventTriggered = function(eventName, jsonArgs)
        if "PlayerStatusChanged" == eventName then
            assert(jsonArgs ~= nil and type(jsonArgs) == "string")
            local parsedJson = json.decode(jsonArgs)
            if (parsedJson["player_status_type"] ~= nil) then
                local statusType = tonumber(parsedJson["player_status_type"])
                if statusType == PlayerStatusType.TOWERS_COUNT_CHANGED then
                    if parsedJson["towers_count"] ~= nil then
                        local newTowersCount = tonumber(parsedJson["towers_count"])
                        if newTowersCount > 0 then
                            canCompletePreparationStage = true
                        else
                            canCompletePreparationStage = false
                        end
                    end
                end
            end
        end
    end
    combatPreparationOverlay.onEngineEventTriggered = function(eventName, jsonArgs) end

    combatPreparationOverlay.derivedUpdateCallback = function()
        if combatPreparationOverlay.allWidgetLuaProxiesReady then
            completeStageButton:setIsButtonActive(canCompletePreparationStage)
        end
    end

    combatPreparationOverlay.onBroadcastEventTriggered = function(eventName, jsonArgs)
        if "CombatLevelEvents" == eventName and combatPreparationOverlay.allWidgetLuaProxiesReady == true then
            assert(jsonArgs ~= nil and type(jsonArgs) == "string")
            local parsedJson = json.decode(jsonArgs)
            if parsedJson["action"] ~= nil then
                local action = tostring(parsedJson["action"])
                if action == "switch_mode" then
                    local mode = tostring(parsedJson["mode"])
                    if mode == "IDLE" then towerGridPanel:handleBroadcastSwitchModeIdle(host) end
                end
            end
        end
    end

    return combatPreparationOverlay
end

return CombatPreparationOverlay
