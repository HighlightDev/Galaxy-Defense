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
local json = require("Ui/Core/3rdparty/json")
local UiCanvas = require("Ui/Core/uiCanvas")
local UiOverlay = require("Ui/Core/uiOverlay")
local UiItem = require("Ui/Core/uiItem")
local UiImage = require("Ui/Core/uiImage")
local ImageButton = require("Ui/Widgets/ImageButton")
local LabelButton = require("Ui/Widgets/LabelButton")
local EventsHelper = require("Ui/Core/eventsHelper")

EditModeType = {
    IDLE = 0,
    EDIT_TOWERS = 1,
    EDIT_ROUTES = 2
}

local EditorContainerState = {
    Expanded = 0,
    Hided = 1
}

LevelEditorOverlay = {
    overlayName = "LevelEditorOverlay",
    editorContainerState = EditorContainerState.Expanded,
    currentEditModeType = EditModeType.IDLE,
    labelFontName = "nimbus_mono",
}

local function getRandomColor()
    local r_byte = math.random(0, 255)
    local g_byte = math.random(0, 255)
    local b_byte = math.random(0, 255)

    local INV_COLOR_MAX_BYTE_VALUE = 1.0 / 255.0;
    return {
        r = r_byte * INV_COLOR_MAX_BYTE_VALUE,
        g = g_byte * INV_COLOR_MAX_BYTE_VALUE,
        b = b_byte * INV_COLOR_MAX_BYTE_VALUE
    }
end

RouteName = 1
TowerName = 1

