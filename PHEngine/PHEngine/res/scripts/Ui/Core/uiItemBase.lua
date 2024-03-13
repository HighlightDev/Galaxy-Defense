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
local UiBaseWidget = require("Ui/Core/uiBaseWidget")
local json = require("Ui/Core/3rdparty/json")

UiItemBase = UiBaseWidget:new()
UiItemBase.UiAnchorType = {
    NONE = 0,
    LEFT = 1,
    RIGHT = 2,
    TOP = 3,
    BOTTOM = 4,
    VERTICAL_CENTER = 5,
    HORIZONTAL_CENTER = 6
}

UiItemBase.UiMouseInputPressState = {
    RELEASED = 0,
    PRESSED = 1
}

UiItemBase.UiMouseInputCursorHoverState = {
    LEAVED = 0,
    ENTERED = 1
}

function UiItemBase:new()
    local uiItemBaseProperties = {
        visible = {
            value = false,
            dirty = false
        },
        intercept_mouse_input_event = {
            value = false,
            dirty = false
        },
        z_order = {
            value = 0,
            dirty = false
        },
        width = {
            value = 0,
            dirty = false
        },
        height = {
            value = 0,
            dirty = false
        },
        horizontalCenterOffset = {
            value = 0,
            dirty = false
        },
        verticalCenterOffset = {
            value = 0,
            dirty = false
        },
        anchors = {
            value = {
                [UiItemBase.UiAnchorType.LEFT] = {
                    dstAnchor = UiItemBase.UiAnchorType.NONE,
                    dstUiItemWidgetName = "",
                    srcAnchorMargin = 0
                },
                [UiItemBase.UiAnchorType.RIGHT] = {
                    dstAnchor = UiItemBase.UiAnchorType.NONE,
                    dstUiItemWidgetName = "",
                    srcAnchorMargin = 0
                },
                [UiItemBase.UiAnchorType.BOTTOM] = {
                    dstAnchor = UiItemBase.UiAnchorType.NONE,
                    dstUiItemWidgetName = "",
                    srcAnchorMargin = 0
                },
                [UiItemBase.UiAnchorType.TOP] = {
                    dstAnchor = UiItemBase.UiAnchorType.NONE,
                    dstUiItemWidgetName = "",
                    srcAnchorMargin = 0
                },
                [UiItemBase.UiAnchorType.VERTICAL_CENTER] = {
                    dstAnchor = UiItemBase.UiAnchorType.NONE,
                    dstUiItemWidgetName = "",
                    srcAnchorMargin = 0
                },
                [UiItemBase.UiAnchorType.HORIZONTAL_CENTER] = {
                    dstAnchor = UiItemBase.UiAnchorType.NONE,
                    dstUiItemWidgetName = "",
                    srcAnchorMargin = 0
                }
            },
            dirty = false
        }
    }

    local uiItemBaseObj = UiItemBase.parentClass.new(self)
    uiItemBaseObj.typeName = "UiItemBase"
    uiItemBaseObj.uiItemBaseClass = self
    uiItemBaseObj.properties = uiItemBaseProperties
    uiItemBaseObj.mouseInputPressState = UiItemBase.UiMouseInputPressState.RELEASED
    uiItemBaseObj.mouseInputCursorHoverState = UiItemBase.UiMouseInputCursorHoverState.LEAVED
    uiItemBaseObj.mouseInputClicked = false
    uiItemBaseObj.onMouseInputPressStateChangedCallbacks = {}
    uiItemBaseObj.onMouseInputCursorHoverStateChangedCallback = nil
    uiItemBaseObj.onMouseInputClickedCallbacks = {}

    return uiItemBaseObj
end

