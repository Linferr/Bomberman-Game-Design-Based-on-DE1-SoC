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

function get_webpage()
    local sk = net.createConnection(net.TCP, 0)
    sk:on("receive", function(sck, c) display(sck, c) end)
    sk:connect(12521, "207.216.17.76")
    --sk:connect(12521, "172.20.10.4")
    sk:send("GET /test HTTP/1.1\r\nConnection: close\r\nAccept: */*\r\n\r\n")
end

function get_render(cookie)
    local sk = net.createConnection(net.TCP, 0)
    sk:on("receive", function(sck, c) display(sck, c) end)
    --sk:connect(12521, "172.20.10.4")
    sk:connect(12521, "207.216.17.76")
    --sk:send("GET /console/render HTTP/1.1\r\nConnection: keep-alive\r\nAccept: */*\r\n\r\n")
    local session_id = tonumber(string.match(cookie, "%d+"))
    sk:send("GET /console/render HTTP/1.1\r\nConnection: close\r\nAccept: */*\r\nCookie: session_id=" .. tostring(session_id) .. "\r\n\r\n")

end
----------------------------------------------------

function post_register()
    local sk = net.createConnection(net.TCP, 0)
    sk:on("receive", function(sck, c) display(sck, c) end)
    sk:connect(12521, "207.216.17.76")
    --sk:connect(12521, "172.20.10.4")
    sk:send("POST /console/register HTTP/1.1\r\nConnection: close\r\nAccept: */*\r\nContent-Length: 0\r\n\r\n")
end

function post_unregister(cookie)
    local sk = net.createConnection(net.TCP, 0)
    sk:on("receive", function(sck, c) display(sck, c) end)
    sk:connect(12521, "207.216.17.76")
    --sk:connect(12521, "172.20.10.4")
    local session_id = tonumber(string.match(cookie, "%d+"))
    sk:send("POST /console/unregister HTTP/1.1\r\nConnection: close\r\nAccept: */*\r\nCookie: session_id=" .. tostring(session_id) .. "\r\n\r\n")
end

-----------------------------------------
-- open website
-----------------------------------------

function print_test()
    local sk = net.createConnection(net.TCP, 0)
    sk:on("receive", function(sck, c) display(sck, c) end)
    sk:connect(80, "52.207.219.136")
    sk:send("GET /testwifi/index.html HTTP/1.1\r\nHost: wifitest.adafruit.com\r\nConnection: keep-alive\r\nAccept: */*\r\n\r\n")
end



------
function send_loop()
    local count = 0
    
    while count <= 15 do
        get_render("session_id=0")
        count = count + 1
        print(count)
    end
end




------------------------------------------
-- server
-------------------------------------------
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
--    sk:connect(12521, "192.168.43.90")
--    --sk:connect(12521, "207.216.17.76")
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
--    sk:connect(80, "52.207.219.136")
--    sk:send("GET /testwifi/index.html HTTP/1.1\r\nHost: wifitest.adafruit.com\r\nConnection: keep-alive\r\nAccept: */*\r\n\r\n")
--end
















--------------------------------------
--**post
-------------------------------------------
--function post_test1()
--    -- create a TCP connection object
--    local sk = net.createConnection(net.TCP, 0)
--    
--    -- set up a callback function to handle incoming data
--    sk:on("receive", function(sck, c) 
--      display("Received data: " .. sck, c) 
--    end)
--    
--    -- initiate the TCP connection to the remote server
--    sk:connect(12521, "207.216.17.76")
--        
--    -- construct the HTTP POST request header
--    local post_request = "POST /console/register HTTP/1.1\r\n" ..
--                         "Host: 207.216.17.76:12521\r\n" ..
--                         "Content-Type: application/json\r\n" ..
--                         "Content-Length: 0\r\n" ..
--                         "Connection: close\r\n\r\n"
--    
--    -- send the HTTP POST request to the server
--    sk:send(post_request)
--
--end
----------------------------------------------
----**
---------------------------------------------
--function post_test2()
--    local sk = net.createConnection(net.TCP, 0)
--
--    -- set up a callback function to handle incoming data
--    --sk:on("receive", function(sck, c) print("Received data: " .. c) end)
--    sk:on("receive", function(sck, c) 
--      display("Received data: " .. sck, c) 
--    end)
--
--    -- initiate the TCP connection to the remote server
--    sk:connect(12521, "207.216.17.76")
--    --sk:connect(9000,"10.0.0.36")
--    
--    -- construct the HTTP POST request body in JSON format
--    local post_data = '{"username":"john"}'
--
--    -- construct the HTTP POST request header
--    local post_request = "POST /test HTTP/1.1\r\n" ..
--                        "Host: 207.216.17.76:12521\r\n" ..
--                        "Content-Type: application/json\r\n" ..
--                        "Content-Length: " .. string.len(post_data) .. "\r\n" ..
--                        "Connection: close\r\n\r\n" ..
--                        post_data
--
--    -- send the HTTP POST request to the server
--    sk:send(post_request)
--end
-------------------------------------------------
----**
-------------------------------------------------
--
--function post_multiple_data()
--  -- create a TCP connection object
--  local sk = net.createConnection(net.TCP, 0)
--  
--  -- set up a callback function to handle incoming data
--  --sk:on("receive", function(sck, c) print("Received data: " .. c) end)
--  sk:on("receive", function(sck, c) 
--      display("Received data: " .. sck, c) 
--    end)
--  
--  -- initiate the TCP connection to the remote server
--  sk:connect(12521, "207.216.17.76")
--  --sk:connect(9000,"10.0.0.36")
--
--  -- define the data to be posted
--  local data = {
--        { username = "john", age = 30 },
--        { username = "jane", age = 25 },
--        { username = "bob", age = 40 }
--    }
--
--    local json_data = "{\n"
--    for i, item in ipairs(data) do
--        json_data = json_data .. '\t"' .. tostring(i) .. '": {'
--        for key, value in pairs(item) do
--            json_data = json_data .. '"' .. key .. '":"' .. tostring(value) .. '",'
--        end
--        json_data = string.sub(json_data, 1, -2) -- remove the trailing comma
--        json_data = json_data .. '},\n'
--    end
--    json_data = string.sub(json_data, 1, -3) -- remove the trailing comma and newline
--    json_data = json_data .. "\n}"
--
--
--  -- construct the HTTP POST request header
--  local post_request = "POST /test HTTP/1.1\r\n" ..
--                       "Host: 207.216.17.76:12521\r\n" ..
--                       "Content-Type: application/json\r\n" ..
--                       "Content-Length: " .. tostring(string.len(json_data)) .. "\r\n" ..
--                       "Connection: close\r\n\r\n" ..
--                       json_data
--
--  -- send the HTTP POST request to the server
--  sk:send(post_request)
--end
