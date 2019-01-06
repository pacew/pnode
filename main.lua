if stop ~= nil then
   stop()
end


rev = "beacon1"

input_pins = {}

-- 0 GPIO16 USER WAKE
-- 1 GPIO5
-- 2 GPIO4 
-- 3 GPIO0 FLASH
-- 4 GPIO2 TXD1
-- 5 GPIO14 HSCLK
-- 6 GPIO12 HMISO
-- 7 GPIO13 RXD2 HMOSI
-- 8 GPIO15 TXD2 HCS
-- 9 GPIO3 RXD0 console serial rcv 
-- 10 GPIO1 TXD0 console serial xmit
-- 11 GPIO9 SDD2
-- 12 GPIO10 SDD3


for idx, val in ipairs {1, 2, 4, 5, 6, 7, 8} do
   pin = {}
   pin.ioidx = val
   pin.mask = 2 ^ val
   pin.stable_val = 0
   pin.changing = 0
   pin.usecs = tmr.now()
   table.insert (input_pins, pin)

   gpio.mode(pin.ioidx, gpio.INPUT, gpio.PULLUP)
end

if sock ~= nil then
   sock:close()
end
sock = net.createUDPSocket()
sock:send(13277, "224.0.0.1", "hello")

something_changed = false

function debounce ()
   for idx, pin in ipairs (input_pins) do
      local curval = gpio.read (pin.ioidx)
      if pin.changing == 0 then
	 if curval ~= pin.stable_val then
	    pin.changing = 1
	    pin.usecs = tmr.now ()
	 end
      else
	 local delta_usecs = tmr.now() - pin.usecs
	 if curval == pin.stable_val then
	    -- went back to stable val, so cancel changing for now
	    pin.changing = 0
	 elseif delta_usecs > 10 * 1000 then
	    -- been in new state for long enough, so lock it in
	    pin.stable_val = curval
	    pin.changing = 0
	    something_changed = true
	 end
      end
   end
end

function stop()
   tmr.stop (0)
end

last_report_usecs = tmr.now()

function report_status()
   local delta = tmr.now() - last_report_usecs
   if delta > 1 * 1000 * 1000 then
      last_report_usecs = tmr.now()
      print ("heap avail", node.heap())
   end
end


seq = 0

last_xmit_usecs = tmr.now()
min_usecs = 10 * 1000
max_usecs = 2 * 1000 * 1000

function tick()
   report_status()
   
   seq = seq + 1

   debounce()

   xmit_this_time = false

   local delta = tmr.now () - last_xmit_usecs
   if delta > max_usecs then
      xmit_this_time = true
   end

   if something_changed and delta > min_usecs then
      xmit_this_time = true
      something_changed = false
   end

   if not xmit_this_time then
      tmr.start (0)
      return
   end

   last_xmit_usecs = tmr.now()

   local inputs = 0

   for idx, pin in ipairs (input_pins) do
      if pin.stable_val == 1 then
	 inputs = inputs + pin.mask
      end
   end

   local ret = {}
   table.insert(ret, rev)
   table.insert(ret, wifi.ap.getmac())
   table.insert(ret, seq)
   table.insert(ret, tmr.now())
   table.insert(ret, string.format ("0x%x", inputs))
   
   local str = table.concat (ret, ' ')
   print (str)
   sock:send(13277, "224.0.0.1", str)

   tmr.start (0)
end

tmr.alarm (0, 100, tmr.ALARM_SEMI, tick)