function UiItemBase:setParent(host, canvasName, uiWidgetParentName)
    --[[print("UiItemBase:setParent => host :" ..
        tostring(host) ..
        ", canvas name: " .. tostring(canvasName) ..
        ", uiWidgetParentName: " ..
        tostring(uiWidgetParentName) ..
        ", myName: " .. tostring(self.widgetName) .. ", self.luaProxyReady: " .. tostring(self.luaProxyReady))]]
    --
    assert(
        self.luaProxyReady == true and host ~= nil and type(host) == "userdata" and type(canvasName) == "string" and
        canvasName ~= "" and
        type(uiWidgetParentName) == "string" and
        uiWidgetParentName ~= "", debug.traceback())
    _SetUiWidgetParent(host, self.luaProxyId, canvasName, uiWidgetParentName)
end

function UiItemBase:extractUiItemBaseReplicatorData(parsedJsonData)
    if parsedJsonData["visible"] ~= nil then
        self.properties.visible.value = parsedJsonData["visible"]
    end
    if parsedJsonData["intercept_mouse_input_event"] ~= nil then
        self.properties.intercept_mouse_input_event.value = parsedJsonData["intercept_mouse_input_event"]
    end
    if parsedJsonData["z_order"] ~= nil then
        self.properties.z_order.value = parsedJsonData["z_order"]
    end
    if parsedJsonData["width"] ~= nil then
        self.properties.width.value = parsedJsonData["width"]
    end
    if parsedJsonData["height"] ~= nil then
        self.properties.height.value = parsedJsonData["height"]
    end
    if parsedJsonData["horizontalCenterOffset"] ~= nil then
        self.properties.horizontalCenterOffset.value = parsedJsonData["horizontalCenterOffset"]
    end
    if parsedJsonData["verticalCenterOffset"] ~= nil then
        self.properties.verticalCenterOffset.value = parsedJsonData["verticalCenterOffset"]
    end
    if parsedJsonData["anchors"] ~= nil then
        local anchorsTable = parsedJsonData["anchors"]
        for _, value in pairs(anchorsTable) do
            local srcAnchor = tonumber(value[1])
            local dstAnchor = tonumber(value[2][1])
            local dstUiItemWidgetName = tostring(value[2][2])
            local srcAnchorMargin = tonumber(value[2][3])

            if srcAnchor ~= nil and dstAnchor ~= nil and dstUiItemWidgetName ~= nil and srcAnchorMargin ~= nil then
                self.properties.anchors.value[srcAnchor] = {
                    dstAnchor = dstAnchor,
                    dstUiItemWidgetName = dstUiItemWidgetName,
                    srcAnchorMargin = srcAnchorMargin
                }
            end
        end
    end
end

function UiItemBase:getUiItemBaseDataToReplicator()
    local propertiesData = {}
    local isPropsDirty = false
    for key, value in pairs(self.properties) do
        if value.dirty then
            isPropsDirty = true
            propertiesData[tostring(key)] = value.value
            value.dirty = false
        end
    end

    return propertiesData, isPropsDirty
end

function UiItemBase:updateFromReplicatorMouseInputData(host)
    assert(host ~= nil and type(host) == "userdata")
    if self.luaProxyReady then
        local replicatorMouseInputJsonData = _GetMouseInputData(host, self.luaProxyId)
        if replicatorMouseInputJsonData ~= "" then
            local parsedJson = json.decode(replicatorMouseInputJsonData)
            if parsedJson["input_press_state"] ~= nil then
                local newState = tonumber(parsedJson["input_press_state"])
                if newState ~= self.mouseInputPressState then
                    self.mouseInputPressState = newState
                    for _, callback in pairs(self.onMouseInputPressStateChangedCallbacks) do
                        callback(newState)
                    end
                end
            end
            if parsedJson["input_cursor_hover_state"] ~= nil then
                local newState = tonumber(parsedJson["input_cursor_hover_state"])
                if newState ~= self.mouseInputCursorHoverState then
                    self.mouseInputCursorHoverState = newState
                    if self.onMouseInputCursorHoverStateChangedCallback ~= nil then
                        self.onMouseInputCursorHoverStateChangedCallback(newState)
                    end
                end
            end
            if parsedJson["input_clicked"] ~= nil then
                local newState = parsedJson["input_clicked"]
                if newState ~= self.mouseInputClicked then
                    for _, callback in pairs(self.onMouseInputClickedCallbacks) do
                        callback()
                    end
                end
            end
        end
    end
