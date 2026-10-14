--[[ BEGIN *** this snippet has to be inserted everywhere where your want to require custom modules *** BEGIN]] --
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
local UiRectangle = require("Ui/Core/uiRectangle")
local UiScrollList = require("Ui/Core/uiScrollList")
local UiItem = require("Ui/Core/uiItem")
local UiLabel = require("Ui/Core/uiLabel")
local ImageButton = require("Ui/Widgets/ImageButton")
local UiItemBase = require("Ui/Core/uiItemBase")
local Styles = require("Ui/Common/styles")
local EventsHelper = require("Ui/Core/eventsHelper")
local UiTextSizing = require("Ui/Common/uiTextSizing")
local UpgradeIcon = require("Ui/Core/uiUpgradeIcon")
local ConnectionLine = require("Ui/Core/uiConnectionLine")
local WeaponInfoPanel = require("Ui/Widgets/WeaponInfoPanel")

TowerUpgradesPanel = {}

-- Weapon nodes, tiers, dependencies and stats mirror UiMockup/menu-techtree-centered.jsx
-- (TT_WEAPONS). x/y are percentage positions inside the tree canvas; y aligns with the tier rows.
local treeNodes = {
    {
        id = "he_rocket",
        name = "Фугасная Ракета",
        texture = "he_rocket.png",
        color = 0xe85d04,
        glow = 0xff8c38,
        x = 50,
        y = 7,
        parents = {},
        category = "offense",
        weaponType = "Кинетика",
        damage = 2,
        speed = 2,
        range = 2,
        area = 1,
        slow = 0,
        chain = 0,
        repair = 0,
        desc = "Базовый боеприпас. Прямое попадание с фугасным взрывом. Надёжно, просто, смертоносно."
    }, {
        id = "ice_rocket",
        name = "Ледяная Ракета",
        texture = "ice_rocket.png",
        color = 0x48cae4,
        glow = 0x90e0ef,
        x = 14,
        y = 26,
        parents = {"he_rocket"},
        category = "offense",
        weaponType = "Криогеника",
        damage = 1,
        speed = 2,
        range = 2,
        area = 2,
        slow = 3,
        chain = 0,
        repair = 0,
        desc = "Криогенный заряд. При взрыве распыляет хладагент, замедляя корабли в зоне поражения."
    }, {
        id = "repair_beam",
        name = "Ремонтный Луч",
        texture = "repair_beam.png",
        color = 0x22c55e,
        glow = 0x86efac,
        x = 50,
        y = 26,
        parents = {"he_rocket"},
        category = "support",
        weaponType = "Поддержка",
        damage = 0,
        speed = 3,
        range = 3,
        area = 1,
        slow = 0,
        chain = 0,
        repair = 3,
        desc = "Направляет исцеляющий луч на повреждённые секции барьера. Постепенно восстанавливает структурную целостность защитных заграждений."
    }, {
        id = "plasma_rocket",
        name = "Плазменная Ракета",
        texture = "plasma_rocket.png",
        color = 0xf72585,
        glow = 0xff49a0,
        x = 86,
        y = 26,
        parents = {"he_rocket"},
        category = "offense",
        weaponType = "Энергетика",
        damage = 3,
        speed = 3,
        range = 2,
        area = 2,
        slow = 0,
        chain = 0,
        repair = 0,
        desc = "Плазменный заряд повышенной мощности. Прожигает броню и оставляет ожоговый след."
    }, {
        id = "ice_beam",
        name = "Ледяной Луч",
        texture = "ice_beam.png",
        color = 0x00b4d8,
        glow = 0x48cae4,
        x = 6,
        y = 48,
        parents = {"ice_rocket"},
        category = "offense",
        weaponType = "Криогеника",
        damage = 1,
        speed = 5,
        range = 4,
        area = 1,
        slow = 5,
        chain = 0,
        repair = 0,
        desc = "Непрерывный луч абсолютного холода. Постепенно замораживает корабль до полной остановки."
    }, {
        id = "electric_beam",
        name = "Электрический Луч",
        texture = "electric_beam.png",
        color = 0x9d4edd,
        glow = 0xc77dff,
        x = 29,
        y = 48,
        parents = {"plasma_rocket", "ice_rocket"},
        category = "offense",
        weaponType = "Дуговой разряд",
        damage = 3,
        speed = 5,
        range = 3,
        area = 3,
        slow = 1,
        chain = 4,
        repair = 0,
        desc = "Дуговой разряд перепрыгивает на соседние корабли, нанося цепной урон по ближайшим целям."
    }, {
        id = "nano_beam",
        name = "Нано-Луч",
        texture = "nano_beam.png",
        color = 0x10b981,
        glow = 0x6ee7b7,
        x = 62,
        y = 48,
        parents = {"repair_beam"},
        category = "support",
        weaponType = "Нанотехнологии",
        damage = 0,
        speed = 4,
        range = 4,
        area = 2,
        slow = 0,
        chain = 0,
        repair = 5,
        desc = "Поток нано-роботов восстанавливает барьер почти мгновенно. Может распределяться по нескольким соседним секциям одновременно."
    }, {
        id = "cryo_cannon",
        name = "Крио-Пушка",
        texture = "cryo_cannon.png",
        color = 0x3a56d4,
        glow = 0x5e7bff,
        x = 87,
        y = 48,
        parents = {"plasma_rocket"},
        category = "offense",
        weaponType = "Криогеника+",
        damage = 2,
        speed = 4,
        range = 3,
        area = 4,
        slow = 4,
        chain = 0,
        repair = 0,
        desc = "Широкий конус ледяного шторма. Одновременно накрывает несколько кораблей в секторе обстрела."
    }, {
        id = "gravity_bomb",
        name = "Гравитационная Бомба",
        texture = "gravity_bomb.png",
        color = 0x9333ea,
        glow = 0xc084fc,
        x = 28,
        y = 69,
        parents = {"ice_beam", "electric_beam"},
        category = "offense",
        weaponType = "Сингулярность",
        damage = 5,
        speed = 2,
        range = 5,
        area = 5,
        slow = 5,
        chain = 0,
        repair = 0,
        desc = "Создаёт микросингулярность. Затягивает флот в точку коллапса и уничтожает под действием гравитационных приливов."
    }, {
        id = "force_barrier",
        name = "Форс-Барьер",
        texture = "force_barrier.png",
        color = 0xf59e0b,
        glow = 0xfcd34d,
        x = 72,
        y = 69,
        parents = {"nano_beam", "cryo_cannon"},
        category = "support",
        weaponType = "Фортификация",
        damage = 0,
        speed = 3,
        range = 3,
        area = 4,
        slow = 2,
        chain = 0,
        repair = 4,
        desc = "Проецирует временный силовой щит поверх уничтоженных секций барьера. Усиливает соседние секции и замедляет врагов, пытающихся прорваться через зону защиты."
    }, {
        id = "quantum_nexus",
        name = "Квантовый Нексус",
        texture = "quantum_nexus.png",
        color = 0x818cf8,
        glow = 0xe0e7ff,
        x = 50,
        y = 88,
        parents = {"gravity_bomb", "force_barrier"},
        category = "hybrid",
        weaponType = "Квантовая Сингулярность",
        damage = 5,
        speed = 1,
        range = 5,
        area = 5,
        slow = 5,
        chain = 0,
        repair = 5,
        desc = "Снаряд создаёт гравитационный коллапс, уничтожающий все корабли в зоне взрыва. Одновременно вспышка квантовой энергии накрывает все барьеры в радиусе, проецируя на каждый из них полноценный форс-щит."
    }
}

