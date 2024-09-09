wifi.setmode(wifi.STATION)
apcfg = {}

apcfg.ssid = "AL0723"
apcfg.pwd = "206901201"

--apcfg.ssid = "l2b26"
--apcfg.pwd = "pssm7007"

--apcfg.ssid = "Sonya's iphoneX"
--apcfg.pwd = "zxcvbnm"

--apcfg.ssid = "Shiboiphone"
--apcfg.pwd = "shibo!!020517"

------------------------------------------------
function print_c(c)
    local sdata = 1
    local maxsize =5
    while sdata <= string.len(c) do
        if (string.len(c) - sdata < maxsize) then
            uart.write(0, string.sub(c, sdata, string.len(c)))
        else
            uart.write(0, string.sub(c, sdata, sdata + maxsize - 1))
        end
        tmr.delay(500) -- 500 ms delay (in microseconds)
        sdata = sdata + maxsize
    end
    uart.write(0, "\n")
    sdata = 1
end

function display(sck, c)
    if (c ~= nil) then
        print_c(c)
    else
        print("error: c=nil")
    end
    collectgarbage()
end
-------------------------------------------



-----------------------------------------
function wifi_check()
    print(wifi.sta.status())
    print(wifi.sta.getip())
end

------------------------------------------
---- server
---------------------------------------------
--function get_webpage()
--    local sk = net.createConnection(net.TCP, 0)
--    sk:on("receive", function(sck, c) display(sck, c) end)
--    --sk:connect(12521, "207.216.17.76")
--    sk:connect(12521, "192.168.43.90")
--    sk:send("GET /test HTTP/1.1\r\nConnection: keep-alive\r\nAccept: */*\r\n\r\n")
--end
--
--function get_render(cookie)
--    local sk = net.createConnection(net.TCP, 0)
--    sk:on("receive", function(sck, c) display(sck, c) end)
--    --sk:connect(12521, "207.216.17.76")
--    sk:connect(12521, "192.168.43.90")
--    --sk:send("GET /console/render HTTP/1.1\r\nConnection: keep-alive\r\nAccept: */*\r\n\r\n")
--    local session_id = tonumber(string.match(cookie, "%d+"))
--    sk:send("GET /console/render HTTP/1.1\r\nConnection: keep-alive\r\nAccept: */*\r\nCookie: session_id=" .. tostring(session_id) .. "\r\n\r\n")
--
--end
------------------------------------------------------
--
--function post_register()
--    local sk = net.createConnection(net.TCP, 0)
--    sk:on("receive", function(sck, c) display(sck, c) end)
--    --sk:connect(12521, "207.216.17.76")
--    sk:connect(12521, "192.168.43.90")
--    sk:send("POST /console/register HTTP/1.1\r\nConnection: keep-alive\r\nAccept: */*\r\nContent-Length: 0\r\n\r\n")
--end
--
--function post_unregister(cookie)
--    local sk = net.createConnection(net.TCP, 0)
--    sk:on("receive", function(sck, c) display(sck, c) end)
--    --sk:connect(12521, "207.216.17.76")
--    sk:connect(12521, "192.168.43.90")
--    local session_id = tonumber(string.match(cookie, "%d+"))
--    sk:send("POST /console/unregister HTTP/1.1\r\nConnection: keep-alive\r\nAccept: */*\r\nCookie: session_id=" .. tostring(session_id) .. "\r\n\r\n")
--end
--
-------------------------------------------
---- open website
-------------------------------------------
--
--function print_test()
--    local sk = net.createConnection(net.TCP, 0)
--    sk:on("receive", function(sck, c) display(sck, c) end)
--    --sk:connect(80, "52.207.219.136")
--    sk:connect(12521, "192.168.43.90")
--    sk:send("GET /testwifi/index.html HTTP/1.1\r\nHost: wifitest.adafruit.com\r\nConnection: keep-alive\r\nAccept: */*\r\n\r\n")
--end
----------------------------------------
----**post
---------------------------------------------
