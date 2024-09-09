
wifi.setmode(wifi.STATION)
apcfg = {}
--apcfg.ssid = "ShiboiPhone"
--apcfg.pwd = "shibo!!020517"
--apcfg.ssid = "l2b26"
--apcfg.pwd = "pssm7007"
apcfg.ssid = "AL0723"
apcfg.pwd = "206901201"
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
