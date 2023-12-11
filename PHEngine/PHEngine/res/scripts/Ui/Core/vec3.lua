Vec3 = {
    __add = function(t1, t2)
        local ret = Vec3:new(0, 0, 0)
        ret.x = t1.x + t2.x
        ret.y = t1.y + t2.y
        ret.z = t1.z + t2.z
        return ret
    end,
    __sub = function(t1, t2)
        local ret = Vec3:new(0, 0, 0)
        ret.x = t1.x - t2.x
        ret.y = t1.y - t2.y
        ret.z = t1.z - t2.z
        return ret
    end
}

function Vec3:new(x, y, z)
    local newObj = {
        x = x,
        y = y,
        z = z
    }
    self.__index = self
    return setmetatable(newObj, self)
end

function Vec3:__len()
    local length = 0
    for _, _ in pairs(self) do
        length = length + 1
    end
    return length
end

return Vec3
