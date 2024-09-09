
  wifi.setmode(wifi.STATION)
  apcfg = {}
  --apcfg.ssid = "ELE-AL0723"
  --apcfg.pwd = "206901201"
  apcfg.ssid = "SPSETUP-1915"
  apcfg.pwd = "celery8357board"

  wifi.sta.config(apcfg)
  wifi.sta.connect()

  tmr.delay(1000000)   -- wait 1,000,000 us = 1 second
  print(wifi.sta.status())
  print(wifi.sta.getip())
  
  sk = net.createConnection(net.TCP, 0)
  sk:on("receive", function(sck, c) print(c) end )
  sk:connect(9000,"10.0.0.36")
  sk:send("GET /user/sessionlist HTTP/1.1\r\nConnection: keep-alive\r\nAccept: */*\r\n\r\n")

