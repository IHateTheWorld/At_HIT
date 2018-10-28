# -*- coding: utf-8 -*-

import sys
import os
import time
from selenium import webdriver
from bs4 import BeautifulSoup

encode_type = sys.getfilesystemencoding() #UTF-8 in my ubuntu 14.04

def crawl_drive(url_addr, file_name):
    driver = webdriver.Chrome("C:\Program Files (x86)\Google\Chrome\Application\chromedriver.exe")
    driver.set_page_load_timeout(30)
    driver.maximize_window()
    driver.get(url_addr)
    
    count = 0
    len_flag = 0
    while True:
        count += 1
        driver.execute_script("window.scrollBy(0,"+str(10000*count)+")")
        new_len = len(driver.page_source)
        print '>>> %d' % new_len
        if len_flag < new_len:
            len_flag = new_len
        else:
            break
        time.sleep(2)
    '''
    driver.execute_script("""
        (function () {
            var y = document.body.scrollTop;
            var step = 100;
            window.scroll(0, y);
            function f() {
                if (y < document.body.scrollHeight) {
                    y += step;
                    window.scroll(0, y);
                    setTimeout(f, 50);
                }
                else {
                    window.scroll(0, y);
                    document.title += "scroll-done";
                }
            }
            setTimeout(f, 1000);
        })();
        """
    )
    '''
    print 'Sleeping 1 hour...'
    time.sleep(3600)
    print 'Writing file...'
    fp = file(file_name, "wb")
    fp.write((driver.page_source).encode('UTF-8')) #driver.page_source is the unicode
    fp.close()
    driver.close()

def filter_html(in_name, out_name):
    fp = file(in_name, 'rb')
    fp_out = file(out_name, 'wb')
    pageSource = fp.read()
    bsObj = BeautifulSoup(pageSource, 'html.parser', from_encoding='UTF-8')
    collect = set()
    words_list = bsObj.findAll("div")
    for naive_word in words_list:
        for word in naive_word.findAll("div", class_="waterFall_content_title"):
            w_cont = word.get_text()
            w_cont = w_cont.strip()
            collect.add(w_cont)
            #fp_out.write(w_cont.encode("UTF-8")+'\n')
    clst = list(collect)
    clst.sort()
    for w in clst:
        fp_out.write(w.encode('utf-8')+'\n')
    fp.close()
    fp_out.close()

if __name__ == "__main__":
    start = time.clock()
    url_list = (("http://baike.baidu.com/wikitag/taglist?tagId=75954", u"科学百科药物分类"), \
                ("http://baike.baidu.com/wikitag/taglist?tagId=75953", u"科学百科疾病症状分类"), \
                ("http://baike.baidu.com/wikitag/taglist?tagId=75955", u"科学百科诊疗方法分类"))
    for url_addr, file_name in url_list:
        crawl_drive(url_addr, file_name)
        pass
    in_list = (u"科学百科药物分类", \
                u"科学百科疾病症状分类", \
                u"科学百科诊疗方法分类")
    for in_name in in_list:
        filter_html(in_name, in_name+'_filter')
        pass
    end = time.clock()
    print "Time to run: %f" % (end-start)
