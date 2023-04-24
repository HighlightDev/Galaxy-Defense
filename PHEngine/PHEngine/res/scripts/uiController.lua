--[[ BEGIN *** this snippet h to be inserted everywhere where your want to require custom modules *** BEGIN]]
--
local function setup()
    local str = debug.getinfo(2, "S").source:sub(2)
    local pathToCurrentScript = str:match("(.*/)")
    if pathToCurrentScript ~= nil then
        package.path = package.path .. ";" .. pathToCurrentScript .. "?.lua"
    end
end

setup()
--
--[[ END   *** this snippet has to be inserted everywhere where your want to require custom modules  ***  END]]
local EngineInputReceiver = require("core/engineInputReceiver")
local UiOverlayManager = require("core/uiOverlayManager")
local EngineEventsHolder = require("core/engineEventsHolder")
local UiCanvas = require("core/uiCanvas")
local UiOverlay = require("core/uiOverlay")
local UiItem = require("core/uiItem")
local UiRectangle = require("core/uiRectangle")
local UiLabel = require("core/uiLabel")
local UiToggleButton = require("core/uiToggleButton")

GlobalContext = {
}

UiOverlays = {
}

local pressButtonCooldown = 0.0

local function onPressedKeyboardButtons(host, keyboardPressedKeyNames)
    if keyboardPressedKeyNames ~= nil then
        for _, value in pairs(keyboardPressedKeyNames) do
            if value == "L" then
                UiOverlayManager:openOverlay(host, "TestOverlay")
            end
            if value == "Escape" then
                if pressButtonCooldown >= 0.5 then
                    pressButtonCooldown = 0.0
                    if "PauseMenu" == UiOverlayManager:getCurrentOverlayName(host) then
                        EngineEventsHolder:sendPauseGameThreadEvent(host,
                            EngineEventsHolder.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH, false)
                        UiOverlayManager:closeCurrentOverlay(host)
                    else
                        EngineEventsHolder:sendPauseGameThreadEvent(host,
                            EngineEventsHolder.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH, true)
                        UiOverlayManager:openOverlay(host, "PauseMenu");
                    end
                end
            end
        end
    end
end

