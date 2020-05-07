import machine
import gc
gc.enable()

def connect():
    import access
    import network

    station = network.WLAN(network.STA_IF)
    station.active(True)

    if station.isconnected():
        print('Already connected {0}'.format(station.ifconfig()))
        gc.collect()
        return True

    for _ in range(0, 10):
        found = False
        wifis = station.scan()
        for wifi in wifis:
            ssid = wifi[0].decode()
            if ssid in access.WIFI:
                found = True
                print('Trying to connect to {}'.format(ssid))
                password = access.WIFI[ssid]
                station.connect(ssid, password)
                for i in range(0, 10):
                    machine.lightsleep(1000)
                    if station.isconnected():
                        print('Connected to {}'.format(ssid))
                        print('Ifconfig: {}'.format(station.ifconfig()))
                        gc.collect()
                        return True
                    print('Ifconfig: {}'.format(station.ifconfig()))
                    print('Retrying to connect {}'.format(i))
                else:
                    print('Failed to connected to {}'.format(ssid))
                    print('Status: {}'.format(station.status()))
                    print('is connected: {}'.format(station.isconnected()))
        if station.isconnected():
            print('Connected {0}'.format(station.ifconfig()))
            machine.lightsleep(100)
            gc.collect()
            return True
        elif not found:
            print('No known WiFis available')
        machine.lightsleep(5000)

    gc.collect()

    return False
