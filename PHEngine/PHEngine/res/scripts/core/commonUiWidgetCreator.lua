CommonUiWidgetCreator = {
    CommonUiWidgetType = {
        UI_CANVAS = 0
    }
}

function CommonUiWidgetCreator:createUiWidget(host, commonUiWidgetType, jsonParameters)
    assert(host ~= nil and commonUiWidgetType ~= nil)
    jsonParameters = jsonParameters and jsonParameters or ""
    local widgetLuaProxyId = _CreateCommonUiWidget(host, commonUiWidgetType, jsonParameters)
    print("CommonUiWidgetCreator::createUiWidget => commonUiWidgetType: " ..
        tostring(commonUiWidgetType) ..
        " jsonParameters: " ..
        tostring(jsonParameters) ..
        " widgetLuaProxyId: " ..
        tostring(widgetLuaProxyId))

    return widgetLuaProxyId
end

function CommonUiWidgetCreator:isLuaProxyReady(host, widgetLuaProxyId)
    return _IsLuaProxyReady(host, widgetLuaProxyId)
end

return CommonUiWidgetCreator