-- tier index per node (drives the corner badge); matches TT_WEAPONS[].tier in the reference.
local TIER_BY_ID = {
    he_rocket = 1,
    ice_rocket = 2,
    repair_beam = 2,
    plasma_rocket = 2,
    ice_beam = 3,
    electric_beam = 3,
    nano_beam = 3,
    cryo_cannon = 3,
    gravity_bomb = 4,
    force_barrier = 4,
    quantum_nexus = 5
}

-- Left rail: one entry per tier (y aligns with that tier's node row), mirroring TT_TIERS.
local TIERS = {
    {y = 7, name = "УРОВЕНЬ I", sub = "Базовое вооружение"},
    {y = 26, name = "УРОВЕНЬ II", sub = "Специализация"},
    {y = 48, name = "УРОВЕНЬ III", sub = "Продвинутые системы"},
    {y = 69, name = "УРОВЕНЬ IV", sub = "Элитное оружие"},
    {y = 88, name = "УРОВЕНЬ V", sub = "Финальный арсенал"}
}

-- Horizontal dividers between tier bands (TT y values from the reference).
local DIVIDER_Y = {16.5, 37, 58.5, 78.5}

-- Header legend: category dot + label.
local LEGEND = {
    {label = "АТАКА", color = Styles.TechTree.offenseColor},
    {label = "ПОДДЕРЖКА", color = Styles.TechTree.supportColor},
    {label = "ГИБРИД", color = Styles.TechTree.hybridColor}
}

local nodeById = {}
for _, node in ipairs(treeNodes) do
    node.tier = TIER_BY_ID[node.id]
    nodeById[node.id] = node
end

local connections = {}
for _, node in ipairs(treeNodes) do
    for _, parentId in ipairs(node.parents) do connections[#connections + 1] = {from = parentId, to = node.id} end
end

local BORDER_RADIUS = 6
local CONTENT_NAME = "TowerUpgradesContent"
local LINE_THICKNESS = 2.0
local NODE_GLOW_SIZE = 18.0
-- Note: the reference uses Orbitron for titles/labels, but Orbitron ships Latin-only and every
-- string here is Cyrillic — the engine has no font fallback, so JetBrains Mono (which carries
-- Cyrillic) is used throughout. In the browser mockup Orbitron silently falls back for Cyrillic too.
local LABEL_FONT = "JetBrainsMono-VariableFont_wght"
-- vertical padding (fraction of content height) so the top/bottom tiers are not clipped.
local V_PAD = 0.05
local BADGE_TEXT_COLOR = 0xffffff

-- Palette pulled from menu.css :root tokens + the tech-tree stylesheet.
local COLOR_CANVAS_BG = 0x081221 -- .tt-canvas (a touch darker than the panel)
local COLOR_HEADER_BORDER = 0x16314f -- header bottom hairline (#4a9eff22 over the panel)
local COLOR_RULE = 0x1a3050 -- --accent / footer & body 1px rules
local COLOR_PRE = 0x4a9eff -- pre-text (cyan, dimmed via opacity)
local COLOR_LEGEND_TEXT = 0x5e7a99 -- --ink-3
local COLOR_NODE_LABEL = 0x8899bb -- default node label (.tt-node__label)
local COLOR_TIER_CYAN = 0x4a9eff -- tier rail (cyan, dimmed)
local COLOR_TIER_FINAL = 0x818cf8 -- final tier rail + final divider (--purple)
local COLOR_TIER_SUB = 0x36475a -- .tt-tier__sub
local COLOR_FOOTER_TEXT = 0x556677 -- .tt-panel__ftr

local function nodeWidgetName(nodeId) return "TowerUpgradesNode_" .. nodeId end

local function clamp(v, lo, hi)
    if v < lo then return lo end
    if v > hi then return hi end
    return v
end

function TowerUpgradesPanel:new(host, overlay)
    assert(host ~= nil and overlay ~= nil, debug.traceback())

    local newObj = {
        host = host,
        isVisible = false,
        background = nil,
        scrollList = nil,
        contentContainer = nil,
        closeButton = nil,
        preLabel = nil,
        titleLabel = nil,
        footerLabel = nil,
        legendDots = {},
        legendLabels = {},
        cornerBraces = {},
        tierRailMain = {},
        tierRailSub = {},
        dividers = {},
        upgradeButtons = {},
        nodeLabels = {},
        nodeLabelRestColor = {},
        tierBadges = {},
        tierBadgeLabels = {},
        connectionLines = {},
        infoPanel = nil,
        panelWidth = 0,
        panelHeight = 0
    }
    setmetatable(newObj, self)
    self.__index = self

    newObj.background = UiRectangle:new(host, "TowerUpgradesPanelBg")
    overlay:addWidget(newObj.background)

    -- canvas surface (behind the tree), 1px hairlines under the header / over the footer,
    -- and the vertical rule splitting the canvas from the info column. (.tt-canvas / borders in CSS)
    newObj.canvasBg = UiRectangle:new(host, "TowerUpgradesCanvasBg")
    overlay:addWidget(newObj.canvasBg)
    newObj.headerBorder = UiRectangle:new(host, "TowerUpgradesHeaderBorder")
    overlay:addWidget(newObj.headerBorder)
    newObj.footerBorder = UiRectangle:new(host, "TowerUpgradesFooterBorder")
    overlay:addWidget(newObj.footerBorder)
    newObj.bodyDivider = UiRectangle:new(host, "TowerUpgradesBodyDivider")
    overlay:addWidget(newObj.bodyDivider)

    -- ── Header chrome ─────────────────────────────────────────────────────────
    newObj.preLabel = UiLabel:new(host, LABEL_FONT, "TowerUpgradesPre")
    overlay:addWidget(newObj.preLabel)
    newObj.titleLabel = UiLabel:new(host, LABEL_FONT, "TowerUpgradesTitle")
    overlay:addWidget(newObj.titleLabel)
    newObj.footerLabel = UiLabel:new(host, LABEL_FONT, "TowerUpgradesFooter")
    overlay:addWidget(newObj.footerLabel)

    for i = 1, #LEGEND do
        local dot = UiRectangle:new(host, "TowerUpgradesLegendDot" .. tostring(i))
        overlay:addWidget(dot)
        newObj.legendDots[i] = dot
        local lbl = UiLabel:new(host, LABEL_FONT, "TowerUpgradesLegendLabel" .. tostring(i))
        overlay:addWidget(lbl)
        newObj.legendLabels[i] = lbl
    end

    for i = 1, 8 do
        local brace = UiRectangle:new(host, "TowerUpgradesBrace" .. tostring(i))
        overlay:addWidget(brace)
        newObj.cornerBraces[i] = brace
    end

    -- ── Tree scroll area ──────────────────────────────────────────────────────
    newObj.scrollList = UiScrollList:new(host, "TowerUpgradesScrollList")
    overlay:addWidget(newObj.scrollList)

    -- One container = single item of the scroll list. Tree nodes, labels and connection lines live inside it.
    newObj.contentContainer = UiItem:new(host, CONTENT_NAME)
    overlay:addWidget(newObj.contentContainer)

    for i = 1, #TIERS do
        local main = UiLabel:new(host, LABEL_FONT, "TowerUpgradesTierMain" .. tostring(i))
        overlay:addWidget(main)
        newObj.tierRailMain[i] = main
        local sub = UiLabel:new(host, LABEL_FONT, "TowerUpgradesTierSub" .. tostring(i))
        overlay:addWidget(sub)
        newObj.tierRailSub[i] = sub
    end

    for i = 1, #DIVIDER_Y do
        local divider = UiRectangle:new(host, "TowerUpgradesDivider" .. tostring(i))
        overlay:addWidget(divider)
        newObj.dividers[i] = divider
    end

    for i = 1, #connections do
        local line = ConnectionLine:new(host, "TowerUpgradesLine" .. tostring(i))
        overlay:addWidget(line)
        newObj.connectionLines[i] = line
    end

    for i = 1, #treeNodes do
        local btn = UpgradeIcon:new(host, nodeWidgetName(treeNodes[i].id))
        overlay:addWidget(btn)
        newObj.upgradeButtons[i] = btn

        local label = UiLabel:new(host, LABEL_FONT, "TowerUpgradesLabel_" .. treeNodes[i].id)
        overlay:addWidget(label)
        newObj.nodeLabels[i] = label

        local badge = UiRectangle:new(host, "TowerUpgradesBadge_" .. treeNodes[i].id)
        overlay:addWidget(badge)
        newObj.tierBadges[i] = badge
        local badgeLabel = UiLabel:new(host, LABEL_FONT, "TowerUpgradesBadgeLabel_" .. treeNodes[i].id)
        overlay:addWidget(badgeLabel)
        newObj.tierBadgeLabels[i] = badgeLabel
    end

    newObj.closeButton = ImageButton:new(host, overlay, "TowerUpgradesCloseButton")
    overlay:addCompoundWidget(newObj.closeButton)

    newObj.infoPanel = WeaponInfoPanel:new(host, overlay)
    newObj.infoPanel.weaponById = nodeById

    newObj.closeButton:subscribeOnMouseInputClickedCallback(function()
        newObj:setIsVisible(false)
        newObj.infoPanel:hide()
        EventsHelper:sendPauseGameThreadEvent(host, EventsHelper.enqueueJobPolicy.IF_DUPLICATE_NO_PUSH, false)
    end)
    newObj.closeButton:subscribeOnMouseInputCursorHoverStateChangedCallback(function(newState)
        if newState == UiItemBase.UiMouseInputCursorHoverState.ENTERED then
            newObj.closeButton:setButtonColorHexValue(Styles.TechTree.hoveredButtonColor)
        else
            newObj.closeButton:setButtonColorHexValue(Styles.TechTree.buttonColor)
        end
    end)

    for i = 1, #treeNodes do
        local btn = newObj.upgradeButtons[i]
        btn:subscribeOnMouseInputClickedCallback(function() newObj:selectNode(i) end)
    end

    return newObj
end

function TowerUpgradesPanel:onPreCompoundWidgetInitialize() end

function TowerUpgradesPanel:onCompoundWidgetInitialize() end

function TowerUpgradesPanel:setupLayout(canvasName, windowWidth, windowHeight)
    local A = UiItemBase.UiAnchorType
    local HALIGN = UiLabel.TextHorizontalAlignmentType
    local VALIGN = UiLabel.TextVerticalAlignmentType
    local TT = Styles.TechTree
    local host = self.host
    local function lh(fontSize) return UiTextSizing.safeLineHeight(fontSize, LABEL_FONT) end

    -- Preserve the reference panel aspect (1480 x 840), bound by height first then width.
    local panelHeight = math.floor(windowHeight * 0.92)
    local panelWidth = math.floor(panelHeight * (1480.0 / 840.0))
    local maxWidth = math.floor(windowWidth * 0.94)
    if panelWidth > maxWidth then
        panelWidth = maxWidth
        panelHeight = math.floor(panelWidth * (840.0 / 1480.0))
    end
    self.panelWidth = panelWidth
    self.panelHeight = panelHeight

    local pad = math.floor(panelWidth * 0.018)
    local headerHeight = math.floor(panelHeight * 0.10)
    local footerHeight = math.floor(panelHeight * 0.05)
    local bodyHeight = panelHeight - headerHeight - footerHeight
    local bodyPad = math.floor(panelWidth * 0.016)
    local infoWidth = math.floor(panelWidth * 0.216)

    -- ── Panel background ──────────────────────────────────────────────────────
    self.background:setParent(host, canvasName, canvasName)
    self.background:setAnchor(A.HORIZONTAL_CENTER, A.HORIZONTAL_CENTER, canvasName)
    self.background:setAnchor(A.VERTICAL_CENTER, A.VERTICAL_CENTER, canvasName)
    self.background:setWidth(panelWidth)
    self.background:setHeight(panelHeight)
    self.background:setColorHexValue(TT.panelColor)
    self.background:setBorderRadius(TT.panelBorderRadius)
    self.background:setZOrder(10)
    self.background:setIsVisible(false)
    local panelName = self.background.widgetName

    -- ── Canvas surface + 1px rules (header bottom / footer top / canvas|info) ──
    self.canvasBg:setParent(host, canvasName, panelName)
    self.canvasBg:setAnchor(A.TOP, A.TOP, panelName, headerHeight)
    self.canvasBg:setAnchor(A.BOTTOM, A.BOTTOM, panelName, footerHeight)
    self.canvasBg:setAnchor(A.LEFT, A.LEFT, panelName, bodyPad)
    self.canvasBg:setAnchor(A.RIGHT, A.RIGHT, panelName, 2 * bodyPad + infoWidth)
    self.canvasBg:setColorHexValue(COLOR_CANVAS_BG)
    self.canvasBg:setZOrder(10)
    self.canvasBg:setIsVisible(false)

    self.headerBorder:setParent(host, canvasName, panelName)
    self.headerBorder:setAnchor(A.LEFT, A.LEFT, panelName, 0)
    self.headerBorder:setAnchor(A.RIGHT, A.RIGHT, panelName, 0)
    self.headerBorder:setAnchor(A.TOP, A.TOP, panelName, headerHeight)
    self.headerBorder:setHeight(1)
    self.headerBorder:setColorHexValue(COLOR_HEADER_BORDER)
    self.headerBorder:setZOrder(13)
    self.headerBorder:setIsVisible(false)

    self.footerBorder:setParent(host, canvasName, panelName)
    self.footerBorder:setAnchor(A.LEFT, A.LEFT, panelName, 0)
    self.footerBorder:setAnchor(A.RIGHT, A.RIGHT, panelName, 0)
    self.footerBorder:setAnchor(A.BOTTOM, A.BOTTOM, panelName, footerHeight)
    self.footerBorder:setHeight(1)
    self.footerBorder:setColorHexValue(COLOR_RULE)
    self.footerBorder:setZOrder(13)
    self.footerBorder:setIsVisible(false)

    self.bodyDivider:setParent(host, canvasName, panelName)
    self.bodyDivider:setAnchor(A.TOP, A.TOP, panelName, headerHeight)
    self.bodyDivider:setAnchor(A.BOTTOM, A.BOTTOM, panelName, footerHeight)
    self.bodyDivider:setAnchor(A.RIGHT, A.RIGHT, panelName, bodyPad + infoWidth)
    self.bodyDivider:setWidth(1)
    self.bodyDivider:setColorHexValue(COLOR_RULE)
    self.bodyDivider:setZOrder(12)
    self.bodyDivider:setIsVisible(false)

    -- ── Corner braces (L-brackets) ────────────────────────────────────────────
    local armLen = math.floor(panelWidth * 0.018)
    local braceThick = 2
    local braceInset = math.floor(panelHeight * 0.012)
    -- {horizEdge, vertEdge} per corner: which side each arm anchors to.
    local corners = {
        {h = A.LEFT, v = A.TOP}, {h = A.RIGHT, v = A.TOP}, {h = A.LEFT, v = A.BOTTOM}, {h = A.RIGHT, v = A.BOTTOM}
    }
    for c = 1, 4 do
        local corner = corners[c]
        local hArm = self.cornerBraces[(c - 1) * 2 + 1]
        local vArm = self.cornerBraces[(c - 1) * 2 + 2]
        for _, arm in ipairs({hArm, vArm}) do
            arm:setParent(host, canvasName, panelName)
            arm:setColorHexValue(TT.titleAccent) -- --cyan-glow brace
            arm:setZOrder(16)
            arm:setIsVisible(false)
            arm:setAnchor(corner.h, corner.h, panelName, braceInset)
            arm:setAnchor(corner.v, corner.v, panelName, braceInset)
        end
        hArm:setWidth(armLen)
        hArm:setHeight(braceThick)
        vArm:setWidth(braceThick)
        vArm:setHeight(armLen)
    end

    -- ── Header: pre-text + bracketed title ────────────────────────────────────
    local preFontSize = clamp(math.floor(panelHeight * 0.013), 10, 14)
    local titleFontSize = clamp(math.floor(panelHeight * 0.028), 18, 30)
    local preTop = math.floor(headerHeight * 0.22)
    local titleTop = preTop + lh(preFontSize) + math.floor(headerHeight * 0.04)

    self.preLabel:setParent(host, canvasName, panelName)
    self.preLabel:setAnchor(A.LEFT, A.LEFT, panelName, pad)
    self.preLabel:setAnchor(A.TOP, A.TOP, panelName, preTop)
    self.preLabel:setWidth(math.floor(panelWidth * 0.6))
    self.preLabel:setHeight(lh(preFontSize))
    self.preLabel:setFontSize(preFontSize)
    self.preLabel:setTextColorHexValue(COLOR_PRE)
    self.preLabel:setOpacity(0.6)
    self.preLabel:setTextHorizontalAlignment(HALIGN.LEFT)
    self.preLabel:setTextVerticalAlignment(VALIGN.CENTER)
    self.preLabel:setText("// КОМАНДНЫЙ ЦЕНТР · СЕКТОР ВООРУЖЕНИЙ")
    self.preLabel:setZOrder(14)
    self.preLabel:setIsVisible(false)

    self.titleLabel:setParent(host, canvasName, panelName)
    self.titleLabel:setAnchor(A.LEFT, A.LEFT, panelName, pad)
    self.titleLabel:setAnchor(A.TOP, A.TOP, panelName, titleTop)
    self.titleLabel:setWidth(math.floor(panelWidth * 0.6))
    self.titleLabel:setHeight(lh(titleFontSize))
    self.titleLabel:setFontSize(titleFontSize)
    self.titleLabel:setTextColorHexValue(TT.titleAccent)
    self.titleLabel:setTextHorizontalAlignment(HALIGN.LEFT)
    self.titleLabel:setTextVerticalAlignment(VALIGN.CENTER)
    self.titleLabel:setText("[ ДЕРЕВО ТЕХНОЛОГИЙ ]")
    self.titleLabel:setZOrder(14)
    self.titleLabel:setIsVisible(false)
    -- .tt-panel__title gradient (135deg cyan-glow -> blue -> purple); vertical 2-stop approximation.
    self.titleLabel:setTextGradientHexValues(UiLabel.TextGradientColorType.VERTICAL, 0x90e0ef, 0xc084fc)

    -- ── Close button (top-right) ──────────────────────────────────────────────
    local closeBtnSize = math.floor(headerHeight * 0.42)
    self.closeButton:setParent(host, canvasName, panelName)
    self.closeButton:setWidth(closeBtnSize)
    self.closeButton:setHeight(closeBtnSize)
    self.closeButton:setButtonColorHexValue(Styles.TechTree.buttonColor)
    self.closeButton:setButtonBorderRadius(BORDER_RADIUS)
    self.closeButton:setImageTextureSource("cancel.png")
    self.closeButton:setAnchor(A.TOP, A.TOP, panelName, math.floor(pad * 0.6))
    self.closeButton:setAnchor(A.RIGHT, A.RIGHT, panelName, math.floor(pad * 0.6))
    self.closeButton:setZOrder(18)
    self.closeButton:setIsVisible(false)

    -- ── Legend (category dots), right-aligned left of the close button ─────────
    local legendFontSize = clamp(math.floor(panelHeight * 0.014), 11, 15)
    local dotSize = math.floor(legendFontSize * 0.75)
    local dotGap = math.floor(legendFontSize * 0.45)
    local itemGap = math.floor(legendFontSize * 1.25)
    local labelH = lh(legendFontSize)
    local itemWidths = {}
    local total = 0
    for i = 1, #LEGEND do
        local textW = UiTextSizing.measureLabelWidthPx(host, LABEL_FONT, legendFontSize, LEGEND[i].label)
        local w = dotSize + dotGap + textW
        itemWidths[i] = {w = w, textW = textW}
        total = total + w
        if i < #LEGEND then total = total + itemGap end
    end
    local legendRight = panelWidth - math.floor(pad * 0.6) - closeBtnSize - math.floor(pad * 0.5)
    local cursor = legendRight - total
    local dotTop = math.floor(headerHeight * 0.52 - dotSize / 2)
    local labelTop = math.floor(headerHeight * 0.52 - labelH / 2)
    for i = 1, #LEGEND do
        local dot = self.legendDots[i]
        dot:setParent(host, canvasName, panelName)
        dot:setAnchor(A.LEFT, A.LEFT, panelName, cursor)
        dot:setAnchor(A.TOP, A.TOP, panelName, dotTop)
        dot:setWidth(dotSize)
        dot:setHeight(dotSize)
        dot:setColorHexValue(LEGEND[i].color)
        dot:setBorderRadius(dotSize // 2)
        dot:setZOrder(14)
        dot:setIsVisible(false)

        local lbl = self.legendLabels[i]
        lbl:setParent(host, canvasName, panelName)
        lbl:setAnchor(A.LEFT, A.LEFT, panelName, cursor + dotSize + dotGap)
        lbl:setAnchor(A.TOP, A.TOP, panelName, labelTop)
        lbl:setWidth(itemWidths[i].textW + 6)
        lbl:setHeight(labelH)
        lbl:setFontSize(legendFontSize)
        lbl:setTextColorHexValue(COLOR_LEGEND_TEXT)
        lbl:setTextHorizontalAlignment(HALIGN.LEFT)
        lbl:setTextVerticalAlignment(VALIGN.CENTER)
        lbl:setText(LEGEND[i].label)
        lbl:setZOrder(14)
        lbl:setIsVisible(false)

        cursor = cursor + itemWidths[i].w + itemGap
    end

    -- ── Footer hint ───────────────────────────────────────────────────────────
    local footerFontSize = clamp(math.floor(panelHeight * 0.012), 9, 12)
    self.footerLabel:setParent(host, canvasName, panelName)
    self.footerLabel:setAnchor(A.LEFT, A.LEFT, panelName, math.floor(panelWidth * 0.018))
    self.footerLabel:setAnchor(A.BOTTOM, A.BOTTOM, panelName, math.floor((footerHeight - lh(footerFontSize)) * 0.5))
    self.footerLabel:setWidth(panelWidth - 2 * pad)
    self.footerLabel:setHeight(lh(footerFontSize))
    self.footerLabel:setFontSize(footerFontSize)
    self.footerLabel:setTextColorHexValue(COLOR_FOOTER_TEXT)
    self.footerLabel:setTextHorizontalAlignment(HALIGN.LEFT)
    self.footerLabel:setTextVerticalAlignment(VALIGN.CENTER)
    self.footerLabel:setText(
        "Нажмите на узел для характеристик   ·   Квантовый Нексус требует обе ветки: атаку и поддержку")
    self.footerLabel:setZOrder(14)
    self.footerLabel:setIsVisible(false)

    -- ── Tree scroll area (left column of the body) ────────────────────────────
    self.scrollList:setParent(host, canvasName, panelName)
    self.scrollList:setAnchor(A.TOP, A.TOP, panelName, headerHeight)
    self.scrollList:setAnchor(A.BOTTOM, A.BOTTOM, panelName, footerHeight)
    self.scrollList:setAnchor(A.LEFT, A.LEFT, panelName, bodyPad)
    self.scrollList:setAnchor(A.RIGHT, A.RIGHT, panelName, 2 * bodyPad + infoWidth)
    self.scrollList:setSpacing(0)
    self.scrollList:setScrollSpeed(40)
    self.scrollList:setZOrder(11)
    self.scrollList:setIsVisible(false)
    self.scrollList:setScrollbarSide(UiScrollList.ScrollbarSide.NONE)
    self.scrollList:setCanBloomBeApplied(false)

    local canvasW = panelWidth - 3 * bodyPad - infoWidth
    local contentWidth = canvasW - 16
    local contentHeight = math.floor(bodyHeight * 1.12)
    self.contentContainer:setParent(host, canvasName, self.scrollList.widgetName)
    self.contentContainer:setWidth(contentWidth)
    self.contentContainer:setHeight(contentHeight)
    self.contentContainer:setZOrder(11)
    self.contentContainer:setIsVisible(false)

    -- node circle 58px / hybrid 70px on the 840px reference panel.
    local nodeSize = math.floor(panelHeight * 0.07)
    local hybridSize = math.floor(nodeSize * 1.2)
    local tierRailWidth = math.floor(contentWidth * 0.14)
    local nodeAreaLeft = tierRailWidth
    local nodeAreaRight = contentWidth - math.floor(nodeSize * 0.4)
    local nodeAreaW = nodeAreaRight - nodeAreaLeft

    local function xOffset(xPercent)
        local cx = nodeAreaLeft + (xPercent / 100.0) * nodeAreaW
        return math.floor(cx - contentWidth / 2.0)
    end
    local function yOffset(yPercent)
        local yMapped = V_PAD + (yPercent / 100.0) * (1.0 - 2.0 * V_PAD)
        return math.floor((0.5 - yMapped) * contentHeight)
    end

    -- ── Tier rail labels (left of the tree) ───────────────────────────────────
    local tierMainFont = clamp(math.floor(nodeSize * 0.16), 11, 16)
    local tierSubFont = clamp(math.floor(nodeSize * 0.11), 8, 12)
    for i = 1, #TIERS do
        local tier = TIERS[i]
        local main = self.tierRailMain[i]
        main:setParent(host, canvasName, CONTENT_NAME)
        main:setAnchor(A.LEFT, A.LEFT, CONTENT_NAME, 2)
        main:setAnchor(A.VERTICAL_CENTER, A.VERTICAL_CENTER, CONTENT_NAME)
        main:setVerticalCenterOffset(yOffset(tier.y) - math.floor(lh(tierMainFont) * 0.5))
        main:setWidth(tierRailWidth - 4)
        main:setHeight(lh(tierMainFont))
        main:setFontSize(tierMainFont)
        if i == #TIERS then
            main:setTextColorHexValue(COLOR_TIER_FINAL)
            main:setOpacity(0.6)
        else
            main:setTextColorHexValue(COLOR_TIER_CYAN)
            main:setOpacity(0.5)
        end
        main:setTextHorizontalAlignment(HALIGN.LEFT)
        main:setTextVerticalAlignment(VALIGN.CENTER)
        main:setText(tier.name)
        main:setZOrder(12)
        main:setIsVisible(false)

        local sub = self.tierRailSub[i]
        sub:setParent(host, canvasName, CONTENT_NAME)
        sub:setAnchor(A.LEFT, A.LEFT, CONTENT_NAME, 2)
        sub:setAnchor(A.VERTICAL_CENTER, A.VERTICAL_CENTER, CONTENT_NAME)
        sub:setVerticalCenterOffset(yOffset(tier.y) + math.floor(lh(tierSubFont) * 0.55))
        sub:setWidth(tierRailWidth - 4)
        sub:setHeight(lh(tierSubFont))
        sub:setFontSize(tierSubFont)
        sub:setTextColorHexValue(COLOR_TIER_SUB)
        sub:setTextHorizontalAlignment(HALIGN.LEFT)
        sub:setTextVerticalAlignment(VALIGN.CENTER)
        sub:setText(tier.sub)
        sub:setZOrder(12)
        sub:setIsVisible(false)
    end

    -- ── Dividers between tiers ────────────────────────────────────────────────
    for i = 1, #DIVIDER_Y do
        local divider = self.dividers[i]
        divider:setParent(host, canvasName, CONTENT_NAME)
        divider:setAnchor(A.LEFT, A.LEFT, CONTENT_NAME, 0)
        divider:setAnchor(A.VERTICAL_CENTER, A.VERTICAL_CENTER, CONTENT_NAME)
        divider:setVerticalCenterOffset(yOffset(DIVIDER_Y[i]))
        divider:setWidth(contentWidth)
        divider:setHeight(1)
        if i == #DIVIDER_Y then
            divider:setColorHexValue(COLOR_TIER_FINAL) -- .tt-divider--final
            divider:setOpacity(0.45)
        else
            divider:setColorHexValue(COLOR_RULE)
        end
        divider:setZOrder(11)
        divider:setIsVisible(false)
    end

    -- ── Connection lines ──────────────────────────────────────────────────────
    for i = 1, #connections do
        local conn = connections[i]
        local line = self.connectionLines[i]
        line:setParent(host, canvasName, CONTENT_NAME)
        line:setAnchor(A.LEFT, A.LEFT, CONTENT_NAME)
        line:setAnchor(A.RIGHT, A.RIGHT, CONTENT_NAME)
        line:setAnchor(A.TOP, A.TOP, CONTENT_NAME)
        line:setAnchor(A.BOTTOM, A.BOTTOM, CONTENT_NAME)
        line:setStartAnchorTarget(nodeWidgetName(conn.from))
        line:setEndAnchorTarget(nodeWidgetName(conn.to))
        line:setColorHexValue(nodeById[conn.to].glow)
        line:setThicknessPx(LINE_THICKNESS)
        line:setZOrder(12)
        line:setIsVisible(false)
        line:setCanBloomBeApplied(false)
        line:setDashPattern(5, 4)
    end

    -- ── Nodes (icon + tier badge + label) ─────────────────────────────────────
    local labelFontSize = clamp(math.floor(nodeSize * 0.18), 11, 16)
    local labelHeight = lh(labelFontSize)
    for i = 1, #treeNodes do
        local node = treeNodes[i]
        local isHybrid = node.category == "hybrid"
        local size = isHybrid and hybridSize or nodeSize
        local labelGap = math.floor(size * 0.12)
        local badgeSize = math.floor(size * 0.31)
        local badgeFontSize = clamp(math.floor(badgeSize * 0.62), 9, 14)

        local btn = self.upgradeButtons[i]
        btn:setParent(host, canvasName, CONTENT_NAME)
        btn:setWidth(size)
        btn:setHeight(size)
        btn:setAnchor(A.HORIZONTAL_CENTER, A.HORIZONTAL_CENTER, CONTENT_NAME)
        btn:setAnchor(A.VERTICAL_CENTER, A.VERTICAL_CENTER, CONTENT_NAME)
        btn:setHorizontalCenterOffset(xOffset(node.x))
        btn:setVerticalCenterOffset(yOffset(node.y))
        btn:setTextureSource(node.texture)
        btn:setZOrder(13)
        btn:setRotationDegrees(180)
        btn:setBorderColorHexValue(node.color)
        btn:setGlowColorHexValue(node.glow)
        btn:setFillColorHexValue(node.color)
        btn:setGlowSizePx(isHybrid and (NODE_GLOW_SIZE * 1.5) or NODE_GLOW_SIZE)
        btn:setIsVisible(false)
        btn:setCanBloomBeApplied(false)
        btn:enableMouseInputReceiverBase(host)

        -- resting label colour: support -> node accent, hybrid -> glow, otherwise muted (.tt-node__label)
        local restColor = COLOR_NODE_LABEL
        if isHybrid then
            restColor = node.glow
        elseif node.category == "support" then
            restColor = node.color
        end
        self.nodeLabelRestColor[i] = restColor

        local label = self.nodeLabels[i]
        label:setParent(host, canvasName, CONTENT_NAME)
        label:setWidth(math.floor(size * 3.4))
        label:setHeight(labelHeight)
        label:setAnchor(A.HORIZONTAL_CENTER, A.HORIZONTAL_CENTER, nodeWidgetName(node.id))
        label:setAnchor(A.TOP, A.BOTTOM, nodeWidgetName(node.id), labelGap)
        label:setText(node.name)
        label:setFontSize(labelFontSize)
        label:setTextColorHexValue(restColor)
        label:setTextHorizontalAlignment(HALIGN.CENTER)
        label:setZOrder(14)
        label:setIsVisible(false)

        local badge = self.tierBadges[i]
        badge:setParent(host, canvasName, CONTENT_NAME)
        badge:setWidth(badgeSize)
        badge:setHeight(badgeSize)
        badge:setAnchor(A.RIGHT, A.RIGHT, nodeWidgetName(node.id), math.floor(badgeSize * 0.08))
        badge:setAnchor(A.TOP, A.TOP, nodeWidgetName(node.id), math.floor(badgeSize * 0.08))
        badge:setColorHexValue(node.color)
        badge:setBorderRadius(badgeSize // 2)
        badge:setZOrder(15)
        badge:setIsVisible(false)

        local badgeLabel = self.tierBadgeLabels[i]
        badgeLabel:setParent(host, canvasName, badge.widgetName)
        badgeLabel:fill(badge.widgetName)
        badgeLabel:setFontSize(badgeFontSize)
        badgeLabel:setTextColorHexValue(BADGE_TEXT_COLOR)
        badgeLabel:setTextHorizontalAlignment(HALIGN.CENTER)
        badgeLabel:setTextVerticalAlignment(VALIGN.CENTER)
        badgeLabel:setText(tostring(node.tier))
        badgeLabel:setZOrder(16)
        badgeLabel:setIsVisible(false)
    end

    -- ── Info panel (right column of the body, inside the panel) ───────────────
    self.infoPanel:setupLayout(canvasName, panelName, infoWidth, bodyHeight - bodyPad, headerHeight, bodyPad)
end

-- Highlights a node: glow ring on the selected icon, its label lit with the node glow, all others
-- restored to their resting colour, and the info column refreshed. Mirrors .is-selected in the CSS.
function TowerUpgradesPanel:selectNode(index)
    for i = 1, #self.upgradeButtons do
        self.upgradeButtons[i]:setGlowVisible(false)
        if self.nodeLabelRestColor[i] ~= nil then
            self.nodeLabels[i]:setTextColorHexValue(self.nodeLabelRestColor[i])
        end
    end
    self.upgradeButtons[index]:setGlowVisible(true)
    self.nodeLabels[index]:setTextColorHexValue(treeNodes[index].glow)
    self.infoPanel:showFor(treeNodes[index])
end

function TowerUpgradesPanel:setIsVisible(isVisible)
    self.isVisible = isVisible
    self.background:setIsVisible(isVisible)
    self.canvasBg:setIsVisible(isVisible)
    self.headerBorder:setIsVisible(isVisible)
    self.footerBorder:setIsVisible(isVisible)
    self.bodyDivider:setIsVisible(isVisible)
    self.preLabel:setIsVisible(isVisible)
    self.titleLabel:setIsVisible(isVisible)
    self.footerLabel:setIsVisible(isVisible)
    self.closeButton:setIsVisible(isVisible)
    self.scrollList:setIsVisible(isVisible)
    self.contentContainer:setIsVisible(isVisible)
    for i = 1, #self.legendDots do self.legendDots[i]:setIsVisible(isVisible) end
    for i = 1, #self.legendLabels do self.legendLabels[i]:setIsVisible(isVisible) end
    for i = 1, #self.cornerBraces do self.cornerBraces[i]:setIsVisible(isVisible) end
    for i = 1, #self.tierRailMain do self.tierRailMain[i]:setIsVisible(isVisible) end
    for i = 1, #self.tierRailSub do self.tierRailSub[i]:setIsVisible(isVisible) end
    for i = 1, #self.dividers do self.dividers[i]:setIsVisible(isVisible) end
    for i = 1, #self.connectionLines do self.connectionLines[i]:setIsVisible(isVisible) end
    for i = 1, #self.upgradeButtons do self.upgradeButtons[i]:setIsVisible(isVisible) end
    for i = 1, #self.nodeLabels do self.nodeLabels[i]:setIsVisible(isVisible) end
    for i = 1, #self.tierBadges do self.tierBadges[i]:setIsVisible(isVisible) end
    for i = 1, #self.tierBadgeLabels do self.tierBadgeLabels[i]:setIsVisible(isVisible) end
    -- Open with the root weapon selected (matches the reference, which highlights he_rocket by default).
    if isVisible then
        self:selectNode(1)
    else
        self.infoPanel:hide()
    end
end

function TowerUpgradesPanel:update() end

return TowerUpgradesPanel
