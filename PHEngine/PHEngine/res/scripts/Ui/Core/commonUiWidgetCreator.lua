CommonUiWidgetCreator = {
    CommonUiWidgetType = {
        UI_OVERLAY = 0,
        UI_CANVAS = 1,
        UI_ITEM = 2,
        UI_RECTANGLE = 3,
        UI_IMAGE = 4,
        UI_LABEL = 5,
        UI_TOGGLE_BUTTON = 6,
        UI_BACKGROUND_OVERLAY = 7,
        UI_PROGRESS_BAR = 8,
        UI_ROW_LAYOUT = 9,
        UI_SLIDER_BAR = 10,
        UI_TEXT_BLOCK = 11,
        UI_GRID_LAYOUT = 12,
        UI_SCROLL_LIST = 13
    }
}

local function getTypeToString(commonUiWidgetType)
    if commonUiWidgetType == CommonUiWidgetCreator.CommonUiWidgetType.UI_OVERLAY then
        return "UI_OVERLAY"
    elseif commonUiWidgetType == CommonUiWidgetCreator.CommonUiWidgetType.UI_CANVAS then
        return "UI_CANVAS"
    elseif commonUiWidgetType == CommonUiWidgetCreator.CommonUiWidgetType.UI_ITEM then
        return "UI_ITEM"
    elseif commonUiWidgetType == CommonUiWidgetCreator.CommonUiWidgetType.UI_RECTANGLE then
        return "UI_RECTANGLE"
    elseif commonUiWidgetType == CommonUiWidgetCreator.CommonUiWidgetType.UI_IMAGE then
        return "UI_IMAGE"
    elseif commonUiWidgetType == CommonUiWidgetCreator.CommonUiWidgetType.UI_LABEL then
        return "UI_LABEL"
    elseif commonUiWidgetType == CommonUiWidgetCreator.CommonUiWidgetType.UI_TOGGLE_BUTTON then
        return "UI_TOGGLE_BUTTON"
    elseif commonUiWidgetType == CommonUiWidgetCreator.CommonUiWidgetType.UI_BACKGROUND_OVERLAY then
        return "UI_BACKGROUND_OVERLAY"
    elseif commonUiWidgetType == CommonUiWidgetCreator.CommonUiWidgetType.UI_PROGRESS_BAR then
        return "UI_PROGRESS_BAR"
    elseif commonUiWidgetType == CommonUiWidgetCreator.CommonUiWidgetType.UI_ROW_LAYOUT then
        return "UI_ROW_LAYOUT"
    elseif commonUiWidgetType == CommonUiWidgetCreator.CommonUiWidgetType.UI_SLIDER_BAR then
        return "UI_SLIDER_BAR"
    elseif commonUiWidgetType == CommonUiWidgetCreator.CommonUiWidgetType.UI_TEXT_BLOCK then
        return "UI_TEXT_BLOCK"
    elseif commonUiWidgetType == CommonUiWidgetCreator.CommonUiWidgetType.UI_GRID_LAYOUT then
        return "UI_GRID_LAYOUT"
    elseif commonUiWidgetType == CommonUiWidgetCreator.CommonUiWidgetType.UI_SCROLL_LIST then
        return "UI_SCROLL_LIST"
    else
        return "GAME_OR_UNKNOWN(" .. tostring(commonUiWidgetType) .. ")"
    end
end

function CommonUiWidgetCreator:createUiWidget(host, commonUiWidgetType, jsonParameters)
    assert(host ~= nil and commonUiWidgetType ~= nil, debug.traceback())
    jsonParameters = jsonParameters and jsonParameters or ""
    local widgetLuaProxyId = _CreateCommonUiWidget(host, commonUiWidgetType, jsonParameters)
    print(
        "CommonUiWidgetCreator::createUiWidget: commonUiWidgetType: " .. tostring(getTypeToString(commonUiWidgetType)) ..
            " jsonParameters: " .. tostring(jsonParameters) .. " widgetLuaProxyId: " .. tostring(widgetLuaProxyId))

    return widgetLuaProxyId
end

function CommonUiWidgetCreator:destroyUiWidget(host, luaProxyId)
    assert(host ~= nil, debug.traceback())
    assert(luaProxyId ~= nil and type(luaProxyId) == "number" and luaProxyId > -1, debug.traceback())
    -- print("CommonUiWidgetCreator::destroyUiWidget: luaProxyId: " .. tostring(luaProxyId))
    -- invocation of this function leads to exception, memory is already deallocated
    -- _DestroyCommonUiWidget(host, luaProxyId)
end

return CommonUiWidgetCreator
