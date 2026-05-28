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

UiRowLayout = UiItemBase:new()
UiRowLayout.UiRowAlignmentType = {LEFT = 0, RIGHT = 1, CENTER = 2}

function UiRowLayout:new(host, name)
    assert(host ~= nil, debug.traceback())

    local jsonParameters = nil;
    if name ~= nil then
        assert(type(name) == "string" and name ~= "", debug.traceback())
        jsonParameters = json.encode({name = name})
    end

    local luaProxyId = CommonUiWidgetCreator:createUiWidget(host,
                                                            CommonUiWidgetCreator.CommonUiWidgetType.UI_ROW_LAYOUT,
                                                            jsonParameters)

    local rowLayoutProperties = {
        spacing = {value = 0, dirty = false},
        alignment = {value = UiRowLayout.UiRowAlignmentType.LEFT, dirty = false}
    }

    local uiRowLayoutObj = UiRowLayout.uiItemBaseClass.new(self)
    uiRowLayoutObj.typeName = "UiRowLayout"
    uiRowLayoutObj.luaProxyId = luaProxyId
    uiRowLayoutObj.rowLayoutProperties = rowLayoutProperties
    uiRowLayoutObj.host = host

    return uiRowLayoutObj
end

function UiRowLayout:updateFromReplicatorData(host)
    self:checkLuaProxyReady(host)
    if self.luaProxyReady then
        local replicatorJsonData = _GetGameThreadData(host, self.luaProxyId)
        if replicatorJsonData ~= "" then
            local parsedJson = json.decode(replicatorJsonData)
            self:extractUiItemBaseReplicatorData(parsedJson)

            if parsedJson["spacing"] ~= nil then
                self.rowLayoutProperties.spacing.value = tonumber(parsedJson["spacing"])
            end
            if parsedJson["alignment"] ~= nil then
                self.rowLayoutProperties.alignment.value = tonumber(parsedJson["alignment"])
            end
        end
    end
end

function UiRowLayout:sendDataToReplicator(host)
    local propertiesData, basePropsDirty = self:getUiItemBaseDataToReplicator()

    local isPropsDirty = false
    for key, value in pairs(self.rowLayoutProperties) do
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

function UiRowLayout:update(host, deltaTimeSec) end

function UiRowLayout:setSpacing(spacing)
    assert(spacing ~= nil and type(spacing) == "number", debug.traceback())
    if self.rowLayoutProperties.spacing.value ~= spacing then
        self.rowLayoutProperties.spacing.value = spacing
        self.rowLayoutProperties.spacing.dirty = true
    end
end

function UiRowLayout:setAlignment(alignment)
    assert(alignment ~= nil and alignment >= UiRowLayout.UiRowAlignmentType.LEFT and alignment <=
               UiRowLayout.UiRowAlignmentType.CENTER, debug.traceback())
    if self.rowLayoutProperties.alignment.value ~= alignment then
        self.rowLayoutProperties.alignment.value = alignment
        self.rowLayoutProperties.alignment.dirty = true
    end
end

return UiRowLayout
