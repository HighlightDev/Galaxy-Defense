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
        package.path = package.path .. "" .. unixLikePath .. "?.lua"
    end
end

setup()
--
--[[ END   *** this snippet has to be inserted everywhere where your want to require custom modules  ***  END]]
local json = require("Ui/Core/3rdparty/json")
local UiCanvas = require("Ui/Core/uiCanvas")
local UiOverlay = require("Ui/Core/uiOverlay")
local UiItem = require("Ui/Core/uiItem")
local UiRowLayout = require("Ui/Core/uiRowLayout")
local UiRectangle = require("Ui/Core/uiRectangle")
local UiLabel = require("Ui/Core/uiLabel")
local UiImage = require("Ui/Core/uiImage")
local ImageAndLabelTile = require("Ui/Widgets/ImageAndLabelTile")
local ImageButton = require("Ui/Widgets/ImageButton")
local LabelButton = require("Ui/Widgets/LabelButton")
local Styles = require("Ui/Common/styles")
local UiTextBlock = require("Ui/Core/uiTextBlock")
local EventsHelper = require("Ui/Core/eventsHelper")
local UiOverlayManager = require("Ui/Core/uiOverlayManager")
local TowerGridPanel = require("Ui/Widgets/TowerGridPanel")
local SelectedTowerPanel = require("Ui/Widgets/SelectedTowerPanel")
local TowerUpgradesPanel = require("Ui/Widgets/TowerUpgradesPanel")
local MissileTypes = require("Ui/Common/missileTypes")

local FONT = "JetBrainsMono-VariableFont_wght"

-- Russian display names for the tower context panel title, keyed by Game::eMissileType value.
local WEAPON_DISPLAY_NAME = {
    [MissileTypes.MissileType.BOMB] = "РАКЕТА",
    [MissileTypes.MissileType.FREEZING_BOMB] = "КРИО-РАКЕТА",
    [MissileTypes.MissileType.ELECTRO_RAY] = "ЭЛЕКТРО-ЛУЧ",
    [MissileTypes.MissileType.BLACK_HOLE] = "ЧЁРНАЯ ДЫРА",
    [MissileTypes.MissileType.FREEZING_RAY] = "КРИО-ЛУЧ"
}

local LevelProgressStatusType = {NONE = 0, CURRENT_STAGE_CHANGED = 1, REQUIREMENT_TRACKERS_STATUS_CHANGED = 2}

local GameModeType = {INIT = 0, COMBAT = 1, SPACE_STATION_PLACEMENT = 2}

PlayerStatusType = {
    CRYSTALS_COUNT_CHANGED = 0,
    DESTROYED_ENEMY_SPACESHIPS_COUNT_CHANGED = 1,
    SELECTED_TOWER_CHANGED = 2,
    TOWERS_COUNT_CHANGED = 3
}

-- The combat HUD is a single overlay with two states. PREPARATION lets the player place the space
-- station / build towers, COMBAT runs the wave. Both states share the build palette and upgrade panel;
-- only a small subset of widgets is state-specific, so switching the state just toggles their visibility.
local CombatState = {PREPARATION = 0, COMBAT = 1}

local CombatHudOverlay = {
    levelProgressContainer = nil,
    levelProgressRowLayout = nil,
    CombatState = CombatState,
    preparationButtonRadius = 6
}

local RequirementTrackers = {}
local RequirementTrackersIdle = {}

local RequirementTrackerHint = nil

local TileSize = 80 -- temporary for now

local function showTileRequirementAchived(requirementTile)
    requirementTile:setTextureSource("check.png")
    requirementTile:setLabelVisibility(false)
    requirementTile:setIsFlipped(true)
    requirementTile:setImageColorHexValue(0xFFFFFF)
    requirementTile:setUseImageCustomColor(true)
end

local function showTileRequirementFailed(requirementTile)
    requirementTile:setTextureSource("cancel.png")
    requirementTile:setLabelVisibility(false)
    requirementTile:setImageColorHexValue(0xFFFFFF)
    requirementTile:setUseImageCustomColor(true)
end

local function showAllRequirementsAchived()
    for _, requirementTile in pairs(RequirementTrackers) do showTileRequirementAchived(requirementTile) end
end

local function updateRequirementTileData(host, levelProgressTile, trackerJson)
    local name = trackerJson["name"]
    if name == "DestroySpaceshipsTracker" then
        local leftSpaceshipsCount = trackerJson["left_to_destroy_spaceships_count"]
        if tonumber(leftSpaceshipsCount) > 0 then
            levelProgressTile:setLabelText(leftSpaceshipsCount)
            levelProgressTile:setTextureSource("skull.png")
        else
            showTileRequirementAchived(levelProgressTile)
        end
    elseif name == "MissedSpaceshipsTracker" then
        local doNotMissCount = tonumber(trackerJson["not_to_miss_spaceships_count"])
        local missedCount = tonumber(trackerJson["missed_spaceships_count"])
        local stillCanMiss = math.max(doNotMissCount - missedCount, 0)
        if stillCanMiss > 0 then
            levelProgressTile:setLabelText(tostring(stillCanMiss))
            levelProgressTile:setTextureSource("warning.png")
        else
            showTileRequirementFailed(levelProgressTile)
            UiOverlayManager:openOverlay(host, "LevelFailedOverlay")
            EventsHelper:sendPauseGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH, true)
        end
    else
        assert(false, "Not supported requirement: " .. name, debug.traceback())
    end
    local hintText = tostring(trackerJson["hint"])
    levelProgressTile.hintText = hintText
end

local function fillRequirementTilesPool(host, combatOverlay, count)
    for i = 1, count, 1 do
        local levelProgressTile = ImageAndLabelTile:new(host, combatOverlay)
        combatOverlay:addCompoundWidget(levelProgressTile)
        RequirementTrackersIdle[#RequirementTrackersIdle + 1] = levelProgressTile

        levelProgressTile:subscribeOnLuaProxiesReady(function(host)
            levelProgressTile:setParent(host, combatOverlay:getOverlayCanvas().widgetName,
                                        combatOverlay.levelProgressRowLayout.widgetName)
            levelProgressTile:setWidth(TileSize)
            levelProgressTile:setHeight(TileSize)
            levelProgressTile:setBackgroundTileOpacity(1.0)
            levelProgressTile:setIsVisible(false)
            levelProgressTile:setBackgroundTileColorHexValue(Styles.Combat.chipColor)
            levelProgressTile:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
                if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
                    RequirementTrackerHint:setIsVisible(true)
                    RequirementTrackerHint:setAttachTargetUiItemName(levelProgressTile.widgetName)
                    RequirementTrackerHint:setText(levelProgressTile.hintText)
                else
                    RequirementTrackerHint:setIsVisible(false)
                    RequirementTrackerHint:setAttachTargetUiItemName("")
                end
            end)
        end)
    end
