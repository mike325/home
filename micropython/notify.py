import machine
import gc
gc.enable()

def send(title, message, markdown=False):
    import access
    import urequests

    gotify = access.GOTIFY
    url = gotify['url']
    token = 'token=' + gotify['token']
    url_path = 'message?'
    data = {'title': title, 'message': message, 'priority': 10}
    print('Sending notification {0}'.format(title))

    for i in range(0, 5):
        try:
            rsp = urequests.post(url + url_path + token, json=data)
            if rsp.status_code == 200:
                print('Succed!! Notification sent')
                gc.collect()
                return True
            else:
                print('Failed to send notification, status code is {0}, Try {1}'.format(str(rsp.status_code), i))
        except OSError as e:
            print('Failed to reach internet, try {index}'.format(index=i))
            import sys
            sys.print_exception(e)
        machine.lightsleep(100)

    gc.collect()
    return False
