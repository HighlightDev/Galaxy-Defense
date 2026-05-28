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

UiGridLayout = UiItemBase:new()
UiGridLayout.UiGridHorizontalAlignmentType = {LEFT = 0, CENTER = 1, RIGHT = 2}

UiGridLayout.UiGridVerticalAlignmentType = {TOP = 0, CENTER = 1, BOTTOM = 2}

function UiGridLayout:new(host, name)
    assert(host ~= nil, debug.traceback())

    local jsonParameters = nil;
    if name ~= nil then
        assert(type(name) == "string" and name ~= "", debug.traceback())
        jsonParameters = json.encode({name = name})
    end

    local luaProxyId = CommonUiWidgetCreator:createUiWidget(host,
                                                            CommonUiWidgetCreator.CommonUiWidgetType.UI_GRID_LAYOUT,
                                                            jsonParameters)

    local gridLayoutProperties = {
        horizontal_spacing = {value = 0, dirty = false},
        vertical_spacing = {value = 0, dirty = false},
        columns_count = {value = 1, dirty = false},
        rows_count = {value = 1, dirty = false},
        horizontal_alignment = {value = UiGridLayout.UiGridHorizontalAlignmentType.LEFT, dirty = false},
        vertical_alignment = {value = UiGridLayout.UiGridVerticalAlignmentType.TOP, dirty = false}
    }

    local uiGridLayoutObj = UiGridLayout.uiItemBaseClass.new(self)
    uiGridLayoutObj.typeName = "UiGridLayout"
    uiGridLayoutObj.luaProxyId = luaProxyId
    uiGridLayoutObj.gridLayoutProperties = gridLayoutProperties
    uiGridLayoutObj.host = host
    return uiGridLayoutObj
end

function UiGridLayout:updateFromReplicatorData(host)
    self:checkLuaProxyReady(host)
    if self.luaProxyReady then
        local replicatorJsonData = _GetGameThreadData(host, self.luaProxyId)
        if replicatorJsonData ~= "" then
            local parsedJson = json.decode(replicatorJsonData)
            self:extractUiItemBaseReplicatorData(parsedJson)

            if parsedJson["horizontal_spacing"] ~= nil then
                self.gridLayoutProperties.horizontal_spacing.value = tonumber(parsedJson["horizontal_spacing"])
            end
            if parsedJson["vertical_spacing"] ~= nil then
                self.gridLayoutProperties.vertical_spacing.value = tonumber(parsedJson["vertical_spacing"])
            end
            if parsedJson["columns_count"] ~= nil then
                self.gridLayoutProperties.columns_count.value = tonumber(parsedJson["columns_count"])
            end
            if parsedJson["rows_count"] ~= nil then
                self.gridLayoutProperties.rows_count.value = tonumber(parsedJson["rows_count"])
            end
            if parsedJson["horizontal_alignment"] ~= nil then
                self.gridLayoutProperties.horizontal_alignment.value = tonumber(parsedJson["horizontal_alignment"])
            end
            if parsedJson["vertical_alignment"] ~= nil then
                self.gridLayoutProperties.vertical_alignment.value = tonumber(parsedJson["vertical_alignment"])
            end
        end
    end
end

function UiGridLayout:sendDataToReplicator(host)
    local propertiesData, basePropsDirty = self:getUiItemBaseDataToReplicator()

    local isPropsDirty = false
    for key, value in pairs(self.gridLayoutProperties) do
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

function UiGridLayout:update(host, deltaTimeSec) end

function UiGridLayout:setHorizontalSpacing(spacing)
    assert(spacing ~= nil and type(spacing) == "number", debug.traceback())
    if self.gridLayoutProperties.horizontal_spacing.value ~= spacing then
        self.gridLayoutProperties.horizontal_spacing.value = spacing
        self.gridLayoutProperties.horizontal_spacing.dirty = true
    end
end

function UiGridLayout:setVerticalSpacing(spacing)
    assert(spacing ~= nil and type(spacing) == "number", debug.traceback())
    if self.gridLayoutProperties.vertical_spacing.value ~= spacing then
        self.gridLayoutProperties.vertical_spacing.value = spacing
        self.gridLayoutProperties.vertical_spacing.dirty = true
    end
end

function UiGridLayout:setColumnsCount(count)
    assert(count ~= nil and type(count) == "number" and count >= 1, debug.traceback())
    if self.gridLayoutProperties.columns_count.value ~= count then
        self.gridLayoutProperties.columns_count.value = count
        self.gridLayoutProperties.columns_count.dirty = true
    end
end

function UiGridLayout:setRowsCount(count)
    assert(count ~= nil and type(count) == "number" and count >= 1, debug.traceback())
    if self.gridLayoutProperties.rows_count.value ~= count then
        self.gridLayoutProperties.rows_count.value = count
        self.gridLayoutProperties.rows_count.dirty = true
    end
end

function UiGridLayout:setAlignment(horizontalAlignment, verticalAlignment)
    assert(horizontalAlignment ~= nil and type(horizontalAlignment) == "number", debug.traceback())
    assert(verticalAlignment ~= nil and type(verticalAlignment) == "number", debug.traceback())
    assert(horizontalAlignment >= UiGridLayout.UiGridHorizontalAlignmentType.LEFT and horizontalAlignment <=
               UiGridLayout.UiGridHorizontalAlignmentType.RIGHT, debug.traceback())
    assert(verticalAlignment >= UiGridLayout.UiGridVerticalAlignmentType.TOP and verticalAlignment <=
               UiGridLayout.UiGridVerticalAlignmentType.BOTTOM)

    if self.gridLayoutProperties.horizontal_alignment.value ~= horizontalAlignment then
        self.gridLayoutProperties.horizontal_alignment.value = horizontalAlignment
        self.gridLayoutProperties.horizontal_alignment.dirty = true
    end

    if self.gridLayoutProperties.vertical_alignment.value ~= verticalAlignment then
        self.gridLayoutProperties.vertical_alignment.value = verticalAlignment
        self.gridLayoutProperties.vertical_alignment.dirty = true
    end
end

return UiGridLayout
