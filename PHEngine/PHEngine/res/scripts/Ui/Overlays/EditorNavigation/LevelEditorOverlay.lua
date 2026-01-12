--[[ BEGIN *** this snippet has to be inserted everywhere where your want to require custom modules *** BEGIN]] --
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
local UiSlider = require("Ui/Core/uiSlider")
local UiLabel = require("Ui/Core/uiLabel")
local ImageButton = require("Ui/Widgets/ImageButton")
local LabelButton = require("Ui/Widgets/LabelButton")
local EventsHelper = require("Ui/Core/eventsHelper")
local Styles = require("Ui/Common/styles")

EditModeType = {IDLE = 0, EDIT_TOWERS = 1, EDIT_ROUTES = 2, EDIT_BARRIERS = 3}

local EditorContainerState = {Expanded = 0, Hided = 1}

LevelEditorOverlay = {
    overlayName = "LevelEditorOverlay",
    editorContainerState = EditorContainerState.Expanded,
    currentEditModeType = EditModeType.IDLE,
    labelFontName = "Lora-VariableFont_wght"
}

local function getRandomColor()
    local r = math.random()
    local g = math.random()
    local b = math.random()

    return {r = r, g = g, b = b}
end

RouteName = 1
BarrierName = 1

function LevelEditorOverlay:new(host)
    local windowWidth = _GetWindowWidth(host)
    local windowHeight = _GetWindowHeight(host)

    local canvas = UiCanvas:new(host, 0, 0, windowWidth, windowHeight, "EditorCanvas")
    canvas:subscribeOnLuaProxyReady(function(host) _InitializeCanvasInputSystem(host, canvas.luaProxyId) end)
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

    local levelWidthLabel = UiLabel:new(host, "Lora-VariableFont_wght", "LevelWidthLabel")
    overlay:addWidget(levelWidthLabel)

    local levelWidthSlider = UiSlider:new(host, "LevelWidthSlider")
    overlay:addWidget(levelWidthSlider)

    local levelHeightLabel = UiLabel:new(host, "Lora-VariableFont_wght", "LevelHeightLabel")
    overlay:addWidget(levelHeightLabel)

    local levelLengthSlider = UiSlider:new(host, "LevelHeightSlider")
    overlay:addWidget(levelLengthSlider)

    changeContainerStateButton:subscribeOnMouseInputClickedCallback(function()
        self.editorContainerState = self.editorContainerState == EditorContainerState.Expanded and
                                        EditorContainerState.Hided or EditorContainerState.Expanded

        changeContainerStateButton:setImageRotationDegrees(math.fmod(
                                                               changeContainerStateButton:getImageRotationDegrees() +
                                                                   180.0, 360.0))
        local buttonsVisible = self.editorContainerState == EditorContainerState.Expanded
        editStationSocketsButton:setIsVisible(buttonsVisible)
        editRoutesButton:setIsVisible(buttonsVisible)
        editBarriersButton:setIsVisible(buttonsVisible)
        undoLastActionButton:setIsVisible(buttonsVisible)
        saveLevelButton:setIsVisible(buttonsVisible)
        newRouteButton:setIsVisible(buttonsVisible)
        newBarrierButton:setIsVisible(buttonsVisible)
    end)

    changeContainerStateButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            changeContainerStateButton:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
        else
            changeContainerStateButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        end
    end)

    editStationSocketsButton:subscribeOnMouseInputClickedCallback(function()
        self.currentEditModeType = EditModeType.EDIT_TOWERS == self.currentEditModeType and EditModeType.IDLE or
                                       EditModeType.EDIT_TOWERS
        EventsHelper:sendChangeEditModeGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
                                                       self.currentEditModeType)
    end)

    editStationSocketsButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            editStationSocketsButton:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
        else
            editStationSocketsButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        end
    end)

    editRoutesButton:subscribeOnMouseInputClickedCallback(function()
        self.currentEditModeType = EditModeType.EDIT_ROUTES == self.currentEditModeType and EditModeType.IDLE or
                                       EditModeType.EDIT_ROUTES
        EventsHelper:sendChangeEditModeGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
                                                       self.currentEditModeType)
    end)
    editRoutesButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            editRoutesButton:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
        else
            editRoutesButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        end
    end)

    editBarriersButton:subscribeOnMouseInputClickedCallback(function()
        self.currentEditModeType = EditModeType.EDIT_BARRIERS == self.currentEditModeType and EditModeType.IDLE or
                                       EditModeType.EDIT_BARRIERS
        EventsHelper:sendChangeEditModeGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
                                                       self.currentEditModeType)
    end)
    editBarriersButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            editBarriersButton:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
        else
            editBarriersButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        end
    end)

    undoLastActionButton:subscribeOnMouseInputClickedCallback(function()
        EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
                                                  "EditorLevelEvents", json.encode({action = "undo"}))
    end)

    saveLevelButton:subscribeOnMouseInputClickedCallback(function()
        EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
                                                  "EditorLevelEvents",
                                                  json.encode({action = "save", name = "TestLevelName"}))
    end)
    saveLevelButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            saveLevelButton:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
        else
            saveLevelButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        end
    end)

    newRouteButton:subscribeOnMouseInputClickedCallback(function()
        local newRouteColor = getRandomColor()
        EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
                                                  "EditorLevelEvents", json.encode({
            action = "new_route",
            route_name = "route_" .. tostring(RouteName),
            route_color = {r = newRouteColor.r, g = newRouteColor.g, b = newRouteColor.b}
        }))
        RouteName = RouteName + 1
    end)
    newRouteButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            newRouteButton:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
        else
            newRouteButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        end
    end)

    newBarrierButton:subscribeOnMouseInputClickedCallback(function()
        local newBarrierColor = getRandomColor()
        EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
                                                  "EditorLevelEvents", json.encode({
            action = "new_barrier",
            barrier_name = "barrier_" .. tostring(BarrierName),
            barrier_color = {r = newBarrierColor.r, g = newBarrierColor.g, b = newBarrierColor.b}
        }))
        BarrierName = BarrierName + 1
    end)
    newBarrierButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            newBarrierButton:setButtonColorHexValue(Styles.Colors.hoveredButtonColor)
        else
            newBarrierButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        end
    end)

    overlay.onWindowSizeChanged = function(width, height)
        assert(width ~= nil and type(width) == "number" and height ~= nil and type(height) == "number")

        editorContainer:setHeight(height * 0.25)

        local buttonWidth = editorContainer:getHeight() * 0.35
        local buttonHeight = editorContainer:getHeight() * 0.35

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
        editorContainer:setHeight(windowHeight * 0.35)
        editorContainer:setZOrder(1)
        editorContainer:setOpacity(0.0)
        editorContainer:setColorHexValue(0x000000)

        levelWidthLabel:setParent(host, canvas.widgetName, editorContainer.widgetName)
        levelWidthLabel:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
                                  editorContainer.widgetName, 10)
        levelWidthLabel:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, editorContainer.widgetName,
                                  10)
        levelWidthLabel:setHeight(30.0)
        levelWidthLabel:setWidth(windowWidth * 0.25)
        levelWidthLabel:setText("Level width")
        levelWidthLabel:setTextColorHexValue(0xFFFFFF)
        levelWidthLabel:setFontSize(15)
        levelWidthLabel:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.LEFT)
        levelWidthLabel:setZOrder(2)

        levelWidthSlider:setParent(host, canvas.widgetName, editorContainer.widgetName)
        levelWidthSlider:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
                                   editorContainer.widgetName, 10)
        levelWidthSlider:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM,
                                   levelWidthLabel.widgetName, 10.0)
        levelWidthSlider:setHeight(30.0)
        levelWidthSlider:setWidth(windowWidth * 0.25)
        levelWidthSlider:setZOrder(2)
        levelWidthSlider:setSliderValue(_GetEditorLevelAreaBoundingBoxWidth(host))
        levelWidthSlider:setMaxSliderValue(200.0)
        levelWidthSlider:setMinSliderValue(40.0)
        levelWidthSlider:setSliderStep(20)
        levelWidthSlider:setSliderThicknessPixels(10.0)
        levelWidthSlider:setBlobThicknessPixels(30.0)
        levelWidthSlider:setSliderType(UiSlider.UiSliderType.SLIDER_TYPE_HORIZONTAL)
        levelWidthSlider:setBlobColorHexValue(Styles.Colors.hoveredButtonColor)
        levelWidthSlider:setSliderColorHexValue(Styles.Colors.notActiveButtonColor)
        levelWidthSlider:enableSliderMouseInputReceiver(host)
        levelWidthSlider:subscribeOnSliderValueChangedCallback(function(newValue)
            EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
                                                      "EditorLevelEvents",
                                                      json.encode({action = "set_level_width", width = newValue}))
        end)

        levelHeightLabel:setParent(host, canvas.widgetName, editorContainer.widgetName)
        levelHeightLabel:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
                                   editorContainer.widgetName, 10)
        levelHeightLabel:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM,
                                   levelWidthSlider.widgetName, 20)
        levelHeightLabel:setHeight(30.0)
        levelHeightLabel:setWidth(windowWidth * 0.25)
        levelHeightLabel:setText("Level length")
        levelHeightLabel:setTextColorHexValue(0xFFFFFF)
        levelHeightLabel:setFontSize(15)
        levelHeightLabel:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.LEFT)
        levelHeightLabel:setZOrder(2)

        levelLengthSlider:setParent(host, canvas.widgetName, editorContainer.widgetName)
        levelLengthSlider:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
                                    editorContainer.widgetName, 10)
        levelLengthSlider:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.BOTTOM,
                                    levelHeightLabel.widgetName, 10.0)
        levelLengthSlider:setHeight(30.0)
        levelLengthSlider:setWidth(windowWidth * 0.25)
        levelLengthSlider:setZOrder(2)
        levelLengthSlider:setSliderValue(_GetEditorLevelAreaBoundingBoxLength(host))
        levelLengthSlider:setMaxSliderValue(200.0)
        levelLengthSlider:setMinSliderValue(40.0)
        levelLengthSlider:setSliderStep(20)
        levelLengthSlider:setSliderThicknessPixels(10.0)
        levelLengthSlider:setBlobThicknessPixels(30.0)
        levelLengthSlider:setSliderType(UiSlider.UiSliderType.SLIDER_TYPE_HORIZONTAL)
        levelLengthSlider:setBlobColorHexValue(Styles.Colors.hoveredButtonColor)
        levelLengthSlider:setSliderColorHexValue(Styles.Colors.notActiveButtonColor)
        levelLengthSlider:enableSliderMouseInputReceiver(host)
        levelLengthSlider:subscribeOnSliderValueChangedCallback(function(newValue)
            EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
                                                      "EditorLevelEvents",
                                                      json.encode({action = "set_level_length", length = newValue}))
        end)

        local buttonWidth = editorContainer:getHeight() * 0.3
        local buttonHeight = editorContainer:getHeight() * 0.3

        changeContainerStateButton:setParent(host, canvas.widgetName, editorContainer.widgetName)
        changeContainerStateButton:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
                                             editorContainer.widgetName, 10)
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
        undoLastActionButton:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
                                       editorContainer.widgetName, 10)
        undoLastActionButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT,
                                       changeContainerStateButton.widgetName, 40)
        undoLastActionButton:setWidth(buttonWidth)
        undoLastActionButton:setHeight(buttonHeight)
        undoLastActionButton:setButtonBorderRadius(8)
        undoLastActionButton:setImageTextureSource("arrow_counter_clockwise.png")
        undoLastActionButton:setZOrder(2)
        undoLastActionButton:setButtonColorHexValue(Styles.Colors.buttonColor)

        editStationSocketsButton:setParent(host, canvas.widgetName, editorContainer.widgetName)
        editStationSocketsButton:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
                                           editorContainer.widgetName, 10)
        editStationSocketsButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT,
                                           undoLastActionButton.widgetName, 10)
        editStationSocketsButton:setWidth(buttonWidth)
        editStationSocketsButton:setHeight(buttonHeight)
        editStationSocketsButton:setButtonBorderRadius(8)
        editStationSocketsButton:setLabelText("Edit stations")
        editStationSocketsButton:setLabelTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)
        editStationSocketsButton:setLabelTextVerticalAlignment(UiLabel.TextVerticalAlignmentType.CENTER)
        editStationSocketsButton:setLabelTextColorHexValue(0x000000)
        editStationSocketsButton:setZOrder(2)
        editStationSocketsButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        editStationSocketsButton:setLabelFontSize(13)

        editRoutesButton:setParent(host, canvas.widgetName, editorContainer.widgetName)
        editRoutesButton:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
                                   editorContainer.widgetName, 10)
        editRoutesButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT,
                                   editStationSocketsButton.widgetName, 10)
        editRoutesButton:setWidth(buttonWidth)
        editRoutesButton:setHeight(buttonHeight)
        editRoutesButton:setButtonBorderRadius(8)
        editRoutesButton:setLabelText("Edit routes")
        editRoutesButton:setLabelTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)
        editRoutesButton:setLabelTextVerticalAlignment(UiLabel.TextVerticalAlignmentType.CENTER)
        editRoutesButton:setLabelTextColorHexValue(0x000000)
        editRoutesButton:setZOrder(2)
        editRoutesButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        editRoutesButton:setLabelFontSize(13)

        editBarriersButton:setParent(host, canvas.widgetName, editorContainer.widgetName)
        editBarriersButton:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
                                     editorContainer.widgetName, 10)
        editBarriersButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT,
                                     editRoutesButton.widgetName, 10)
        editBarriersButton:setWidth(buttonWidth)
        editBarriersButton:setHeight(buttonHeight)
        editBarriersButton:setButtonBorderRadius(8)
        editBarriersButton:setLabelText("Edit barriers")
        editBarriersButton:setLabelTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)
        editBarriersButton:setLabelTextVerticalAlignment(UiLabel.TextVerticalAlignmentType.CENTER)
        editBarriersButton:setLabelTextColorHexValue(0x000000)
        editBarriersButton:setZOrder(2)
        editBarriersButton:setButtonColorHexValue(Styles.Colors.buttonColor)
        editBarriersButton:setLabelFontSize(13)

        newRouteButton:setParent(host, canvas.widgetName, editorContainer.widgetName)
        newRouteButton:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
                                 editorContainer.widgetName, 10)
        newRouteButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT,
                                 editBarriersButton.widgetName, 40)
        newRouteButton:setWidth(buttonWidth)
        newRouteButton:setHeight(buttonHeight)
        newRouteButton:setButtonBorderRadius(8)
        newRouteButton:setImageTextureSource("route.png")
        newRouteButton:setZOrder(2)
        newRouteButton:setButtonColorHexValue(Styles.Colors.buttonColor)

        newBarrierButton:setParent(host, canvas.widgetName, editorContainer.widgetName)
        newBarrierButton:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
                                   editorContainer.widgetName, 10)
        newBarrierButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.RIGHT,
                                   newRouteButton.widgetName, 10)
        newBarrierButton:setWidth(buttonWidth)
        newBarrierButton:setHeight(buttonHeight)
        newBarrierButton:setButtonBorderRadius(8)
        newBarrierButton:setImageTextureSource("wall.png")
        newBarrierButton:setZOrder(2)
        newBarrierButton:setButtonColorHexValue(Styles.Colors.buttonColor)

        saveLevelButton:setParent(host, canvas.widgetName, editorContainer.widgetName)
        saveLevelButton:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM,
                                  editorContainer.widgetName, 10)
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
        if "EditLevelAreaBoundingBoxChanged" == eventName and overlay.allWidgetLuaProxiesReady == true then
            assert(jsonArgs ~= nil and type(jsonArgs) == "string")
            local parsedJson = json.decode(jsonArgs);
            if parsedJson["width"] ~= nil then levelWidthSlider:setSliderValue(tonumber(parsedJson["width"])) end
            if parsedJson["length"] ~= nil then
                levelLengthSlider:setSliderValue(tonumber(parsedJson["length"]))
            end
        end
    end

    overlay.onEngineEventTriggered = function(eventName, jsonArgs)
        if "WindowSizeChanged" == eventName and overlay.allWidgetLuaProxiesReady == true then
            assert(jsonArgs ~= nil and type(jsonArgs) == "string")
            local parsedJson = json.decode(jsonArgs)
            local windowSize = {}
            if parsedJson["width"] ~= nil then windowSize.width = tonumber(parsedJson["width"]) end
            if parsedJson["height"] ~= nil then windowSize.height = tonumber(parsedJson["height"]) end
            overlay.onWindowSizeChanged(windowSize.width, windowSize.height)
        end
    end

    overlay.onBroadcastEventTriggered = function(eventName, jsonArgs)
        -- Currently no implementation is needed here
    end

    return overlay
end

return LevelEditorOverlay
