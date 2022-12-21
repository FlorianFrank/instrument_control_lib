import requests
from time import sleep

url = 'http://132.231.14.168/HttpCommand'

payloads = []

payloads.append({"command": "reset",
                "value": True})


for payload in payloads:
    x = requests.post(url, json = payload)
    print(x.text)
    sleep(0.001)