end

function UiItemBase:setIsVisible(isVisible)
    if self.properties.visible.value ~= isVisible then
        self.properties.visible.value = isVisible
        self.properties.visible.dirty = true
    end
end

function UiItemBase:getIsVisible()
    return self.properties.visible.value
end

function UiItemBase:setIfCanInterceptMouseInputEvent(intercept)
    if self.properties.intercept_mouse_input_event.value ~= intercept then
        self.properties.intercept_mouse_input_event.value = intercept
        self.properties.intercept_mouse_input_event.dirty = true
    end
end

function UiItemBase:getIfCanInterceptMouseInputEvent()
    return self.properties.intercept_mouse_input_event.value
end

function UiItemBase:setZOrder(z_order)
    if self.properties.z_order.value ~= z_order then
        self.properties.z_order.value = z_order
        self.properties.z_order.dirty = true
    end
end

function UiItemBase:getZOrder()
    return self.properties.z_order.value
end

function UiItemBase:setWidth(width)
    if self.properties.width.value ~= width then
        self.properties.width.value = width
        self.properties.width.dirty = true
    end
end

function UiItemBase:getWidth()
    return self.properties.width.value
end

function UiItemBase:setHeight(height)
    if self.properties.height.value ~= height then
        self.properties.height.value = height
        self.properties.height.dirty = true
    end
end

function UiItemBase:getHeight()
    return self.properties.height.value
end

function UiItemBase:setVerticalCenterOffset(verticalCenterOffset)
    if self.properties.verticalCenterOffset.value ~= verticalCenterOffset then
        self.properties.verticalCenterOffset.value = verticalCenterOffset
        self.properties.verticalCenterOffset.dirty = true
    end
end

function UiItemBase:getVerticalCenterOffset()
    return self.properties.verticalCenterOffset.value
end

function UiItemBase:setHorizontalCenterOffset(horizontalCenterOffset)
    if self.properties.horizontalCenterOffset.value ~= horizontalCenterOffset then
        self.properties.horizontalCenterOffset.value = horizontalCenterOffset
        self.properties.horizontalCenterOffset.dirty = true
    end
end

function UiItemBase:getHorizontalCenterOffset()
    return self.properties.horizontalCenterOffset.value
end

function UiItemBase:setAnchor(srcAnchor, dstAnchor, dstUiItemWidgetName, anchorMargin)
    assert(srcAnchor ~= nil and dstAnchor ~= nil and dstUiItemWidgetName ~= nil and
        srcAnchor > UiItemBase.UiAnchorType.NONE and srcAnchor <= UiItemBase.UiAnchorType.HORIZONTAL_CENTER)
    self.properties.anchors.dirty = true
    self.properties.anchors.value[srcAnchor].dstAnchor = dstAnchor
    self.properties.anchors.value[srcAnchor].dstUiItemWidgetName = dstUiItemWidgetName
    self.properties.anchors.value[srcAnchor].srcAnchorMargin = anchorMargin ~= nil and anchorMargin or 0
end

function UiItemBase:setAnchorMargin(srcAnchor, anchorMargin)
    assert(srcAnchor ~= nil and srcAnchor > UiItemBase.UiAnchorType.NONE and
        srcAnchor <= UiItemBase.UiAnchorType.HORIZONTAL_CENTER and anchorMargin ~= nil and type(anchorMargin) == "number")
    self.properties.anchors.dirty = true
    self.properties.anchors.value[srcAnchor].srcAnchorMargin = anchorMargin
end

function UiItemBase:enableMouseInputReceiverBase(host)
    assert(host ~= nil and type(host) == "userdata" and self.luaProxyReady == true)
    _EnableMouseInputReceiverBase(host, self.luaProxyId)
