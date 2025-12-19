UiOverlayManager = {}

function UiOverlayManager:getCurrentOverlayName(host) return _GetCurrentOverlayName(host) end

function UiOverlayManager:closeCurrentOverlay(host) _CloseCurrentOverlay(host) end

function UiOverlayManager:openOverlay(host, overlayName)
    assert(host ~= nil and type(host) == "userdata" and overlayName ~= nil and type(overlayName) == "string")
    _OpenOverlay(host, overlayName)
end

function UiOverlayManager:openBackgroundOverlay(host, overlayName)
    assert(host ~= nil and type(host) == "userdata" and overlayName ~= nil and type(overlayName) == "string")
    _OpenBackgroundOverlay(host, overlayName)
end

function UiOverlayManager:closeBackgroundOverlay(host, overlayName)
    assert(host ~= nil and type(host) == "userdata" and overlayName ~= nil and type(overlayName) == "string")
    _CloseBackgroundOverlay(host, overlayName)
end

return UiOverlayManager
