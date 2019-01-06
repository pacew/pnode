function rcvfile(name)
   if rcvfile_fd ~= nil then
      rcvfile_fd:close()
   end
   rcvfile_fd = file.open(name, "w")
   local nbytes = 0
   
   uart.write (0, "\r\nready to receive\r\n")

   uart.on ("data", "\n",
	    function (line)
	       if line == ".\n" then
		  uart.on ("data")
		  rcvfile_fd:close()
		  rcvfile_fd = nil
		  uart.write(0, "\r\nrcvd "..tostring(nbytes).."\r\n> ")
	       else
		  nbytes = nbytes + line:len()
		  rcvfile_fd:write (line)
		  uart.write(0, "#")
	       end
	    end,
	    0)
end
