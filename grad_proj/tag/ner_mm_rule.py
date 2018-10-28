# -*- coding: utf-8 -*-

import pynlpir
import sys
import re
import os
import json
import time

#decode: str ==> unicode
#encode: unicode ==> str
encode_type = sys.getfilesystemencoding() #UTF-8 in my ubuntu 14.04
MAX_WIN = 5
dict_list = []
dict_counter = 0

rep_dict = {u"\[\[<MED>\d+\]\]":u"<MED>", u"\[\[.{0,5}<DIS>.{0,5}\d+\]\]":u"<DIS>", \
                u"\[\[.{0,5}<SYM>.{0,5}\d+\]\]":u"<SYM>", u"\[\[<TRE>\d+\]\]":u"<TRE>"}

def orginal_ner_pat(pat_name):
    global rep_dict
    fp = file(os.path.join('./manual_pattern/', pat_name), 'rb')
    fp_out = file(os.path.join('./extend_pattern', pat_name), 'wb')
    for pat_line in fp:
        pat_line = (pat_line.strip()).decode('UTF-8')
        for k,v in rep_dict.iteritems():
            pat_line = pat_line.replace(k, v)
        fp_out.write(pat_line.encode('UTF-8')+'\n')
    fp_out.close()
    fp.close()

def restore_ner_pat(pat_name):
    global rep_dict
    fp = file(os.path.join('./manual_pattern', pat_name), 'rb')
    fp_out = file(os.path.join('./extend_pattern', pat_name+'_ex'), 'wb')
    for pat_line in fp:
        pat_line = (pat_line.strip()).decode('UTF-8')
        pat_line = pat_line.split()
        for k,v in rep_dict.iteritems():
            for p_i in range(len(pat_line)):
                if p_i != 1:
                    pat_line[p_i] = pat_line[p_i].replace(v, k) #there is the point different
        fp_out.write(pat_line[0].encode('UTF-8'))
        for i in range(1, len(pat_line)):
            fp_out.write(' '+pat_line[i].encode('UTF-8'))
        fp_out.write('\n')
    fp.close()
    fp_out.close()

def map_dict(path, all_dicts, targ_name):
    global dict_list
    global dict_counter
    map_dic = {}
    for name, tag in all_dicts.iteritems():
        fp = file(os.path.join(path, name+'.dic'), 'rb')
        for word in fp:
            word = (word.strip()).decode('UTF-8')
            if word not in map_dic:
                map_dic[word] = '<'+tag+'>'
            else:
                map_dic[word] = map_dic[word]+'<'+tag+'>'
        fp.close()
    fp_out = file(os.path.join(path, targ_name), 'wb')
    dict_list = sorted(map_dic.items(), key=lambda d:d[0]) #the result returned is a list
    dict_counter = len(dict_list)
    for name, tag in dict_list:
        fp_out.write('%s %s\n' % (name.encode('UTF-8'), tag))
    fp_out.close()

def load_dict(load_path):
    words_dict = {}
    fp = file(load_path, 'rb')
    for word in fp:
        words = (word.strip()).split()
        words_dict[words[0]] = words[1]
    fp.close()
    return words_dict

def cws_mm(line_words, words_dict):
    new_list = []
    i = 0
    length = len(line_words)
    while i < length:
        pace = 1
        for j in range(MAX_WIN):
            if j == MAX_WIN - 1:
                new_list.append(line_words[i])
                break
            if i+(MAX_WIN-j) <= length:
                new_word = ''.join(line_words[i:i+(MAX_WIN-j)])
                if new_word in words_dict.keys():
                    new_list.append(new_word)
                    pace = MAX_WIN-j
                    break
        i += pace
    return new_list

def cws(path, words_dict):
    fp = open(os.path.join('./medical_texts', path), 'rb')
    outfp = open(os.path.join('./after_tag', path+'_cws'), 'wb')
    patt1 = re.compile(ur'\s+')
    for line in fp:
        seg_line = pynlpir.segment(line, pos_tagging=False)
        new_line = ' '.join(word.encode(encode_type) for word in seg_line)
