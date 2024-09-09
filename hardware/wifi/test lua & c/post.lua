node.output(nil)
wifi.setmode(wifi.STATION)
apcfg = {}
apcfg.ssid = "ELE-AL0723"
apcfg.pwd = "206901201"


wifi.sta.config(apcfg)
wifi.sta.connect()

tmr.delay(1000000)   -- wait 1,000,000 us = 1 second
print(wifi.sta.status())
print(wifi.sta.getip())

-- create a TCP connection object
sk = net.createConnection(net.TCP, 0)
    
-- set up a callback function to handle incoming data
sk:on("receive", function(sck, c) 
  print("Received data: " .. c) 
end)

-- initiate the TCP connection to the remote server
--sk:connect(12521, "207.216.17.76")
  --sk:connect(9000,"127.0.0.1")  
sk:connect(9000,"10.0.0.36")
    
-- construct the HTTP POST request header
post_request = "POST /console/register HTTP/1.1\r\n" ..
                     "Host: 10.0.0.36:9000\r\n" ..
                     "Content-Type: application/json\r\n" ..
                     "Content-Length: 0\r\n" ..
                     "Connection: close\r\n\r\n"

-- send the HTTP POST request to the server
sk:send(post_request)