end

function UiItemBase:subscribeOnMouseInputPressStateChanged(callback)
    assert(callback ~= nil and type(callback) == "function")
    self.onMouseInputPressStateChangedCallbacks[#self.onMouseInputPressStateChangedCallbacks + 1] = callback
end

function UiItemBase:setOnMouseInputCursorHoverStateChangedCallback(callback)
    assert(callback ~= nil and type(callback) == "function")
    self.onMouseInputCursorHoverStateChangedCallback = callback
end

function UiItemBase:subscriveOnMouseInputClickedCallback(callback)
    assert(callback ~= nil and type(callback) == "function")
    self.onMouseInputClickedCallbacks[#self.onMouseInputClickedCallbacks + 1] = callback
end

function UiItemBase:addAnimation(host, animationName, animationFunctionType, animationDuration, animatedPropertyName,
                                 animatedPropertyType, propertySrcValue, propertyDstValue)
    assert(host ~= nil and type(host) == "userdata" and self.luaProxyReady == true)
    assert(animationName ~= nil and type(animationName) == "string" and animationFunctionType ~= nil and
        type(animationFunctionType) == "number" and animationDuration ~= nil and
        type(animationDuration) == "number" and animatedPropertyName ~= nil and type(animatedPropertyName) == "string" and
        animatedPropertyType ~= nil and type(animatedPropertyType) == "number")
    assert(propertySrcValue ~= nil and propertyDstValue ~= nil and type(propertySrcValue) == type(propertyDstValue))

    local animationJsonData = json.encode({
        animatedPropertyType = animatedPropertyType,
        animationFunctionType = animationFunctionType,
        animationDuration = animationDuration,
        animatedPropertyName = animatedPropertyName,
        propertySrcValue = propertySrcValue,
        propertyDstValue = propertyDstValue
    })
    _AddUiItemAnimation(host, self.luaProxyId, animationName, animationJsonData)
end

function UiItemBase:addSequenceAnimation(host, animationName, animationDataList)
    assert(host ~= nil and type(host) == "userdata" and self.luaProxyReady == true)
    assert(animationName ~= nil and type(animationName) == "string")
    assert(animationDataList ~= nil and type(animationDataList) == "table")

    -- check validity of animation sequence
    for _, animationData in pairs(animationDataList) do
        assert(animationData ~= nil and type(animationData) == "table")
        local animationFunctionType = animationData.animationFunctionType
        local animationDuration = animationData.animationDuration
        local animatedPropertyName = animationData.animatedPropertyName
        local animatedPropertyType = animationData.animatedPropertyType
        local propertySrcValue = animationData.propertySrcValue
        local propertyDstValue = animationData.propertyDstValue

        assert(type(animationFunctionType) == "number" and animationDuration ~= nil and
            type(animationDuration) == "number" and animatedPropertyName ~= nil and
            type(animatedPropertyName) == "string" and
            animatedPropertyType ~= nil and type(animatedPropertyType) == "number")
        assert(propertySrcValue ~= nil and propertyDstValue ~= nil and type(propertySrcValue) == type(propertyDstValue))
    end

    _AddUiItemSequenceAnimation(host, self.luaProxyId, animationName, json.encode(animationDataList))
end

function UiItemBase:startAnimation(host, animationName)
    assert(host ~= nil and type(host) == "userdata")
    assert(animationName ~= nil and type(animationName) == "string" and self.luaProxyReady == true)

    _StartUiItemAnimation(host, self.luaProxyId, animationName)
end

function UiItemBase:startSequenceAnimation(host, animationSequenceName)
    assert(host ~= nil and type(host) == "userdata")
    assert(animationSequenceName ~= nil and type(animationSequenceName) == "string" and self.luaProxyReady == true)

    _StartUiItemSequenceAnimation(host, self.luaProxyId, animationSequenceName)
end

return UiItemBase