#        new_line = re.sub(patt1, '/', new_line, count=0, flags=0)
#        outfp.write(new_line+'\n')
        line_words = new_line.split()
        line_words = cws_mm(line_words, words_dict)
        for i in range(len(line_words)):
            if line_words[i] in words_dict.keys():
                #line_words[i] += '<T>'
                line_words[i] += words_dict[line_words[i]]
                line_words[i] = '[[' + line_words[i] + ']]'
        outfp.write(''.join(line_words)+'\n')
    fp.close()
    outfp.close()
    return

def cws_drive():
    pynlpir.open()
    load_path = u'./dictionary/汇总词典.dic'
    words_dict = load_dict(load_path)
    texts_list = os.listdir('./medical_texts')
    for text in texts_list:
        print ('taging %s' % text)
        cws(text, words_dict)
    return 

def binarysearch(low, high, index, target, loc):
    if loc == -1 or loc < low or loc > high:
        pass
    elif loc == low:
        low += 1
    elif loc == high:
        high -= 1
    else:
        low_near = dict_list[loc-1][0][index]
        high_near = dict_list[loc+1][0][index]
        if low_near > target:
            high = (loc-1) - 1
        elif low_near == target:
            return -(loc-1) - 1
        elif high_near == target:
            return -(loc+1) - 1
        elif high_near < target:
            low = (loc+1) + 1
        else:
            return 0 #stand for can find

    while high > low - 1:
        mid = (low + high) / 2
        key_char = dict_list[mid][0][index]
        if target < key_char:
            high =  mid - 1
        elif target > key_char:
            low = mid + 1
        else:
            return -mid - 1
    return low

def search_ne(ne, ne_list):
    for i in range(len(ne_list)):
        if ne_list[i] == ne:
            return i
    return -1

def add_ne(ne, ne_list):
    search_ret = search_ne(ne, ne_list)
    if search_ret == -1:
        ne_list.append(ne)
        return len(ne_list)-1
    else:
        return search_ret

def ner_drive():
    pynlpir.open()
    texts_list = os.listdir('./medical_texts')
    for text in texts_list:
        print ('taging %s' % text)
        fp = file(os.path.join('./medical_texts', text), 'rb')
        fp_out = file(os.path.join('./after_tag', text+'_mm'), 'wb')
        for line in fp:
            line = (line.strip()).decode('UTF-8')
            if line[0] == u'\ufeff': #Attention point
                line = line[1:]
            low = 0
            high = dict_counter - 1
            index = 0
            is_find = False
            loc = -1
            i = 0
            ne_list = []
            fp_out.write('='*50+'\n')
            seg_line = pynlpir.segment(line, pos_tagging=True)
            for s_i in range(len(seg_line)-1, -1, -1):
                seg_line[s_i] = list(seg_line[s_i])
                if seg_line[s_i][1] == None:
                    seg_line[s_i][1] = u'None'
                    if seg_line[s_i][0] == u' ':
                        del seg_line[s_i]
                else:
                    seg_line[s_i][1] = seg_line[s_i][1].replace(' ', '-')
            seg_line = ' '.join('/'.join(y) for y in seg_line)
            #fp_out.write(seg_line.encode('UTF-8')+'\n')
            fp_out.write(line.encode('UTF-8')+'\n')
            fp_out.write((u'文件信息:').encode('UTF-8')+' '+(text.decode(encode_type)).encode('UTF-8')+'\n')
            while i < len(line):
                target = line[i]
                bin_ret = binarysearch(low, high, index, target, loc)
                if bin_ret >= 0:
                    if is_find == False:
                        i -= (index - 1)
                        fp_out.write(line[i-1].encode('UTF-8'))
                    else:
                        add_pos = add_ne(dict_list[loc][0], ne_list)
                        fp_out.write('[['+dict_list[loc][1]+str(add_pos)+']]')
                        i = i - index + len(dict_list[loc][0])
                        is_find = False
                    low = 0
                    high = dict_counter - 1
                    index = 0
                    loc = -1
                    continue
                else:
                    bin_ret = -bin_ret - 1
                    new_low = bin_ret
                    new_high = bin_ret + 1
                    while new_low >= low and index < len(dict_list[new_low][0]) and dict_list[new_low][0][index] == target:
                        if index == (len(dict_list[new_low][0]) - 1):
                            loc = new_low
                            is_find = True
                        new_low -= 1
                    new_low += 1
                    while new_high <= high and index < len(dict_list[new_high][0]) and dict_list[new_high][0][index] == target:
                        if index == (len(dict_list[new_high][0]) - 1):
                            loc = new_high
                            is_find = True
                        new_high += 1
                    new_high -= 1
                    low = new_low
                    high = new_high
                    if i == len(line) - 1: #don't forget this exit point
                        if is_find == False:
                            i -= (index - 1)
                            fp_out.write(line[i-1].encode('UTF-8'))
                        else:
                            add_pos = add_ne(dict_list[loc][0], ne_list)
                            fp_out.write('[['+dict_list[loc][1]+str(add_pos)+']]')
                            i = i - index + len(dict_list[loc][0])
                            is_find = False
                        low = 0
                        high = dict_counter - 1
                        index = 0
                        loc = -1
                        continue
                i += 1
                index += 1
            fp_out.write('\n')
            fp_out.write((u'命名实体:').encode('UTF-8'))
            for ne in ne_list:
                fp_out.write(' '+ne.encode('UTF-8'))
            fp_out.write('\n')
        fp.close()
        fp_out.close()
    return 

