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
local UiItemBase = require("Ui/Core/uiItemBase")
local CommonUiWidgetCreator = require("Ui/Core/commonUiWidgetCreator")
local json = require("Ui/Core/3rdparty/json")

UiScrollList = UiItemBase:new()

UiScrollList.ScrollbarSide = {NONE = 0, LEFT = 1, RIGHT = 2}

function UiScrollList:new(host, name)
    assert(host ~= nil, debug.traceback())

    local jsonParameters = nil
    if name ~= nil then
        assert(type(name) == "string" and name ~= "", debug.traceback())
        jsonParameters = json.encode({name = name})
    end

    local luaProxyId = CommonUiWidgetCreator:createUiWidget(host,
                                                            CommonUiWidgetCreator.CommonUiWidgetType.UI_SCROLL_LIST,
                                                            jsonParameters)

    local scrollListProperties = {
        spacing = {value = 0, dirty = false},
        scroll_offset = {value = 0, dirty = false},
        scroll_speed = {value = 30, dirty = false},
        scrollbar_side = {value = UiScrollList.ScrollbarSide.NONE, dirty = false},
        scrollbar_background_color = {value = {r = 0.2, g = 0.2, b = 0.2}, dirty = false},
        scrollbar_thumb_color = {value = {r = 0.6, g = 0.6, b = 0.6}, dirty = false},
        scrollbar_thickness = {value = 8, dirty = false}
    }

    local uiScrollListObj = UiScrollList.uiItemBaseClass.new(self)
    uiScrollListObj.typeName = "UiScrollList"
    uiScrollListObj.luaProxyId = luaProxyId
    uiScrollListObj.scrollListProperties = scrollListProperties
    uiScrollListObj.host = host

    return uiScrollListObj
end

function UiScrollList:updateFromReplicatorData(host)
    self:checkLuaProxyReady(host)
    if self.luaProxyReady then
        local replicatorJsonData = _GetGameThreadData(host, self.luaProxyId)
        if replicatorJsonData ~= "" then
            local parsedJson = json.decode(replicatorJsonData)
            self:extractUiItemBaseReplicatorData(parsedJson)

            if parsedJson["spacing"] ~= nil then
                self.scrollListProperties.spacing.value = tonumber(parsedJson["spacing"])
            end
            if parsedJson["scroll_offset"] ~= nil then
                self.scrollListProperties.scroll_offset.value = tonumber(parsedJson["scroll_offset"])
            end
            if parsedJson["scroll_speed"] ~= nil then
                self.scrollListProperties.scroll_speed.value = tonumber(parsedJson["scroll_speed"])
            end
            if parsedJson["scrollbar_side"] ~= nil then
                self.scrollListProperties.scrollbar_side.value = tonumber(parsedJson["scrollbar_side"])
            end
            if parsedJson["scrollbar_background_color"] ~= nil then
                local c = parsedJson["scrollbar_background_color"]
                self.scrollListProperties.scrollbar_background_color.value.r = c[1]
                self.scrollListProperties.scrollbar_background_color.value.g = c[2]
                self.scrollListProperties.scrollbar_background_color.value.b = c[3]
            end
            if parsedJson["scrollbar_thumb_color"] ~= nil then
                local c = parsedJson["scrollbar_thumb_color"]
                self.scrollListProperties.scrollbar_thumb_color.value.r = c[1]
                self.scrollListProperties.scrollbar_thumb_color.value.g = c[2]
                self.scrollListProperties.scrollbar_thumb_color.value.b = c[3]
            end
            if parsedJson["scrollbar_thickness"] ~= nil then
                self.scrollListProperties.scrollbar_thickness.value = tonumber(parsedJson["scrollbar_thickness"])
            end
        end
    end
end

function UiScrollList:sendDataToReplicator(host)
    local propertiesData, basePropsDirty = self:getUiItemBaseDataToReplicator()

    local isPropsDirty = false
    for key, value in pairs(self.scrollListProperties) do
        if value.dirty then
            isPropsDirty = true
            propertiesData[tostring(key)] = value.value
            value.dirty = false
        end
    end
    if basePropsDirty or isPropsDirty then
        _OnCommonUiWidgetDataUpdated(host, self.luaProxyId, json.encode(propertiesData))
    end
end

function UiScrollList:update(host, deltaTimeSec) end

