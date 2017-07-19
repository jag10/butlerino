# !/usr/bin/env python3
# -*- coding: utf-8 -*-

import serial
import time
import subprocess
import functools
import json

import requests

import newspaper

import schedule

msgs = []

LOCATION = "Albacete"
NEWS_SOURCE = "http://www.bbc.com/news"

def comm():
    if len(msgs) > 0:
        msg = msgs.pop(0)
        print("sending: ", msg)
        sendString(msg)

def sendString(string):
    line = ''
    while line != '<Ready>':
        line = s.readline()
        line = str(line.strip().decode('utf-8'))
        print(line)
        time.sleep(1)

    string = string.strip()
    string += '\n'
    s.write(string.encode('ascii'))
    time.sleep(5)

# decorator
def logger(func):
    @functools.wraps(func)
    def wrapper(*args, **kwargs):
        # print('LOG: Running job "%s"' % func.__name__)
        result = func(*args, **kwargs)
        # print('LOG: Job "%s" completed' % func.__name__)
        msgs.append(result)
        return result
    return wrapper

# SCROLLING LINE FUNCTIONS

@logger
def news(source):
    paper = newspaper.build(source, memoize_articles=False)
    first_article = paper.articles[0]
    first_article.download()
    first_article.parse()
    json_str = staticLine(first_article.title)
    # msgs.append(json_str)
    return json_str

# TODO: twitter

# STATIC LINE FUNCTIONS

# https://developer.yahoo.com/weather/
def get_weather(location):
    q = 'select item.condition from weather.forecast where woeid in (select woeid from geo.places(1) where text="'+location+'")'
    baseurl = "https://query.yahooapis.com/v1/public/yql"

    payload = {'q': q, 'format': 'json'}
    r = requests.get(baseurl, params=payload)
    r = r.json()
    temp_f = int(r['query']['results']['channel']['item']['condition']['temp'])
    temp_c = int((temp_f - 32) * 5/9)
    condition = str(r['query']['results']['channel']['item']['condition']['text']).lower()
    if "cloud" in condition:
        condition = 25 # GLYPHDUINO_CLOUD
    elif "clear" in condition or "sun" in condition:
        condition = 26 # GLYPHDUINO_SUN
    elif "rain" in condition or "snow" in condition or "shower" in condition or "storm" in condition:
        condition = 27 # GLYPDUINO_RAIN
    else:
        condition = "?"
    return (temp_f, temp_c, condition)

def git():
    cmd = "git log --since='7d' --oneline | wc -l"
    ps = subprocess.Popen(cmd,shell=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
    output = ps.communicate()[0]
    return str(int(output)).zfill(2)

def staticLine(scrollingLine):
    temp_f, temp_c, condition = get_weather(LOCATION)
    commits = git()
    emails = "0"
    json_mock = {
    	# "scrollingLine": "Lincoln: The war has begun",
    	"scrollingLine": scrollingLine,
    	"staticLine": " " + str(temp_c) + "   " + emails + "       " + str(commits),
    	"glyphs": {
    		"0": condition,
    		"3": 24, # GLYPHDUINO_CELSIUS
    		"13": 23, # GLYPHDUINO_GITHUB
            "5": 11 # GLYPHDUINO_BELL
    	}
    }
    return json.dumps(json_mock)

if __name__ == '__main__':
    s = serial.Serial("/dev/tty.usbmodem1421", 9600)
    time.sleep(2)    # wait for the Serial to initialize

    news(NEWS_SOURCE)
    comm()
    # schedule.every(60).seconds.do(news, NEWS_SOURCE)
    # schedule.every().second.do(comm)
    #
    # while True:
    #     schedule.run_pending()
    #     time.sleep(1)

    # UNCOMMENT FOR DEBUG INFO
    # while True:
    #     try:
    #         line = s.readline()
    #         line = str(line.strip().decode('utf-8'))
    #         if "error" in line or "debug" in line:
    #             print(line)
    #     except Exception:
    #         print("error while reading from serial")