def ne_pattern(file_name=u'./extend_pattern/命名实体规则.pat_ex'):
    fp = file(file_name, 'rb')
    pat_list = []
    for line in fp:
        line = (line.strip()).decode('UTF-8')
        eles = line.split()
        pat_list.append(eles)
    fp.close()
    return pat_list

def ner_rule():
    print 'ner-rule ing...'
    texts_list = os.listdir('./after_tag')
    pat_list = ne_pattern()
    for text in texts_list:
        ###print '>>>>>>>>>>>>>>>>>',
        ###print text
        if len(re.findall(ur'.*_mm$', text)) > 0:
            pass
        else:
            continue
        fp = file(os.path.join('./after_tag', text), 'rb')
        fp_out = file(os.path.join('./after_tag', text+'_ner'), 'wb')
        part_no = 5
        count = 0
        part_lines = []
        for line in fp:
            part_lines.append(line.strip())
            count += 1
            if count == part_no:
                count = 0
                ne_list = ((part_lines[4].decode('UTF-8')).split())[1:]
                string = part_lines[3].decode('UTF-8')
                for pat_i in range(len(pat_list)):
                    pat_str = pat_list[pat_i][0]
                    ne_type = pat_list[pat_i][1]
                    sub_str = pat_list[pat_i][2]
                    pattern = re.compile(pat_str)
                    #ret_list = re.findall(ur'\[\[.{0,5}<MED>.{0,5}\d\]\].{0,4}(?:胶囊|片)', string)
                    ret_list = pattern.findall(string)
                    #ret_list = list(set(ret_list)) #remove the duplication
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
                        ###print ' '.join(y.encode(encode_type) for y in ret_list)
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
                fp_out.write(part_lines[0]+'\n')
                fp_out.write(part_lines[1]+'\n')
                fp_out.write(part_lines[2]+'\n')
                fp_out.write(string.encode('UTF-8')+'\n')
                fp_out.write('命名实体: '+' '.join(y.encode('UTF-8') for y in ne_list)+'\n')
                part_lines = []
        fp.close()
        fp_out.close()

if __name__ == '__main__':
    start = time.clock()
    targ_name = u'汇总词典.dic'
    all_dicts = {u'药物':'MED', u'疾病':'DIS', u'症状':'SYM', u'手术检查':'TRE'}
    map_dict('./dictionary', all_dicts, targ_name)
    pat_name = u'命名实体规则.pat'
    ###orginal_ner_pat(pat_name)
    restore_ner_pat(pat_name)
    ner_drive()
    ###cws_drive()
    ner_rule()
    end = time.clock()
    print 'used time: %f' % (end-start)
