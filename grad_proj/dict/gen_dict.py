# -*- coding: utf-8 -*-

import re
import os
import time
import urllib
import urllib2
from bs4 import BeautifulSoup
import urlparse

#decode: str ==> unicode
#encode: unicode ==> str

def if_exit_chinese(test_unicode):
    for ch in test_unicode:
        if ch >= u'\u4e00' and ch <= u'\u9fa5':
            return True
    return False

def rm_dup(in_name, out_name):
    fp = file(in_name, 'rb')
    fp_out = file(out_name, 'wb')
    lines_set = set()
    for line in fp:
        if line not in lines_set:
            fp_out.write(line)
            lines_set.add(line)
    fp.close()
    fp_out.close()
    os.remove(in_name)
    return 

def cardiovascular_words(cardiovascular_name):
    fp = file('dict_texts/cardiovascular_words.txt', 'rb')
    fp_out = file(cardiovascular_name+'_tmp', 'wb')
    for line in fp:
        segs = line.decode('utf-8').split()
        for i in range(len(segs)):
            if if_exit_chinese(segs[i]):
                if segs[i][0] == u',' or segs[i][0] == u'，' or len(segs[i]) == 1 or segs[i][0] == u'波' or \
                        segs[i] == u'斜率' or segs[i] == u'原理' or segs[i] == u'手术':
                    fp_out.write(segs[i-1].encode('utf-8'))
                for j in range(i, len(segs)):
                    fp_out.write(segs[j].encode('utf-8'))
                fp_out.write('\n')
                break
    fp.close()
    fp_out.close()

    # need to remove the duplication
    rm_dup(cardiovascular_name+'_tmp', cardiovascular_name)

    return 

def xiangya_medical_words(xiangya_name):
    fp = file('dict_texts/xiangya-medical.txt', 'rb')
    fp_out = file(xiangya_name, 'wb')
    pattern1 = re.compile(r'[a-zA-Z0-9]+\]')
    pattern2 = re.compile(r'<.+>')
    pattern3 = re.compile(r'^[a-zA-Z0-9]+\(.+\)$')
    counter = {}
    for line in fp:
        line = line.replace('\\n', ' ')
        line = re.sub(pattern2, '', line, count=1, flags=0) #remove the "<.+>" named the type of the medicial
        segs = line.decode('utf-8').split()
        lc = 0
        for i in range(len(segs)):
            if if_exit_chinese(segs[i]):
                segs[i] = re.sub(pattern1, '', segs[i], count=1, flags=0)
                if re.search(pattern3, segs[i], flags=0):
                    #print segs[i].encode('utf-8')
                    pass
                fp_out.write(segs[i].encode('utf-8')+'\n')
                lc += 1
        if lc in counter:
            counter[lc] += 1
        else:
            counter[lc] = 1
    print counter
    fp.close()
    fp_out.close()

    return 

def fetch_words(url):
    if url is None:
        return None
    #add the Request Headers
    headers = {}
    headers["Accept"] = "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8"
    headers["Host"] = "dict.bioon.com"
    headers["User-Agent"] = "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/56.0.2924.87 Safari/537.36"
    data = None
    request = urllib2.Request(url, data, headers)
    response = urllib2.urlopen(request)
    if response.getcode() != 200:
        return None
    html_cont = response.read()
    
    #analyse the web_page to fetch the words needed
    fetched_words = set()
    bsObj = BeautifulSoup(html_cont, 'html.parser', from_encoding='utf-8')
    words_list = bsObj.findAll("div", {"class":"worditem"})
    for naive_word in words_list:
        for word in naive_word.findAll("a"):
            fetched_words.add(word.get_text())
    return fetched_words

def claw_cardi(page_range, claw_cardi_name):
    url_base = 'http://dict.bioon.com/class.asp?classid=111&page='
    gotten_words = set()
    old_len = 0
    new_len = 0
    try:
        for i in range(page_range):
            url = url_base + str(i)
            fetched_words = fetch_words(url)
            for word in fetched_words:
                gotten_words.add(word)
            new_len = len(gotten_words)
            print ">>>>>>>>  page%d: %d-%d=%d" % (i, new_len, old_len, new_len-old_len)
            old_len = new_len #remember to update the old_len
    finally:
        #collect the words to the disk
        fp = file(claw_cardi_name[:-4]+'_'+str(page_range)+'.txt', 'wb')
        for word in gotten_words:
            fp.write(word.encode('utf-8')+'\n')
        fp.close()
        return -1
    return 0

