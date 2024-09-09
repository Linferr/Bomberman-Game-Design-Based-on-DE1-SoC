import time
import requests

ORIGIN = "http://207.216.17.76:12521"

s = requests.Session()

r = s.post(ORIGIN + '/console/register')
print(r.text)

print(s.cookies)

while True:
    r = s.get(ORIGIN + '/console/render')
    print(r.text)
    time.sleep(0.2)