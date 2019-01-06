function dump_table (T)
      for k,v in pairs (T) do
	 print (k, v)
      end
      print ("===")
end      

function setup_wifi()
   if not file.exists ("auth.lua") then
      print ("missing auth.lua")
      return
   end
   dofile ("auth.lua")
   print ("connecting to", wifi_auth.ssid)

   local cfg = {}
   cfg.ssid = wifi_auth.ssid
   cfg.pwd = wifi_auth.pw
   cfg.save = true

   cfg.connected_cb = function (T) 
      print ("wifi connected")
      dump_table (T)
   end

   cfg.disconnected_cb = function (T) 
      print ("wifi disconnected")
      dump_table (T)
   end

   cfg.got_ip_cb = function (T) 
      print ("wifi got ip")
      dump_table (T)
   end

   cfg.dhcp_timeout_cb = function (T) 
      print ("wifi dhcp timeout")
      dump_table (T)
   end

   wifi.sta.config (cfg)
   
end

function getip ()
   return wifi.sta.getip()
end

function ip ()
   print (getip ())
end

setup_wifi ()
