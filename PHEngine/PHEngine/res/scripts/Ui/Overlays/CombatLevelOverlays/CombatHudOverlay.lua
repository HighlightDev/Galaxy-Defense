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
local ObjectivesPanel = require("Ui/Widgets/ObjectivesPanel")
local ImageButton = require("Ui/Widgets/ImageButton")
local LabelButton = require("Ui/Widgets/LabelButton")
local Styles = require("Ui/Common/styles")
local UiTextBlock = require("Ui/Core/uiTextBlock")
local EventsHelper = require("Ui/Core/eventsHelper")
local UiOverlayManager = require("Ui/Core/uiOverlayManager")
local TowerGridPanel = require("Ui/Widgets/TowerGridPanel")
local SelectedTowerPanel = require("Ui/Widgets/SelectedTowerPanel")
local SelectedBarrierPanel = require("Ui/Widgets/SelectedBarrierPanel")
local TowerUpgradesPanel = require("Ui/Widgets/TowerUpgradesPanel")
local MissileTypes = require("Ui/Common/missileTypes")

local FONT = "JetBrainsMono-VariableFont_wght"

-- Russian display names for the tower context panel title, keyed by Game::eMissileType value.
local WEAPON_DISPLAY_NAME = {
    [MissileTypes.MissileType.BOMB] = "РАКЕТА",
    [MissileTypes.MissileType.FREEZING_BOMB] = "КРИО-РАКЕТА",
    [MissileTypes.MissileType.ELECTRO_RAY] = "ЭЛЕКТРО-ЛУЧ",
    [MissileTypes.MissileType.BLACK_HOLE] = "ЧЁРНАЯ ДЫРА",
    [MissileTypes.MissileType.FREEZING_RAY] = "КРИО-ЛУЧ",
    [MissileTypes.MissileType.PLASMA_BOMB] = "ПЛАЗМА-БОМБА",
    [MissileTypes.MissileType.REPAIR_BEAM] = "РЕМОНТ-ЛУЧ",
    [MissileTypes.MissileType.FORCE_BARRIER] = "СИЛОВОЙ ЩИТ"
}

local LevelProgressStatusType = {NONE = 0, CURRENT_STAGE_CHANGED = 1, REQUIREMENT_TRACKERS_STATUS_CHANGED = 2}

local GameModeType = {INIT = 0, COMBAT = 1, SPACE_STATION_PLACEMENT = 2}

PlayerStatusType = {
    CRYSTALS_COUNT_CHANGED = 0,
    DESTROYED_ENEMY_SPACESHIPS_COUNT_CHANGED = 1,
    SELECTED_SPACE_OBJECT_CHANGED = 2,
    TOWERS_COUNT_CHANGED = 3
}

-- The combat HUD is a single overlay with two states. PREPARATION lets the player place the space
-- station / build towers, COMBAT runs the wave. Both states share the build palette and upgrade panel;
-- only a small subset of widgets is state-specific, so switching the state just toggles their visibility.
local CombatState = {PREPARATION = 0, COMBAT = 1}

local CombatHudOverlay = {CombatState = CombatState, preparationButtonRadius = 6}

local TileSize = 80 -- tower grid main button size (px)

-- Default objective labels per tracker type, used when the engine provides no hint text.
local OBJECTIVE_DEFAULT_LABEL = {
    DestroySpaceshipsTracker = "Уничтожить корабли",
    MissedSpaceshipsTracker = "Не пропустить корабли"
}

