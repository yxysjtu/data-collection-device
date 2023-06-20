import os

num = b'\x10\x0e\x00\x00'

file_exist = os.path.exists("device.cfg")
if not file_exist:
    with open("device.cfg", "wb") as file:
        file.write(num)
'''
with open("device.cfg", "rb") as file:
    num = file.read()
    #print(num)

with open("device.cfg", "wb") as file:
    file.write(num)
'''
print("sampling time seperation (in hours):")
hours = float(input())
errcnt = 0
while(errcnt >= 0 and errcnt < 10):
    try:
        with open("device.cfg", "wb") as file:
            file.write(int(hours*3600).to_bytes(4, 'little'))
        errcnt = -1
    except:
        errcnt = errcnt + 1
if(errcnt >= 10):
    print("write fail")