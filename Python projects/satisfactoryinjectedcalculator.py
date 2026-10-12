devicecount = int(input("Device count: "))
deviceinput = float(input("Device input: "))
injection = float(input("Injection volume: "))

currentflow = 0.0
for i in range(devicecount):
    if currentflow < deviceinput:
        print("Injection")
        currentflow += injection
    print(i+1)
    currentflow -= deviceinput
    