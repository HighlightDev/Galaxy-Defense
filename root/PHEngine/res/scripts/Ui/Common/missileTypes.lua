-- Mirrors Game::eMissileType from GalaxyDefense/src/Implementation/MissileType.h
local MissileType = {
    NONE = 0,
    BOMB = 1,
    FREEZING_BOMB = 2,
    ELECTRO_RAY = 3,
    BLACK_HOLE = 4,
    FREEZING_RAY = 5,
    PLASMA_BOMB = 6,
    REPAIR_BEAM = 7,
    FORCE_BARRIER = 8
}

local IconByName = {
    BOMB = "he_rocket.png",
    FREEZING_BOMB = "ice_rocket.png",
    ELECTRO_RAY = "electric_beam.png",
    BLACK_HOLE = "gravity_bomb.png",
    FREEZING_RAY = "ice_beam.png",
    PLASMA_BOMB = "plasma_rocket.png",
    REPAIR_BEAM = "repair_beam.png",
    FORCE_BARRIER = "force_barrier.png"
}

local IconByValue = {}
for name, value in pairs(MissileType) do if IconByName[name] then IconByValue[value] = IconByName[name] end end

local function nameByValue(value)
    for name, v in pairs(MissileType) do if v == value then return name end end
    return nil
end

return {MissileType = MissileType, IconByName = IconByName, IconByValue = IconByValue, nameByValue = nameByValue}
