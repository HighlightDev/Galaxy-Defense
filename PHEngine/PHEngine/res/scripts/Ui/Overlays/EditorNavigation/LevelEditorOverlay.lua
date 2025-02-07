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
local ImageButton = require("Ui/Widgets/ImageButton")
local LabelButton = require("Ui/Widgets/LabelButton")
local EventsHelper = require("Ui/Core/eventsHelper")
local Styles = require("Ui/Common/styles")

EditModeType = {
    IDLE = 0,
    EDIT_TOWERS = 1,
    EDIT_ROUTES = 2,
    EDIT_BARRIERS = 3
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
    local r = math.random()
    local g = math.random()
    local b = math.random()

    return {
        r = r,
        g = g,
        b = b
    }
end

RouteName = 1
BarrierName = 1

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

    local editStationSocketsButton = LabelButton:new(host, overlay, LevelEditorOverlay.labelFontName,
        "EditStationSocketsButton")
    overlay:addCompoundWidget(editStationSocketsButton)

    local editRoutesButton = LabelButton:new(host, overlay, LevelEditorOverlay.labelFontName, "EditRoutesButton");
    overlay:addCompoundWidget(editRoutesButton)

    local editBarriersButton = LabelButton:new(host, overlay, LevelEditorOverlay.labelFontName, "EditBarriersButton")
    overlay:addCompoundWidget(editBarriersButton)

    local undoLastActionButton = ImageButton:new(host, overlay, "EditStationSocketsButton")
    overlay:addCompoundWidget(undoLastActionButton)

    local saveLevelButton = ImageButton:new(host, overlay, "SaveLevelButton")
    overlay:addCompoundWidget(saveLevelButton)

    local newRouteButton = ImageButton:new(host, overlay, "NewRouteButton")
    overlay:addCompoundWidget(newRouteButton)

    local newBarrierButton = ImageButton:new(host, overlay, "NewBarrierButton")
    overlay:addCompoundWidget(newBarrierButton)

    changeContainerStateButton:subscribeOnMouseInputClickedCallback(function()
        self.editorContainerState = self.editorContainerState == EditorContainerState.Expanded and
            EditorContainerState.Hided or EditorContainerState.Expanded

        changeContainerStateButton:setImageRotationDegrees(math.fmod(
            changeContainerStateButton:getImageRotationDegrees() + 180.0, 360.0))
        local buttonsVisible = self.editorContainerState == EditorContainerState.Expanded
        editStationSocketsButton:setIsVisible(buttonsVisible)
        editRoutesButton:setIsVisible(buttonsVisible)
        editBarriersButton:setIsVisible(buttonsVisible)
        undoLastActionButton:setIsVisible(buttonsVisible)
        saveLevelButton:setIsVisible(buttonsVisible)
        newRouteButton:setIsVisible(buttonsVisible)
        newBarrierButton:setIsVisible(buttonsVisible)
    end)

    editStationSocketsButton:subscribeOnMouseInputClickedCallback(function()
        self.currentEditModeType = EditModeType.EDIT_TOWERS == self.currentEditModeType and EditModeType.IDLE or
            EditModeType.EDIT_TOWERS
        EventsHelper:sendChangeEditModeGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
            self.currentEditModeType)
    end)
    editRoutesButton:subscribeOnMouseInputClickedCallback(function()
        self.currentEditModeType = EditModeType.EDIT_ROUTES == self.currentEditModeType and EditModeType.IDLE or
            EditModeType.EDIT_ROUTES
        EventsHelper:sendChangeEditModeGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
            self.currentEditModeType)
    end)
    editBarriersButton:subscribeOnMouseInputClickedCallback(function()
        self.currentEditModeType = EditModeType.EDIT_BARRIERS == self.currentEditModeType and EditModeType.IDLE or
            EditModeType.EDIT_BARRIERS
        EventsHelper:sendChangeEditModeGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
            self.currentEditModeType)
    end)
    undoLastActionButton:subscribeOnMouseInputClickedCallback(function()
        EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
            "EditorLevelEvents", json.encode({ action = "undo" }))
    end)

    saveLevelButton:subscribeOnMouseInputClickedCallback(function()
        EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
            "EditorLevelEvents", json.encode({
                action = "save",
                name = "TestLevelName"
            }))
    end)

    newRouteButton:subscribeOnMouseInputClickedCallback(function()
        local newRouteColor = getRandomColor()
        EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
            "EditorLevelEvents", json.encode({
                action = "new_route",
                route_name = "route_" .. tostring(RouteName),
                route_color = { r = newRouteColor.r, g = newRouteColor.g, b = newRouteColor.b }
            }))
        RouteName = RouteName + 1
    end)

    newBarrierButton:subscribeOnMouseInputClickedCallback(function()
        local newBarrierColor = getRandomColor()
        EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
            "EditorLevelEvents", json.encode({
                action = "new_barrier",
                barrier_name = "barrier_" .. tostring(BarrierName),
                barrier_color = { r = newBarrierColor.r, g = newBarrierColor.g, b = newBarrierColor.b }
            }))
        BarrierName = BarrierName + 1
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

        editBarriersButton:setWidth(buttonWidth)
        editBarriersButton:setHeight(buttonHeight)

        undoLastActionButton:setWidth(buttonWidth)
        undoLastActionButton:setHeight(buttonHeight)

        saveLevelButton:setWidth(buttonWidth)
        saveLevelButton:setHeight(buttonHeight)

        newRouteButton:setWidth(buttonWidth)
        newRouteButton:setHeight(buttonHeight)

        newBarrierButton:setWidth(buttonWidth)
        newBarrierButton:setHeight(buttonHeight)
    end

    overlay:subscribeOnAllWidgetLuaProxiesReady(function()
        editorContainer:setParent(host, canvas.widgetName, canvas.widgetName)
        editorContainer:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, canvas.widgetName, 0)
        editorContainer:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, canvas.widgetName, 0)
        editorContainer:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM, canvas.widgetName, 0)
        editorContainer:setHeight(windowHeight * 0.15)
        editorContainer:setZOrder(1)
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
        changeContainerStateButton:setButtonBorderRadius(8)
        changeContainerStateButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        changeContainerStateButton:setUseImageCustomColor(true)
        changeContainerStateButton:setImageColorHexValue(0x000000)
        changeContainerStateButton:setZOrder(2)

        undoLastActionButton:setParent(host, canvas.widgetName, editorContainer.widgetName)
        undoLastActionButton:setAnchor(UiItemBase.UiAnchorType.TOP,
            UiItemBase.UiAnchorType.TOP, editorContainer.widgetName, 10)
        undoLastActionButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT,
            changeContainerStateButton.widgetName, 40)
        undoLastActionButton:setWidth(buttonWidth)
        undoLastActionButton:setHeight(buttonHeight)
        undoLastActionButton:setButtonBorderRadius(8)
        undoLastActionButton:setImageTextureSource("arrow_counter_clockwise.png")
        undoLastActionButton:setZOrder(2)
        undoLastActionButton:setButtonColorHexValue(Styles.Colors.buttonColor)

        editStationSocketsButton:setParent(host, canvas.widgetName, editorContainer.widgetName)
        editStationSocketsButton:setAnchor(UiItemBase.UiAnchorType.TOP,
            UiItemBase.UiAnchorType.TOP, editorContainer.widgetName, 10)
        editStationSocketsButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT,
            undoLastActionButton.widgetName, 10)
        editStationSocketsButton:setWidth(buttonWidth)
        editStationSocketsButton:setHeight(buttonHeight)
        editStationSocketsButton:setButtonBorderRadius(8)
        editStationSocketsButton:setLabelText("Edit stations")
        editStationSocketsButton:setLabelTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)
        editStationSocketsButton:setLabelTextColorHexValue(0x000000)
        editStationSocketsButton:setZOrder(2)
        editStationSocketsButton:setButtonColorHexValue(Styles.Colors.buttonColor)

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
        editRoutesButton:setButtonColorHexValue(Styles.Colors.buttonColor)

        editBarriersButton:setParent(host, canvas.widgetName, editorContainer.widgetName)
        editBarriersButton:setAnchor(UiItemBase.UiAnchorType.TOP,
            UiItemBase.UiAnchorType.TOP, editorContainer.widgetName, 10)
        editBarriersButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT,
            editRoutesButton.widgetName, 10)
        editBarriersButton:setWidth(buttonWidth)
        editBarriersButton:setHeight(buttonHeight)
        editBarriersButton:setButtonBorderRadius(8)
        editBarriersButton:setLabelText("Edit barriers")
        editBarriersButton:setLabelTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)
        editBarriersButton:setLabelTextColorHexValue(0x000000)
        editBarriersButton:setZOrder(2)
        editBarriersButton:setButtonColorHexValue(Styles.Colors.buttonColor)

        newRouteButton:setParent(host, canvas.widgetName, editorContainer.widgetName)
        newRouteButton:setAnchor(UiItemBase.UiAnchorType.TOP,
            UiItemBase.UiAnchorType.TOP, editorContainer.widgetName, 10)
        newRouteButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT,
            editBarriersButton.widgetName, 40)
        newRouteButton:setWidth(buttonWidth)
        newRouteButton:setHeight(buttonHeight)
        newRouteButton:setButtonBorderRadius(8)
        newRouteButton:setImageTextureSource("route.png")
        newRouteButton:setZOrder(2)
        newRouteButton:setButtonColorHexValue(Styles.Colors.buttonColor)

        newBarrierButton:setParent(host, canvas.widgetName, editorContainer.widgetName)
        newBarrierButton:setAnchor(UiItemBase.UiAnchorType.TOP,
            UiItemBase.UiAnchorType.TOP, editorContainer.widgetName, 10)
        newBarrierButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT,
            newRouteButton.widgetName, 10)
        newBarrierButton:setWidth(buttonWidth)
        newBarrierButton:setHeight(buttonHeight)
        newBarrierButton:setButtonBorderRadius(8)
        newBarrierButton:setImageTextureSource("wall.png")
        newBarrierButton:setZOrder(2)
        newBarrierButton:setButtonColorHexValue(Styles.Colors.buttonColor)

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
        saveLevelButton:setButtonColorHexValue(Styles.Colors.buttonColor)
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
