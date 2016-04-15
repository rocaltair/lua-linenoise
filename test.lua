l = require "linenoise"

local filename = "history.txt"

l.setCompletor(function(lc, pre)
	if string.sub(pre, 1, 1) == "h" then
		l.addComplete(lc, "hi")
		l.addComplete(lc, "hello")
	elseif string.sub(pre, 1, 1) == "w" then
		l.addComplete(lc, "world")
		l.addComplete(lc, "well")
	end
end)

l.historyLoad(filename)

while true do
	local line = l.line(">")
	if not line then
		break
	end
	l.historyAdd(line)
	print(string.format("line:[%s]", line))
	l.historySave(filename)
end