end

local function updateRequirementTiles(host, combatOverlay, requirementTrackers)
    -- move active requirements tiles to idle state
    for _, activeTrackerTile in pairs(RequirementTrackers) do
        activeTrackerTile:setIsVisible(false)
        RequirementTrackersIdle[#RequirementTrackersIdle + 1] = activeTrackerTile
    end
    RequirementTrackers = {}

    local action = function()
        for index = 1, #requirementTrackers, 1 do
            local trackerTile = RequirementTrackersIdle[index]
            table.remove(RequirementTrackersIdle, index)
            RequirementTrackers[index] = trackerTile
            local trackerJson = requirementTrackers[index]
            updateRequirementTileData(host, trackerTile, trackerJson)
            trackerTile:setIsVisible(true)
        end
    end

    local predicate = function() return combatOverlay.allWidgetLuaProxiesReady end

    if predicate() then
        action()
    else
        combatOverlay:addActionWithPredicate(action, predicate)
    end
end

function CombatHudOverlay:new(host)
    local windowWidth = _GetWindowWidth(host)
    local windowHeight = _GetWindowHeight(host)

    local combatOverlayCanvas = UiCanvas:new(host, 0, 0, windowWidth, windowHeight)
    combatOverlayCanvas:subscribeOnLuaProxyReady(function(host)
        _InitializeCanvasInputSystem(host, combatOverlayCanvas.luaProxyId)
    end)
    local combatOverlay = UiOverlay:createOverlay(host, "CombatHudOverlay", combatOverlayCanvas)

    -- ─── Shared widgets (visible in both states) ──────────────────────────────
    local towerUpgradesPanel = TowerUpgradesPanel:new(host, combatOverlay)
    combatOverlay:addCompoundWidget(towerUpgradesPanel)

    local towerGridPanel = TowerGridPanel:new(host, combatOverlay, {
        buttonRadius = TowerGridPanel.DEFAULT_BUTTON_RADIUS,
        mainButtonSize = TileSize,
        onTowerUpgradesButtonClicked = function() towerUpgradesPanel:setIsVisible(true) end
    })
    combatOverlay:addCompoundWidget(towerGridPanel)

    local canCompletePreparationStage = false

    -- ─── Bottom wave-dock ──────────────────────────────────────────────────────
    -- Mockup: bottom dock. In PREPARATION it shows the incoming-wave preview and a big СТАРТ ВОЛНЫ button
    -- (which completes the build stage → COMBAT). In COMBAT it shows the wave progress bar and speed
    -- controls. Wave numbers / names / enemy counts / progress / speed have no engine data yet → placeholders.
    local waveDockPanel = UiRectangle:new(host, "CombatWaveDock")
    combatOverlay:addWidget(waveDockPanel)
    local waveInfoLabel = UiLabel:new(host, FONT, "CombatWaveInfo")
    combatOverlay:addWidget(waveInfoLabel)

    -- preparation content
    local enemyPreviewLabel = UiLabel:new(host, FONT, "CombatEnemyPreview")
    combatOverlay:addWidget(enemyPreviewLabel)
    local startWaveButton = LabelButton:new(host, combatOverlay, FONT, "CombatStartWave")
    combatOverlay:addCompoundWidget(startWaveButton)

    -- combat content
    local progressBarBg = UiRectangle:new(host, "CombatWaveProgressBg")
    combatOverlay:addWidget(progressBarBg)
    local progressBarFill = UiRectangle:new(host, "CombatWaveProgressFill")
    combatOverlay:addWidget(progressBarFill)
    local progressLegendLabel = UiLabel:new(host, FONT, "CombatWaveLegend")
    combatOverlay:addWidget(progressLegendLabel)

    local selectedSpeed = 2 -- placeholder; no game-speed API yet
    local speedPauseButton = LabelButton:new(host, combatOverlay, FONT, "CombatSpeedPause")
    combatOverlay:addCompoundWidget(speedPauseButton)
    local speedButtons = {}
    local speedValues = {1, 2, 4}
    for i = 1, #speedValues do
        local b = LabelButton:new(host, combatOverlay, FONT, "CombatSpeed" .. tostring(speedValues[i]))
        combatOverlay:addCompoundWidget(b)
        speedButtons[i] = b
    end

    -- ─── Top HUD bar (visible in both states) ─────────────────────────────────
    -- Mockup: full-width translucent bar with МЕНЮ button, resource stats (crystals/station HP/fleet),
    -- a centered wave-state pill and right-side sector/commander chips.
    local topHudBar = UiRectangle:new(host, "CombatTopHudBar")
    combatOverlay:addWidget(topHudBar)

    local menuButton = LabelButton:new(host, combatOverlay, FONT, "CombatMenuButton")
    combatOverlay:addCompoundWidget(menuButton)

    local crystalIcon = UiImage:new(host, "CombatCrystalIcon")
    combatOverlay:addWidget(crystalIcon)
    local crystalValueLabel = UiLabel:new(host, FONT, "CombatCrystalValue")
    combatOverlay:addWidget(crystalValueLabel)

    local stationIcon = UiImage:new(host, "CombatStationIcon")
    combatOverlay:addWidget(stationIcon)
    local stationHpLabel = UiLabel:new(host, FONT, "CombatStationHp")
    combatOverlay:addWidget(stationHpLabel)

    local wavePill = UiRectangle:new(host, "CombatWavePill")
    combatOverlay:addWidget(wavePill)
    local waveStateLabel = UiLabel:new(host, FONT, "CombatWaveState")
    combatOverlay:addWidget(waveStateLabel)
    local waveNumberLabel = UiLabel:new(host, FONT, "CombatWaveNumber")
    combatOverlay:addWidget(waveNumberLabel)

    local sectorChip = UiRectangle:new(host, "CombatSectorChip")
    combatOverlay:addWidget(sectorChip)
    local sectorLabel = UiLabel:new(host, FONT, "CombatSectorLabel")
    combatOverlay:addWidget(sectorLabel)

    -- ─── Side dock (left vertical rail with panel toggles) ─────────────────────
    -- Mockup: icon rail toggling the side panels. Engine mapping: АРСЕНАЛ → build-create popup,
    -- ТЕХ → tower upgrades panel (both functional); КАРТА / ЖУРНАЛ are placeholders (no engine data yet).
    local dockPanel = UiRectangle:new(host, "CombatDockPanel")
    combatOverlay:addWidget(dockPanel)
    -- dock buttons carry both an icon (UiImage on top) and a text label (LabelButton, bottom-aligned)
    local dockArsenalButton = LabelButton:new(host, combatOverlay, FONT, "CombatDockArsenal")
    combatOverlay:addCompoundWidget(dockArsenalButton)
    local dockArsenalIcon = UiImage:new(host, "CombatDockArsenalIcon")
    combatOverlay:addWidget(dockArsenalIcon)
    local dockTechButton = LabelButton:new(host, combatOverlay, FONT, "CombatDockTech")
    combatOverlay:addCompoundWidget(dockTechButton)
    local dockTechIcon = UiImage:new(host, "CombatDockTechIcon")
    combatOverlay:addWidget(dockTechIcon)
    local dockMapButton = LabelButton:new(host, combatOverlay, FONT, "CombatDockMap")
    combatOverlay:addCompoundWidget(dockMapButton)
    local dockMapIcon = UiImage:new(host, "CombatDockMapIcon")
    combatOverlay:addWidget(dockMapIcon)
    local dockLogButton = LabelButton:new(host, combatOverlay, FONT, "CombatDockLog")
    combatOverlay:addCompoundWidget(dockLogButton)
    local dockLogIcon = UiImage:new(host, "CombatDockLogIcon")
    combatOverlay:addWidget(dockLogIcon)

    -- Map / log have no backing engine panel yet, so their "active" state is purely local (placeholder).
    local dockMapActive = false
    local dockLogActive = false

    -- Combined hover + active visual for a dock button. Cached so it only replicates on actual change.
    local function refreshDockButton(d)
        local isActive = d.isActive()
        local bg = isActive and Styles.Combat.dockActiveColor
                       or (d.hovered and Styles.Combat.chipHoverColor or Styles.Combat.chipColor)
        local tint = isActive and Styles.Combat.cyanGlow or Styles.Combat.textDim
        if d.appliedBg ~= bg then
            d.button:setButtonColorHexValue(bg)
            d.appliedBg = bg
        end
        if d.appliedTint ~= tint then
            d.button:setLabelTextColorHexValue(tint)
            d.icon:setColorHexValue(tint)
            d.appliedTint = tint
        end
    end

    -- "active" reflects the real panel visibility for the wired buttons, and the local flag for placeholders.
    local dockButtons = {
        {button = dockArsenalButton, icon = dockArsenalIcon, label = "АРСЕНАЛ", iconTexture = "castle-turret.png",
         hovered = false, rotationDegrees = 180.0, isActive = function() return towerGridPanel.isCreatePanelVisible() end},
        {button = dockTechButton, icon = dockTechIcon, label = "РАЗВИТИЕ", iconTexture = "graph.png",
         hovered = false, rotationDegrees = 0.0, isActive = function() return towerUpgradesPanel.background:getIsVisible() end},
        {button = dockMapButton, icon = dockMapIcon, label = "КАРТА", iconTexture = "map-trifold.png",
         hovered = false, rotationDegrees = 0.0, isActive = function() return dockMapActive end},
        {button = dockLogButton, icon = dockLogIcon, label = "ЖУРНАЛ", iconTexture = "info.png",
         hovered = false, rotationDegrees = 0.0, isActive = function() return dockLogActive end}
    }

    for _, d in ipairs(dockButtons) do
        d.button:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
            d.hovered = (newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED)
        end)
    end

    dockArsenalButton:subscribeOnMouseInputClickedCallback(function() towerGridPanel.toggleCreatePanel() end)
    dockTechButton:subscribeOnMouseInputClickedCallback(function()
        local willShow = not towerUpgradesPanel.background:getIsVisible()
        towerUpgradesPanel:setIsVisible(willShow)
        EventsHelper:sendPauseGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH, willShow)
    end)
    dockMapButton:subscribeOnMouseInputClickedCallback(function()
        dockMapActive = not dockMapActive -- TODO: wire the tactical minimap panel when engine data is available
    end)
    dockLogButton:subscribeOnMouseInputClickedCallback(function()
        dockLogActive = not dockLogActive -- TODO: wire the combat log panel when engine data is available
    end)

    -- ─── Combat-state widgets ─────────────────────────────────────────────────
    local levelProgressContainer = UiItem:new(host, "LvlProgressContainer")
    combatOverlay:addWidget(levelProgressContainer)
    combatOverlay.levelProgressContainer = levelProgressContainer

    local selectedTowerPanel = SelectedTowerPanel:new(host, combatOverlay, "SelectedTowerPanelObj", {
        -- ДЕМОНТАЖ: relocated from the build palette — enters tower-removal mode (same broadcast as before).
        onDemolishClicked = function()
            EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.PUSH_ANYWAY,
                                                      "CombatLevelEvents",
                                                      json.encode({action = "remove_tower_marker_visibility", visible = true}))
        end,
        -- ПРОКАЧАТЬ: open the tower tech tree (same path as the dock ТЕХ button).
        onUpgradeClicked = function()
            towerUpgradesPanel:setIsVisible(true)
            EventsHelper:sendPauseGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH, true)
        end
    })
    combatOverlay:addCompoundWidget(selectedTowerPanel)

    local levelProgressRowLayout = UiRowLayout:new(host, "LvlProgressRow")
    combatOverlay:addWidget(levelProgressRowLayout)
    combatOverlay.levelProgressRowLayout = levelProgressRowLayout

    RequirementTrackerHint = UiTextBlock:new(host, "JetBrainsMono-VariableFont_wght", "TestTextBlock")
    combatOverlay:addWidget(RequirementTrackerHint)

    fillRequirementTilesPool(host, combatOverlay, 10)

    -- ─── State machine ────────────────────────────────────────────────────────
    -- Toggles which subset of widgets is visible. Event-driven combat widgets (selected tower panel,
    -- requirement tiles, hint) are force-hidden when leaving COMBAT; they are shown again by their own
    -- gameplay events while in COMBAT.
    local function applyStateVisibility(state)
        local isPreparation = (state == CombatState.PREPARATION)
        local isCombat = (state == CombatState.COMBAT)

        -- Wave-dock content is state-specific: preparation shows the wave preview + СТАРТ ВОЛНЫ; combat
        -- shows the progress bar + speed controls. The dock panel and wave-info caption stay in both.
        enemyPreviewLabel:setIsVisible(isPreparation)
        startWaveButton:setIsVisible(isPreparation)
        progressBarBg:setIsVisible(isCombat)
        progressBarFill:setIsVisible(isCombat)
        progressLegendLabel:setIsVisible(isCombat)
        speedPauseButton:setIsVisible(isCombat)
        for i = 1, #speedButtons do speedButtons[i]:setIsVisible(isCombat) end

        -- The top HUD bar (crystals/HP/wave pill/sector) stays visible in both states; only its wave-state
        -- caption changes. The crystals value is always live, so it is not gated by state.
        waveStateLabel:setText(isCombat and "АКТИВНАЯ ВОЛНА" or "ПОДГОТОВКА")

        -- The combat log dock toggle only exists during the wave (mockup parity). Its icon is parented to
        -- the dock panel (not the button), so it must be toggled explicitly alongside the button.
        dockLogButton:setIsVisible(isCombat)
        dockLogIcon:setIsVisible(isCombat)
        if not isCombat then dockLogActive = false end

        -- NOTE: do NOT toggle levelProgressContainer / levelProgressRowLayout visibility. They are
        -- non-drawing layout nodes that own the requirement tiles and the hint as children. Hiding a layout
        -- node excludes it from the C++ layout solve, so its rect goes stale and the anchored children
        -- (tiles, hint) inherit garbage positions — which manifests as a full-screen grey plate. We hide the
        -- combat HUD in PREPARATION by toggling only the leaf widgets that actually render below.

        if not isCombat then
            selectedTowerPanel:setIsVisible(false)
            RequirementTrackerHint:setIsVisible(false)
            for _, tile in pairs(RequirementTrackers) do tile:setIsVisible(false) end
            for _, tile in pairs(RequirementTrackersIdle) do tile:setIsVisible(false) end
        else
            -- re-sync combat HUD on entry: while in PREPARATION the combat-only events are gated out, so the
            -- requirement tiles must be pulled from the current game state explicitly.
            combatOverlay.onCurrentLevelProgressStageChanged()
        end
    end

    combatOverlay.currentCombatState = CombatState.PREPARATION

    combatOverlay.setCombatState = function(host, newState)
        assert(newState == CombatState.PREPARATION or newState == CombatState.COMBAT, debug.traceback())
        combatOverlay.currentCombatState = newState
        local apply = function() applyStateVisibility(newState) end
        if combatOverlay.allWidgetLuaProxiesReady then
            apply()
        else
            combatOverlay:addActionWithPredicate(apply, function() return combatOverlay.allWidgetLuaProxiesReady end)
        end
    end

    local function isInCombatState() return combatOverlay.currentCombatState == CombatState.COMBAT end

    -- ─── Wave-dock: СТАРТ ВОЛНЫ button (PREPARATION) ───────────────────────────
    -- Completes the build stage → COMBAT. Enabled only once at least one tower is placed; its enabled
    -- visual is driven from derivedUpdateCallback.
    startWaveButton:subscribeOnMouseInputClickedCallback(function()
        if not canCompletePreparationStage then return end
        towerGridPanel.hideCreatePanel()
        EventsHelper:sendChangeGameModeGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH,
                                                       GameModeType.COMBAT)
    end)
    startWaveButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if not canCompletePreparationStage then return end
        startWaveButton:setButtonColorHexValue(
            newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED and Styles.Combat.green or Styles.Combat.cyan)
    end)

    -- ─── Wave-dock: speed controls (COMBAT) — placeholders, no game-speed API yet ──────────────────
    local function refreshSpeedButtons()
        for i = 1, #speedButtons do
            speedButtons[i]:setButtonColorHexValue(
                speedValues[i] == selectedSpeed and Styles.Combat.dockActiveColor or Styles.Combat.chipColor)
            speedButtons[i]:setLabelTextColorHexValue(
                speedValues[i] == selectedSpeed and Styles.Combat.cyanGlow or Styles.Combat.textDim)
        end
    end
    for i = 1, #speedButtons do
        local value = speedValues[i]
        speedButtons[i]:subscribeOnMouseInputClickedCallback(function()
            selectedSpeed = value -- TODO: drive real game speed once the engine exposes it
            refreshSpeedButtons()
        end)
    end

    combatOverlay.onCurrentLevelProgressStageChanged = function()
        local currentProgressRequirementsCount = _GetCurrentProgressRequirementsCount(host)
        print("CombatHudOverlay::onCurrentLevelProgressStageChanged : req count: " ..
                  tostring(currentProgressRequirementsCount))

        if currentProgressRequirementsCount > 0 then
            local requirementTrackersJson = _GetCurrentProgressStageRequirementTrackers(host)
            if requirementTrackersJson ~= nil and requirementTrackersJson ~= "" then
                local parsedJson = json.decode(requirementTrackersJson)
                updateRequirementTiles(host, combatOverlay, parsedJson)
            end
        elseif #RequirementTrackers > 0 then
            showAllRequirementsAchived()
        end
    end

    combatOverlay.onRequirementTrackersStatusChanged = function()
        local requirementTrackersJson = _GetCurrentProgressStageRequirementTrackers(host)
        if requirementTrackersJson ~= nil and requirementTrackersJson ~= "" then
            local parsedJson = json.decode(requirementTrackersJson)
            if parsedJson ~= nil and parsedJson ~= "" then
                assert(#parsedJson == #RequirementTrackers, debug.traceback())
                for index, trackerJsonRoot in pairs(parsedJson) do
                    local requirementTrackerTile = RequirementTrackers[index]
                    assert(requirementTrackerTile ~= nil, debug.traceback())
                    updateRequirementTileData(host, requirementTrackerTile, trackerJsonRoot)
                end
            end
        end
    end

    combatOverlay.onWindowSizeChanged = function(width, height)
        assert(width ~= nil and type(width) == "number" and height ~= nil and type(height) == "number",
               debug.traceback())
    end

    combatOverlay:subscribeOnAllWidgetLuaProxiesReady(function()
        print("CombatHudOverlay: allWidgetLuaProxiesReady fired!")
        -- placed just below the top HUD bar (56px) so the two do not overlap
        levelProgressContainer:setParent(host, combatOverlayCanvas.widgetName, combatOverlayCanvas.widgetName)
        levelProgressContainer:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
                                         combatOverlayCanvas.widgetName, 10)
        levelProgressContainer:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP,
                                         combatOverlayCanvas.widgetName, 66)
        local levelProgressContainerWidth = windowWidth / 5.0
        levelProgressContainer:setWidth(levelProgressContainerWidth)
        levelProgressContainer:setHeight(levelProgressContainerWidth * 0.5)

        levelProgressRowLayout:setParent(host, combatOverlayCanvas.widgetName, levelProgressContainer.widgetName)
        levelProgressRowLayout:fill(levelProgressContainer.widgetName)
        levelProgressRowLayout:setSpacing(35)
        levelProgressRowLayout:setAlignment(UiRowLayout.UiRowAlignmentType.CENTER)

        RequirementTrackerHint:setParent(host, combatOverlayCanvas.widgetName, levelProgressContainer.widgetName)
        RequirementTrackerHint:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
                                         levelProgressRowLayout.widgetName, 20)
        RequirementTrackerHint:setWidth(levelProgressContainerWidth)
        RequirementTrackerHint:setHeight(levelProgressContainerWidth)
        RequirementTrackerHint:setFontSize(15)
        RequirementTrackerHint:setRectangleOpacity(1.0)
        RequirementTrackerHint:setRectangleColor(0.6, 0.6, 0.6)
        RequirementTrackerHint:setTextColorHexValue(0x000000)
        RequirementTrackerHint:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)
        RequirementTrackerHint:setTextVerticalAlignment(UiLabel.TextVerticalAlignmentType.CENTER)
        RequirementTrackerHint:setRectangleRadius(6)
        RequirementTrackerHint:setBorderColorHexValue(0xFFFFFF)
        RequirementTrackerHint:setBorderRadius(0)
        RequirementTrackerHint:setBorderOpacity(1.0)
        RequirementTrackerHint:setIsVisible(false)
        RequirementTrackerHint:setBorderThickness(40)

        -- ── Top HUD bar layout ────────────────────────────────────────────────
        local Combat = Styles.Combat
        local A = UiItemBase.UiAnchorType
        local canvasName = combatOverlayCanvas.widgetName
        local barHeight = 56

        topHudBar:setParent(host, canvasName, canvasName)
        topHudBar:setAnchor(A.TOP, A.TOP, canvasName, 0)
        topHudBar:setAnchor(A.LEFT, A.LEFT, canvasName, 0)
        topHudBar:setAnchor(A.RIGHT, A.RIGHT, canvasName, 0)
        topHudBar:setHeight(barHeight)
        topHudBar:setColorHexValue(Combat.hudBarColor)
        topHudBar:setOpacity(Combat.hudBarOpacity)
        topHudBar:setLayer(UiItemBase.UiLayer.HUD)
        topHudBar:setIsVisible(true)

        -- МЕНЮ button — opens the pause overlay (same path as the Escape handler).
        menuButton:setParent(host, canvasName, topHudBar.widgetName)
        menuButton:setWidth(86)
        menuButton:setHeight(32)
        menuButton:setAnchor(A.LEFT, A.LEFT, topHudBar.widgetName, 16)
        menuButton:setAnchor(A.VERTICAL_CENTER, A.VERTICAL_CENTER, topHudBar.widgetName, 0)
        menuButton:setButtonColorHexValue(Combat.chipColor)
        menuButton:setButtonBorderRadius(Combat.chipBorderRadius)
        menuButton:setLabelText("МЕНЮ")
        menuButton:setLabelFontSize(13)
        menuButton:setLabelTextColorHexValue(Combat.textBright)
        menuButton:setLabelTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)
        menuButton:setLabelTextVerticalAlignment(UiLabel.TextVerticalAlignmentType.CENTER)
        menuButton:setIsVisible(true)
        menuButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
            if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
                menuButton:setButtonColorHexValue(Combat.chipHoverColor)
            else
                menuButton:setButtonColorHexValue(Combat.chipColor)
            end
        end)
        menuButton:subscribeOnMouseInputClickedCallback(function()
            EventsHelper:sendPauseGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH, true)
            UiOverlayManager:openOverlay(host, "PauseMenuOverlay")
        end)

        -- Crystals (live data).
        crystalIcon:setParent(host, canvasName, topHudBar.widgetName)
        crystalIcon:setAnchor(A.LEFT, A.RIGHT, menuButton.widgetName, 18)
        crystalIcon:setAnchor(A.VERTICAL_CENTER, A.VERTICAL_CENTER, topHudBar.widgetName, 0)
        crystalIcon:setWidth(22)
        crystalIcon:setHeight(22)
        crystalIcon:setRotationDegrees(180)
        crystalIcon:setTextureSource("diamond.png")
        crystalIcon:setUseImageCustomColor(true)
        crystalIcon:setColorHexValue(Combat.cyanGlow)
        crystalIcon:setIsVisible(true)

        crystalValueLabel:setParent(host, canvasName, topHudBar.widgetName)
        crystalValueLabel:setAnchor(A.LEFT, A.RIGHT, crystalIcon.widgetName, 8)
        crystalValueLabel:setAnchor(A.VERTICAL_CENTER, A.VERTICAL_CENTER, topHudBar.widgetName, 0)
        crystalValueLabel:setWidth(90)
        crystalValueLabel:setHeight(28)
        crystalValueLabel:setFontSize(16)
        crystalValueLabel:setTextColorHexValue(Combat.cyanGlow)
        crystalValueLabel:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.LEFT)
        crystalValueLabel:setTextVerticalAlignment(UiLabel.TextVerticalAlignmentType.CENTER)
        crystalValueLabel:setText(tostring(_GetCollectedCrystalsCount(host)))
        crystalValueLabel:setIsVisible(true)

        -- Station HP — TODO: wire real station integrity from the engine once it is exposed to Lua.
        stationIcon:setParent(host, canvasName, topHudBar.widgetName)
        stationIcon:setAnchor(A.LEFT, A.RIGHT, crystalValueLabel.widgetName, 14)
        stationIcon:setAnchor(A.VERTICAL_CENTER, A.VERTICAL_CENTER, topHudBar.widgetName, 0)
        stationIcon:setWidth(22)
        stationIcon:setHeight(22)
        stationIcon:setTextureSource("space_station_img.png")
        stationIcon:setUseImageCustomColor(true)
        stationIcon:setColorHexValue(Combat.cyan)
        stationIcon:setIsVisible(true)

        stationHpLabel:setParent(host, canvasName, topHudBar.widgetName)
        stationHpLabel:setAnchor(A.LEFT, A.RIGHT, stationIcon.widgetName, 8)
        stationHpLabel:setAnchor(A.VERTICAL_CENTER, A.VERTICAL_CENTER, topHudBar.widgetName, 0)
        stationHpLabel:setWidth(70)
        stationHpLabel:setHeight(28)
        stationHpLabel:setFontSize(16)
        stationHpLabel:setTextColorHexValue(Combat.cyan)
        stationHpLabel:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.LEFT)
        stationHpLabel:setTextVerticalAlignment(UiLabel.TextVerticalAlignmentType.CENTER)
        stationHpLabel:setText("100%")
        stationHpLabel:setIsVisible(true)

        -- Center wave-state pill: caption is live (PREPARATION/COMBAT), the number is a placeholder.
        wavePill:setParent(host, canvasName, topHudBar.widgetName)
        wavePill:setAnchor(A.HORIZONTAL_CENTER, A.HORIZONTAL_CENTER, topHudBar.widgetName, 0)
        wavePill:setAnchor(A.VERTICAL_CENTER, A.VERTICAL_CENTER, topHudBar.widgetName, 0)
        wavePill:setWidth(260)
        wavePill:setHeight(34)
        wavePill:setColorHexValue(Combat.chipColor)
        wavePill:setOpacity(Combat.chipOpacity)
        wavePill:setBorderRadius(Combat.chipBorderRadius)
        wavePill:setIsVisible(true)

        waveStateLabel:setParent(host, canvasName, wavePill.widgetName)
        waveStateLabel:setAnchor(A.LEFT, A.LEFT, wavePill.widgetName, 14)
        waveStateLabel:setAnchor(A.VERTICAL_CENTER, A.VERTICAL_CENTER, wavePill.widgetName, 0)
        waveStateLabel:setWidth(150)
        waveStateLabel:setHeight(26)
        waveStateLabel:setFontSize(13)
        waveStateLabel:setTextColorHexValue(Combat.cyanGlow)
        waveStateLabel:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.LEFT)
        waveStateLabel:setTextVerticalAlignment(UiLabel.TextVerticalAlignmentType.CENTER)
        waveStateLabel:setText("ПОДГОТОВКА")
        waveStateLabel:setIsVisible(true)

        -- TODO: wire current/total wave numbers from the engine once exposed to Lua.
        waveNumberLabel:setParent(host, canvasName, wavePill.widgetName)
        waveNumberLabel:setAnchor(A.RIGHT, A.RIGHT, wavePill.widgetName, 14)
        waveNumberLabel:setAnchor(A.VERTICAL_CENTER, A.VERTICAL_CENTER, wavePill.widgetName, 0)
        waveNumberLabel:setWidth(80)
        waveNumberLabel:setHeight(26)
        waveNumberLabel:setFontSize(14)
        waveNumberLabel:setTextColorHexValue(Combat.textBright)
        waveNumberLabel:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.RIGHT)
        waveNumberLabel:setTextVerticalAlignment(UiLabel.TextVerticalAlignmentType.CENTER)
        waveNumberLabel:setText("14 / 30")
        waveNumberLabel:setIsVisible(true)

        -- Right-side sector chip — TODO: wire real sector name from the engine once exposed to Lua.
        sectorChip:setParent(host, canvasName, topHudBar.widgetName)
        sectorChip:setAnchor(A.RIGHT, A.RIGHT, topHudBar.widgetName, 16)
        sectorChip:setAnchor(A.VERTICAL_CENTER, A.VERTICAL_CENTER, topHudBar.widgetName, 0)
        sectorChip:setWidth(200)
        sectorChip:setHeight(34)
        sectorChip:setColorHexValue(Combat.chipColor)
        sectorChip:setOpacity(Combat.chipOpacity)
        sectorChip:setBorderRadius(Combat.chipBorderRadius)
        sectorChip:setIsVisible(true)

        sectorLabel:setParent(host, canvasName, sectorChip.widgetName)
        sectorLabel:fill(sectorChip.widgetName)
        sectorLabel:setFontSize(12)
        sectorLabel:setTextColorHexValue(Combat.textDim)
        sectorLabel:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)
        sectorLabel:setTextVerticalAlignment(UiLabel.TextVerticalAlignmentType.CENTER)
        sectorLabel:setText("СЕКТОР · КОТЭЛИН-IV")
        sectorLabel:setIsVisible(true)

        -- ── Side dock layout ──────────────────────────────────────────────────
        local dockButtonW = 72
        local dockButtonH = 58
        local dockSpacing = 10
        local dockPad = 10
        local dockPanelW = dockButtonW + dockPad * 2
        local dockPanelH = dockButtonH * 4 + dockSpacing * 3 + dockPad * 2

        dockPanel:setParent(host, canvasName, canvasName)
        dockPanel:setAnchor(A.LEFT, A.LEFT, canvasName, 12)
        dockPanel:setAnchor(A.VERTICAL_CENTER, A.VERTICAL_CENTER, canvasName, 0)
        dockPanel:setWidth(dockPanelW)
        dockPanel:setHeight(dockPanelH)
        dockPanel:setColorHexValue(Combat.panelColor)
        dockPanel:setOpacity(Combat.panelOpacity)
        dockPanel:setBorderRadius(Combat.panelBorderRadius)
        dockPanel:setLayer(UiItemBase.UiLayer.HUD)
        dockPanel:setIsVisible(true)

        for i, d in ipairs(dockButtons) do
            local btn = d.button
            btn:setParent(host, canvasName, dockPanel.widgetName)
            btn:setWidth(dockButtonW)
            btn:setHeight(dockButtonH)
            btn:setAnchor(A.HORIZONTAL_CENTER, A.HORIZONTAL_CENTER, dockPanel.widgetName, 0)
            if i == 1 then
                btn:setAnchor(A.TOP, A.TOP, dockPanel.widgetName, dockPad)
            else
                btn:setAnchor(A.TOP, A.BOTTOM, dockButtons[i - 1].button.widgetName, dockSpacing)
            end
            btn:setButtonColorHexValue(Combat.chipColor)
            btn:setButtonBorderRadius(Combat.chipBorderRadius)
            btn:setLabelText(d.label)
            btn:setLabelFontSize(9)
            btn:setLabelTextColorHexValue(Combat.textDim)
            btn:setLabelTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)
            btn:setLabelTextVerticalAlignment(UiLabel.TextVerticalAlignmentType.BOTTOM)
            btn:setIsVisible(true)

            -- icon sits on top of the button, label sits at the bottom. The icon is parented to the dock
            -- panel (already attached) rather than the button — the button's background tile is only
            -- attached later in its onCompoundWidgetInitialize, so it is not yet a valid parent here.
            d.icon:setParent(host, canvasName, dockPanel.widgetName)
            d.icon:setAnchor(A.HORIZONTAL_CENTER, A.HORIZONTAL_CENTER, btn.widgetName, 0)
            d.icon:setAnchor(A.TOP, A.TOP, btn.widgetName, 7)
            d.icon:setWidth(26)
            d.icon:setHeight(26)
            d.icon:setTextureSource(d.iconTexture)
            d.icon:setUseImageCustomColor(true)
            d.icon:setColorHexValue(Combat.textDim)
            d.icon:setZOrder(5)
            d.icon:setIsVisible(true)
            d.icon:setRotationDegrees(d.rotationDegrees)
        end

        -- Tower context panel docked to the right edge (mockup right shell).
        selectedTowerPanel:setParent(host, combatOverlayCanvas.widgetName, combatOverlayCanvas.widgetName)
        selectedTowerPanel:setAnchor(A.RIGHT, A.RIGHT, combatOverlayCanvas.widgetName, 16)
        selectedTowerPanel:setAnchor(A.VERTICAL_CENTER, A.VERTICAL_CENTER, combatOverlayCanvas.widgetName, 0)
        selectedTowerPanel:setWidth(270)
        selectedTowerPanel:setHeight(280)
        selectedTowerPanel:setupLayout()
        selectedTowerPanel:setIsVisible(false)

        -- ── Bottom wave-dock layout ───────────────────────────────────────────
        local HALIGN = UiLabel.TextHorizontalAlignmentType
        local VALIGN = UiLabel.TextVerticalAlignmentType
        local waveDockW = 720
        local waveDockH = 92

        waveDockPanel:setParent(host, canvasName, canvasName)
        waveDockPanel:setAnchor(A.HORIZONTAL_CENTER, A.HORIZONTAL_CENTER, canvasName, 0)
        waveDockPanel:setAnchor(A.BOTTOM, A.BOTTOM, canvasName, 16)
        waveDockPanel:setWidth(waveDockW)
        waveDockPanel:setHeight(waveDockH)
        waveDockPanel:setColorHexValue(Combat.hudBarColor)
        waveDockPanel:setOpacity(Combat.hudBarOpacity)
        waveDockPanel:setBorderRadius(Combat.panelBorderRadius)
        waveDockPanel:setLayer(UiItemBase.UiLayer.HUD)
        waveDockPanel:setIsVisible(true)

        -- wave info caption (both states) — TODO: real wave number / name
        waveInfoLabel:setParent(host, canvasName, waveDockPanel.widgetName)
        waveInfoLabel:setAnchor(A.LEFT, A.LEFT, waveDockPanel.widgetName, 20)
        waveInfoLabel:setAnchor(A.TOP, A.TOP, waveDockPanel.widgetName, 12)
        waveInfoLabel:setWidth(440)
        waveInfoLabel:setHeight(22)
        waveInfoLabel:setFontSize(15)
        waveInfoLabel:setTextColorHexValue(Combat.cyanGlow)
        waveInfoLabel:setTextHorizontalAlignment(HALIGN.LEFT)
        waveInfoLabel:setTextVerticalAlignment(VALIGN.CENTER)
        waveInfoLabel:setText("ВОЛНА 14 / 30 · «РАЗВЕДКА БЕЗДНЫ»")
        waveInfoLabel:setIsVisible(true)

        -- preparation content: enemy preview + СТАРТ ВОЛНЫ (TODO: real enemy preview)
        enemyPreviewLabel:setParent(host, canvasName, waveDockPanel.widgetName)
        enemyPreviewLabel:setAnchor(A.LEFT, A.LEFT, waveDockPanel.widgetName, 20)
        enemyPreviewLabel:setAnchor(A.BOTTOM, A.BOTTOM, waveDockPanel.widgetName, 14)
        enemyPreviewLabel:setWidth(460)
        enemyPreviewLabel:setHeight(20)
        enemyPreviewLabel:setFontSize(12)
        enemyPreviewLabel:setTextColorHexValue(Combat.textDim)
        enemyPreviewLabel:setTextHorizontalAlignment(HALIGN.LEFT)
        enemyPreviewLabel:setTextVerticalAlignment(VALIGN.CENTER)
        enemyPreviewLabel:setText("ИСТРЕБ. ×18    БОМБАРД. ×6    ДРОНЫ ×24    ЭЛИТА ×2")
        enemyPreviewLabel:setIsVisible(true)

        startWaveButton:setParent(host, canvasName, waveDockPanel.widgetName)
        startWaveButton:setWidth(190)
        startWaveButton:setHeight(60)
        startWaveButton:setAnchor(A.RIGHT, A.RIGHT, waveDockPanel.widgetName, 16)
        startWaveButton:setAnchor(A.VERTICAL_CENTER, A.VERTICAL_CENTER, waveDockPanel.widgetName, 0)
        startWaveButton:setButtonColorHexValue(Combat.cyan)
        startWaveButton:setButtonBorderRadius(Combat.chipBorderRadius)
        startWaveButton:setLabelText("СТАРТ ВОЛНЫ")
        startWaveButton:setLabelFontSize(15)
        startWaveButton:setLabelTextColorHexValue(Combat.textBright)
        startWaveButton:setLabelTextHorizontalAlignment(HALIGN.CENTER)
        startWaveButton:setLabelTextVerticalAlignment(VALIGN.CENTER)
        startWaveButton:setIsVisible(true)

        -- combat content: progress bar + legend (TODO: real progress) + speed controls (placeholder)
        progressLegendLabel:setParent(host, canvasName, waveDockPanel.widgetName)
        progressLegendLabel:setAnchor(A.LEFT, A.LEFT, waveDockPanel.widgetName, 20)
        progressLegendLabel:setAnchor(A.BOTTOM, A.BOTTOM, waveDockPanel.widgetName, 30)
        progressLegendLabel:setWidth(440)
        progressLegendLabel:setHeight(18)
        progressLegendLabel:setFontSize(11)
        progressLegendLabel:setTextColorHexValue(Combat.textDim)
        progressLegendLabel:setTextHorizontalAlignment(HALIGN.LEFT)
        progressLegendLabel:setTextVerticalAlignment(VALIGN.CENTER)
        progressLegendLabel:setText("ВРАГОВ 50 / 86 · СЛЕД. ВОЛНА 02:14")
        progressLegendLabel:setIsVisible(true)

        progressBarBg:setParent(host, canvasName, waveDockPanel.widgetName)
        progressBarBg:setAnchor(A.LEFT, A.LEFT, waveDockPanel.widgetName, 20)
        progressBarBg:setAnchor(A.BOTTOM, A.BOTTOM, waveDockPanel.widgetName, 16)
        progressBarBg:setWidth(440)
        progressBarBg:setHeight(10)
        progressBarBg:setColorHexValue(Combat.chipColor)
        progressBarBg:setBorderRadius(5)
        progressBarBg:setIsVisible(true)

        progressBarFill:setParent(host, canvasName, progressBarBg.widgetName)
        progressBarFill:setAnchor(A.LEFT, A.LEFT, progressBarBg.widgetName, 0)
        progressBarFill:setAnchor(A.TOP, A.TOP, progressBarBg.widgetName, 0)
        progressBarFill:setAnchor(A.BOTTOM, A.BOTTOM, progressBarBg.widgetName, 0)
        progressBarFill:setWidth(440 * 0.42)
        progressBarFill:setColorHexValue(Combat.cyanGlow)
        progressBarFill:setBorderRadius(5)
        progressBarFill:setIsVisible(true)

        -- speed controls: pause + ×1 / ×2 / ×4, right-aligned row
        local speedBtnSize = 42
        local speedGap = 6
        speedButtons[3]:setAnchor(A.RIGHT, A.RIGHT, waveDockPanel.widgetName, 16)
        speedButtons[2]:setAnchor(A.RIGHT, A.LEFT, speedButtons[3].widgetName, speedGap)
        speedButtons[1]:setAnchor(A.RIGHT, A.LEFT, speedButtons[2].widgetName, speedGap)
        speedPauseButton:setAnchor(A.RIGHT, A.LEFT, speedButtons[1].widgetName, speedGap)
        for _, sb in ipairs({speedPauseButton, speedButtons[1], speedButtons[2], speedButtons[3]}) do
            sb:setParent(host, canvasName, waveDockPanel.widgetName)
            sb:setWidth(speedBtnSize)
            sb:setHeight(speedBtnSize)
            sb:setAnchor(A.VERTICAL_CENTER, A.VERTICAL_CENTER, waveDockPanel.widgetName, 0)
            sb:setButtonColorHexValue(Combat.chipColor)
            sb:setButtonBorderRadius(Combat.chipBorderRadius)
            sb:setLabelFontSize(13)
            sb:setLabelTextColorHexValue(Combat.textDim)
            sb:setLabelTextHorizontalAlignment(HALIGN.CENTER)
            sb:setLabelTextVerticalAlignment(VALIGN.CENTER)
            sb:setIsVisible(true)
        end
        speedPauseButton:setLabelText("II")
        for i = 1, #speedButtons do speedButtons[i]:setLabelText("×" .. tostring(speedValues[i])) end
        refreshSpeedButtons()

        towerGridPanel:setupLayout(combatOverlayCanvas.widgetName)
        towerGridPanel:updateAffordability(_GetCollectedCrystalsCount(host))
        towerGridPanel:setControlBarVisible(false) -- build controls now live in the side dock

        towerUpgradesPanel:setupLayout(combatOverlayCanvas.widgetName, windowWidth, windowHeight)

        applyStateVisibility(combatOverlay.currentCombatState)
    end)

    combatOverlay.onGameEventTriggered = function(eventName, jsonArgs)
        if "LevelProgressChanged" == eventName then
            if not isInCombatState() then return end
            assert(jsonArgs ~= nil and type(jsonArgs) == "string", debug.traceback())
            local parsedJson = json.decode(jsonArgs)
            if parsedJson["level_progress_status_type"] ~= nil then
                local lvlProgressStatusType = tonumber(parsedJson["level_progress_status_type"])
                if lvlProgressStatusType == LevelProgressStatusType.CURRENT_STAGE_CHANGED then
                    combatOverlay.onCurrentLevelProgressStageChanged()
                elseif lvlProgressStatusType == LevelProgressStatusType.REQUIREMENT_TRACKERS_STATUS_CHANGED then
                    combatOverlay.onRequirementTrackersStatusChanged()
                end
            end
        elseif "PlayerStatusChanged" == eventName then
            assert(jsonArgs ~= nil and type(jsonArgs) == "string", debug.traceback())
            local parsedJson = json.decode(jsonArgs)
            if parsedJson["player_status_type"] ~= nil then
                local statusType = tonumber(parsedJson["player_status_type"])
                if statusType == PlayerStatusType.TOWERS_COUNT_CHANGED then
                    -- preparation gating: enables the complete-stage button once at least one tower exists
                    if parsedJson["towers_count"] ~= nil then
                        local newTowersCount = tonumber(parsedJson["towers_count"])
                        canCompletePreparationStage = newTowersCount > 0
                    end
                elseif statusType == PlayerStatusType.CRYSTALS_COUNT_CHANGED then
                    local crystalsCount = tonumber(parsedJson["crystals_count"])
                    crystalValueLabel:setText(tostring(crystalsCount))
                    towerGridPanel:updateAffordability(crystalsCount)
                elseif statusType == PlayerStatusType.SELECTED_TOWER_CHANGED then
                    -- tower context panel works in both states (select a placed tower to demolish / upgrade)
                    if parsedJson["has_selected_tower"] ~= nil then
                        if parsedJson["has_selected_tower"] == true then
                            assert(parsedJson["tower_weapon_type"] ~= nil, debug.traceback())
                            local weaponType = tonumber(parsedJson["tower_weapon_type"])
                            local imageToSet = MissileTypes.IconByValue[weaponType] or "space_station_img.png"
                            selectedTowerPanel:setWeaponImage(imageToSet)
                            selectedTowerPanel:setTowerTitle(WEAPON_DISPLAY_NAME[weaponType] or "БАШНЯ")
                            selectedTowerPanel:setIsVisible(true)
                        else
                            selectedTowerPanel:setIsVisible(false)
                        end
                    end
                end
            end
        end
    end

    combatOverlay.onEngineEventTriggered = function(eventName, jsonArgs)
        if "WindowSizeChanged" == eventName and combatOverlay.allWidgetLuaProxiesReady == true then
            assert(jsonArgs ~= nil and type(jsonArgs) == "string", debug.traceback())
            local parsedJson = json.decode(jsonArgs)
            local windowSize = {}
            if parsedJson["width"] ~= nil then windowSize.width = tonumber(parsedJson["width"]) end
            if parsedJson["height"] ~= nil then windowSize.height = tonumber(parsedJson["height"]) end
            combatOverlay.onWindowSizeChanged(windowSize.width, windowSize.height)
        end
    end

    local appliedStartEnabled = nil
    combatOverlay.derivedUpdateCallback = function()
        if combatOverlay.allWidgetLuaProxiesReady then
            -- СТАРТ ВОЛНЫ enabled visual (bright cyan + clickable) vs disabled (dim) — applied on change only
            if appliedStartEnabled ~= canCompletePreparationStage then
                appliedStartEnabled = canCompletePreparationStage
                startWaveButton:setButtonColorHexValue(
                    canCompletePreparationStage and Styles.Combat.cyan or Styles.Combat.chipColor)
                startWaveButton:setLabelTextColorHexValue(
                    canCompletePreparationStage and Styles.Combat.textBright or Styles.Combat.textVeryDim)
            end
            -- keep the dock highlights in sync with the real panel visibility (handles panels closed
            -- via their own controls, not just via the dock)
            for _, d in ipairs(dockButtons) do refreshDockButton(d) end
        end
    end

    combatOverlay.onBroadcastEventTriggered = function(eventName, jsonArgs)
        if "CombatLevelEvents" == eventName and combatOverlay.allWidgetLuaProxiesReady == true then
            assert(jsonArgs ~= nil and type(jsonArgs) == "string", debug.traceback())
            local parsedJson = json.decode(jsonArgs)
            if parsedJson["action"] ~= nil then
                local action = tostring(parsedJson["action"])
                if action == "switch_mode" then
                    local mode = tostring(parsedJson["mode"])
                    if mode == "IDLE" then towerGridPanel:handleBroadcastSwitchModeIdle(host) end
                end
            end
        end
    end

    return combatOverlay
end

return CombatHudOverlay
