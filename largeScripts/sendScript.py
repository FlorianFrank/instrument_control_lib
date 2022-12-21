import requests
from time import sleep

url = 'http://132.231.14.168/HttpCommand'

script_name = "bufferedScript"
script = """b.clear()
for i = 1, 10 do
smua.measure.i(b)
end"""

script_content = f"""loadscript {script_name}
{script}
endscript
"""

exit_payload = {"command": "keyInput",
                "value": "K"}

batch_size = 32

lines = script_content.split("\n")
batches = len(lines) // batch_size
remaining = len(lines) % batch_size

payloads = []

for i in range(batches):
    offset = i * batch_size

    content = ""
    for i in range(offset, offset + batch_size):
        content += lines[i] + "\n"
    payloads.append({"command": "shellInput", "value": content})

if remaining > 0:
    offset = batches * batch_size

    content = ""
    for i in range(offset, len(lines)):
        content += lines[i] + "\n"
    payloads.append({"command": "shellInput", "value": content})

payloads.append({"command": "shellInput",
                "value": f"{script_name}.save()"})

#payloads.append({"command": "shellInput",
#                "value": "bufferedScript()"})

x = requests.post(url, json=exit_payload)
print(x.text)
sleep(1)


for payload in payloads:
    x = requests.post(url, json = payload)
    print(payload, '->', x.text)
    sleep(0.01)

sleep(1)
x = requests.post(url, json=exit_payload)
print(x.text)
