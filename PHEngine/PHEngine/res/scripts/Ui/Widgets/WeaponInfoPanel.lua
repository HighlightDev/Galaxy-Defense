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
local UiLabel = require("Ui/Core/uiLabel")
local UpgradeIcon = require("Ui/Core/uiUpgradeIcon")
local UiItemBase = require("Ui/Core/uiItemBase")
local Styles = require("Ui/Common/styles")

WeaponInfoPanel = {}

local LABEL_FONT = "Lora-VariableFont_wght"

local DESC_LINE_COUNT = 7
local STAT_SEGMENTS = 5
local PARENT_ROW_COUNT = 2

local STAT_DEFS = {
    {key = "damage", label = "Урон", color = 0xe85d04, alwaysShow = true},
    {key = "speed", label = "Скорострельность", color = 0x4a9eff, alwaysShow = true},
    {key = "range", label = "Дальность", color = 0x2ecc71, alwaysShow = true},
    {key = "area", label = "Площадь взрыва", color = 0xf72585, alwaysShow = true},
    {key = "slow", label = "Замедление", color = 0x00b4d8, alwaysShow = true},
    {key = "chain", label = "Цепной удар", color = 0x7b2d8b, alwaysShow = false},
    {key = "repair", label = "Ремонт щита", color = 0x22c55e, alwaysShow = false}
}
local STAT_COUNT = #STAT_DEFS

local CATEGORY_INFO = {
    offense = {label = "АТАКА", color = 0xe85d04},
    support = {label = "ПОДДЕРЖКА", color = 0x22c55e},
    hybrid = {label = "ГИБРИД", color = 0x818cf8}
}

local COLOR_PANEL_BG = Styles.TechTree.panelColor
local COLOR_TAG_TEXT = 0x0a0f18
local COLOR_TYPE_TEXT = 0x8294ab
local COLOR_DESC_TEXT = 0x8a99ac
local COLOR_SECTION_TEXT = 0x5a6e86
local COLOR_STAT_NAME = 0x8294ab
local COLOR_SEGMENT_OFF = 0x1c2738
local COLOR_SEPARATOR = Styles.TechTree.accentColor
local COLOR_PARENT_BG = 0x141f33

local function ulen(s) return utf8.len(s) or #s end

