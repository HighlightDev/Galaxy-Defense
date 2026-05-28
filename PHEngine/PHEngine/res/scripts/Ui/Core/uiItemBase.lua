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

UiItemBase.UiMouseInputPressState = {RELEASED = 0, PRESSED = 1}

UiItemBase.UiMouseInputCursorHoverState = {LEAVED = 0, ENTERED = 1}
-- Render layer. INHERIT — take layer from parent. Inside the layer, order is defined by tree and local z.
UiItemBase.UiLayer = {INHERIT = -1, BACKGROUND = 0, HUD = 100, PANEL = 200, OVERLAY = 300, MODAL = 400, TOOLTIP = 500}

function UiItemBase:new()
    local uiItemBaseProperties = {
        visible = {value = false, dirty = false},
        intercept_mouse_input_event = {value = false, dirty = false},
        z_order = {value = 0, dirty = false},
        layer = {value = -1, dirty = false},
        width = {value = 0, dirty = false},
        height = {value = 0, dirty = false},
        horizontalCenterOffset = {value = 0, dirty = false},
        verticalCenterOffset = {value = 0, dirty = false},
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
        },
        is_gui_scissors_slave = {value = false, dirty = false},
        is_gui_scissors_master = {value = false, dirty = false},
        can_bloom_be_applied = {value = false, dirty = false}
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
    uiItemBaseObj.isUiInputEnabled = true

    return uiItemBaseObj
end

function UiItemBase:setParent(host, canvasName, uiWidgetParentName)
    print("UiItemBase:setParent: host :" .. tostring(host) .. ", canvas name: " .. tostring(canvasName) ..
              ", uiWidgetParentName: " .. tostring(uiWidgetParentName) .. ", myName: " .. tostring(self.widgetName) ..
              ", self.luaProxyReady: " .. tostring(self.luaProxyReady))

    assert(self.luaProxyReady == true and host ~= nil and type(host) == "userdata" and type(canvasName) == "string" and
               canvasName ~= "" and type(uiWidgetParentName) == "string" and uiWidgetParentName ~= "", debug.traceback())
    _SetUiWidgetParent(host, self.luaProxyId, canvasName, uiWidgetParentName)
end

function UiItemBase:setIsUiInputEnabled(isUiInputEnabled)
    assert(isUiInputEnabled ~= nil and type(isUiInputEnabled) == "boolean", debug.traceback())
    self.isUiInputEnabled = isUiInputEnabled
end

function UiItemBase:extractUiItemBaseReplicatorData(parsedJsonData)
    if parsedJsonData["visible"] ~= nil then self.properties.visible.value = parsedJsonData["visible"] end
    if parsedJsonData["intercept_mouse_input_event"] ~= nil then
        self.properties.intercept_mouse_input_event.value = parsedJsonData["intercept_mouse_input_event"]
    end
    if parsedJsonData["z_order"] ~= nil then self.properties.z_order.value = parsedJsonData["z_order"] end
    if parsedJsonData["width"] ~= nil then self.properties.width.value = parsedJsonData["width"] end
    if parsedJsonData["height"] ~= nil then self.properties.height.value = parsedJsonData["height"] end
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
    if parsedJsonData["is_gui_scissors_slave"] ~= nil then
        self.properties.is_gui_scissors_slave.value = parsedJsonData["is_gui_scissors_slave"]
    end
    if parsedJsonData["is_gui_scissors_master"] ~= nil then
        self.properties.is_gui_scissors_master.value = parsedJsonData["is_gui_scissors_master"]
    end
    if parsedJsonData["can_bloom_be_applied"] ~= nil then
        self.properties.can_bloom_be_applied.value = parsedJsonData["can_bloom_be_applied"]
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
    assert(host ~= nil and type(host) == "userdata", debug.traceback())
    if self.luaProxyReady then
        local replicatorMouseInputJsonData = _GetMouseInputData(host, self.luaProxyId)
        if self.isUiInputEnabled and replicatorMouseInputJsonData ~= "" then
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
                    for _, callback in pairs(self.onMouseInputClickedCallbacks) do callback() end
                end
            end
        end
    end
end

function UiItemBase:setIsVisible(isVisible)
    assert(isVisible ~= nil and type(isVisible) == "boolean", debug.traceback())
    if self.properties.visible.value ~= isVisible then
        self.properties.visible.value = isVisible
        self.properties.visible.dirty = true
    end
end

function UiItemBase:getIsVisible() return self.properties.visible.value end

function UiItemBase:setIfCanInterceptMouseInputEvent(intercept)
    if self.properties.intercept_mouse_input_event.value ~= intercept then
        self.properties.intercept_mouse_input_event.value = intercept
        self.properties.intercept_mouse_input_event.dirty = true
    end