--[[


            const auto &soundLabel = std::make_shared<UiLabel>("nimbus_mono");
            soundLabel->SetParents(mPauseSettingsMenuCanvas, backgroundRect);
            soundLabel->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, backgroundRect->GetName());
            soundLabel->SetAnchor(eUiAnchor::RIGHT, eUiAnchor::LEFT, soundToggleButton->GetName());
            soundLabel->SetAnchor(eUiAnchor::TOP, eUiAnchor::TOP, backgroundRect->GetName());
            soundLabel->SetAnchorMargin(eUiAnchor::LEFT, buttonHorizontalMargin);
            soundLabel->SetAnchorMargin(eUiAnchor::RIGHT, buttonHorizontalMargin);
            soundLabel->SetAnchorMargin(eUiAnchor::TOP, 50);
            soundLabel->SetHeight(buttonWidth / 4);
            soundLabel->SetText("Enable sound effects");
            soundLabel->SetTextColor(0xFFFFFF);
            soundLabel->SetFontSize(11.0f);
            soundLabel->SetTextHorizontalAlignment(eTextHorizontalAlignmentType::LEFT);
            soundLabel->SetZOrder(2);

            const auto &applyButton = std::make_shared<UiRectangle>();
            applyButton->SetParents(mPauseSettingsMenuCanvas, backgroundRect);
            applyButton->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, backgroundRect->GetName());
            applyButton->SetAnchor(eUiAnchor::BOTTOM, eUiAnchor::BOTTOM, backgroundRect->GetName());
            applyButton->SetAnchorMargin(eUiAnchor::LEFT, buttonHorizontalMargin);
            applyButton->SetAnchorMargin(eUiAnchor::BOTTOM, 50);
            applyButton->SetWidth(buttonWidth);
            applyButton->SetHeight(100);
            applyButton->SetColor(s_buttonColor);
            applyButton->SetZOrder(2);

            const auto &applyButtonLabel = std::make_shared<UiLabel>("nimbus_mono");
            applyButtonLabel->SetParents(mPauseSettingsMenuCanvas, applyButton);
            applyButtonLabel->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, applyButton->GetName());
            applyButtonLabel->SetAnchor(eUiAnchor::RIGHT, eUiAnchor::RIGHT, applyButton->GetName());
            applyButtonLabel->SetAnchor(eUiAnchor::TOP, eUiAnchor::TOP, applyButton->GetName());
            applyButtonLabel->SetAnchor(eUiAnchor::BOTTOM, eUiAnchor::BOTTOM, applyButton->GetName());
            applyButtonLabel->SetText("Apply");
            applyButtonLabel->SetTextColor(0xFFFFFF);
            applyButtonLabel->SetFontSize(20.0f);
            applyButtonLabel->SetTextHorizontalAlignment(eTextHorizontalAlignmentType::CENTER);
            applyButtonLabel->SetZOrder(3);

            const auto &cancelButton = std::make_shared<UiRectangle>();
            cancelButton->SetParents(mPauseSettingsMenuCanvas, backgroundRect);
            cancelButton->SetAnchor(eUiAnchor::LEFT, eUiAnchor::RIGHT, applyButton->GetName());
            cancelButton->SetAnchor(eUiAnchor::BOTTOM, eUiAnchor::BOTTOM, backgroundRect->GetName());
            cancelButton->SetAnchorMargin(eUiAnchor::LEFT, buttonHorizontalMargin);
            cancelButton->SetAnchorMargin(eUiAnchor::BOTTOM, 50);
            cancelButton->SetWidth(buttonWidth);
            cancelButton->SetHeight(100);
            cancelButton->SetColor(s_buttonColor);
            cancelButton->SetZOrder(2);

            const auto &cancelButtonLabel = std::make_shared<UiLabel>("nimbus_mono");
            cancelButtonLabel->SetParents(mPauseSettingsMenuCanvas, cancelButton);
            cancelButtonLabel->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, cancelButton->GetName());
            cancelButtonLabel->SetAnchor(eUiAnchor::RIGHT, eUiAnchor::RIGHT, cancelButton->GetName());
            cancelButtonLabel->SetAnchor(eUiAnchor::TOP, eUiAnchor::TOP, cancelButton->GetName());
            cancelButtonLabel->SetAnchor(eUiAnchor::BOTTOM, eUiAnchor::BOTTOM, cancelButton->GetName());
            cancelButtonLabel->SetText("Cancel");
            cancelButtonLabel->SetTextColor(0xFFFFFF);
            cancelButtonLabel->SetFontSize(20.0f);
            cancelButtonLabel->SetTextHorizontalAlignment(eTextHorizontalAlignmentType::CENTER);
            cancelButtonLabel->SetZOrder(3);
]]
local function createTestOverlay(host)
    local windowWidth = _GetWindowWidth(host)
    local windowHeight = _GetWindowHeight(host)

    local testOverlayCanvas = UiCanvas:new(host, 0, 0, windowWidth, windowHeight)
    testOverlayCanvas:subscribeOnLuaProxyReady(function(host)
        _InitializeCanvasInputSystem(host, testOverlayCanvas.luaProxyId)
    end)
    local testOverlay = UiOverlay:new(host, "TestOverlay", testOverlayCanvas)

    local menuHorizontalMargin = windowWidth / 4.0;
    local menuVerticalMargin = windowHeight / 7.0;

    local backgroundRect = UiRectangle:new(host)
    testOverlay:addWidget(backgroundRect)

    local rowButtonsCount = 2.0;
    local backgroundRectWidth = windowWidth - (menuHorizontalMargin * 2.0);
    local buttonHorizontalMargin = backgroundRectWidth / 10.0;
    local buttonWidth = (backgroundRectWidth - (buttonHorizontalMargin * (rowButtonsCount + 1.0))) / rowButtonsCount;

    local soundToggleButton = UiToggleButton:new(host, false)
    testOverlay:addWidget(soundToggleButton)

    testOverlay:subscribeOnAllWidgetLuaProxiesReady(function(host, sender)
        print("testOverlay:OnAllWidgetLuaProxiesReady => name: " .. tostring(sender.overlayName))

        backgroundRect:setParent(host, testOverlayCanvas.widgetName, testOverlayCanvas.widgetName)
        backgroundRect:setAnchor(UiItemBase.UiAnchorType.LEFT, UiItemBase.UiAnchorType.LEFT, testOverlayCanvas.widgetName, menuHorizontalMargin)
        backgroundRect:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, testOverlayCanvas.widgetName, menuHorizontalMargin)
        backgroundRect:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, testOverlayCanvas.widgetName, menuVerticalMargin)
        backgroundRect:setAnchor(UiItemBase.UiAnchorType.BOTTOM, UiItemBase.UiAnchorType.BOTTOM, testOverlayCanvas.widgetName, menuVerticalMargin)
        backgroundRect:setColorHexValue(0x6C5B7B)
        backgroundRect:setZOrder(1)

        soundToggleButton:setParent(host, testOverlayCanvas.widgetName, backgroundRect.widgetName)
        soundToggleButton:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT, backgroundRect.widgetName, buttonHorizontalMargin)
        soundToggleButton:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP, backgroundRect.widgetName, 50)
        soundToggleButton:setWidth(buttonWidth / 4.0)
        soundToggleButton:setHeight(buttonWidth / 4.0)
        soundToggleButton:setZOrder(2)
        soundToggleButton:setToggleOnColorHexValue(0xFFB732)
        soundToggleButton:setToggleOffColorHexValue(0x403649)
    end)

    return testOverlay
end

local function initialize(host)
    UiOverlays["TestOverlay"] = createTestOverlay(host)
end

function System_OnStart(host)
    local engineReceiver = EngineInputReceiver:new()
    engineReceiver.subscribeToMouseEvents = false
    engineReceiver:subscribeOnPressedKeyboardButton(onPressedKeyboardButtons)
    GlobalContext["inputReceiver"] = engineReceiver

    initialize(host)
end

function System_OnUpdate(host, deltaTimeSec)
    if math.abs(pressButtonCooldown) > 1 then
        pressButtonCooldown = 0
    end

    pressButtonCooldown = pressButtonCooldown + deltaTimeSec

    for _, value in pairs(UiOverlays) do
        value:updateFromReplicatorData(host)
    end

    for _, value in pairs(UiOverlays) do
        value:update(host, deltaTimeSec)
    end

    for _, value in pairs(GlobalContext) do
        if value.canUpdate then
            value:update(host)
        end
    end

    for _, value in pairs(UiOverlays) do
        value:sendDataToReplicator(host)
    end
end

HasOnStart = (_G["System_OnStart"] ~= nil and 1 or 0)
HasOnUpdate = (_G["System_OnUpdate"] ~= nil and 1 or 0)
