--[[ BEGIN *** this snippet h to be inserted everywhere where your want to require custom modules *** BEGIN]]
--
local function setup()
    local str = debug.getinfo(2, "S").source:sub(2)
    local pathToCurrentScript = str:match("(.*/)")
    if pathToCurrentScript ~= nil then
        package.path = package.path .. ";" .. pathToCurrentScript .. "?.lua"
    end
end

setup()
--
--[[ END   *** this snippet has to be inserted everywhere where your want to require custom modules  ***  END]]
local UiBaseWidget = require("uiBaseWidget")

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

function UiItemBase:new()
    print("UiItemBase::ctor")

    local uiItemBaseProperties = {
        visible = {
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

    return uiItemBaseObj
end

local function this(ptr)
    if ptr.typeName == "UiItemBase" then
        return ptr
    else
        assert(ptr.uiItemBaseClass ~= nil)
        print("MY NAME: self: " .. tostring(ptr) .. ", name: " .. tostring(ptr.widgetName) .. ", ptr.uiItemBaseClass: " .. tostring(ptr.uiItemBaseClass) .. ", properties: " .. tostring(ptr.properties) .. ", parentClass: " .. tostring(ptr.parentClass))
        return ptr.uiItemBaseClass
    end
end

function UiItemBase:setParent(host, canvasName, uiWidgetParentName)
    print("UiItemBase:setParent => host :" ..
        tostring(host) ..
        ", canvas name: " .. tostring(canvasName) ..
        ", uiWidgetParentName: " ..
        tostring(uiWidgetParentName) ..
        ", myName: " .. tostring(self.widgetName) .. ", self.luaProxyReady: " .. tostring(self.luaProxyReady))
    assert(self.luaProxyReady == true and host ~= nil and type(host) == "userdata" and type(canvasName) == "string" and canvasName ~= "" and
        type(uiWidgetParentName) == "string" and
        uiWidgetParentName ~= "", debug.traceback())
    _SetUiWidgetParent(host, self.luaProxyId, canvasName, uiWidgetParentName)
end

function UiItemBase:extractUiItemBaseReplicatorData(parsedJsonData)
    local this = this(self)

    if parsedJsonData["visible"] ~= nil then
        this.properties.visible.value = parsedJsonData["visible"]
    end
    if parsedJsonData["z_order"] ~= nil then
        this.properties.z_order.value = parsedJsonData["z_order"]
    end
    if parsedJsonData["width"] ~= nil then
        this.properties.width.value = parsedJsonData["width"]
    end
    if parsedJsonData["height"] ~= nil then
        this.properties.height.value = parsedJsonData["height"]
    end
    if parsedJsonData["horizontalCenterOffset"] ~= nil then
        this.properties.horizontalCenterOffset.value = parsedJsonData["horizontalCenterOffset"]
    end
    if parsedJsonData["verticalCenterOffset"] ~= nil then
        this.properties.verticalCenterOffset.value = parsedJsonData["verticalCenterOffset"]
    end
    if parsedJsonData["anchors"] ~= nil then
        local anchorsTable = parsedJsonData["anchors"]
        print("UiItemBase:extractUiItemBaseReplicatorData => anchors :")
        for _, value in pairs(anchorsTable) do
            local srcAnchor = tonumber(value[1])
            local dstAnchor = tonumber(value[2][1])
            local dstUiItemWidgetName = tostring(value[2][2])
            local srcAnchorMargin = tonumber(value[2][3])
            print(string.format("srcAnchor[%d]: {dstAnchor: %d, dstUiItemWidgetName: %s, srcAnchorMargin: %d}", srcAnchor,
                dstAnchor, dstUiItemWidgetName, srcAnchorMargin))

            if srcAnchor ~= nil and dstAnchor ~= nil and dstUiItemWidgetName ~= nil and srcAnchorMargin ~= nil then
                this.properties.anchors.value[srcAnchor] = {
                    dstAnchor = dstAnchor,
                    dstUiItemWidgetName = dstUiItemWidgetName,
                    srcAnchorMargin = srcAnchorMargin
                }
            end
        end
    end
end

function UiItemBase:getUiItemBaseDataToReplicator()
    local this = this(self)
    local propertiesData = {}
    local isPropsDirty = false
    for key, value in pairs(this.properties) do
        if value.dirty then
            isPropsDirty = true
            propertiesData[tostring(key)] = value.value
            value.dirty = false
        end
    end

    return propertiesData, isPropsDirty
end

function UiItemBase:setIsVisible(isVisible)
    local this = this(self)
    if this.properties.visible.value ~= isVisible then
        this.properties.visible.value = isVisible
        this.properties.visible.dirty = true
    end
end

function UiItemBase:setZOrder(z_order)
    local this = this(self)
    if this.properties.z_order.value ~= z_order then
        this.properties.z_order.value = z_order
        this.properties.z_order.dirty = true
    end
end

function UiItemBase:setWidth(width)
    local this = this(self)
    if this.properties.width.value ~= width then
        this.properties.width.value = width
        this.properties.width.dirty = true
    end
end

function UiItemBase:setHeight(height)
    local this = this(self)
    if this.properties.height.value ~= height then
        this.properties.height.value = height
        this.properties.height.dirty = true
    end
end

function UiItemBase:setVerticalCenterOffset(verticalCenterOffset)
    local this = this(self)
    if this.properties.verticalCenterOffset.value ~= verticalCenterOffset then
        this.properties.verticalCenterOffset.value = verticalCenterOffset
        this.properties.verticalCenterOffset.dirty = true
    end
end

function UiItemBase:setHorizontalCenterOffset(horizontalCenterOffset)
    local this = this(self)
    if this.properties.horizontalCenterOffset.value ~= horizontalCenterOffset then
        this.properties.horizontalCenterOffset.value = horizontalCenterOffset
        this.properties.horizontalCenterOffset.dirty = true
    end
end

function UiItemBase:setAnchor(srcAnchor, dstAnchor, dstUiItemWidgetName, anchorMargin)
    assert(srcAnchor ~= nil and dstAnchor ~= nil and dstUiItemWidgetName ~= nil and
        srcAnchor > UiItemBase.UiAnchorType.NONE and srcAnchor <= UiItemBase.UiAnchorType.HORIZONTAL_CENTER)
    local this = this(self)
    print("UiItemBase:setAnchor => myName: " .. tostring(self.luaProxyId) .. ", srcAnchor: " .. tostring(srcAnchor) .. ", dstAnchor: " .. tostring(dstAnchor) .. ", dstUiItemWidgetName: " .. tostring(dstUiItemWidgetName))
    this.properties.anchors.dirty = true
    this.properties.anchors.value[srcAnchor].dstAnchor = dstAnchor
    this.properties.anchors.value[srcAnchor].dstUiItemWidgetName = dstUiItemWidgetName
    this.properties.anchors.value[srcAnchor].srcAnchorMargin = anchorMargin
end

return UiItemBase
