#use ctrl+t for real-time simulation
import time, math

speakerlatency = 12

speakerschedule = [30,30,30,30,30,30]

ticktime = 0.03
amplification = 50 #Info on this in the document. https://docs.google.com/document/d/1vReHKVhpaulzn1IsMsSJpQRdfSiK0yU5TeAdcrz__NA/edit?tab=t.0
micpos = 0
speakerpos = 83
ambientcatch = 0.534 #This metric defines the ambient acoustic energy retention of a room one tick after sound is created. More info: https://docs.google.com/document/d/1vReHKVhpaulzn1IsMsSJpQRdfSiK0yU5TeAdcrz__NA/edit?tab=t.0
ambientdecay = 0.394 #Calculated by the RT60 of an average conference room (445ms) and the tick duration of 30 ms. This metric describes the retention percentage of the ambient sound every tick
#NOTE: RT60 is the time it takes for ambient sound to decay by 60 dB (aka a factor of 1 000 000 acoustic energy wise)
# the formula to calculate ambientdecay for a given ticktime and RT60:  (1 000 000)^(-1 * ticktime/RT60)
miccap = 50



sound = 0
ambient = 0

def registersound(sound):
    global speakerschedule, tick, speakerlatency
    speakerschedule[tick + speakerlatency] += sound * amplification * 1/math.pow(micpos-speakerpos, 2)

for tick in range(0,999999):
    sound = 0
    
    while len(speakerschedule) < tick + speakerlatency + 10:
        speakerschedule.append(0)
    
    sound += speakerschedule[tick] * amplification * 1/math.pow(micpos-speakerpos, 2)
    
    ambient *= ambientdecay
    sound += ambient * ambientcatch
    
    ambient += speakerschedule[tick]
    
    if sound > miccap:
        sound = 0
    
    registersound(sound)
    for i in range(math.ceil(sound)):
        print("#", end="")
    print("  " + str(tick))
    
    time.sleep(ticktime)