function cat(filename)
   local fd = file.open(filename, "r")
   while true do
      local line = fd:readline ()
      if line == nil then
	 break
      end
      local len = line:len()
      if len > 0 and line:byte(len) == 10 then
	 line = line:match("(.*).$")
      end
      uart.write (0, line)
      uart.write (0, "\r\n")
   end
end
