local function setup()
    local slash = package.config:sub(1, 1);
    assert(slash ~= nil and type(slash) == "string" and slash ~= "");
    local pattern = "";
    if slash == "/" then
        pattern = "(.*/)";
    elseif slash == "\\" then
        pattern = "(.*\\)";
    end
    local str = (debug.getinfo(2, "S")).source:sub(2);
    local pathToCurrentScript = str:match(pattern);
    if pathToCurrentScript ~= nil then
        local unixLikePath = pathToCurrentScript:gsub("\\", "/");
        unixLikePath = unixLikePath:gsub("//", "/");
        local _, endindex = string.find(unixLikePath, "scripts/");
        unixLikePath = string.sub(unixLikePath, 1, endindex);
        package.path = package.path .. ";" .. unixLikePath .. "?.lua";
    end
end
setup();
local UiItemBase = require("Ui/Core/uiItemBase");
local CommonUiWidgetCreator = require("Ui/Core/commonUiWidgetCreator");
local json = require("Ui/Core/3rdparty/json");
UiSlider = UiItemBase:new();
UiSlider.UiSliderType = {SLIDER_TYPE_VERTICAL = 0, SLIDER_TYPE_HORIZONTAL = 1};
function UiSlider:new(host, name)
    assert(host ~= nil);
    local jsonParameters = nil;
    if name ~= nil then
        assert(type(name) == "string" and name ~= "");
        jsonParameters = json.encode({name = name});
    end
    local luaProxyId = CommonUiWidgetCreator:createUiWidget(host,
                                                            CommonUiWidgetCreator.CommonUiWidgetType
                                                                .UI_SLIDER_BAR,
                                                            jsonParameters);
    local sliderProperties = {
        max_slider_value = {value = 0, dirty = false},
        min_slider_value = {value = 0, dirty = false},
        slider_value = {value = 0, dirty = false},
        slider_step = {value = 0, dirty = false},
        slider_thickness_pixels = {value = 20, dirty = false},
        blob_thickness_pixels = {value = 40, dirty = false},
        slider_type = {
            value = UiSlider.UiSliderType.SLIDER_TYPE_HORIZONTAL,
            dirty = false
        },
        opacity = {value = 1, dirty = false},
        slider_color = {value = {r = 1, g = 1, b = 1}, dirty = false},
        blob_color = {value = {r = 1, g = 1, b = 1}, dirty = false}
    };
    local uiSliderObj = UiSlider.uiItemBaseClass.new(self);
    uiSliderObj.typeName = "UiSlider";
    uiSliderObj.luaProxyId = luaProxyId;
    uiSliderObj.sliderProperties = sliderProperties;
    uiSliderObj.host = host;
    uiSliderObj.onSliderValueChangedCallbacks = {};
    return uiSliderObj;
