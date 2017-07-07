# !/usr/bin/env python3
# -*- coding: utf-8 -*-

import serial
import time
import subprocess
import functools

import requests

import newspaper

import schedule

msgs = []

# decorator
def logger(func):
    @functools.wraps(func)
    def wrapper(*args, **kwargs):
        # print('LOG: Running job "%s"' % func.__name__)
        result = func(*args, **kwargs)
        # print('LOG: Job "%s" completed' % func.__name__)
        msgs.append(result)
        # print('Result: ', result)
        return result
    return wrapper

def comm():
    if len(msgs) > 0:
        print("popeando bro")
        print(msgs.pop(0))
    # TODO: send serial msg

@logger
def get_weather(location):
    q = 'select item.condition from weather.forecast where woeid in (select woeid from geo.places(1) where text="'+location+'")'
    baseurl = "https://query.yahooapis.com/v1/public/yql"

    payload = {'q': q, 'format': 'json'}
    r = requests.get(baseurl, params=payload)
    r = r.json()
    temp_f = float(r['query']['results']['channel']['item']['condition']['temp'])
    temp_c = (temp_f - 32) * 5/9
    condition = str(r['query']['results']['channel']['item']['condition']['text'])

    return (temp_f, temp_c, condition)

@logger
def news(source):
    paper = newspaper.build(source, memoize_articles=False)
    first_article = paper.articles[0]
    first_article.download()
    first_article.parse()
    return first_article.title

@logger
def git():
    cmd = "git log --since='week' --oneline | wc -l"
    ps = subprocess.Popen(cmd,shell=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
    output = ps.communicate()[0]
    return str(int(output))

print(get_weather("albacete"))
# print(news('http://nytimes.com'))
print(git())

schedule.every().second.do(comm)
schedule.every(5).seconds.do(git)
schedule.every(5).seconds.do(get_weather, "albacete")

while True:
    schedule.run_pending()
    time.sleep(1)

# s = serial.Serial("/dev/tty.usbmodem1421", 9600) #port is 11 (for COM12, and baud rate is 9600
# time.sleep(2)    #wait for the Serial to initialize
# s.write(b'Ready. ' + str(int(output)))
# while True:
#     string = raw_input('Enter text: ')
#     string = string.strip()
#     if string == 'exit' :
#         break
#     if string == 'git':
#         s.write(str(int(output)) + " ")
#     s.write(string)
