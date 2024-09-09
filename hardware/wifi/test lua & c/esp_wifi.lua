
wifi.setmode(wifi.STATION)
apcfg = {}
--apcfg.ssid = "SPSETUP-1915"
--apcfg.pwd = "celery8357board"
--apcfg.ssid = "l2b26"
--apcfg.pwd = "pssm7007"
apcfg.ssid = "AL0723"
apcfg.pwd = "206901201"
--SSID = "SHAW-EDD220"
--SSID_PASSWORD = "25115B030212"

wifi.sta.config(apcfg)
wifi.sta.connect()

tmr.delay(1000000)   -- wait 1,000,000 us = 1 second
print(wifi.sta.status())
print(wifi.sta.getip())

-------------------------------------------
--**
-------------------------------------------
function get_test()
  local sk = net.createConnection(net.TCP, 0)
    sk:on("receive", function(sck, c) print(c) end )

    sk:connect(80,"52.207.219.136")
    sk:send("GET /testwifi/index.html HTTP/1.1\r\nHost: wifitest.adafruit.com\r\nConnection: keep-alive\r\nAccept: */*\r\n\r\n")

end
-------------------------------------------
function get_webpage()
  local sk = net.createConnection(net.TCP, 0)
    sk:on("receive", function(sck, c) print(c) end )

    sk:connect(12521,"207.216.17.76")
    --sk:connect(9000,"10.43.141.56")
    
    sk:send("GET /user/sessionlist HTTP/1.1\r\nConnection: keep-alive\r\nAccept: */*\r\n\r\n")
end


--------------------------------------
--**
-------------------------------------------
function post_test1()
    -- create a TCP connection object
    local sk = net.createConnection(net.TCP, 0)
    
    -- set up a callback function to handle incoming data
    sk:on("receive", function(sck, c) 
      print("Received data: " .. c) 
    end)
    
    -- initiate the TCP connection to the remote server
    sk:connect(12521, "207.216.17.76")
      --sk:connect(9000,"10.43.141.56")  
    --sk:connect(9000,"10.0.0.36")
        
    -- construct the HTTP POST request header
    local post_request = "POST /console/register HTTP/1.1\r\n" ..
                         "Host: 207.216.17.76:12521\r\n" ..
                         "Content-Type: application/json\r\n" ..
                         "Content-Length: 0\r\n" ..
                         "Connection: close\r\n\r\n"
    
    -- send the HTTP POST request to the server
    sk:send(post_request)

end
--------------------------------------------
--**
-------------------------------------------
function post_test2()
    local sk = net.createConnection(net.TCP, 0)

    -- set up a callback function to handle incoming data
    sk:on("receive", function(sck, c) print("Received data: " .. c) end)

    -- initiate the TCP connection to the remote server
    sk:connect(12521, "207.216.17.76")
    --sk:connect(9000,"10.0.0.36")
    
    -- construct the HTTP POST request body in JSON format
    local post_data = '{"username":"john"}'

    -- construct the HTTP POST request header
    local post_request = "POST /test HTTP/1.1\r\n" ..
                        "Host: 207.216.17.76:12521\r\n" ..
                        "Content-Type: application/json\r\n" ..
                        "Content-Length: " .. string.len(post_data) .. "\r\n" ..
                        "Connection: close\r\n\r\n" ..
                        post_data

    -- send the HTTP POST request to the server
    sk:send(post_request)
end
-----------------------------------------------
--**
-----------------------------------------------

function post_multiple_data()
  -- create a TCP connection object
  local sk = net.createConnection(net.TCP, 0)
  
  -- set up a callback function to handle incoming data
  sk:on("receive", function(sck, c) 
    print("Received data: " .. c) 
  end)
  
  -- initiate the TCP connection to the remote server
  sk:connect(12521, "207.216.17.76")
  --sk:connect(9000,"10.0.0.36")

  -- define the data to be posted
  local data = {
        { username = "john", age = 30 },
        { username = "jane", age = 25 },
        { username = "bob", age = 40 }
    }

    local json_data = "{\n"
    for i, item in ipairs(data) do
        json_data = json_data .. '\t"' .. tostring(i) .. '": {'
        for key, value in pairs(item) do
            json_data = json_data .. '"' .. key .. '":"' .. tostring(value) .. '",'
        end
        json_data = string.sub(json_data, 1, -2) -- remove the trailing comma
        json_data = json_data .. '},\n'
    end
    json_data = string.sub(json_data, 1, -3) -- remove the trailing comma and newline
    json_data = json_data .. "\n}"


  -- construct the HTTP POST request header
  local post_request = "POST /test HTTP/1.1\r\n" ..
                       "Host: 207.216.17.76:12521\r\n" ..
                       "Content-Type: application/json\r\n" ..
                       "Content-Length: " .. tostring(string.len(json_data)) .. "\r\n" ..
                       "Connection: close\r\n\r\n" ..
                       json_data

  -- send the HTTP POST request to the server
  sk:send(post_request)
end
