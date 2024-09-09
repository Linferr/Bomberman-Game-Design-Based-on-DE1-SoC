
wifi.setmode(wifi.STATION)
apcfg = {}
apcfg.ssid = "ShiboiPhone"
apcfg.pwd = "shibo!!020517"
--apcfg.ssid = "l2b26"
--apcfg.pwd = "pssm7007"
--apcfg.ssid = "AL0723"
--apcfg.pwd = "206901201"
--apcfg.ssid = "ubcvisitor"
--apcfg.pwd = ""

wifi.sta.config(apcfg)
wifi.sta.connect()

tmr.delay(2000000)   -- wait 1,000,000 us = 1 second
print(wifi.sta.status())
print(wifi.sta.getip())

-------------------------------------------
function check_wifi()
    print(wifi.sta.status())
    print(wifi.sta.getip())
end
-------------------------------------------
--function get_test()
--  local sk = net.createConnection(net.TCP, 0)
--    sk:on("receive", function(sck, c) print(c) end )
--    sk:connect(80,"52.207.219.136")
--    sk:send("GET /testwifi/index.html HTTP/1.1\r\nHost: wifitest.adafruit.com\r\nConnection: keep-alive\r\nAccept: */*\r\n\r\n")
--
--end
