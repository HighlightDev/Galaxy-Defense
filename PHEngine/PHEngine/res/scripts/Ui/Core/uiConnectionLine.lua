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
local GameUiWidgetType = require("Ui/Common/gameUiWidgetType")
local json = require("Ui/Core/3rdparty/json")

UiConnectionLine = UiItemBase:new()

function UiConnectionLine:new(host, name)
    assert(host ~= nil)

    local jsonParameters = nil
    if name ~= nil then
        assert(type(name) == "string" and name ~= "")
        jsonParameters = json.encode({name = name})
    end

    local luaProxyId = CommonUiWidgetCreator:createUiWidget(host, GameUiWidgetType.UI_CONNECTION_LINE, jsonParameters)

    local lineProperties = {
        start_point = {value = {x = 0.0, y = 0.0}, dirty = false},
        end_point = {value = {x = 1.0, y = 1.0}, dirty = false},
        start_anchor_target = {value = "", dirty = false},
        end_anchor_target = {value = "", dirty = false},
        color = {value = {r = 0.35, g = 0.55, b = 0.78}, dirty = false},
        thickness_px = {value = 2.0, dirty = false},
        dash_length_px = {value = 0.0, dirty = false},
        gap_length_px = {value = 0.0, dirty = false},
        opacity = {value = 1.0, dirty = false}
    }

    local uiConnectionLineObj = UiConnectionLine.uiItemBaseClass.new(self)
    uiConnectionLineObj.host = host
    uiConnectionLineObj.typeName = "UiConnectionLine"
    uiConnectionLineObj.luaProxyId = luaProxyId
    uiConnectionLineObj.lineProperties = lineProperties

    return uiConnectionLineObj
end

function UiConnectionLine:updateFromReplicatorData(host)
    self:checkLuaProxyReady(host)
    if self.luaProxyReady then
        local replicatorJsonData = _GetGameThreadData(host, self.luaProxyId)
        if replicatorJsonData ~= "" then
            local parsedJson = json.decode(replicatorJsonData)
            self:extractUiItemBaseReplicatorData(parsedJson)
            if parsedJson["start_point"] ~= nil then
                self.lineProperties.start_point.value = parsedJson["start_point"]
            end
            if parsedJson["end_point"] ~= nil then
                self.lineProperties.end_point.value = parsedJson["end_point"]
            end
            if parsedJson["start_anchor_target"] ~= nil then
                self.lineProperties.start_anchor_target.value = parsedJson["start_anchor_target"]
            end
            if parsedJson["end_anchor_target"] ~= nil then
                self.lineProperties.end_anchor_target.value = parsedJson["end_anchor_target"]
            end
            if parsedJson["color"] ~= nil then self.lineProperties.color.value = parsedJson["color"] end
            if parsedJson["thickness_px"] ~= nil then
                self.lineProperties.thickness_px.value = parsedJson["thickness_px"]
            end
            if parsedJson["dash_length_px"] ~= nil then
                self.lineProperties.dash_length_px.value = parsedJson["dash_length_px"]
            end
            if parsedJson["gap_length_px"] ~= nil then
                self.lineProperties.gap_length_px.value = parsedJson["gap_length_px"]
            end
            if parsedJson["opacity"] ~= nil then self.lineProperties.opacity.value = parsedJson["opacity"] end
        end
    end
end

function UiConnectionLine:sendDataToReplicator(host)
    local propertiesData, basePropsDirty = self:getUiItemBaseDataToReplicator()

    local isPropsDirty = false
    for key, value in pairs(self.lineProperties) do
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

function UiConnectionLine:update(host, deltaTimeSec) end

-- Точки задаются в нормализованных координатах [0..1] внутри bounding box виджета.
function UiConnectionLine:setStartPoint(x, y)
    assert(x ~= nil and type(x) == "number" and y ~= nil and type(y) == "number")
    self.lineProperties.start_point.value = {x = x, y = y}
    self.lineProperties.start_point.dirty = true
end

function UiConnectionLine:setEndPoint(x, y)
    assert(x ~= nil and type(x) == "number" and y ~= nil and type(y) == "number")
    self.lineProperties.end_point.value = {x = x, y = y}
    self.lineProperties.end_point.dirty = true
end

-- Привязывает начало линии к центру виджета targetUiItemName. Endpoint пересчитывается
-- при смещении цели (скролл и т.п.). Пустая строка — отвязать (вернуться к ручному start_point).
function UiConnectionLine:setStartAnchorTarget(targetUiItemName)
    assert(targetUiItemName ~= nil and type(targetUiItemName) == "string")
    if self.lineProperties.start_anchor_target.value ~= targetUiItemName then
        self.lineProperties.start_anchor_target.value = targetUiItemName
        self.lineProperties.start_anchor_target.dirty = true
    end
end

function UiConnectionLine:setEndAnchorTarget(targetUiItemName)
    assert(targetUiItemName ~= nil and type(targetUiItemName) == "string")
    if self.lineProperties.end_anchor_target.value ~= targetUiItemName then
        self.lineProperties.end_anchor_target.value = targetUiItemName
        self.lineProperties.end_anchor_target.dirty = true
    end
end

function UiConnectionLine:setColor(r, g, b)
    assert(
        r ~= nil and type(r) == "number" and g ~= nil and type(g) == "number" and b ~= nil and type(b) == "number" and r >=
            0.0 and r <= 1.0 and g >= 0.0 and g <= 1.0 and b >= 0.0 and b <= 1.0)
    self.lineProperties.color.value = {r = r, g = g, b = b}
    self.lineProperties.color.dirty = true
end

function UiConnectionLine:setColorHexValue(colorHex)
    assert(colorHex ~= nil and type(colorHex) == "number")
    local mask_b = 0xFF
    local mask_g = 0xFF << 0x8
    local mask_r = 0xFF << 0x10
    local r = (mask_r & colorHex) >> 0x10
    local g = (mask_g & colorHex) >> 0x8
    local b = mask_b & colorHex
    local INV = 1.0 / 255.0
    self:setColor(r * INV, g * INV, b * INV)
end

function UiConnectionLine:setThicknessPx(thicknessPx)
    assert(thicknessPx ~= nil and type(thicknessPx) == "number" and thicknessPx >= 0.0)
    if self.lineProperties.thickness_px.value ~= thicknessPx then
        self.lineProperties.thickness_px.value = thicknessPx
        self.lineProperties.thickness_px.dirty = true
    end
end

-- dashLengthPx = 0 — сплошная линия.
function UiConnectionLine:setDashPattern(dashLengthPx, gapLengthPx)
    assert(dashLengthPx ~= nil and type(dashLengthPx) == "number" and dashLengthPx >= 0.0)
    assert(gapLengthPx ~= nil and type(gapLengthPx) == "number" and gapLengthPx >= 0.0)
    self.lineProperties.dash_length_px.value = dashLengthPx
    self.lineProperties.dash_length_px.dirty = true
    self.lineProperties.gap_length_px.value = gapLengthPx
    self.lineProperties.gap_length_px.dirty = true
end

function UiConnectionLine:setOpacity(opacity)
    assert(opacity ~= nil and type(opacity) == "number")
    if self.lineProperties.opacity.value ~= opacity then
        self.lineProperties.opacity.value = opacity
        self.lineProperties.opacity.dirty = true
    end
end

return UiConnectionLine
