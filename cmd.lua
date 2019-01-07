if stop ~= nil then
   stop()
end

if sock ~= nil then
   sock:close()
end

sock = net.createUDPSocket()
sock:listen()
port = sock:getaddr()
print ("listening on port", port)

sock:on ("receive", 
	 function(s, data)
	    print ("udp rcv", data)
end)