end
function UiSlider:updateFromReplicatorData(host)
    self:checkLuaProxyReady(host);
    if self.luaProxyReady then
        local replicatorJsonData = _GetGameThreadData(host, self.luaProxyId);
        if replicatorJsonData ~= "" then
            local parsedJson = json.decode(replicatorJsonData);
            self:extractUiItemBaseReplicatorData(parsedJson);
            if parsedJson["max_slider_value"] ~= nil then
                self.sliderProperties.max_slider_value.value = tonumber(
                                                                   parsedJson["max_slider_value"]);
            end
            if parsedJson["min_slider_value"] ~= nil then
                self.sliderProperties.min_slider_value.value = tonumber(
                                                                   parsedJson["min_slider_value"]);
            end
            if parsedJson["slider_value"] ~= nil then
                self.sliderProperties.slider_value.value = tonumber(
                                                               parsedJson["slider_value"]);
                if self.onSliderValueChangedCallbacks ~= nil and
                    #self.onSliderValueChangedCallbacks > 0 then
                    for _, callback in
                        ipairs(self.onSliderValueChangedCallbacks) do
                        if callback ~= nil and type(callback) == "function" then
                            callback(self.sliderProperties.slider_value.value);
                        end
                    end
                end
            end
            if parsedJson["slider_step"] ~= nil then
                self.sliderProperties.slider_step.value = tonumber(
                                                              parsedJson["slider_step"]);
            end
            if parsedJson["slider_thickness_pixels"] ~= nil then
                self.sliderProperties.slider_thickness_pixels.value = tonumber(
                                                                          parsedJson["slider_thickness_pixels"]);
            end
            if parsedJson["blob_thickness_pixels"] ~= nil then
                self.sliderProperties.blob_thickness_pixels.value = tonumber(
                                                                        parsedJson["blob_thickness_pixels"]);
            end
            if parsedJson["slider_type"] ~= nil then
                self.sliderProperties.slider_type.value = tonumber(
                                                              parsedJson["slider_type"]);
            end
            if parsedJson["opacity"] ~= nil then
                self.sliderProperties.opacity.value = tonumber(
                                                          parsedJson["opacity"]);
            end
            if parsedJson["slider_color"] ~= nil then
                local colorArray = parsedJson["slider_color"]
                self.sliderProperties.slider_color.value.r = colorArray[1]
                self.rectangleProperties.slider_color.value.g = colorArray[2]
                self.rectangleProperties.slider_color.value.b = colorArray[3]
            end
            if parsedJson["blob_color"] ~= nil then
                local colorArray = parsedJson["blob_color"]
                self.sliderProperties.blob_color.value.r = colorArray[1]
                self.sliderProperties.blob_color.value.g = colorArray[2]
                self.sliderProperties.blob_color.value.b = colorArray[3]
            end
        end
    end
end
function UiSlider:sendDataToReplicator(host)
    local propertiesData, basePropsDirty = self:getUiItemBaseDataToReplicator();
    local isPropsDirty = false;
    for key, value in pairs(self.sliderProperties) do
        if value.dirty then
            isPropsDirty = true;
            propertiesData[tostring(key)] = value.value;
            value.dirty = false;
        end
    end
    if basePropsDirty or isPropsDirty then
        _OnCommonUiWidgetDataUpdated(host, self.luaProxyId,
                                     json.encode(propertiesData));
    end
end
function UiSlider:update(host) end
function UiSlider:enableSliderMouseInputReceiver(host)
    assert(host ~= nil and type(host) == "userdata");
    _EnableSliderMouseInputReceiver(host, self.luaProxyId);
end
function UiSlider:setOpacity(opacity)
    assert(opacity ~= nil and type(opacity) == "number");
    if self.sliderProperties.opacity.value ~= opacity then
        self.sliderProperties.opacity.value = opacity;
        self.sliderProperties.opacity.dirty = true;
    end
end
function UiSlider:setSliderValue(sliderValue)
    assert(sliderValue ~= nil and type(sliderValue) == "number");
    if self.sliderProperties.slider_value.value ~= sliderValue then
        self.sliderProperties.slider_value.value = sliderValue;
        self.sliderProperties.slider_value.dirty = true;
    end
end
function UiSlider:getSliderValue()
    return self.sliderProperties.slider_value.value;
end
function UiSlider:setMaxSliderValue(maxSliderValue)
    assert(maxSliderValue ~= nil and type(maxSliderValue) == "number");
    if self.sliderProperties.max_slider_value.value ~= maxSliderValue then
        self.sliderProperties.max_slider_value.value = maxSliderValue;
        self.sliderProperties.max_slider_value.dirty = true;
    end
end
function UiSlider:setMinSliderValue(minSliderValue)
    assert(minSliderValue ~= nil and type(minSliderValue) == "number");
    if self.sliderProperties.min_slider_value.value ~= minSliderValue then
        self.sliderProperties.min_slider_value.value = minSliderValue;
        self.sliderProperties.min_slider_value.dirty = true;
    end
end
function UiSlider:setSliderStep(sliderStep)
    assert(sliderStep ~= nil and type(sliderStep) == "number");
    if self.sliderProperties.slider_step.value ~= sliderStep then
        self.sliderProperties.slider_step.value = sliderStep;
        self.sliderProperties.slider_step.dirty = true;
    end