function LevelEditorOverlay:new(host)
    local windowWidth = _GetWindowWidth(host)
    local windowHeight = _GetWindowHeight(host)

    local canvas = UiCanvas:new(host, 0, 0, windowWidth, windowHeight, "EditorCanvas")
    canvas:subscribeOnLuaProxyReady(function(host)
        _InitializeCanvasInputSystem(host, canvas.luaProxyId)
    end)
    local overlay = UiOverlay:createBackgroundOverlay(host, LevelEditorOverlay.overlayName, canvas)

    local editorContainer = UiRectangle:new(host, "EditorPanelContainer")
    overlay:addWidget(editorContainer)

    local changeContainerStateButton = ImageButton:new(host, overlay, "ChangeContainerStateButton")
    overlay:addCompoundWidget(changeContainerStateButton)

    local editStationSocketsButton = LabelButton:new(host, overlay, LevelEditorOverlay.labelFontName)
    overlay:addCompoundWidget(editStationSocketsButton)

    local editRoutesButton = LabelButton:new(host, overlay, LevelEditorOverlay.labelFontName);
    overlay:addCompoundWidget(editRoutesButton)

    local undoLastActionButton = ImageButton:new(host, overlay, "EditStationSocketsButton")
    overlay:addCompoundWidget(undoLastActionButton)

    local saveLevelButton = ImageButton:new(host, overlay, "SaveLevelButton")
    overlay:addCompoundWidget(saveLevelButton)

    local newRouteButton = ImageButton:new(host, overlay, "NewRouteButton")
    overlay:addCompoundWidget(newRouteButton)

    local newTowerButton = ImageButton:new(host, overlay, "NewTowerButton")
    overlay:addCompoundWidget(newTowerButton)

    changeContainerStateButton:subscriveOnMouseInputClickedCallback(function()
        self.editorContainerState = self.editorContainerState == EditorContainerState.Expanded and
            EditorContainerState.Hided or EditorContainerState.Expanded

        changeContainerStateButton:setImageRotationDegrees(math.fmod(
            changeContainerStateButton:getImageRotationDegrees() + 180.0, 360.0))
        local buttonsVisible = self.editorContainerState == EditorContainerState.Expanded
        editStationSocketsButton:setIsVisible(buttonsVisible)
        editRoutesButton:setIsVisible(buttonsVisible)
        undoLastActionButton:setIsVisible(buttonsVisible)
        saveLevelButton:setIsVisible(buttonsVisible)
        newRouteButton:setIsVisible(buttonsVisible)
        newTowerButton:setIsVisible(buttonsVisible)
    end)

    editStationSocketsButton:subscriveOnMouseInputClickedCallback(function()
        self.currentEditModeType = EditModeType.EDIT_TOWERS == self.currentEditModeType and EditModeType.IDLE or
            EditModeType.EDIT_TOWERS
        EventsHelper:sendChangeEditModeGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
            self.currentEditModeType)
    end)
    editRoutesButton:subscriveOnMouseInputClickedCallback(function()
        self.currentEditModeType = EditModeType.EDIT_ROUTES == self.currentEditModeType and EditModeType.IDLE or
            EditModeType.EDIT_ROUTES
        EventsHelper:sendChangeEditModeGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
            self.currentEditModeType)
    end)
    undoLastActionButton:subscriveOnMouseInputClickedCallback(function()
        EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
            "EditorLevelEvents", json.encode({ action = "undo" }))
    end)

    saveLevelButton:subscriveOnMouseInputClickedCallback(function()
        EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
            "EditorLevelEvents", json.encode({
                action = "save",
                name = "TestLevelName"
            }))
    end)

    newRouteButton:subscriveOnMouseInputClickedCallback(function()
        local newRouteColor = getRandomColor()
        EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
            "EditorLevelEvents", json.encode({
                action = "new_route",
                route_name = "route_" .. tostring(RouteName),
                route_color = { r = newRouteColor.r, g = newRouteColor.g, b = newRouteColor.b }
            }))
        RouteName = RouteName + 1
    end)

    newTowerButton:subscriveOnMouseInputClickedCallback(function()
        local newTowerColor = getRandomColor()
        EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
            "EditorLevelEvents", json.encode({
                action = "new_tower",
                tower_name = "tower_" .. tostring(TowerName),
                tower_color = { r = newTowerColor.r, g = newTowerColor.g, b = newTowerColor.b }
            }))
    end)

    overlay.onWindowSizeChanged = function(width, height)
        assert(width ~= nil and type(width) == "number" and height ~= nil and type(height) == "number")

        editorContainer:setHeight(height * 0.15)

        local buttonWidth = editorContainer:getHeight() * 0.55
        local buttonHeight = editorContainer:getHeight() * 0.55

        changeContainerStateButton:setWidth(buttonWidth)
        changeContainerStateButton:setHeight(buttonHeight)

        editStationSocketsButton:setWidth(buttonWidth)
        editStationSocketsButton:setHeight(buttonHeight)

        editRoutesButton:setWidth(buttonWidth)
        editRoutesButton:setHeight(buttonHeight)

        undoLastActionButton:setWidth(buttonWidth)
        undoLastActionButton:setHeight(buttonHeight)

        saveLevelButton:setWidth(buttonWidth)
        saveLevelButton:setHeight(buttonHeight)

        newRouteButton:setWidth(buttonWidth)
        newRouteButton:setHeight(buttonHeight)

        newTowerButton.setWidth(buttonWidth)
        newTowerButton.setHeight(buttonHeight)
    end

    overlay:subscribeOnAllWidgetLuaProxiesReady(function()
        editorContainer:setParent(host, canvas.widgetName, canvas.widgetName)
        editorContainer:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, canvas.widgetName, 0)
        editorContainer:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, canvas.widgetName, 0)
        editorContainer:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM, canvas.widgetName, 0)
        editorContainer:setHeight(windowHeight * 0.15)
        editorContainer:setZOrder(1)
        editorContainer:setColorHexValue(0xffffff)
        editorContainer:setOpacity(0.0)
        editorContainer:setIfCanInterceptMouseInputEvent(false)

        local buttonWidth = editorContainer:getHeight() * 0.55
        local buttonHeight = editorContainer:getHeight() * 0.55

        changeContainerStateButton:setParent(host, canvas.widgetName, editorContainer.widgetName)
        changeContainerStateButton:setAnchor(UiItemBase.UiAnchorType.TOP,
            UiItemBase.UiAnchorType.TOP, editorContainer.widgetName, 10)
        changeContainerStateButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
            editorContainer.widgetName, 10)
        changeContainerStateButton:setWidth(buttonWidth)
        changeContainerStateButton:setHeight(buttonHeight)
        changeContainerStateButton:setImageTextureSource("arrow_left.png")
        changeContainerStateButton:setImageRotationDegrees(180.0)
        changeContainerStateButton:setButtonBorderRadius(8)
        changeContainerStateButton:setButtonColorHexValue(0xdb9427)
        changeContainerStateButton:setUseImageCustomColor(true)
        changeContainerStateButton:setImageColorHexValue(0x000000)
        changeContainerStateButton:setZOrder(2)

        editStationSocketsButton:setParent(host, canvas.widgetName, editorContainer.widgetName)
        editStationSocketsButton:setAnchor(UiItemBase.UiAnchorType.TOP,
            UiItemBase.UiAnchorType.TOP, editorContainer.widgetName, 10)
        editStationSocketsButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT,
            changeContainerStateButton.widgetName, 10)
        editStationSocketsButton:setWidth(buttonWidth)
        editStationSocketsButton:setHeight(buttonHeight)
        editStationSocketsButton:setButtonBorderRadius(8)
        editStationSocketsButton:setLabelText("Edit stations")
        editStationSocketsButton:setLabelTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)
        editStationSocketsButton:setLabelTextColorHexValue(0x000000)
        editStationSocketsButton:setZOrder(2)
        editStationSocketsButton:setButtonColorHexValue(0xdb9427)

        editRoutesButton:setParent(host, canvas.widgetName, editorContainer.widgetName)
        editRoutesButton:setAnchor(UiItemBase.UiAnchorType.TOP,
            UiItemBase.UiAnchorType.TOP, editorContainer.widgetName, 10)
        editRoutesButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT,
            editStationSocketsButton.widgetName, 10)
        editRoutesButton:setWidth(buttonWidth)
        editRoutesButton:setHeight(buttonHeight)
        editRoutesButton:setButtonBorderRadius(8)
        editRoutesButton:setLabelText("Edit routes")
        editRoutesButton:setLabelTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)
        editRoutesButton:setLabelTextColorHexValue(0x000000)
        editRoutesButton:setZOrder(2)
        editRoutesButton:setButtonColorHexValue(0xdb9427)

        undoLastActionButton:setParent(host, canvas.widgetName, editorContainer.widgetName)
        undoLastActionButton:setAnchor(UiItemBase.UiAnchorType.TOP,
            UiItemBase.UiAnchorType.TOP, editorContainer.widgetName, 10)
        undoLastActionButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT,
            editRoutesButton.widgetName, 10)
        undoLastActionButton:setWidth(buttonWidth)
        undoLastActionButton:setHeight(buttonHeight)
        undoLastActionButton:setButtonBorderRadius(8)
        undoLastActionButton:setImageTextureSource("arrow_counter_clockwise.png")
        undoLastActionButton:setZOrder(2)
        undoLastActionButton:setButtonColorHexValue(0xdb9427)

        newRouteButton:setParent(host, canvas.widgetName, editorContainer.widgetName)
        newRouteButton:setAnchor(UiItemBase.UiAnchorType.TOP,
            UiItemBase.UiAnchorType.TOP, editorContainer.widgetName, 10)
        newRouteButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT,
            undoLastActionButton.widgetName, 10)
        newRouteButton:setWidth(buttonWidth)
        newRouteButton:setHeight(buttonHeight)
        newRouteButton:setButtonBorderRadius(8)
        newRouteButton:setImageTextureSource("route.png")
        newRouteButton:setZOrder(2)
        newRouteButton:setButtonColorHexValue(0xdb9427)

        newTowerButton:setParent(host, canvas.widgetName, editorContainer.widgetName)
        newTowerButton:setAnchor(UiItemBase.UiAnchorType.TOP,
            UiItemBase.UiAnchorType.TOP, editorContainer.widgetName, 10)
        newTowerButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT,
            newRouteButton.widgetName, 10)
        newTowerButton:setWidth(buttonWidth)
        newTowerButton:setHeight(buttonHeight)
        newTowerButton:setButtonBorderRadius(8)
        newTowerButton:setImageTextureSource("tower.png")
        newTowerButton:setZOrder(2)
        newTowerButton:setButtonColorHexValue(0xdb9427)
        newTowerButton:setImageRotationDegrees(180)

        saveLevelButton:setParent(host, canvas.widgetName, editorContainer.widgetName)
        saveLevelButton:setAnchor(UiItemBase.UiAnchorType.TOP,
            UiItemBase.UiAnchorType.TOP, editorContainer.widgetName, 10)
        saveLevelButton:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
            editorContainer.widgetName, 10)
        saveLevelButton:setWidth(buttonWidth)
        saveLevelButton:setHeight(buttonHeight)
        saveLevelButton:setButtonBorderRadius(8)
        saveLevelButton:setImageTextureSource("floppy-disk.png")
        saveLevelButton:setZOrder(2)
        saveLevelButton:setButtonColorHexValue(0xdb9427)
    end)

    overlay.onGameEventTriggered = function(eventName, jsonArgs)
    end

    overlay.onEngineEventTriggered = function(eventName, jsonArgs)
        if "WindowSizeChanged" == eventName and overlay.allWidgetLuaProxiesReady == true then
            assert(jsonArgs ~= nil and type(jsonArgs) == "string")
            local parsedJson = json.decode(jsonArgs)
            local windowSize = {}
            if parsedJson["width"] ~= nil then
                windowSize.width = tonumber(parsedJson["width"])
            end
            if parsedJson["height"] ~= nil then
                windowSize.height = tonumber(parsedJson["height"])
            end
            overlay.onWindowSizeChanged(windowSize.width, windowSize.height)
        end
    end

    return overlay
end

return LevelEditorOverlay