end

function UiItemBase:getIfCanInterceptMouseInputEvent() return self.properties.intercept_mouse_input_event.value end

function UiItemBase:setZOrder(z_order)
    if self.properties.z_order.value ~= z_order then
        self.properties.z_order.value = z_order
        self.properties.z_order.dirty = true
    end
end

function UiItemBase:getZOrder() return self.properties.z_order.value end

-- layer — value from UiItemBase.UiLayer (UiLayer.INHERIT to inherit parent's layer).
function UiItemBase:setLayer(layer)
    assert(layer ~= nil and type(layer) == "number", debug.traceback())
    if self.properties.layer.value ~= layer then
        self.properties.layer.value = layer
        self.properties.layer.dirty = true
    end
end

function UiItemBase:getLayer() return self.properties.layer.value end

function UiItemBase:setWidth(width)
    if self.properties.width.value ~= width then
        self.properties.width.value = width
        self.properties.width.dirty = true
    end
end

function UiItemBase:getWidth() return self.properties.width.value end

function UiItemBase:setHeight(height)
    if self.properties.height.value ~= height then
        self.properties.height.value = height
        self.properties.height.dirty = true
    end
end

function UiItemBase:getHeight() return self.properties.height.value end

function UiItemBase:setVerticalCenterOffset(verticalCenterOffset)
    if self.properties.verticalCenterOffset.value ~= verticalCenterOffset then
        self.properties.verticalCenterOffset.value = verticalCenterOffset
        self.properties.verticalCenterOffset.dirty = true
    end
end

function UiItemBase:getVerticalCenterOffset() return self.properties.verticalCenterOffset.value end

function UiItemBase:setHorizontalCenterOffset(horizontalCenterOffset)
    if self.properties.horizontalCenterOffset.value ~= horizontalCenterOffset then
        self.properties.horizontalCenterOffset.value = horizontalCenterOffset
        self.properties.horizontalCenterOffset.dirty = true
    end
end

function UiItemBase:getHorizontalCenterOffset() return self.properties.horizontalCenterOffset.value end

function UiItemBase:setAnchor(srcAnchor, dstAnchor, dstUiItemWidgetName, anchorMargin)
    assert(srcAnchor ~= nil and dstAnchor ~= nil and dstUiItemWidgetName ~= nil and srcAnchor >
               UiItemBase.UiAnchorType.NONE and srcAnchor <= UiItemBase.UiAnchorType.HORIZONTAL_CENTER,
           debug.traceback())
    self.properties.anchors.dirty = true
    self.properties.anchors.value[srcAnchor].dstAnchor = dstAnchor
    self.properties.anchors.value[srcAnchor].dstUiItemWidgetName = dstUiItemWidgetName
    self.properties.anchors.value[srcAnchor].srcAnchorMargin = anchorMargin ~= nil and anchorMargin or 0
end

function UiItemBase:fill(dstUiItemWidgetName)
    assert(dstUiItemWidgetName ~= nil and type(dstUiItemWidgetName) == "string", debug.traceback())
    self.properties.anchors.dirty = true
    for anchor = UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.BOTTOM, 1 do
        self.properties.anchors.value[anchor].dstAnchor = anchor
        self.properties.anchors.value[anchor].dstUiItemWidgetName = dstUiItemWidgetName
        local existingMargin = self.properties.anchors.value[anchor].srcAnchorMargin
        self.properties.anchors.value[anchor].srcAnchorMargin = existingMargin ~= nil and existingMargin or 0
    end
end

function UiItemBase:setAnchorMargin(srcAnchor, anchorMargin)
    assert(srcAnchor ~= nil and srcAnchor > UiItemBase.UiAnchorType.NONE and srcAnchor <=
               UiItemBase.UiAnchorType.HORIZONTAL_CENTER and anchorMargin ~= nil and type(anchorMargin) == "number",
           debug.traceback())
    self.properties.anchors.dirty = true
    self.properties.anchors.value[srcAnchor].srcAnchorMargin = anchorMargin
end

function UiItemBase:enableMouseInputReceiverBase(host)
    assert(host ~= nil and type(host) == "userdata" and self.luaProxyReady == true, debug.traceback())
    _EnableMouseInputReceiverBase(host, self.luaProxyId)
    self:setIfCanInterceptMouseInputEvent(true)
end

function UiItemBase:subscribeOnMouseInputPressStateChanged(callback)
    assert(callback ~= nil and type(callback) == "function", debug.traceback())
    self.onMouseInputPressStateChangedCallbacks[#self.onMouseInputPressStateChangedCallbacks + 1] = callback
end

function UiItemBase:subscribeOnMouseInputCursorHoverStateChangedCallback(callback)
    assert(callback ~= nil and type(callback) == "function", debug.traceback())
    self.onMouseInputCursorHoverStateChangedCallback = callback
end

function UiItemBase:subscribeOnMouseInputClickedCallback(callback)
    assert(callback ~= nil and type(callback) == "function", debug.traceback())
    self.onMouseInputClickedCallbacks[#self.onMouseInputClickedCallbacks + 1] = callback
end

function UiItemBase:addAnimation(host, animationName, animationFunctionType, animationDuration, animatedPropertyName,
                                 animatedPropertyType, propertySrcValue, propertyDstValue)
    assert(host ~= nil and type(host) == "userdata" and self.luaProxyReady == true, debug.traceback())
    assert(animationName ~= nil and type(animationName) == "string" and animationFunctionType ~= nil and
               type(animationFunctionType) == "number" and animationDuration ~= nil and type(animationDuration) ==
               "number" and animatedPropertyName ~= nil and type(animatedPropertyName) == "string" and
               animatedPropertyType ~= nil and type(animatedPropertyType) == "number", debug.traceback())
    assert(propertySrcValue ~= nil and propertyDstValue ~= nil and type(propertySrcValue) == type(propertyDstValue),
           debug.traceback())

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
    assert(host ~= nil and type(host) == "userdata" and self.luaProxyReady == true, debug.traceback())
    assert(animationName ~= nil and type(animationName) == "string", debug.traceback())
    assert(animationDataList ~= nil and type(animationDataList) == "table", debug.traceback())

    -- check validity of animation sequence
    for _, animationData in pairs(animationDataList) do
        assert(animationData ~= nil and type(animationData) == "table", debug.traceback())
        local animationFunctionType = animationData.animationFunctionType
        local animationDuration = animationData.animationDuration
        local animatedPropertyName = animationData.animatedPropertyName
        local animatedPropertyType = animationData.animatedPropertyType
        local propertySrcValue = animationData.propertySrcValue
        local propertyDstValue = animationData.propertyDstValue

        assert(type(animationFunctionType) == "number", debug.traceback())
        assert(animationDuration ~= nil and type(animationDuration) == "number", debug.traceback())
        assert(animatedPropertyName ~= nil and type(animatedPropertyName) == "string", debug.traceback())
        assert(animatedPropertyType ~= nil and type(animatedPropertyType) == "number", debug.traceback())
        assert(propertySrcValue ~= nil and propertyDstValue ~= nil and type(propertySrcValue) == type(propertyDstValue),
               debug.traceback())
    end

    _AddUiItemSequenceAnimation(host, self.luaProxyId, animationName, json.encode(animationDataList))
end

function UiItemBase:startAnimation(host, animationName)
    assert(host ~= nil and type(host) == "userdata", debug.traceback())
    assert(animationName ~= nil and type(animationName) == "string" and self.luaProxyReady == true, debug.traceback())

    _StartUiItemAnimation(host, self.luaProxyId, animationName)
end

function UiItemBase:startSequenceAnimation(host, animationSequenceName)
    assert(host ~= nil and type(host) == "userdata", debug.traceback())
    assert(animationSequenceName ~= nil and type(animationSequenceName) == "string" and self.luaProxyReady == true,
           debug.traceback())

    _StartUiItemSequenceAnimation(host, self.luaProxyId, animationSequenceName)
end

function UiItemBase:setIsGuiScissorsSlave(isGuiScissorsSlave)
    if self.properties.is_gui_scissors_slave.value ~= isGuiScissorsSlave then
        self.properties.is_gui_scissors_slave.value = isGuiScissorsSlave
        self.properties.is_gui_scissors_slave.dirty = true
    end
end

function UiItemBase:getIsGuiScissorsSlave() return self.properties.is_gui_scissors_slave.value end

function UiItemBase:setIsGuiScissorsMaster(isGuiScissorsMaster)
    if self.properties.is_gui_scissors_master.value ~= isGuiScissorsMaster then
        self.properties.is_gui_scissors_master.value = isGuiScissorsMaster
        self.properties.is_gui_scissors_master.dirty = true
    end
end

function UiItemBase:getIsGuiScissorsMaster() return self.properties.is_gui_scissors_master.value end

function UiItemBase:setCanBloomBeApplied(canBloomBeApplied)
    if self.properties.can_bloom_be_applied.value ~= canBloomBeApplied then
        self.properties.can_bloom_be_applied.value = canBloomBeApplied
        self.properties.can_bloom_be_applied.dirty = true
    end
end

function UiItemBase:getCanBloomBeApplied() return self.properties.can_bloom_be_applied.value end

return UiItemBase
