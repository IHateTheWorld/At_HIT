# -*- coding: utf-8 -*-
import os
import sys
import time
import pynlpir
import re

from tag import *

def dict_ne(line):
    tag_str = u''
    low = 0
    high = ner_mm_rule.dict_counter - 1
    index = 0
    is_find = False
    loc = -1
    i = 0
    ne_list = []
    while i < len(line):
        target = line[i]
        bin_ret = ner_mm_rule.binarysearch(low, high, index, target, loc)
        if bin_ret >= 0:
            if is_find == False:
                i -= (index - 1)
                tag_str += line[i-1]
            else:
                add_pos = ner_mm_rule.add_ne(ner_mm_rule.dict_list[loc][0], ne_list)
                tag_str += '[['+ner_mm_rule.dict_list[loc][1]+str(add_pos)+']]'
                i = i - index + len(ner_mm_rule.dict_list[loc][0])
                is_find = False
            low = 0
            high = ner_mm_rule.dict_counter - 1
            index = 0
            loc = -1
            continue
        else:
            bin_ret = -bin_ret - 1
            new_low = bin_ret
            new_high = bin_ret + 1
            while new_low >= low and index < len(ner_mm_rule.dict_list[new_low][0]) and ner_mm_rule.dict_list[new_low][0][index] == target:
                if index == (len(ner_mm_rule.dict_list[new_low][0]) - 1):
                    loc = new_low
                    is_find = True
                new_low -= 1
            new_low += 1
            while new_high <= high and index < len(ner_mm_rule.dict_list[new_high][0]) and ner_mm_rule.dict_list[new_high][0][index] == target:
                if index == (len(ner_mm_rule.dict_list[new_high][0]) - 1):
                    loc = new_high
                    is_find = True
                new_high += 1
            new_high -= 1
            low = new_low
            high = new_high
            if i == len(line) - 1: #don't forget this exit point
                if is_find == False:
                    i -= (index - 1)
                    tag_str += line[i-1]
                else:
                    add_pos = ner_mm_rule.add_ne(ner_mm_rule.dict_list[loc][0], ne_list)
                    tag_str += '[['+ner_mm_rule.dict_list[loc][1]+str(add_pos)+']]'
                    i = i - index + len(ner_mm_rule.dict_list[loc][0])
                    is_find = False
                low = 0
                high = ner_mm_rule.dict_counter - 1
                index = 0
                loc = -1
                continue
        i += 1
        index += 1
    return (tag_str, ne_list)

def ne_rule(string, ne_list):
    pat_list = ner_mm_rule.ne_pattern(u'./tag/extend_pattern/命名实体规则.pat_ex')
    for pat_i in range(len(pat_list)):
        pat_str = pat_list[pat_i][0]
        ne_type = pat_list[pat_i][1]
        sub_str = pat_list[pat_i][2]
        pattern = re.compile(pat_str)
        ret_list = pattern.findall(string)
        if len(ret_list) > 0: #this way can also exist some problem
            next_ele = ret_list[len(ret_list)-1]
            max_index = len(ret_list)-2
            for i in range(max_index, -1, -1):
                if ret_list[i] == next_ele:
                    del ret_list[i]
                else:
                    next_ele = ret_list[i]
            ret_list = ret_list[::-1] 
            ne_count = len(ne_list)
        if len(ret_list) > 0:
            if sub_str == u':':
                for ret in ret_list:
                    old_ne_pat = re.findall(ur'\[\[.{0,5}<[A-Z]{3}>\d+\]\]', ret)
                    old_ne_pat = list(set(old_ne_pat)) #remove the duplication
                    if len(old_ne_pat) > 0:
                        ne_new = ret
                        ne_no_list = set()
                        for pat in old_ne_pat:
                            ne_type_no = re.findall(ur'\d+', pat)
                            assert len(ne_type_no) == 1
                            ne_type_no = int(ne_type_no[0])
                            ne_no_list.add(ne_type_no)
                            ne_old = ne_list[ne_type_no]
                            ne_new = ne_new.replace(pat, ne_list[ne_type_no])
                        old_mid_str = '[['+ne_type+'xx'+']]' #this is a temp
                        string = string.replace(ret, old_mid_str)
                        ne_no_list = list(ne_no_list)
                        ne_no_list.sort(reverse=True) #Attention point
                        for no in ne_no_list:
                            if len(re.findall('\[\[.{0,5}<[A-Z]{3}>'+str(no)+'\]\]', string)) == 0:
                                del ne_list[no]
                                ne_count -= 1
                                for i in range(no, ne_count):
                                    big_no = re.findall('\[\[.{0,5}<[A-Z]{3}>'+str(i+1)+'\]\]', string)
                                    big_no = list(set(big_no))
                                    assert len(big_no) == 1
                                    big_no = big_no[0]
                                    small_no = big_no.replace(str(i+1), str(i))
                                    string = string.replace(big_no, small_no)
                        new_mid_str = '[['+ne_type+str(ne_count)+']]'
                        string = string.replace(old_mid_str, new_mid_str)
                        ne_list.append(ne_new)
                        ne_count += 1
                    else:
                        string = string.replace(ret, '[['+ne_type+str(ne_count)+']]')
                        ne_list.append(ret)
                        ne_count += 1
            elif sub_str == u'x':
                for ret in ret_list:
                    new_ne = ret
                    for mov_i in range(3, len(pat_list[pat_i])):
                        new_ne = re.sub(pat_list[pat_i][mov_i], '', new_ne, count=1, flags=0)
                    if new_ne not in ne_list:
                        string = string.replace(new_ne, '[['+ne_type+str(ne_count)+']]')
                        ne_list.append(new_ne)
                        ne_count += 1
            elif sub_str == u'=':
                assert ne_type == u'<xxx>'
                for ret in ret_list:
                    old_ne_pat = re.findall(ur'\[\[.{0,5}<[A-Z]{3}>\d+\]\]', ret)
                    old_ne_pat = list(set(old_ne_pat)) #remove the duplication
                    assert len(old_ne_pat) > 0
                    ne_new = ret
                    ne_no_list = set()
                    for pat in old_ne_pat:
                        ne_type_no = re.findall(ur'\d+', pat)
                        assert len(ne_type_no) == 1
                        ne_type_no = int(ne_type_no[0])
                        ne_no_list.add(ne_type_no)
                        ne_old = ne_list[ne_type_no]
                        ne_new = ne_new.replace(pat, ne_list[ne_type_no])
                    if ne_new == pat_list[pat_i][3]: #disambiguation
                        string = string.replace(ret, ne_new)
                        ne_no_list = list(ne_no_list)
                        ne_no_list.sort(reverse=True) #Attention point
                        for no in ne_no_list:
                            if len(re.findall('\[\[.{0,5}<[A-Z]{3}>'+str(no)+'\]\]', string)) == 0:
                                del ne_list[no]
                                ne_count -= 1
                                for i in range(no, ne_count):
                                    big_no = re.findall('\[\[.{0,5}<[A-Z]{3}>'+str(i+1)+'\]\]', string)
                                    big_no = list(set(big_no))
                                    assert len(big_no) == 1
                                    big_no = big_no[0]
                                    small_no = big_no.replace(str(i+1), str(i))
                                    string = string.replace(big_no, small_no)
            elif sub_str == u'?':
                assert ne_type == u'<xxx>'
                for ret in ret_list:
                    old_ne_pat = re.findall(ur'\[\[.{0,5}<[A-Z]{3}>\d+\]\]', ret)
                    assert len(old_ne_pat) == 1
                    ne_new = ret
                    pat = old_ne_pat[0]
                    ne_type_no = re.findall(ur'\d+', pat)
                    assert len(ne_type_no) == 1
                    no = int(ne_type_no[0])
                    ne_old = ne_list[no]
                    ne_new = ne_new.replace(pat, ne_old)
                    if len(re.findall(pat_list[pat_i][3], ne_old)) > 0: #disambiguation
                        string = string.replace(ret, ne_new)
                        if len(re.findall('\[\[.{0,5}<[A-Z]{3}>'+str(no)+'\]\]', string)) == 0:
                            del ne_list[no]
                            ne_count -= 1
                            for i in range(no, ne_count):
                                big_no = re.findall('\[\[.{0,5}<[A-Z]{3}>'+str(i+1)+'\]\]', string)
                                big_no = list(set(big_no))
                                assert len(big_no) == 1
                                big_no = big_no[0]
                                small_no = big_no.replace(str(i+1), str(i))
                                string = string.replace(big_no, small_no)
            else:
                assert False
    return string, ne_list

