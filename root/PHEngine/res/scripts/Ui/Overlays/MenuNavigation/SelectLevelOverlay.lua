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

local UiOverlayManager = require("Ui/Core/uiOverlayManager")
local EventsHelper = require("Ui/Core/eventsHelper")
local UiCanvas = require("Ui/Core/uiCanvas")
local UiOverlay = require("Ui/Core/uiOverlay")
local UiRectangle = require("Ui/Core/uiRectangle")
local UiScrollList = require("Ui/Core/uiScrollList")
local Styles = require("Ui/Common/styles")
local UiTextSizing = require("Ui/Common/uiTextSizing")
local UiLabel = require("Ui/Core/uiLabel")
local json = require("Ui/Core/3rdparty/json")

local FONT = "JetBrainsMono-VariableFont_wght"
local LEVEL_LABEL_FONT_SIZE = 16
local MIN_LIST_WIDTH = 300

local function ParseLevelsList(levelsListJson)
    return json.decode(levelsListJson)["level_files"]
end

SelectLevelOverlay = {}

function SelectLevelOverlay:new(host)
    local windowWidth = _GetWindowWidth(host)
    local windowHeight = _GetWindowHeight(host)
    local Combat = Styles.Combat

    local overlayCanvas = UiCanvas:new(host, 0, 0, windowWidth, windowHeight, "SelectLevelCanvas")
    overlayCanvas:subscribeOnLuaProxyReady(function(host)
        overlayCanvas:setCanvasZOrder(100)
        _InitializeCanvasInputSystem(host, overlayCanvas.luaProxyId)
        overlayCanvas:addFadeInAnimation(host, UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.3, "Opacity",
            UiBaseWidget.EnginePropertyType.Float, 0.0, 1.0)
        overlayCanvas:addFadeOutAnimation(host, UiBaseWidget.AnimationInterpolationFunctionType.LINEAR, 0.3, "Opacity",
            UiBaseWidget.EnginePropertyType.Float, 1.0, 0.0)
    end)
    local selectLevelOverlay = UiOverlay:createOverlay(host, "SelectLevelOverlay", overlayCanvas)

    -- ── Widgets ────────────────────────────────────────────────────────────────
    local scrim = UiRectangle:new(host, "PauseScrim")
    selectLevelOverlay:addWidget(scrim)

    local scrollList = UiScrollList:new(host, "SelectLevelScrollList")
    selectLevelOverlay:addWidget(scrollList)

    local levelsList = ParseLevelsList(_GetLevelsList(host))
    print("Levels list: " .. table.concat(levelsList, ", "))

    local levelLabels = {}
    local setUpLevelLabelsCount = 0
    local isLevelLabelsDirty = true

    -- labels are direct children of the scroll list (which stacks + scrolls them), so they carry NO anchors
    local function setupLevelLabel(host, levelLabel)
        levelLabel:setParent(host, overlayCanvas.widgetName, scrollList.widgetName)
        -- a label's container height must clear the font's line height, otherwise the engine renders a
        -- literal "container.height < line.height" placeholder instead of the text
        levelLabel:setHeight(UiTextSizing.safeLineHeight(LEVEL_LABEL_FONT_SIZE, FONT))
        levelLabel:setFontSize(LEVEL_LABEL_FONT_SIZE)
        levelLabel:setTextColorHexValue(Combat.cyan)
        levelLabel:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.LEFT)
        levelLabel:setTextVerticalAlignment(UiLabel.TextVerticalAlignmentType.CENTER)
    end

    -- applies levelsList to the labels; labels without a level stay hidden and are reused when the list grows
    local function refreshLevelLabels(host)
        -- a label's container width must clear the text width as well, otherwise the engine renders a
        -- literal "container.width < line.width" placeholder, so the list is as wide as the longest level name
        local listWidth = MIN_LIST_WIDTH
        for i = 1, #levelsList do
            local textWidth = UiTextSizing.measureLabelWidthPx(host, FONT, LEVEL_LABEL_FONT_SIZE, levelsList[i])
            listWidth = math.max(listWidth, math.ceil(textWidth) + 8)
        end
        listWidth = math.min(listWidth, math.floor(windowWidth * 0.8))
        scrollList:setWidth(listWidth)
        scrollList:setScrollOffset(0) -- the old offset can be out of range for a shorter list

        for i = 1, #levelLabels do
            local levelLabel = levelLabels[i]
            local levelName = levelsList[i]
            if levelName ~= nil then
                levelLabel:setWidth(listWidth)
                levelLabel:setText(levelName)
                levelLabel:setIsVisible(true)
            else
                levelLabel:setIsVisible(false)
            end
        end
    end

    local function addLevelLabel(index)
        local levelLabel = UiLabel:new(host, FONT, "SelectLevelLabel_" .. index)
        selectLevelOverlay:addWidget(levelLabel)
        levelLabels[index] = levelLabel
    end

    for i = 1, #levelsList do
        addLevelLabel(i)
    end

    selectLevelOverlay:subscribeOnAllWidgetLuaProxiesReady(function(host, sender)
        print("selectLevelOverlay: proxies are ready")
        local A = UiItemBase.UiAnchorType
        local canvasName = overlayCanvas.widgetName

        -- full-screen dimmer
        scrim:setParent(host, canvasName, canvasName)
        scrim:fill(canvasName)
        scrim:setColorHexValue(0x04060c)
        scrim:setOpacity(0.6)
        scrim:setZOrder(0)

        -- scroll list of levels
        scrollList:setParent(host, canvasName, scrim.widgetName)
        scrollList:setAnchor(A.HORIZONTAL_CENTER, A.HORIZONTAL_CENTER, scrim.widgetName)
        scrollList:setAnchor(A.VERTICAL_CENTER, A.VERTICAL_CENTER, scrim.widgetName)
        scrollList:setHeight(windowHeight * 0.5)
        scrollList:setSpacing(20)
        scrollList:setScrollSpeed(40)
        scrollList:setScrollbarSide(UiScrollList.ScrollbarSide.NONE)
        scrollList:setZOrder(1)
    end)

    -- The labels are changed only here, between updateFromReplicatorData and sendDataToReplicator. Values set in a game
    -- event handler can be overwritten by the replicator data before they are sent
    selectLevelOverlay.derivedUpdateCallback = function()
        if not isLevelLabelsDirty or not selectLevelOverlay.allWidgetLuaProxiesReady then
            return
        end
        for i = 1, #levelLabels do
            if not levelLabels[i].luaProxyReady then
                return
            end
        end

        for i = setUpLevelLabelsCount + 1, #levelLabels do
            setupLevelLabel(host, levelLabels[i])
        end
        setUpLevelLabelsCount = #levelLabels
        refreshLevelLabels(host)
        isLevelLabelsDirty = false
    end

    selectLevelOverlay.onGameEventTriggered = function(eventName, jsonArgs)
        if eventName == "LevelFilesListChanged" then
            levelsList = ParseLevelsList(jsonArgs)
            for i = #levelLabels + 1, #levelsList do
                addLevelLabel(i)
            end
            isLevelLabelsDirty = true
        end
    end
    selectLevelOverlay.onEngineEventTriggered = function(eventName, jsonArgs)
    end
    selectLevelOverlay.onBroadcastEventTriggered = function(eventName, jsonArgs)
    end

    return selectLevelOverlay
end

return SelectLevelOverlay
