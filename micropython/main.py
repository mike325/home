import machine
import gc
import wifi
gc.enable()

def main():
    print('Setting up wifi')
    wifi.connect()
    gc.collect()
    return 0

if __name__ == "__main__":
    main()
    # machine.deepsleep()