def claw_symptom(path_name):
    syms = set()
    fp = file(path_name, 'wb')
    pattern = re.compile(ur'［.+］')
    try:
        for i in range(1):
            url = 'http://www.a-hospital.com/w/%E4%B8%AD%E5%9B%BD%E5%9B%BD%E5%AE%B6%E5%9F%BA%E6%9C%AC%E8%8D%AF%E7%89%A9%E5%88%97%E8%A1%A8'
            print url
            fp.write(url+'\n')
            headers = {}
            headers["Accept"] = "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8"
            headers["Host"] = "www.a-hospital.com"
            headers["User-Agent"] = "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/56.0.2924.87 Safari/537.36"
            data = None
            request = urllib2.Request(url, data, headers)
            response = urllib2.urlopen(request)
            if response.getcode() != 200:
                return None
            html_cont = response.read()
            bsObj = BeautifulSoup(html_cont, 'html.parser', from_encoding='utf-8')
            words_list = bsObj.findAll("li")
            for naive_word in words_list:
                for word in naive_word.findAll("a"):
                    w_cont = word.get_text()
                    w_cont = w_cont.strip()
                    print w_cont
                    syms.add(w_cont)
                    fp.write(w_cont.encode("UTF-8")+'\n')
    finally:
        fp.close()
        return -1
    return 0

def clean_crawl_cardi(cardi_name):
    fp = file(cardi_name[:-4]+'_save.txt', 'rb')
    fp_out = file(cardi_name+'_tmp', 'wb')
    pattern4 = re.compile(r'^.+\s-\s')
    pattern5 = re.compile(ur'\s\(热度.+\)\s')
    for line in fp:
        line = line.decode('utf-8')
        line = re.sub(pattern4, '', line, count=1, flags=0)
        line = re.sub(pattern5, '', line, count=1, flags=0)
        fp_out.write(line.encode('utf-8')+'\n')
    fp.close()
    fp_out.close()

    rm_dup(cardi_name+'_tmp', cardi_name)

    return 

def mov_cop(filename):
    fp = file(filename, 'rb')
    fp_out = file(filename+'_ret', 'wb')
    tw = set()
    for w in fp:
        #w = (w.decode('utf-8')).strip()
        #for i in w.split(u'、'):
        #    tw.add(i)
        w = (w.decode('utf-8')).strip()
        tw.add(w)
    twl = list(tw)
    twl.sort()
    for w in twl:
        fp_out.write(w.encode('utf-8')+'\n')
    fp.close()
    fp_out.close()

def sep_med(file_path):
    fp = file(file_path, 'rb')
    fp_out = file(file_path+'_tmp', 'wb')
    for line in fp:
        line = (line.strip()).decode('UTF-8')
        if len(line) == 0:
            continue
        else:
            l_list = line.split()
            for word in l_list:
                assert word[0] == u'·'
                fp_out.write((word[1:]).encode('UTF-8')+'\n')
    fp.close()
    fp_out.close()

if __name__ == "__main__":
    start_time = time.clock()
    ###xiangya_name = 'xiangya.txt'
    ###cardiovascular_name = 'cardiovascular.txt'
    ###xiangya_medical_words(xiangya_name)
    ###cardiovascular_words(cardiovascular_name)
    ###page_range = 1000
    ###claw_cardi_name = 'crawl_cardi.txt'
    ###claw_cardi(page_range, claw_cardi_name)
    ###clean_crawl_cardi(claw_cardi_name)
    fn = u'./xxxx.txt'
    #claw_symptom(fn)
    mov_cop(fn+'_tmp')
    end_time = time.clock()
    print 'time to generate the dictionary: %f' % (end_time-start_time)
