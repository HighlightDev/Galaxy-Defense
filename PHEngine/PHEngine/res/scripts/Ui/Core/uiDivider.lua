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
local UiItemBase = require("Ui/Core/uiItemBase")
local CommonUiWidgetCreator = require("Ui/Core/commonUiWidgetCreator")
local json = require("Ui/Core/3rdparty/json")

UiDivider = UiItemBase:new()

-- Matches the C++ eDividerOrientation enum (HORIZONTAL = 0, VERTICAL = 1).
UiDivider.Orientation = {HORIZONTAL = 0, VERTICAL = 1}

function UiDivider:new(host, name)
    assert(host ~= nil, debug.traceback())

    local jsonParameters = nil;
    if name ~= nil then
        assert(type(name) == "string" and name ~= "", debug.traceback())
        jsonParameters = json.encode({name = name})
    end

    local luaProxyId = CommonUiWidgetCreator:createUiWidget(host, CommonUiWidgetCreator.CommonUiWidgetType.UI_DIVIDER,
                                                            jsonParameters)

    local dividerProperties = {
        color = {value = {r = 0.0, g = 0.0, b = 0.0}, dirty = false},
        opacity = {value = 1.0, dirty = false},
        line_width_px = {value = 20.0, dirty = false},
        edge_fade = {value = 0.0, dirty = false},
        divider_orientation = {value = UiDivider.Orientation.HORIZONTAL, dirty = false},
    }

    local uiDividerObj = UiDivider.uiItemBaseClass.new(self)
    uiDividerObj.typeName = "UiDivider"
    uiDividerObj.luaProxyId = luaProxyId
    uiDividerObj.dividerProperties = dividerProperties
    uiDividerObj.host = host

    return uiDividerObj
end

function UiDivider:updateFromReplicatorData(host)
    self:checkLuaProxyReady(host)
    if self.luaProxyReady then
        local replicatorJsonData = _GetGameThreadData(host, self.luaProxyId)
        if replicatorJsonData ~= "" then
            local parsedJson = json.decode(replicatorJsonData)
            self:extractUiItemBaseReplicatorData(parsedJson)

            if parsedJson["color"] ~= nil then
                local colorArray = parsedJson["color"]
                self.dividerProperties.color.value.r = colorArray[1]
                self.dividerProperties.color.value.g = colorArray[2]
                self.dividerProperties.color.value.b = colorArray[3]
            end
            if parsedJson["opacity"] ~= nil then
                self.dividerProperties.opacity.value = parsedJson["opacity"]
            end
            if parsedJson["line_width_px"] ~= nil then
                self.dividerProperties.line_width_px.value = parsedJson["line_width_px"]
            end
            if parsedJson["edge_fade"] ~= nil then
                self.dividerProperties.edge_fade.value = parsedJson["edge_fade"]
            end
            if parsedJson["divider_orientation"] ~= nil then
                self.dividerProperties.divider_orientation.value = parsedJson["divider_orientation"]
            end
        end
    end
end

function UiDivider:sendDataToReplicator(host)
    local propertiesData, basePropsDirty = self:getUiItemBaseDataToReplicator()

    local isPropsDirty = false
    for key, value in pairs(self.dividerProperties) do
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

function UiDivider:update(host, deltaTimeSec) end

function UiDivider:setColorHexValue(colorHex)
    assert(colorHex ~= nil and type(colorHex) == "number", debug.traceback())

    local mask_b = 0xFF;
    local mask_g = 0xFF << 0x8;
    local mask_r = 0xFF << 0x10;

    local r = (mask_r & colorHex) >> 0x10;
    local g = (mask_g & colorHex) >> 0x8;
    local b = mask_b & colorHex;

    local INV_COLOR_MAX_BYTE_VALUE = 1.0 / 255.0;
    self:setColor(r * INV_COLOR_MAX_BYTE_VALUE, g * INV_COLOR_MAX_BYTE_VALUE, b * INV_COLOR_MAX_BYTE_VALUE)
end

function UiDivider:setColor(r, g, b)
    assert(
        r ~= nil and type(r) == "number" and g ~= nil and type(g) == "number" and b ~= nil and type(b) == "number" and r >=
            0.0 and r <= 1.0 and g >= 0.0 and g <= 1.0 and b >= 0.0 and b <= 1.0, debug.traceback())
    self.dividerProperties.color.value.r = r
    self.dividerProperties.color.value.g = g
    self.dividerProperties.color.value.b = b

    self.dividerProperties.color.dirty = true
end

function UiDivider:setOpacity(opacity)
    assert(opacity ~= nil and type(opacity) == "number", debug.traceback())
    if self.dividerProperties.opacity.value ~= opacity then
        self.dividerProperties.opacity.value = opacity
        self.dividerProperties.opacity.dirty = true
    end
end

function UiDivider:setLineWidthPx(widthPx)
    assert(widthPx ~= nil and type(widthPx) == "number", debug.traceback())
    if self.dividerProperties.line_width_px.value ~= widthPx then
        self.dividerProperties.line_width_px.value = widthPx
        self.dividerProperties.line_width_px.dirty = true
    end
end

-- Fraction of the line length over which the divider fades out toward each end (0 = solid, 0.5 = fade
-- the whole way). Use a small value (~0.1-0.2) for the mockup's transparent-edged dividers.
function UiDivider:setEdgeFade(edgeFade)
    assert(edgeFade ~= nil and type(edgeFade) == "number", debug.traceback())
    if self.dividerProperties.edge_fade.value ~= edgeFade then
        self.dividerProperties.edge_fade.value = edgeFade
        self.dividerProperties.edge_fade.dirty = true
    end
end

function UiDivider:setDividerOrientation(orientation)
    assert(orientation ~= nil and
               (orientation == UiDivider.Orientation.HORIZONTAL or orientation == UiDivider.Orientation.VERTICAL),
           debug.traceback())
    if self.dividerProperties.divider_orientation.value ~= orientation then
        self.dividerProperties.divider_orientation.value = orientation
        self.dividerProperties.divider_orientation.dirty = true
    end
end

return UiDivider
