# -*- coding: utf-8 -*-

import sys
import re
import os

#decode: str ==> unicode
#encode: unicode ==> str
encode_type = sys.getfilesystemencoding() #UTF-8 in my machine

eng_punc = u'!"#$%&\'()*+,-./:;<=>?@[\\]^_`{|}~±'
chi_punc = u'，。！、（）【】？：＝～￥＃＠……＆×——＋＝－｀／、｜《》｛｝'

def judge_ch(uni_ch):
    # -1==><sp>  0==>chinese  1==>english  2==>number  3==>punctuation  4==>other
    if uni_ch == u' ':
        return -1
    elif uni_ch >= u'\u4e00' and uni_ch <= u'\u9fa5':
        return 0
    elif (uni_ch >= u'A' and uni_ch <= u'Z') or (uni_ch >= u'a' and uni_ch <= u'z'):
        return 1
    elif uni_ch >= u'0' and uni_ch <= u'9':
        return 2
    elif (uni_ch in eng_punc) or (uni_ch in chi_punc):
        return 3
    else:
        return 4

def txt_pretreat(path):
    fp = file(os.path.join('./after_manual', path), 'rb')
    fp_out = file(os.path.join('./after_pretreat', path), 'wb')
    for line in fp:
        line = line.decode('utf-8').strip()
        new_line = ""
        pre = 0 #init to be a chinese
        for p in range(len(line)):
            cur = judge_ch(line[p])
            if cur != -1:
                new_line += line[p]
                pre = cur
                continue
            elif p < len(line)-1:
                pro = judge_ch(line[p+1])
                if pre == 0 or pro == 0 or pro == -1 or pre == 3 or pro == 3 or \
                    (pre == 2 and pro == 1) or (pre == 2 and pro == 2): #skip the blank space
                    continue
                else:
                    new_line += ' '
        fp_out.write(new_line.encode('utf-8')+'\n')
    fp.close()
    fp_out.close()

    return

def pretreat_drive(texts_list):
    for text in texts_list:
        txt_pretreat(text)
    return 

def copy_to_cws(texts_list):
    sep_punc = [u'。', u'?', u'？', u'!', u'！'] #the separate punctuation
    tran_punc = {u'（':'(', u'）':')', u'０':'0', u'１':'1', u'２':'2', u'３':'3', u'４':'4', u'５':'5', u'６':'6', u'７':'7', u'８':'8', u'９':'9', ',':u'，', u'％':'%', ':':u'：', u'～':'~', u'－':'-'}
    for text in texts_list:
        fp = file(os.path.join('./after_pretreat', text), 'rb')
        fp_out = file(os.path.join('../tag/medical_texts', text), 'wb')
        for line in fp:
            line = line.decode('UTF-8')
            for i in range(len(line)): #separate the sentance
                char = line[i]
                if char in tran_punc.keys():
                    char = tran_punc[char]
                fp_out.write(char.encode('UTF-8'))
                if char in sep_punc and i+1 < len(line) and line[i+1] != '\n':
                    fp_out.write('\n')
        fp.close()
        fp_out.close()
    return 

if __name__ == "__main__":
    texts_list = os.listdir('./after_manual')
    pretreat_drive(texts_list)
    copy_to_cws(texts_list)