end
function UiSlider:setSliderThicknessPixels(sliderThicknessPixels)
    assert(sliderThicknessPixels ~= nil and type(sliderThicknessPixels) ==
               "number");
    if self.sliderProperties.slider_thickness_pixels.value ~=
        sliderThicknessPixels then
        self.sliderProperties.slider_thickness_pixels.value =
            sliderThicknessPixels;
        self.sliderProperties.slider_thickness_pixels.dirty = true;
    end
end
function UiSlider:setBlobThicknessPixels(blobThicknessPixels)
    assert(blobThicknessPixels ~= nil and type(blobThicknessPixels) == "number");
    if self.sliderProperties.blob_thickness_pixels.value ~= blobThicknessPixels then
        self.sliderProperties.blob_thickness_pixels.value = blobThicknessPixels;
        self.sliderProperties.blob_thickness_pixels.dirty = true;
    end
end
function UiSlider:setSliderType(sliderType)
    assert(sliderType ~= nil and type(sliderType) == "number" and
               (sliderType == UiSlider.UiSliderType.SLIDER_TYPE_HORIZONTAL or
                   sliderType == UiSlider.UiSliderType.SLIDER_TYPE_VERTICAL));
    if self.sliderProperties.slider_type.value ~= sliderType then
        self.sliderProperties.slider_type.value = sliderType;
        self.sliderProperties.slider_type.dirty = true;
    end
end
function UiSlider:setSliderColorHexValue(colorHex)
    assert(colorHex ~= nil and type(colorHex) == "number");

    local mask_b = 0xFF;
    local mask_g = 0xFF << 0x8;
    local mask_r = 0xFF << 0x10;

    local r = (mask_r & colorHex) >> 0x10;
    local g = (mask_g & colorHex) >> 0x8;
    local b = mask_b & colorHex;

    local INV_COLOR_MAX_BYTE_VALUE = 1.0 / 255.0;
    self:setSliderColor(r * INV_COLOR_MAX_BYTE_VALUE,
                        g * INV_COLOR_MAX_BYTE_VALUE,
                        b * INV_COLOR_MAX_BYTE_VALUE);
end

function UiSlider:setSliderColor(r, g, b)
    assert(
        r ~= nil and type(r) == "number" and g ~= nil and type(g) == "number" and
            b ~= nil and type(b) == "number" and r >= 0.0 and r <= 1.0 and g >=
            0.0 and g <= 1.0 and b >= 0.0 and b <= 1.0);

    self.sliderProperties.slider_color.value.r = r;
    self.sliderProperties.slider_color.value.g = g;
    self.sliderProperties.slider_color.value.b = b;

    self.sliderProperties.slider_color.dirty = true;
end

function UiSlider:setBlobColorHexValue(colorHex)
    assert(colorHex ~= nil and type(colorHex) == "number");

    local mask_b = 0xFF;
    local mask_g = 0xFF << 0x8;
    local mask_r = 0xFF << 0x10;

    local r = (mask_r & colorHex) >> 0x10;
    local g = (mask_g & colorHex) >> 0x8;
    local b = mask_b & colorHex;

    local INV_COLOR_MAX_BYTE_VALUE = 1.0 / 255.0;
    self:setBlobColor(r * INV_COLOR_MAX_BYTE_VALUE,
                      g * INV_COLOR_MAX_BYTE_VALUE, b * INV_COLOR_MAX_BYTE_VALUE);
end

function UiSlider:setBlobColor(r, g, b)
    assert(
        r ~= nil and type(r) == "number" and g ~= nil and type(g) == "number" and
            b ~= nil and type(b) == "number" and r >= 0.0 and r <= 1.0 and g >=
            0.0 and g <= 1.0 and b >= 0.0 and b <= 1.0);

    self.sliderProperties.blob_color.value.r = r;
    self.sliderProperties.blob_color.value.g = g;
    self.sliderProperties.blob_color.value.b = b;

    self.sliderProperties.blob_color.dirty = true;
end

function UiSlider:subscribeOnSliderValueChangedCallback(
    sliderValueChangedCallback)
    assert(sliderValueChangedCallback ~= nil and
               type(sliderValueChangedCallback) == "function");
    self.onSliderValueChangedCallbacks[(#self.onSliderValueChangedCallbacks) + 1] =
        sliderValueChangedCallback;
end
return UiSlider;
