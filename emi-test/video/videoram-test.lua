#!/usr/bin/env lua

local c_name = "videoram-test"
local retval1 = 0

-- 我们重复运行 1 次
for i = 1, 1 do
	retval = os.execute("./" .. c_name)
	if retval ~= 0 then break end
end


dofile("../common.lua")

--add code here

if retval ~= 0 then
	log_error("VID01")
	os.exit(1)
else
	os.exit(0)
end

--add code over
