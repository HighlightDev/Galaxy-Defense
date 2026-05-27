Styles = {
    Colors = {
        panelColor = 0x919191,
        buttonColor = 0xa9a9a9,
        hoveredButtonColor = 0xdedcdc,
        notActiveButtonColor = 0x4b4b4b,
        headerPanelColor = 0x7a7a7a
    },
    -- Shared visual style for the tower tech-tree panels (TowerUpgradesPanel + WeaponInfoPanel),
    -- modelled after the TowerTechTree.jsx reference: dark navy surfaces, large corner radius.
    TechTree = {
        panelColor = 0x0a1628,
        panelBorderRadius = 16,
        accentColor = 0x1a3050,
        buttonColor = 0x16243a,
        hoveredButtonColor = 0x24375a
    },
    -- Main menu palette, modelled after UiDesign/menu-centered.jsx (Galaxy Defense mockup).
    -- A dark sci-fi look on top of the space skybox: translucent surfaces, indigo highlight
    -- for the primary action, dim slate for secondary text. Hex values below match the JSX
    -- accent/glow constants so the runtime look stays close to the reference.
    MainMenu = {
        -- Surfaces
        scrimColor = 0x04060c, -- soft full-screen darkener over the skybox
        scrimOpacity = 0.55,
        buttonBgColor = 0x0a0f18,
        buttonBgOpacity = 0.78,
        buttonHoverBgColor = 0x141d2e,
        buttonHoverBgOpacity = 0.88,
        -- Frosted-glass tap on the menu buttons. Derived from the mockup's
        -- .menu-btn--centered (UiDesign/menu.css L407-441): two-stop gradient
        -- with α ≈ 0.55-0.70 on top of `backdrop-filter: blur(6px)`, which
        -- leaves ~35% of the Gaussian-blurred backdrop showing. The hover /
        -- highlight state goes to α 0.85, dropping that to ~15%.
        buttonBlurMix = 0.35,
        buttonHoverBlurMix = 0.15,
        chipBgColor = 0x0a0f18,
        chipBgOpacity = 0.70,
        cornerColor = 0x4a9eff, -- accent for the corner L-brackets
        borderRadius = 4,
        -- Accents (button per-row "accent" from the JSX MENU_ITEMS list)
        accentPrimary = 0x818cf8, -- highlighted row (indigo)
        accentSecondary = 0x4a9eff, -- bright blue
        accentMuted = 0x5e7a99, -- slate
        -- Text
        textBright = 0xcdd6f4,
        textDim = 0x8a99ac,
        textVeryDim = 0x5a6e86,
        textTitleAccent = 0x90e0ef
    }
}

return Styles