-- Maps one requirement-tracker JSON entry to an ObjectivesPanel item descriptor
-- ({icon, label, value, color, done, failed}); see ObjectivesPanel:setItems.
local function trackerToObjectiveItem(trackerJson)
    local name = trackerJson["name"]
    local hint = trackerJson["hint"]
    if hint == nil or hint == "" or hint == "nil" then hint = nil end
    local label = hint or OBJECTIVE_DEFAULT_LABEL[name] or tostring(name)

    if name == "DestroySpaceshipsTracker" then
        local left = tonumber(trackerJson["left_to_destroy_spaceships_count"]) or 0
        if left > 0 then
            return {icon = "skull.png", label = label, value = tostring(left), color = Styles.Combat.cyanGlow}
        end
        return {icon = "check.png", label = label, color = 0x86efac, done = true}
    elseif name == "MissedSpaceshipsTracker" then
        local doNotMissCount = tonumber(trackerJson["not_to_miss_spaceships_count"]) or 0
        local missedCount = tonumber(trackerJson["missed_spaceships_count"]) or 0
        local stillCanMiss = math.max(doNotMissCount - missedCount, 0)
        if stillCanMiss > 0 then
            return {icon = "warning.png", label = label, value = tostring(stillCanMiss), color = Styles.Combat.danger}
        end
        return {icon = "cancel.png", label = label, color = Styles.Combat.danger, failed = true}
    end

    assert(false, "Not supported requirement: " .. tostring(name), debug.traceback())
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

    local selectedSpeed = _GetPlaySpeed(host)
    local speedPauseButton = ImageButton:new(host, combatOverlay, "CombatSpeedPause")
    combatOverlay:addCompoundWidget(speedPauseButton)
    local speedButtons = {}
    local speedValues = {1, 2, 4}
    local isPaused = false
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

    -- Forward declarations: the objectives panel + its rebuild fn are created after the dock, but the
    -- ЗАДАЧИ dock toggle references them, so declare them up front and assign below.
    local objectivesPanel
    local rebuildObjectives
    -- selectedTowerPanel's own onDemolishClicked closure needs to hide the panel, so it must reference
    -- the variable; declare it before construction (the local's scope would otherwise start only after
    -- the assignment statement, leaving the closure bound to a nil global).
    local selectedTowerPanel
    -- Same closure/scoping reason as selectedTowerPanel above (its onDemolishClicked hides the panel).
    local selectedBarrierPanel
    local objectivesActive = true -- objectives visible by default in combat (mockup parity)

    -- ─── Side dock (left vertical rail with panel toggles) ─────────────────────
    -- Mockup: icon rail toggling the side panels. Engine mapping: АРСЕНАЛ → build-create popup,
    -- ЗАДАЧИ → objectives panel, ТЕХ → tower upgrades panel (functional); КАРТА / ЖУРНАЛ are
    -- placeholders (no engine data yet).
    local dockPanel = UiRectangle:new(host, "CombatDockPanel")
    combatOverlay:addWidget(dockPanel)
    -- dock buttons carry both an icon (UiImage on top) and a text label (LabelButton, bottom-aligned)
    local dockArsenalButton = LabelButton:new(host, combatOverlay, FONT, "CombatDockArsenal")
    combatOverlay:addCompoundWidget(dockArsenalButton)
    local dockArsenalIcon = UiImage:new(host, "CombatDockArsenalIcon")
    combatOverlay:addWidget(dockArsenalIcon)
    local dockObjButton = LabelButton:new(host, combatOverlay, FONT, "CombatDockObj")
    combatOverlay:addCompoundWidget(dockObjButton)
    local dockObjIcon = UiImage:new(host, "CombatDockObjIcon")
    combatOverlay:addWidget(dockObjIcon)
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
        local bg = isActive and Styles.Combat.dockActiveColor or
                       (d.hovered and Styles.Combat.chipHoverColor or Styles.Combat.chipColor)
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
        {
            button = dockArsenalButton,
            icon = dockArsenalIcon,
            label = "АРСЕНАЛ",
            iconTexture = "castle-turret.png",
            hovered = false,
            rotationDegrees = 180.0,
            isActive = function() return towerGridPanel.isCreatePanelVisible() end
        }, {
            button = dockObjButton,
            icon = dockObjIcon,
            label = "ЗАДАЧИ",
            iconTexture = "flag-banner-fold.png",
            hovered = false,
            rotationDegrees = 180.0,
            isActive = function() return objectivesActive end
        }, {
            button = dockTechButton,
            icon = dockTechIcon,
            label = "РАЗВИТИЕ",
            iconTexture = "graph.png",
            hovered = false,
            rotationDegrees = 0.0,
            isActive = function() return towerUpgradesPanel.background:getIsVisible() end
        }, {
            button = dockMapButton,
            icon = dockMapIcon,
            label = "КАРТА",
            iconTexture = "map-trifold.png",
            hovered = false,
            rotationDegrees = 0.0,
            isActive = function() return dockMapActive end
        }, {
            button = dockLogButton,
            icon = dockLogIcon,
            label = "ЖУРНАЛ",
            iconTexture = "info.png",
            hovered = false,
            rotationDegrees = 0.0,
            isActive = function() return dockLogActive end
        }
    }

    for _, d in ipairs(dockButtons) do
        d.button:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
            d.hovered = (newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED)
        end)
    end

    dockArsenalButton:subscribeOnMouseInputClickedCallback(function() towerGridPanel.toggleCreatePanel() end)
    dockObjButton:subscribeOnMouseInputClickedCallback(function()
        objectivesActive = not objectivesActive
        objectivesPanel:setIsVisible(objectivesActive)
        if objectivesActive then rebuildObjectives() end
    end)
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
    selectedTowerPanel = SelectedTowerPanel:new(host, combatOverlay, "SelectedTowerPanelObj", {
        -- ДЕМОНТАЖ: relocated from the build palette — enters tower-removal mode (same broadcast as before).
        onDemolishClicked = function()
            selectedTowerPanel:setIsVisible(false)
            EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.PUSH_ANYWAY,
                                                      "CombatLevelEvents", json.encode({action = "remove_tower"}))
        end,
        -- ПРОКАЧАТЬ: open the tower tech tree (same path as the dock ТЕХ button).
        onUpgradeClicked = function()
            towerUpgradesPanel:setIsVisible(true)
            EventsHelper:sendPauseGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH, true)
        end
    })
    combatOverlay:addCompoundWidget(selectedTowerPanel)

    -- Barrier context panel: trimmed variant of the tower panel with only the ДЕМОНТАЖ action.
    selectedBarrierPanel = SelectedBarrierPanel:new(host, combatOverlay, "SelectedBarrierPanelObj", {
        onDemolishClicked = function()
            selectedBarrierPanel:setIsVisible(false)
            EventsHelper:sendBroadcastGameThreadEvent(host, EventsHelper.enqueueJobPolicy.PUSH_ANYWAY,
                                                      "CombatLevelEvents", json.encode({action = "remove_barrier"}))
        end
    })
    combatOverlay:addCompoundWidget(selectedBarrierPanel)

    -- Operation objectives (right column, above the selected-tower panel). Fed from the engine's
    -- requirement trackers; rendered in the mockup's cb-obj list style. (Forward-declared above the dock.)
    objectivesPanel = ObjectivesPanel:new(host, combatOverlay, "ObjectivesPanel")

    -- Rebuilds the objectives list from the current requirement trackers. Runs on stage change and on
    -- tracker status change, deferred until the overlay's widget proxies are ready.
    -- Win / lose end state. The level ends once (either way); levelEndTriggered guards against re-firing.
    -- hadRequirements flips true once the run actually has requirements, so an empty/not-yet-loaded
    -- requirement set in preparation is never mistaken for "all enemies destroyed".
    local levelEndTriggered = false
    local hadRequirements = false

    local function triggerLevelEnd(mode)
        if levelEndTriggered then return end
        levelEndTriggered = true
        -- UiOverlays is the global registry built by CombatUiController; setMode swaps the unified end
        -- overlay between victory / defeat. Apply the mode now so its ~30 label meshes start syncing to
        -- the render thread, and freeze the game immediately to capture the moment.
        if UiOverlays ~= nil and UiOverlays["LevelEndOverlay"] ~= nil then
            UiOverlays["LevelEndOverlay"].setMode(host, mode)
        end
        EventsHelper:sendPauseGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH, true)
        -- Defer actually showing the overlay a few frames: _OpenOverlay flips visibility on the render
        -- thread instantly, but the mode's text/colours only land via the per-frame replicator sync.
        -- Opening on the same frame would flash the overlay's previous (defeat-default) content first.
        local framesUntilOpen = 3
        combatOverlay:addActionWithPredicate(function() UiOverlayManager:openOverlay(host, "LevelEndOverlay") end,
                                             function()
            framesUntilOpen = framesUntilOpen - 1
            return framesUntilOpen <= 0
        end)
    end

    rebuildObjectives = function()
        local apply = function()
            local inCombat = combatOverlay.currentCombatState == CombatState.COMBAT
            local requirementsCount = _GetCurrentProgressRequirementsCount(host)
            if requirementsCount <= 0 then
                -- Only touch the widgets when the panel is actually shown; the tracker events fire
                -- regardless of the ЗАДАЧИ toggle, and we must not pop the item plates while it's hidden.
                if objectivesActive then objectivesPanel:markAllDone() end
                -- No active requirements left: if the run had any and we are mid-combat, every enemy has
                -- been destroyed and all stages are cleared -> victory.
                if inCombat and hadRequirements then triggerLevelEnd("victory") end
                return
            end
            hadRequirements = true
            local trackersJson = _GetCurrentProgressStageRequirementTrackers(host)
            if trackersJson == nil or trackersJson == "" then return end
            local trackers = json.decode(trackersJson)
            local items = {}
            local anyFailed = false
            local hasDestroyTracker = false
            local allEnemiesDestroyed = true
            for index = 1, #trackers do
                local tracker = trackers[index]
                if tracker["name"] == "DestroySpaceshipsTracker" then
                    hasDestroyTracker = true
                    if (tonumber(tracker["left_to_destroy_spaceships_count"]) or 0) > 0 then
                        allEnemiesDestroyed = false
                    end
                end
                local item = trackerToObjectiveItem(tracker)
                if item ~= nil then
                    items[#items + 1] = item
                    if item.failed then anyFailed = true end
                end
            end
            -- Render the rows only while the panel is visible; the failure side-effect below still runs.
            if objectivesActive then objectivesPanel:setItems(items) end
            if anyFailed then
                triggerLevelEnd("defeat")
            elseif inCombat and hasDestroyTracker and allEnemiesDestroyed then
                -- Every required enemy has been destroyed -> victory.
                triggerLevelEnd("victory")
            end
        end
        if combatOverlay.allWidgetLuaProxiesReady then
            apply()
        else
            combatOverlay:addActionWithPredicate(apply, function()
                return combatOverlay.allWidgetLuaProxiesReady
            end)
        end
    end

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

        -- The tower context panel is combat-only (shown when a tower is selected during the wave).
        if not isCombat then
            selectedTowerPanel:setIsVisible(false)
            selectedBarrierPanel:setIsVisible(false)
        end

        -- Objectives are available in BOTH preparation and combat; the ЗАДАЧИ dock toggle (objectivesActive)
        -- decides whether the panel is shown. Pull the current trackers explicitly on every state change.
        objectivesPanel:setIsVisible(objectivesActive)
        if objectivesActive then rebuildObjectives() end
    end

    combatOverlay.currentCombatState = CombatState.PREPARATION

    combatOverlay.setCombatState = function(host, newState)
        assert(newState == CombatState.PREPARATION or newState == CombatState.COMBAT, debug.traceback())
        combatOverlay.currentCombatState = newState
        local apply = function() applyStateVisibility(newState) end
        if combatOverlay.allWidgetLuaProxiesReady then
            apply()
        else
            combatOverlay:addActionWithPredicate(apply, function()
                return combatOverlay.allWidgetLuaProxiesReady
            end)
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
        startWaveButton:setButtonColorHexValue(newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED and
                                                   Styles.Combat.green or Styles.Combat.cyan)
    end)

    -- ─── Wave-dock: speed controls (COMBAT) — placeholders, no game-speed API yet ──────────────────
    local function refreshSpeedButtons()
        for i = 1, #speedButtons do
            speedButtons[i]:setButtonColorHexValue(speedValues[i] == selectedSpeed and Styles.Combat.dockActiveColor or
                                                       Styles.Combat.chipColor)
            speedButtons[i]:setLabelTextColorHexValue(speedValues[i] == selectedSpeed and Styles.Combat.cyanGlow or
                                                          Styles.Combat.textDim)
        end
    end
    for i = 1, #speedButtons do
        local value = speedValues[i]
        speedButtons[i]:subscribeOnMouseInputClickedCallback(function()
            selectedSpeed = value
            refreshSpeedButtons()
            -- While paused, only cache the choice: do not apply it (that would resume the game). The pause button
            -- restores selectedSpeed when it un-pauses.
            if not isPaused then
                EventsHelper:sendChangePlaySpeedGameThreadEvent(host,
                                                                EventsHelper.enqueueJobPolicy.IF_DUPLICATE_REPLACE,
                                                                value)
            end
        end)
    end

    speedPauseButton:subscribeOnMouseInputClickedCallback(function()
        -- Toggle first, then derive the icon + speed from the NEW state: when paused the button offers "play" (resume),
        -- otherwise "pause". (Deriving from the pre-toggle value showed the wrong icon on the first press.)
        isPaused = not isPaused
        local value = isPaused and 0.0 or selectedSpeed
        speedPauseButton:setImageTextureSource(isPaused and "play.png" or "pause.png")
        EventsHelper:sendChangePlaySpeedGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_REPLACE, value)
    end)

    combatOverlay.onCurrentLevelProgressStageChanged = rebuildObjectives

    combatOverlay.onRequirementTrackersStatusChanged = rebuildObjectives

    combatOverlay.onWindowSizeChanged = function(width, height)
        assert(width ~= nil and type(width) == "number" and height ~= nil and type(height) == "number",
               debug.traceback())
    end

    combatOverlay:subscribeOnAllWidgetLuaProxiesReady(function()
        print("CombatHudOverlay: allWidgetLuaProxiesReady fired!")
        -- Objectives panel: top of the right column, just below the top HUD bar.
        objectivesPanel:setParent(host, combatOverlayCanvas.widgetName, combatOverlayCanvas.widgetName)
        objectivesPanel:setAnchor(UiItemBase.UiAnchorType.RIGHT, UiItemBase.UiAnchorType.RIGHT,
                                  combatOverlayCanvas.widgetName, 24)
        objectivesPanel:setAnchor(UiItemBase.UiAnchorType.TOP, UiItemBase.UiAnchorType.TOP,
                                  combatOverlayCanvas.widgetName, 66)
        objectivesPanel:setWidth(300)
        objectivesPanel:setHeight(300)
        objectivesPanel:setupLayout()
        objectivesPanel:setIsVisible(false)

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
        local dockPanelH = dockButtonH * 5 + dockSpacing * 4 + dockPad * 2

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

        -- Tower context panel — stacked directly under the objectives panel in the right column
        -- (mockup cb-right). Anchored to the objectives layout node, which always keeps a valid rect.
        selectedTowerPanel:setParent(host, combatOverlayCanvas.widgetName, combatOverlayCanvas.widgetName)
        selectedTowerPanel:setAnchor(A.RIGHT, A.RIGHT, combatOverlayCanvas.widgetName, 24)
        selectedTowerPanel:setAnchor(A.TOP, A.BOTTOM, objectivesPanel.mainContainer.widgetName, 12)
        selectedTowerPanel:setWidth(300)
        selectedTowerPanel:setHeight(280)
        selectedTowerPanel:setupLayout()
        selectedTowerPanel:setIsVisible(false)

        -- Barrier panel shares the tower panel's slot (only one space object is selected at a time).
        selectedBarrierPanel:setParent(host, combatOverlayCanvas.widgetName, combatOverlayCanvas.widgetName)
        selectedBarrierPanel:setAnchor(A.RIGHT, A.RIGHT, combatOverlayCanvas.widgetName, 24)
        selectedBarrierPanel:setAnchor(A.TOP, A.BOTTOM, objectivesPanel.mainContainer.widgetName, 12)
        selectedBarrierPanel:setWidth(300)
        selectedBarrierPanel:setHeight(130)
        selectedBarrierPanel:setupLayout()
        selectedBarrierPanel:setIsVisible(false)

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
        for _, sb in ipairs({speedButtons[1], speedButtons[2], speedButtons[3]}) do
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
        speedPauseButton:setAnchor(A.RIGHT, A.LEFT, speedButtons[1].widgetName, speedGap)
        speedPauseButton:setParent(host, canvasName, waveDockPanel.widgetName)
        speedPauseButton:setWidth(speedBtnSize)
        speedPauseButton:setHeight(speedBtnSize)
        speedPauseButton:setAnchor(A.VERTICAL_CENTER, A.VERTICAL_CENTER, waveDockPanel.widgetName, 0)
        speedPauseButton:setButtonColorHexValue(Combat.chipColor)
        speedPauseButton:setButtonBorderRadius(Combat.chipBorderRadius)
        speedPauseButton:setIsVisible(true)
        speedPauseButton:setButtonColorHexValue(Combat.textDim)
        speedPauseButton:setUseImageCustomColor(true)
        speedPauseButton:setImageTextureSource("pause.png")

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
                elseif statusType == PlayerStatusType.SELECTED_SPACE_OBJECT_CHANGED then
                    -- tower context panel works in both states (select a placed tower to demolish / upgrade)
                    if parsedJson["has_selected_tower"] ~= nil then
                        if parsedJson["has_selected_tower"] == true then
                            assert(parsedJson["tower_weapon_type"] ~= nil, debug.traceback())
                            local weaponType = tonumber(parsedJson["tower_weapon_type"])
                            local imageToSet = MissileTypes.IconByValue[weaponType] or "space_station_img.png"
                            selectedTowerPanel:setWeaponImage(imageToSet)
                            selectedTowerPanel:setTowerTitle(WEAPON_DISPLAY_NAME[weaponType] or "БАШНЯ")
                            selectedTowerPanel:setIsVisible(true)
                            selectedBarrierPanel:setIsVisible(false) -- only one space object is selected at a time
                        else
                            selectedTowerPanel:setIsVisible(false)
                        end
                    end
                    if parsedJson["has_selected_barrier"] ~= nil then
                        if parsedJson["has_selected_barrier"] == true then
                            selectedBarrierPanel:setIsVisible(true)
                            selectedTowerPanel:setIsVisible(false) -- only one space object is selected at a time
                        else
                            selectedBarrierPanel:setIsVisible(false)
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
                startWaveButton:setButtonColorHexValue(canCompletePreparationStage and Styles.Combat.cyan or
                                                           Styles.Combat.chipColor)
                startWaveButton:setLabelTextColorHexValue(canCompletePreparationStage and Styles.Combat.textBright or
                                                              Styles.Combat.textVeryDim)
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
