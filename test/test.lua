local function printVec(v)
	assert(v)
	print('x:'..vec3.GetX(v)..'y:'..vec3.GetY(v)..'z:'..vec3.GetZ(v))
end

local function assertVec(v, x, y, z)
	assert(vec3.GetX(v) == x)
	assert(vec3.GetY(v) == y)
	assert(vec3.GetZ(v) == z)
end

print("Testing Math")

print("Constructor")
local v0 = vec3(0, 1, 2)
printVec(v0)
assertVec(v0, 0, 1, 2)

print("Set")
local v1 = vec3(0,0,0)
vec3.Set(v1, 3, 4, 5)
printVec(v1)
assertVec(v1, 3, 4, 5)

local q = Quaternion(0,0,0,1)
local v2 = Quaternion.Rotate(q, v0)

print("Add")
local v3 = vec3.Add(v1, v2)
printVec(v3)
assertVec(v3, 3, 5, 7)

-- Boxing
print("Boxing")
local boxed = vec3.Box()
local v4 = vec3(5, 6, 7)
vec3.Store(boxed, v4)
local v5 = vec3.Retrieve(boxed)
printVec(v5)
assert(vec3.Equal(v4, v5))

print("Done")


