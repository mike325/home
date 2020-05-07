import gc
import esp
import webrepl
import network
esp.osdebug(0)
# uos.dupterm(None, 1) # disable REPL on UART(0)
webrepl.stop()
network.WLAN(network.AP_IF).active(False)
gc.collect()