function UiScrollList:setSpacing(spacing)
    assert(spacing ~= nil and type(spacing) == "number" and spacing >= 0, debug.traceback())
    if self.scrollListProperties.spacing.value ~= spacing then
        self.scrollListProperties.spacing.value = spacing
        self.scrollListProperties.spacing.dirty = true
    end
end

function UiScrollList:setScrollOffset(offset)
    assert(offset ~= nil and type(offset) == "number", debug.traceback())
    if self.scrollListProperties.scroll_offset.value ~= offset then
        self.scrollListProperties.scroll_offset.value = offset
        self.scrollListProperties.scroll_offset.dirty = true
    end
end

function UiScrollList:setScrollSpeed(speed)
    assert(speed ~= nil and type(speed) == "number" and speed > 0, debug.traceback())
    if self.scrollListProperties.scroll_speed.value ~= speed then
        self.scrollListProperties.scroll_speed.value = speed
        self.scrollListProperties.scroll_speed.dirty = true
    end
end

function UiScrollList:getScrollOffset() return self.scrollListProperties.scroll_offset.value end

function UiScrollList:setScrollbarSide(side)
    assert(side == UiScrollList.ScrollbarSide.NONE or side == UiScrollList.ScrollbarSide.LEFT or side ==
               UiScrollList.ScrollbarSide.RIGHT, debug.traceback())
    if self.scrollListProperties.scrollbar_side.value ~= side then
        self.scrollListProperties.scrollbar_side.value = side
        self.scrollListProperties.scrollbar_side.dirty = true
    end
end

function UiScrollList:setScrollbarBackgroundColor(r, g, b)
    assert(
        r ~= nil and type(r) == "number" and g ~= nil and type(g) == "number" and b ~= nil and type(b) == "number" and r >=
            0.0 and r <= 1.0 and g >= 0.0 and g <= 1.0 and b >= 0.0 and b <= 1.0, debug.traceback())
    self.scrollListProperties.scrollbar_background_color.value.r = r
    self.scrollListProperties.scrollbar_background_color.value.g = g
    self.scrollListProperties.scrollbar_background_color.value.b = b
    self.scrollListProperties.scrollbar_background_color.dirty = true
end

function UiScrollList:setScrollbarBackgroundColorHexValue(colorHex)
    assert(colorHex ~= nil and type(colorHex) == "number", debug.traceback())
    local INV = 1.0 / 255.0
    local r = ((0xFF << 0x10) & colorHex) >> 0x10
    local g = ((0xFF << 0x8) & colorHex) >> 0x8
    local b = 0xFF & colorHex
    self:setScrollbarBackgroundColor(r * INV, g * INV, b * INV)
end

function UiScrollList:setScrollbarThumbColor(r, g, b)
    assert(
        r ~= nil and type(r) == "number" and g ~= nil and type(g) == "number" and b ~= nil and type(b) == "number" and r >=
            0.0 and r <= 1.0 and g >= 0.0 and g <= 1.0 and b >= 0.0 and b <= 1.0, debug.traceback())
    self.scrollListProperties.scrollbar_thumb_color.value.r = r
    self.scrollListProperties.scrollbar_thumb_color.value.g = g
    self.scrollListProperties.scrollbar_thumb_color.value.b = b
    self.scrollListProperties.scrollbar_thumb_color.dirty = true
end

function UiScrollList:setScrollbarThumbColorHexValue(colorHex)
    assert(colorHex ~= nil and type(colorHex) == "number", debug.traceback())
    local INV = 1.0 / 255.0
    local r = ((0xFF << 0x10) & colorHex) >> 0x10
    local g = ((0xFF << 0x8) & colorHex) >> 0x8
    local b = 0xFF & colorHex
    self:setScrollbarThumbColor(r * INV, g * INV, b * INV)
end

function UiScrollList:setScrollbarThicknessPixels(thicknessPixels)
    assert(thicknessPixels ~= nil and type(thicknessPixels) == "number" and thicknessPixels >= 0, debug.traceback())
    if self.scrollListProperties.scrollbar_thickness.value ~= thicknessPixels then
        self.scrollListProperties.scrollbar_thickness.value = thicknessPixels
        self.scrollListProperties.scrollbar_thickness.dirty = true
    end
end

return UiScrollList
