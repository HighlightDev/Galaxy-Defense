UiOverlayManager = {
}

function UiOverlayManager:getCurrentOverlayName(host)
    return _GetCurrentOverlayName(host)
end

function UiOverlayManager:closeCurrentOverlay(host)
    _CloseCurrentOverlay(host)
end

function UiOverlayManager:openOverlay(host, overlayName)
    _OpenOverlay(host, overlayName)
end

return UiOverlayManager
