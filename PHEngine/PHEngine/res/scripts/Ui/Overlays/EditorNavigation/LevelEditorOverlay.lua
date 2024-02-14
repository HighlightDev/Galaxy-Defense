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

function LevelEditorOverlay:new(host)
    local windowWidth = _GetWindowWidth(host)
    local windowHeight = _GetWindowHeight(host)

    local canvas = UiCanvas:new(host, 0, 0, windowWidth, windowHeight)
    canvas:subscribeOnLuaProxyReady(function(host)
        _InitializeCanvasInputSystem(host, canvas.luaProxyId)
    end)
    local overlay = UiOverlay:createBackgroundOverlay(host, LevelEditorOverlay.overlayName, canvas)

    local editorContainer = UiRectangle:new(host)
    overlay:addWidget(editorContainer)

    local changeContainerStateButton = ImageButton:new(host, overlay)
    overlay:addCompoundWidget(changeContainerStateButton)

    local editStationSocketsButton = LabelButton:new(host, overlay, LevelEditorOverlay.labelFontName)
    overlay:addCompoundWidget(editStationSocketsButton)

    local editRoutesButton = LabelButton:new(host, overlay, LevelEditorOverlay.labelFontName);
    overlay:addCompoundWidget(editRoutesButton)

    changeContainerStateButton:setOnMouseInputClickedCallback(function()
        self.editorContainerState = self.editorContainerState == EditorContainerState.Expanded and
            EditorContainerState.Hided or EditorContainerState.Expanded

        changeContainerStateButton:setImageRotationDegrees(math.fmod(
            changeContainerStateButton:getImageRotationDegrees() + 180.0, 360.0))
        editStationSocketsButton:setIsVisible(self.editorContainerState == EditorContainerState.Expanded)
        editRoutesButton:setIsVisible(self.editorContainerState == EditorContainerState.Expanded)
    end)

    editStationSocketsButton:setOnMouseInputClickedCallback(function()
        self.currentEditModeType = EditModeType.EDIT_TOWERS == self.currentEditModeType and EditModeType.IDLE or
            EditModeType.EDIT_TOWERS
        EventsHelper:sendChangeEditModeGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
            self.currentEditModeType)
    end)
    editRoutesButton:setOnMouseInputClickedCallback(function()
        self.currentEditModeType = EditModeType.EDIT_ROUTES == self.currentEditModeType and EditModeType.IDLE or
            EditModeType.EDIT_ROUTES
        EventsHelper:sendChangeEditModeGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
            self.currentEditModeType)
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
