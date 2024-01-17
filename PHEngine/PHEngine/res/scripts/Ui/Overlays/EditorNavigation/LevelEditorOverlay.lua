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
local UiImage = require("Ui/Core/uiImage")
local ImageButton = require("Ui/Widgets/ImageButton")

local EditorContainerState = {
    Expanded = 0,
    Shrinked = 1
}

LevelEditorOverlay = {
    editorContainerState = EditorContainerState.Expanded
}

local function getEditorContainerWidth(self, windowWidth)
    assert(self ~= nil and type(self) == "table" and windowWidth ~= nil and type(windowWidth) == "number")
    return self.editorContainerState == EditorContainerState.Expanded and windowWidth * 0.3 or windowWidth * 0.05;
end

function LevelEditorOverlay:new(host)
    local windowWidth = _GetWindowWidth(host)
    local windowHeight = _GetWindowHeight(host)

    local canvas = UiCanvas:new(host, 0, 0, windowWidth, windowHeight)
    canvas:subscribeOnLuaProxyReady(function(host)
        _InitializeCanvasInputSystem(host, canvas.luaProxyId)
    end)
    local overlay = UiOverlay:createBackgroundOverlay(host, "LevelEditorOverlay", canvas)

    local editorContainer = UiRectangle:new(host)
    overlay:addWidget(editorContainer)

    local changeContainerStateButton = ImageButton:new(host, overlay)
    overlay:addCompoundWidget(changeContainerStateButton)

    changeContainerStateButton:setOnMouseInputClickedCallback(function()
        self.editorContainerState = self.editorContainerState == EditorContainerState.Expanded and
            EditorContainerState.Shrinked or EditorContainerState.Expanded
        local newContainerWidth = getEditorContainerWidth(self, _GetWindowWidth(host))
        editorContainer:setWidth(newContainerWidth)

        changeContainerStateButton:setWidth(newContainerWidth * 0.25)
        changeContainerStateButton:setHeight(newContainerWidth * 0.15)
        changeContainerStateButton:setImageRotationDegrees(math.fmod(
            changeContainerStateButton:getImageRotationDegrees() + 180.0, 360.0))
    end)

    overlay.onWindowSizeChanged = function(width, height)
        assert(width ~= nil and type(width) == "number" and height ~= nil and type(height) == "number")

        editorContainer:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, canvas.widgetName, 0)
        editorContainer:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, canvas.widgetName, 0)
        editorContainer:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM, canvas.widgetName, 0)
        editorContainer:setWidth(getEditorContainerWidth(self, width))

        changeContainerStateButton:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER,
            UiItemBase.UiAnchorType.VERTICAL_CENTER, editorContainer.widgetName, 0)
        changeContainerStateButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
            editorContainer.widgetName, 5)
        changeContainerStateButton:setWidth(editorContainer:getWidth() * 0.25)
        changeContainerStateButton:setHeight(editorContainer:getWidth() * 0.15)
    end

    overlay:subscribeOnAllWidgetLuaProxiesReady(function()
        editorContainer:setParent(host, canvas.widgetName, canvas.widgetName)
        editorContainer:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, canvas.widgetName, 0)
        editorContainer:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, canvas.widgetName, 0)
        editorContainer:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM, canvas.widgetName, 0)
        editorContainer:setWidth(getEditorContainerWidth(self, windowWidth))
        editorContainer:setZOrder(1)
        editorContainer:setColorHexValue(0xffffff)

        changeContainerStateButton:setParent(host, canvas.widgetName, editorContainer.widgetName)
        changeContainerStateButton:setAnchor(UiItemBase.UiAnchorType.VERTICAL_CENTER,
            UiItemBase.UiAnchorType.VERTICAL_CENTER, editorContainer.widgetName, 0)
        changeContainerStateButton:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT,
            editorContainer.widgetName, 5)
        changeContainerStateButton:setWidth(editorContainer:getWidth() * 0.25)
        changeContainerStateButton:setHeight(editorContainer:getWidth() * 0.15)
        changeContainerStateButton:setImageTextureSource("arrow_left.png")
        changeContainerStateButton:setImageRotationDegrees(180.0)
        changeContainerStateButton:setButtonBorderRadius(8)
        changeContainerStateButton:setButtonColorHexValue(0xdb9427)
        changeContainerStateButton:setUseImageCustomColor(true)
        changeContainerStateButton:setImageColorHexValue(0x000000)
        changeContainerStateButton:setZOrder(2)
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