def fetch_ner_result(seg_line):
    ret_str = u''
    for seg_part in seg_line:
        ret_str += seg_part[0] + '/' + seg_part[1] + ' '
    assert ret_str[-1] == ' '
    ret_str = ret_str[:-1]
    return ret_str

def tag_pretreat(seg_line):
    ret_str = u''
    for seg_part in seg_line:
        if '<MED>' in seg_part[1] or '<DIS>' in seg_part[1] or '<SYM>' in seg_part[1] or '<TRE>' in seg_part[1]:
            ret_str += '[['+seg_part[0]+seg_part[1]+']]'
        else:
            ret_str += seg_part[0]
    return ret_str

def analysis(raw_str, is_tag_pretreat=False):
    tag_str, ne_list = dict_ne(raw_str) #dictionary matching
    tag_str, ne_list = ne_rule(tag_str, ne_list) #rules are introduced
    seg_line = bootstrapping_init.deal_cws(raw_str)
    ne_pos_list = bootstrapping_init.cal_ne_pos(tag_str, ne_list)
    seg_line = bootstrapping_init.comb_ne_cws(seg_line, ne_pos_list)
    if is_tag_pretreat == False:
        ner_str = fetch_ner_result(seg_line)
    else:
        ner_str = tag_pretreat(seg_line)
    return ner_str

def tag_one_file(text):
    fp = file(os.path.join('./tag/medical_texts', text), 'rb')
    fp_out = file(os.path.join('./tag/after_tag', text+'_ner'), 'wb')
    for line in fp:
        line = (line.strip()).decode('UTF-8')
        if line[0] == u'\ufeff': #Attention point
            line = line[1:]
        new_line = analysis(line)
        fp_out.write(new_line.encode('UTF-8')+'\n')
    fp.close()

def tag_ner():
    texts_list = os.listdir('./tag/medical_texts')
    for text in texts_list:
        tag_one_file(text)

def drive_start(tag_flag = True):
    targ_name = u'汇总词典.dic'
    all_dicts = {u'药物':'MED', u'疾病':'DIS', u'症状':'SYM', u'手术检查':'TRE'}
    ner_mm_rule.map_dict('./tag/dictionary', all_dicts, targ_name)
    pynlpir.open()
    if tag_flag == True:
        tag_ner()

def drive_end():
    pynlpir.close()

if __name__ == "__main__":
    start = time.clock()
    drive_start()
    drive_end()
    end = time.clock()
    print "Time to run: %f" % (end-start)
