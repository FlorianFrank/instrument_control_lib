reset()

-- Configure sweep values
start_voltage = 0
stop_voltage = 550
voltage_delta = stop_voltage - start_voltage
rate = 18

-- Configure smu
smua.source.func = smua.OUTPUT_DCVOLTS
smua.source.output = smua.OUTPUT_ON
smua.source.limitv = (stop_voltage / 1000) + 0.1

-- Set current
smua.source.limiti = 0.0011
smua.source.leveli = 0.001

for v = start_voltage, stop_voltage do
    print("Current sweep voltage: ", v, " at a rate of ", rate, " mv/s")
    smua.source.levelv = v / 1000
    delay(1 / rate)
end

smua.source.output = smua.OUTPUT_OFF
