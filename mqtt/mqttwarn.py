# -*- coding: utf-8 -*-

# mqttwarn example function extensions
# Some example functions to work with mqttwarn

import os
import sys
# import time
# import copy

# try:
#     import json
# except ImportError:
#     import simplejson as json


sys.path.insert(0, os.path.abspath(os.path.dirname(__file__)))
import access

__header__ = """
                        -`
        ...            .o+`
     .+++s+   .h`.    `ooo/
    `+++%++  .h+++   `+oooo:
    +++o+++ .hhs++. `+oooooo:
    +s%%so%.hohhoo'  'oooooo+:
    `+ooohs+h+sh++`/:  ++oooo+:
     hh+o+hoso+h+`/++++.+++++++:
      `+h+++h.+ `/++++++++++++++:
               `/+++ooooooooooooo/`
              ./ooosssso++osssssso+`
             .oossssso-````/osssss::`
            -osssssso.      :ssss``to.
           :osssssss/  Mike  osssl   +
          /ossssssss/   8a   +sssslb
        `/ossssso+/:-        -:/+ossss'.-
       `+sso+:-`                 `.-/+oso:
      `++:.                           `-/+/
      .`                                 `/
"""

_version = '0.1.0'
_author = 'Mike'
_mail = 'mickiller.25@gmail.com'


def GotifyNotify(topic=None, data=None, srv=None):
    """
    TODO:
    """

    import requests

    # optional debug logger
    if srv is not None:
        srv.logging.debug('topic={topic}, data={data}, srv={srv}'.format(**locals()))

    gotify_url = '{url}/message?token={token}'.format(
        url=access.GOTIFY['url'],
        token=access.GOTIFY['token'],
    )

    message = 'Test From MQTT' if 'message' not in data else data['message']

    if topic is not None:
        title = topic
    else:
        title = 'MQTT' if 'title' not in data else data['title']

    args = {
        'title': title,
        'message': message,
        'extras': {
            "client::display": {
                "contentType": "text/markdown"
            }
        }
    }

    # optional debug logger
    if srv is not None:
        srv.logging.debug(args)

    resp = requests.post(
        gotify_url,
        json=args,
    )