local function wrapText(text, maxChars)
    local lines = {}
    local current = ""
    for word in tostring(text):gmatch("%S+") do
        if current == "" then
            current = word
        elseif ulen(current) + 1 + ulen(word) <= maxChars then
            current = current .. " " .. word
        else
            lines[#lines + 1] = current
            current = word
        end
    end
    if current ~= "" then lines[#lines + 1] = current end
    return lines
end

function WeaponInfoPanel:new(host, overlay)
    local newObj = {
        host = host,
        weaponById = nil,
        descLabels = {},
        statRows = {},
        parentRows = {},
        contentWidth = 0,
        descFontSize = 11.0
    }
    setmetatable(newObj, self)
    self.__index = self

    newObj.background = UiRectangle:new(host, "WeaponInfoPanelBg")
    overlay:addWidget(newObj.background)

    newObj.categoryTag = UiRectangle:new(host, "WeaponInfoCategoryTag")
    overlay:addWidget(newObj.categoryTag)
    newObj.categoryLabel = UiLabel:new(host, LABEL_FONT, "WeaponInfoCategoryLabel")
    overlay:addWidget(newObj.categoryLabel)

    newObj.headerIcon = UpgradeIcon:new(host, "WeaponInfoHeaderIcon")
    overlay:addWidget(newObj.headerIcon)
    newObj.nameLabel = UiLabel:new(host, LABEL_FONT, "WeaponInfoNameLabel")
    overlay:addWidget(newObj.nameLabel)
    newObj.typeLabel = UiLabel:new(host, LABEL_FONT, "WeaponInfoTypeLabel")
    overlay:addWidget(newObj.typeLabel)

    for i = 1, DESC_LINE_COUNT do
        local line = UiLabel:new(host, LABEL_FONT, "WeaponInfoDesc" .. tostring(i))
        overlay:addWidget(line)
        newObj.descLabels[i] = line
    end

    newObj.statsSeparator = UiRectangle:new(host, "WeaponInfoStatsSeparator")
    overlay:addWidget(newObj.statsSeparator)
    newObj.statsHeader = UiLabel:new(host, LABEL_FONT, "WeaponInfoStatsHeader")
    overlay:addWidget(newObj.statsHeader)

    for i = 1, STAT_COUNT do
        local row = {segments = {}}
        row.nameLabel = UiLabel:new(host, LABEL_FONT, "WeaponInfoStatName" .. tostring(i))
        overlay:addWidget(row.nameLabel)
        row.valueLabel = UiLabel:new(host, LABEL_FONT, "WeaponInfoStatValue" .. tostring(i))
        overlay:addWidget(row.valueLabel)
        for s = 1, STAT_SEGMENTS do
            local seg = UiRectangle:new(host, "WeaponInfoStatSeg" .. tostring(i) .. "_" .. tostring(s))
            overlay:addWidget(seg)
            row.segments[s] = seg
        end
        newObj.statRows[i] = row
    end

    newObj.parentsSeparator = UiRectangle:new(host, "WeaponInfoParentsSeparator")
    overlay:addWidget(newObj.parentsSeparator)
    newObj.parentsHeader = UiLabel:new(host, LABEL_FONT, "WeaponInfoParentsHeader")
    overlay:addWidget(newObj.parentsHeader)

    for i = 1, PARENT_ROW_COUNT do
        local row = {}
        row.bg = UiRectangle:new(host, "WeaponInfoParentBg" .. tostring(i))
        overlay:addWidget(row.bg)
        row.icon = UpgradeIcon:new(host, "WeaponInfoParentIcon" .. tostring(i))
        overlay:addWidget(row.icon)
        row.label = UiLabel:new(host, LABEL_FONT, "WeaponInfoParentLabel" .. tostring(i))
        overlay:addWidget(row.label)
        newObj.parentRows[i] = row
    end

    return newObj
end

function WeaponInfoPanel:setupLayout(canvasName, refBackgroundName, windowWidth, infoWidth, infoHeight)
    local anchors = UiItemBase.UiAnchorType
    local pad = math.floor(infoWidth * 0.075)
    local contentW = infoWidth - 2 * pad
    self.contentWidth = contentW

    local fTitle, fType, fDesc, fSection, fStat = 14.0, 9.0, 11.0, 9.0, 10.0
    self.descFontSize = fDesc

    local bgName = self.background.widgetName

    local function placeTopLeft(widget, parentName, offX, offY)
        widget:setAnchor(anchors.LEFT, anchors.LEFT, parentName, offX)
        widget:setAnchor(anchors.TOP, anchors.TOP, parentName, offY)
    end

    self.background:setParent(self.host, canvasName, canvasName)
    self.background:setWidth(infoWidth)
    self.background:setHeight(infoHeight)
    self.background:setAnchor(anchors.LEFT, anchors.RIGHT, refBackgroundName, math.floor(windowWidth * 0.012))
    self.background:setAnchor(anchors.TOP, anchors.TOP, refBackgroundName, 0)
    self.background:setColorHexValue(COLOR_PANEL_BG)
    self.background:setBorderRadius(Styles.TechTree.panelBorderRadius)
    self.background:setZOrder(10)
    self.background:setIsVisible(false)

    local cy = pad

    -- Category tag
    local tagWidth = math.floor(contentW * 0.52)
    local tagHeight = math.floor(infoHeight * 0.028)
    self.categoryTag:setParent(self.host, canvasName, bgName)
    self.categoryTag:setWidth(tagWidth)
    self.categoryTag:setHeight(tagHeight)
    placeTopLeft(self.categoryTag, bgName, pad, cy)
    self.categoryTag:setBorderRadius(tagHeight // 2)
    self.categoryTag:setZOrder(11)
    self.categoryTag:setIsVisible(false)

    self.categoryLabel:setParent(self.host, canvasName, self.categoryTag.widgetName)
    self.categoryLabel:setWidth(tagWidth)
    self.categoryLabel:setHeight(tagHeight)
    self.categoryLabel:fill(self.categoryTag.widgetName)
    self.categoryLabel:setFontSize(9.0)
    self.categoryLabel:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.CENTER)
    self.categoryLabel:setTextVerticalAlignment(UiLabel.TextVerticalAlignmentType.CENTER)
    self.categoryLabel:setZOrder(12)
    self.categoryLabel:setIsVisible(false)

    cy = cy + tagHeight + math.floor(infoHeight * 0.018)

    -- Header: icon + name + type
    local iconSize = math.floor(infoWidth * 0.16)
    self.headerIcon:setParent(self.host, canvasName, bgName)
    self.headerIcon:setWidth(iconSize)
    self.headerIcon:setHeight(iconSize)
    placeTopLeft(self.headerIcon, bgName, pad, cy)
    self.headerIcon:setRotationDegrees(180)
    self.headerIcon:setZOrder(11)
    self.headerIcon:setCanBloomBeApplied(false)
    self.headerIcon:setIsVisible(false)

    local headerTextX = pad + iconSize + math.floor(infoWidth * 0.04)
    local headerTextW = contentW - iconSize - math.floor(infoWidth * 0.04)
    self.nameLabel:setParent(self.host, canvasName, bgName)
    self.nameLabel:setWidth(headerTextW)
    self.nameLabel:setHeight(math.floor(iconSize * 0.42))
    placeTopLeft(self.nameLabel, bgName, headerTextX, cy + math.floor(iconSize * 0.14))
    self.nameLabel:setFontSize(fTitle)
    self.nameLabel:setZOrder(11)
    self.nameLabel:setIsVisible(false)

    self.typeLabel:setParent(self.host, canvasName, bgName)
    self.typeLabel:setWidth(headerTextW)
    self.typeLabel:setHeight(math.floor(iconSize * 0.32))
    placeTopLeft(self.typeLabel, bgName, headerTextX, cy + math.floor(iconSize * 0.56))
    self.typeLabel:setFontSize(fType)
    self.typeLabel:setZOrder(11)
    self.typeLabel:setIsVisible(false)

    cy = cy + iconSize + math.floor(infoHeight * 0.022)

    -- Description lines
    local descLineHeight = math.floor(fDesc) + 5
    for i = 1, DESC_LINE_COUNT do
        local line = self.descLabels[i]
        line:setParent(self.host, canvasName, bgName)
        line:setWidth(contentW)
        line:setHeight(descLineHeight)
        placeTopLeft(line, bgName, pad, cy + (i - 1) * descLineHeight)
        line:setFontSize(fDesc)
        line:setZOrder(11)
        line:setIsVisible(false)
    end
    cy = cy + DESC_LINE_COUNT * descLineHeight + math.floor(infoHeight * 0.012)

    -- Stats section
    self.statsSeparator:setParent(self.host, canvasName, bgName)
    self.statsSeparator:setWidth(contentW)
    self.statsSeparator:setHeight(1)
    placeTopLeft(self.statsSeparator, bgName, pad, cy)
    self.statsSeparator:setColorHexValue(COLOR_SEPARATOR)
    self.statsSeparator:setZOrder(11)
    self.statsSeparator:setIsVisible(false)
    cy = cy + math.floor(infoHeight * 0.016)

    self.statsHeader:setParent(self.host, canvasName, bgName)
    self.statsHeader:setWidth(contentW)
    self.statsHeader:setHeight(14)
    placeTopLeft(self.statsHeader, bgName, pad, cy)
    self.statsHeader:setFontSize(fSection)
    self.statsHeader:setZOrder(11)
    self.statsHeader:setIsVisible(false)
    cy = cy + 14 + math.floor(infoHeight * 0.012)

    local segGap = 4
    local segWidth = math.floor((contentW - (STAT_SEGMENTS - 1) * segGap) / STAT_SEGMENTS)
    local segHeight = 7
    local statValueLineH = 14
    local statRowHeight = statValueLineH + 4 + segHeight + 11
    for i = 1, STAT_COUNT do
        local row = self.statRows[i]
        local rowY = cy + (i - 1) * statRowHeight

        row.nameLabel:setParent(self.host, canvasName, bgName)
        row.nameLabel:setWidth(math.floor(contentW * 0.66))
        row.nameLabel:setHeight(statValueLineH)
        placeTopLeft(row.nameLabel, bgName, pad, rowY)
        row.nameLabel:setFontSize(fStat)
        row.nameLabel:setZOrder(11)
        row.nameLabel:setIsVisible(false)

        row.valueLabel:setParent(self.host, canvasName, bgName)
        row.valueLabel:setWidth(math.floor(contentW * 0.3))
        row.valueLabel:setHeight(statValueLineH)
        row.valueLabel:setAnchor(anchors.RIGHT, anchors.RIGHT, bgName, pad)
        row.valueLabel:setAnchor(anchors.TOP, anchors.TOP, bgName, rowY)
        row.valueLabel:setFontSize(fStat)
        row.valueLabel:setTextHorizontalAlignment(UiLabel.TextHorizontalAlignmentType.RIGHT)
        row.valueLabel:setZOrder(11)
        row.valueLabel:setIsVisible(false)

        local segY = rowY + statValueLineH + 4
        for s = 1, STAT_SEGMENTS do
            local seg = row.segments[s]
            seg:setParent(self.host, canvasName, bgName)
            seg:setWidth(segWidth)
            seg:setHeight(segHeight)
            placeTopLeft(seg, bgName, pad + (s - 1) * (segWidth + segGap), segY)
            seg:setBorderRadius(2)
            seg:setZOrder(11)
            seg:setIsVisible(false)
        end
    end
    cy = cy + STAT_COUNT * statRowHeight + math.floor(infoHeight * 0.006)

    -- Parents section
    self.parentsSeparator:setParent(self.host, canvasName, bgName)
    self.parentsSeparator:setWidth(contentW)
    self.parentsSeparator:setHeight(1)
    placeTopLeft(self.parentsSeparator, bgName, pad, cy)
    self.parentsSeparator:setColorHexValue(COLOR_SEPARATOR)
    self.parentsSeparator:setZOrder(11)
    self.parentsSeparator:setIsVisible(false)
    cy = cy + math.floor(infoHeight * 0.016)

    self.parentsHeader:setParent(self.host, canvasName, bgName)
    self.parentsHeader:setWidth(contentW)
    self.parentsHeader:setHeight(14)
    placeTopLeft(self.parentsHeader, bgName, pad, cy)
    self.parentsHeader:setFontSize(fSection)
    self.parentsHeader:setZOrder(11)
    self.parentsHeader:setIsVisible(false)
    cy = cy + 14 + math.floor(infoHeight * 0.012)

    local parentRowHeight = math.floor(infoWidth * 0.1)
    local parentRowGap = math.floor(infoHeight * 0.008)
    local parentIconSize = math.floor(parentRowHeight * 0.74)
    for i = 1, PARENT_ROW_COUNT do
        local row = self.parentRows[i]
        local rowY = cy + (i - 1) * (parentRowHeight + parentRowGap)

        row.bg:setParent(self.host, canvasName, bgName)
        row.bg:setWidth(contentW)
        row.bg:setHeight(parentRowHeight)
        placeTopLeft(row.bg, bgName, pad, rowY)
        row.bg:setColorHexValue(COLOR_PARENT_BG)
        row.bg:setBorderRadius(6)
        row.bg:setZOrder(11)
        row.bg:setIsVisible(false)

        row.icon:setParent(self.host, canvasName, row.bg.widgetName)
        row.icon:setWidth(parentIconSize)
        row.icon:setHeight(parentIconSize)
        row.icon:setAnchor(anchors.LEFT, anchors.LEFT, row.bg.widgetName, math.floor(parentRowHeight * 0.13))
        row.icon:setAnchor(anchors.VERTICAL_CENTER, anchors.VERTICAL_CENTER, row.bg.widgetName)
        row.icon:setRotationDegrees(180)
        row.icon:setZOrder(12)
        row.icon:setCanBloomBeApplied(false)
        row.icon:setIsVisible(false)

        row.label:setParent(self.host, canvasName, row.bg.widgetName)
        row.label:setWidth(contentW - parentIconSize - math.floor(parentRowHeight * 0.5))
        row.label:setHeight(18)
        row.label:setAnchor(anchors.LEFT, anchors.LEFT, row.bg.widgetName,
                            parentIconSize + math.floor(parentRowHeight * 0.26))
        row.label:setAnchor(anchors.VERTICAL_CENTER, anchors.VERTICAL_CENTER, row.bg.widgetName)
        row.label:setFontSize(fStat)
        row.label:setZOrder(12)
        row.label:setIsVisible(false)
    end
end

function WeaponInfoPanel:showFor(weapon)
    assert(weapon ~= nil, debug.traceback())

    self.background:setIsVisible(true)

    local category = CATEGORY_INFO[weapon.category] or CATEGORY_INFO.offense
    self.categoryTag:setIsVisible(true)
    self.categoryTag:setColorHexValue(category.color)
    self.categoryLabel:setIsVisible(true)
    self.categoryLabel:setText(category.label)
    self.categoryLabel:setTextColorHexValue(COLOR_TAG_TEXT)

    self.headerIcon:setIsVisible(true)
    self.headerIcon:setTextureSource(weapon.texture)
    self.headerIcon:setBorderColorHexValue(weapon.color)
    self.headerIcon:setGlowColorHexValue(weapon.glow)
    self.headerIcon:setFillColorHexValue(weapon.color)
    self.headerIcon:setGlowVisible(false)

    self.nameLabel:setIsVisible(true)
    self.nameLabel:setText(weapon.name)
    self.nameLabel:setTextColorHexValue(weapon.glow)

    self.typeLabel:setIsVisible(true)
    self.typeLabel:setText(weapon.weaponType or "")
    self.typeLabel:setTextColorHexValue(COLOR_TYPE_TEXT)

    local maxChars = math.max(12, math.floor(self.contentWidth / (self.descFontSize * 0.62)))
    local lines = wrapText(weapon.desc or "", maxChars)
    for i = 1, DESC_LINE_COUNT do
        local line = self.descLabels[i]
        if lines[i] ~= nil then
            line:setIsVisible(true)
            line:setText(lines[i])
            line:setTextColorHexValue(COLOR_DESC_TEXT)
        else
            line:setIsVisible(false)
        end
    end

    self.statsSeparator:setIsVisible(true)
    self.statsHeader:setIsVisible(true)
    self.statsHeader:setText("ХАРАКТЕРИСТИКИ")
    self.statsHeader:setTextColorHexValue(COLOR_SECTION_TEXT)
    for i = 1, STAT_COUNT do
        local def = STAT_DEFS[i]
        local row = self.statRows[i]
        local value = weapon[def.key] or 0
        local visible = def.alwaysShow or value > 0
        if visible then
            row.nameLabel:setIsVisible(true)
            row.nameLabel:setText(def.label)
            row.nameLabel:setTextColorHexValue(COLOR_STAT_NAME)
            row.valueLabel:setIsVisible(true)
            row.valueLabel:setText(tostring(value) .. "/5")
            row.valueLabel:setTextColorHexValue(def.color)
            for s = 1, STAT_SEGMENTS do
                local seg = row.segments[s]
                seg:setIsVisible(true)
                if s <= value then
                    seg:setColorHexValue(def.color)
                else
                    seg:setColorHexValue(COLOR_SEGMENT_OFF)
                end
            end
        else
            row.nameLabel:setIsVisible(false)
            row.valueLabel:setIsVisible(false)
            for s = 1, STAT_SEGMENTS do row.segments[s]:setIsVisible(false) end
        end
    end

    local parents = weapon.parents or {}
    local hasParents = #parents > 0
    self.parentsSeparator:setIsVisible(hasParents)
    self.parentsHeader:setIsVisible(hasParents)
    if hasParents then
        self.parentsHeader:setText("ТРЕБУЕТСЯ")
        self.parentsHeader:setTextColorHexValue(COLOR_SECTION_TEXT)
    end
    for i = 1, PARENT_ROW_COUNT do
        local row = self.parentRows[i]
        local parentId = parents[i]
        local parent = nil
        if parentId ~= nil and self.weaponById ~= nil then parent = self.weaponById[parentId] end
        if parent ~= nil then
            row.bg:setIsVisible(true)
            row.icon:setIsVisible(true)
            row.icon:setTextureSource(parent.texture)
            row.icon:setBorderColorHexValue(parent.color)
            row.icon:setGlowColorHexValue(parent.glow)
            row.icon:setFillColorHexValue(parent.color)
            row.icon:setGlowVisible(false)
            row.label:setIsVisible(true)
            row.label:setText(parent.name)
            row.label:setTextColorHexValue(parent.glow)
        else
            row.bg:setIsVisible(false)
            row.icon:setIsVisible(false)
            row.label:setIsVisible(false)
        end
    end
end

function WeaponInfoPanel:hide()
    self.background:setIsVisible(false)
    self.categoryTag:setIsVisible(false)
    self.categoryLabel:setIsVisible(false)
    self.headerIcon:setIsVisible(false)
    self.nameLabel:setIsVisible(false)
    self.typeLabel:setIsVisible(false)
    for i = 1, DESC_LINE_COUNT do self.descLabels[i]:setIsVisible(false) end
    self.statsSeparator:setIsVisible(false)
    self.statsHeader:setIsVisible(false)
    for i = 1, STAT_COUNT do
        local row = self.statRows[i]
        row.nameLabel:setIsVisible(false)
        row.valueLabel:setIsVisible(false)
        for s = 1, STAT_SEGMENTS do row.segments[s]:setIsVisible(false) end
    end
    self.parentsSeparator:setIsVisible(false)
    self.parentsHeader:setIsVisible(false)
    for i = 1, PARENT_ROW_COUNT do
        local row = self.parentRows[i]
        row.bg:setIsVisible(false)
        row.icon:setIsVisible(false)
        row.label:setIsVisible(false)
    end
end

return WeaponInfoPanel
