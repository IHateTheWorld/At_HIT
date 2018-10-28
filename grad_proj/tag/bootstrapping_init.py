# -*- coding: utf-8 -*-

import pynlpir
import sys
import re
import os
import time
from bootstrapping_iterate import *
import extend_pattern

#decode: str ==> unicode
#encode: unicode ==> str
encode_type = sys.getfilesystemencoding() #UTF-8 in my machine

ne_pare_list = []
stop_table =[]
stop_pos = ("punctuation-mark")
cont_word_pos = {"verb", "noun"}
seg_sym = u'，：。;￥'
#seg_sym = u'。;'

def relat_pattern(file_name = u'./extend_pattern/实体之间关系规则.pat_ex'):
    fp = file(file_name, 'rb')
    pat_list = []
    for line in fp:
        line = (line.strip()).decode('UTF-8')
        eles = line.split()
        pat_list.append(eles)
    fp.close()
    return pat_list

def relat_rule():
    texts_list = os.listdir('./after_tag')
    pat_list = relat_pattern()
    fp_sample = file('./after_tag/seed_context.xxx', 'wb')
    fp_L = file('./after_tag/L_init.xxx', 'wb')
    fp_U = file('./after_tag/U_init.xxx', 'wb')
    for text in texts_list:
        ###print '>>>>>>>>>>>>>>>>>',
        ###print text
        if len(re.findall(ur'.*_ner$', text)) > 0:
            pass
        elif text == 'seed.xxx' or len(re.findall(ur'.*_rrule$', text)) > 0:
            continue
        else:
            ###os.remove(os.path.join('./after_tag', text))
            continue
        fp = file(os.path.join('./after_tag', text), 'rb')
        fp_out = file(os.path.join('./after_tag', text+'_rrule'), 'wb')
        part_no = 5
        count = 0
        part_lines = []
        for line in fp:
            part_lines.append(line.strip())
            count += 1
            if count == part_no:
                count = 0
                fp_out.write(part_lines[0]+'\n')
                fp_out.write(part_lines[1]+'\n')
                fp_out.write(part_lines[2]+'\n')
                fp_out.write(part_lines[3]+'\n')
                fp_out.write(part_lines[4]+'\n')
                ne_list = ((part_lines[4].decode('UTF-8')).split())[1:]
                string = part_lines[3].decode('UTF-8')
                relat_list = set()
                for pat_i in range(len(pat_list)):
                    pat_str = pat_list[pat_i][0]
                    relat_type = pat_list[pat_i][1]
                    pattern = re.compile(pat_str)
                    ret_list = pattern.findall(string)
                    if len(ret_list) > 0:
                        ###print '='*50
                        ###print (part_lines[1].decode('UTF-8')).encode(encode_type)
                        ###print string.encode(encode_type)
                        ###print (u'命名实体: ').encode(encode_type)+' '.join(y.encode(encode_type) for y in ne_list)
                        ###print (u'匹配规则: ').encode(encode_type)+' '.join(y.encode(encode_type) for y in ret_list)
                        ret_list = list(set(ret_list))
                        for ret in ret_list:
                            ne_str_list = re.findall(ur'\[\[.{0,5}<[A-Z]{3}>.{0,5}\d+\]\]', ret)
                            ne_no_list = []
                            for ne_i in ne_str_list:
                                ne_no = re.findall(ur'\d+', ne_i)
                                assert len(ne_no) == 1
                                ne_no = int(ne_no[0])
                                ne_no_list.append(ne_no)
                            for i in range(2, len(pat_list[pat_i])):
                                ne_pair = pat_list[pat_i][i].split('-')
                                assert len(ne_pair) == 2
                                left_ne = ne_no_list[int(ne_pair[0])]
                                right_ne = ne_no_list[int(ne_pair[1])]
                                relat_list.add('<'+str(left_ne)+','+relat_type+','+str(right_ne)+'>')
                        ###print (u'实体关系: ').encode(encode_type)+' '.join(y.encode('UTF-8') for y in relat_list)
                fp_out.write('实体关系: '+' '.join(y.encode('UTF-8') for y in relat_list)+'\n')
                if len(relat_list) > 0:
                    fp_sample.write(part_lines[0]+'\n')
                    fp_sample.write(part_lines[1]+'\n')
                    fp_sample.write(part_lines[2]+'\n')
                    fp_sample.write(part_lines[3]+'\n')
                    fp_sample.write(part_lines[4]+'\n')
                    fp_sample.write('实体关系: '+' '.join(y.encode('UTF-8') for y in relat_list)+'\n')
                    fp_L.write(part_lines[0]+'\n')
                    fp_L.write(part_lines[1]+'\n')
                    fp_L.write(part_lines[2]+'\n')
                    fp_L.write(part_lines[3]+'\n')
                    fp_L.write(part_lines[4]+'\n')
                    fp_L.write('实体关系: '+' '.join(y.encode('UTF-8') for y in relat_list)+'\n')
                else:
                    fp_U.write(part_lines[0]+'\n')
                    fp_U.write(part_lines[1]+'\n')
                    fp_U.write(part_lines[2]+'\n')
                    fp_U.write(part_lines[3]+'\n')
                    fp_U.write(part_lines[4]+'\n')
                part_lines = []
        fp.close()
        fp_out.close()
        ###os.remove(os.path.join('./after_tag', text))
    fp_sample.close()
    fp_L.close()
    fp_U.close()

def read_seed(seed_path):
    global ne_pare_list
    fp = file(seed_path, 'rb')
    fp_out = file('./after_tag/seed_pare.xxx', 'wb')
    part_no = 6
    count = 0
    part_lines = []
    ne_pare_set = set()
    for line in fp:
        part_lines.append(line.strip())
        count += 1
        if count == part_no:
            count = 0
            ne_list = ((part_lines[4].decode('UTF-8')).split())[1:]
            relat_list = ((part_lines[5].decode('UTF-8')).split())[1:]
            for r_i in range(len(relat_list)):
                new_relat = relat_list[r_i][:]
                relat_type = re.findall(ur',.*,', new_relat)
                assert len(relat_type) == 1
                relat_type = relat_type[0][1:-1]
                no_pare = re.findall(ur'\d+', new_relat)
                assert len(no_pare) == 2
                new_relat = ne_list[int(no_pare[0])] + ' ' + ne_list[int(no_pare[1])] + ' ' + relat_type
                ne_pare_set.add(new_relat)
            part_lines = []
    ne_pare_list = list(ne_pare_set)
    ne_pare_list.sort()
    for np_i in range(len(ne_pare_list)):
        fp_out.write(ne_pare_list[np_i].encode('UTF-8')+'\n')
        ne_pare_list[np_i] = ne_pare_list[np_i].split()
    fp.close()
    fp_out.close()

def fetch_context():
    global ne_pare_list
    texts_list = os.listdir('./after_tag')
    fp_context = file('./after_tag/pare_context.xxx', 'wb')
    for text in texts_list:
        if len(re.findall(ur'.*_rrule$', text)) > 0:
            pass
        else:
            continue
        fp = file(os.path.join('./after_tag', text), 'rb')
        part_no = 6
        count = 0
        part_lines = []
        for line in fp:
            part_lines.append(line.strip())
            count += 1
            if count == part_no:
                count = 0
                ne_list = ((part_lines[4].decode('UTF-8')).split())[1:]
                for ne_pare in ne_pare_list:
                    ne_left = ne_pare[0]
                    ne_right = ne_pare[1]
                    if ne_left in ne_list and ne_right in ne_list:
                        fp_context.write(part_lines[0]+'\n')
                        fp_context.write(part_lines[1]+'\n')
                        fp_context.write(part_lines[2]+'\n')
                        fp_context.write(part_lines[3]+'\n')
                        fp_context.write(part_lines[4]+'\n')
                        fp_context.write(part_lines[5]+'\n')
                part_lines = []
        fp.close()
    fp_context.close()

def deal_cws(in_str):
    seg_line = pynlpir.segment(in_str, pos_tagging=True)
    for s_i in range(len(seg_line)-1, -1, -1):
        seg_line[s_i] = list(seg_line[s_i])
        if seg_line[s_i][1] == None:
            seg_line[s_i][1] = u'None'
            #if seg_line[s_i][0] == u' ': #Attention: can't skip the blank-space
            #    del seg_line[s_i]
        else:
            seg_line[s_i][1] = seg_line[s_i][1].replace(' ', '-')
    cur_pos = 0
    for s_i in range(len(seg_line)):
        ne_pos_pare = []
        ne_pos_pare.append(cur_pos)
        cur_pos += len(seg_line[s_i][0])
        ne_pos_pare.append(cur_pos)
        seg_line[s_i].append(ne_pos_pare)
    ###seg_line = ' '.join('/'.join(y) for y in seg_line)
    return seg_line

def cal_ne_pos(in_str, ne_list):
    tmp = []
    tmp.append(in_str)
    cur_pos = 0
    is_i = 0
    no_str = ''
    ne_type = ''
    ne_pos_list = []
    if len(ne_list) > 0:
        while is_i < len(in_str):
            char = in_str[is_i]
            if char == '[':
                if is_i < (len(in_str) - 1):
                    is_i += 1
                    char = in_str[is_i]
                    if char == '[':
                        is_i += 1
                        char = in_str[is_i]
                        while not (char >= u'0' and char <= u'9'):
                            ne_type += char
                            is_i += 1
                            char = in_str[is_i]
                        while char != ']':
                            no_str += char
                            is_i += 1
                            char = in_str[is_i]
                        is_i += 1
                        char = in_str[is_i]
                        assert char == ']'
                        ne_pos_pare = []
                        ne_pos_pare.append(cur_pos)
                        cur_pos += len(ne_list[int(no_str)])
                        ne_pos_pare.append(cur_pos)
                        ne_pos_pare.append(ne_type)
                        ne_pos_list.append(ne_pos_pare)
                        cur_pos -= 1
                        no_str = ''
                        ne_type = ''
                    else:
                        is_i -= 1
            is_i += 1
            cur_pos += 1
    return ne_pos_list

def fetch_tag_flags(in_str, ne_list, low, high):
    no_str = ''
    cur_pos = 0
    is_i = 0
    new_low = -1
    new_high = -1
    while is_i < len(in_str):
        if cur_pos == low:
            new_low = is_i
        if cur_pos == high:
            new_high = is_i
        char = in_str[is_i]
        if char == '[':
            if is_i < (len(in_str) - 1):
                is_i += 1
                char = in_str[is_i]
                if char == '[':
                    is_i += 1
                    char = in_str[is_i]
                    while not (char >= u'0' and char <= u'9'):
                        is_i += 1
                        char = in_str[is_i]
                    while char != ']':
                        no_str += char
                        is_i += 1
                        char = in_str[is_i]
                    is_i += 1
                    char = in_str[is_i]
                    assert char == ']'
                    cur_pos += len(ne_list[int(no_str)])
                    cur_pos -= 1
                    no_str = ''
                else:
                    is_i -= 1
        is_i += 1
        cur_pos += 1
    return (new_low, new_high)

def comb_ne_cws(seg_line, ne_pos_list):
    sl_i = 0
    npl_i = 0
    if len(ne_pos_list) == 0:
        return seg_line
    ne_low = ne_pos_list[0][0]
    ne_high = ne_pos_list[0][1]
    cur_low = seg_line[0][2][0]
    cur_high = seg_line[0][2][1]
    low_i = -1
    high_i = -1
    while sl_i < len(seg_line) and npl_i < len(ne_pos_list):
        while cur_low < ne_low:
            sl_i += 1
            if sl_i < len(seg_line):
                cur_low = seg_line[sl_i][2][0]
                cur_high = seg_line[sl_i][2][1]
            else:
                break
        assert sl_i <= len(seg_line)
        if sl_i == len(seg_line):
            break
        if cur_low == ne_low:
            low_i = sl_i
            while cur_high < ne_high:
                sl_i += 1
                cur_high = seg_line[sl_i][2][1]
                cur_low = seg_line[sl_i][2][0]
            if cur_high == ne_high: ###succeed...
                high_i = sl_i
                ###TODO...
                ne_str = ''
                for lh_i in range(high_i, low_i-1, -1):
                    ne_str = seg_line[lh_i][0] + ne_str
                    del seg_line[lh_i]
                ne_type = ne_pos_list[npl_i][2]
                new_part = []
                new_part.append(ne_str)
                new_part.append(ne_type)
                new_part.append(ne_pos_list[npl_i][:2])
                seg_line.insert(low_i, new_part)
                sl_i -= (high_i - low_i) #Attention: don't forget this point
                sl_i += 1
                npl_i += 1
                if npl_i < len(ne_pos_list):
                    ne_low = ne_pos_list[npl_i][0]
                    ne_high = ne_pos_list[npl_i][1]
                if sl_i < len(seg_line):
                    cur_low = seg_line[sl_i][2][0]
                    cur_high = seg_line[sl_i][2][1]
                ###DONE...
                low_i = -1
                high_i = -1
            else: ###fail...
                while ne_low < cur_high:
                    ###print 'There is a ne can\'t be recognize...111'
                    npl_i += 1
                    if npl_i < len(ne_pos_list):
                        ne_low = ne_pos_list[npl_i][0]
                        ne_high = ne_pos_list[npl_i][1]
                    else:
                        break
        else: ###fail...
            ###print 'There is a ne can\'t be recognize...222'
            while ne_low < cur_low:
                npl_i += 1
                if npl_i < len(ne_pos_list):
                    ne_low = ne_pos_list[npl_i][0]
                    ne_high = ne_pos_list[npl_i][1]
                else:
                    break
    return seg_line

def read_stop_table(tab_path):
    global stop_table
    fp = file(tab_path, 'rb')
    for stop_w in fp:
        stop_w = (stop_w.strip()).decode('UTF-8')
        new_ele = [] #that is very awful
        new_ele.append(stop_w)
        stop_table.append(new_ele)
    fp.close()

def binarysearch(low, high, index, target, loc, ToD_list):
    if loc == -1 or loc < low or loc > high:
        pass
    elif loc == low:
        low += 1
    elif loc == high:
        high -= 1
    else:
        low_near = ToD_list[loc-1][0][index]
        high_near = ToD_list[loc+1][0][index]
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
        key_char = ToD_list[mid][0][index]
        if target < key_char:
            high =  mid - 1
        elif target > key_char:
            low = mid + 1
        else:
            return -mid - 1
    return low

def sear_interface(ToD_list, list_len, key_str):
    low = 0
    high = list_len - 1
    loc = -1
    is_find = False
    for w_i in range(len(key_str)):
        target = key_str[w_i]
        bin_ret = binarysearch(low, high, w_i, target, loc, ToD_list)
        if bin_ret < 0:
            bin_ret = -bin_ret - 1
            new_low = bin_ret
            new_high = bin_ret + 1
            while new_low >= low and w_i < len(ToD_list[new_low][0]) and ToD_list[new_low][0][w_i] == target:
                if w_i == (len(ToD_list[new_low][0]) - 1):
                    loc = new_low
                    if w_i == len(key_str) - 1:
                        is_find = True
                new_low -= 1
            new_low += 1
            while new_high <= high and w_i < len(ToD_list[new_high][0]) and ToD_list[new_high][0][w_i] == target:
                if w_i == (len(ToD_list[new_high][0]) - 1):
                    loc = new_high
                    if w_i == len(key_str) - 1:
                        is_find = True
                new_high += 1
            new_high -= 1
            low = new_low
            high = new_high
        else:
            return None
    if is_find == True:
        if len(ToD_list[loc]) > 1:
            return ToD_list[loc][1]
        else:
            return True
    else:
        return None

def rm_stop_w(seg_line):
    global stop_table
    global stop_pos
    for seg_i in range(len(seg_line)):
        word = seg_line[seg_i][0]
        ret = sear_interface(stop_table, len(stop_table), word)
        if ret == True:
            seg_line[seg_i][1] = 'STOP'
        elif ret == None:
            if seg_line[seg_i][1] in stop_pos:
                seg_line[seg_i][1] = 'STOP'
            elif seg_line[seg_i][1] == 'None':
                seg_line[seg_i][1] = 'STOP'
        else:
            assert False
    return seg_line

def pos_cont_w(seg_line):
    global cont_word_pos
    for seg_i in range(len(seg_line)):
        pos = seg_line[seg_i][1]
        if pos in cont_word_pos:
            pass
            #seg_line[seg_i][1] += '-CW'
    return seg_line

def gen_rule(seg_line):
    rule_ret = ""
    pre_pos = ''
    for seg_i in range(len(seg_line)):
        word = seg_line[seg_i][0]
        pos = seg_line[seg_i][1]
        region = seg_line[seg_i][2]
        if pos == 'STOP':
            if pre_pos == 'STOP':
                region[-1] = pre_region[0]
                pass
            else:
                rule_ret += '#'+'*'+'@'+str(region[0])+'-'+str(region[1])
        elif pos == 'noun' or pos == 'verb': #Maybe add something
            num_ret = extend_pattern.sear_num_new(word)
            if num_ret != None:
                if len(num_ret) > 1:
                    cilin_str = '|'.join(num_ret)
                    cilin_str = '(?:'+cilin_str+')'
                else:
                    cilin_str = num_ret[0]
                cilin_str = u'￥'+'.*'+cilin_str+'.*'+u'￥'
                rule_ret += ('#'+word+'/'+cilin_str+'@'+str(region[0])+'-'+str(region[1]))
            else:
                rule_ret += ('#'+word+'/'+pos+'@'+str(region[0])+'-'+str(region[1]))
        else:
            if pos[0] != '<' and pos[-1] != '>':
                rule_ret += ('#'+word+'/'+pos+'@'+str(region[0])+'-'+str(region[1]))
            else:
                rule_ret += ('#'+pos+'@'+str(region[0])+'-'+str(region[1]))
        pre_pos = pos
        pre_region = region[:]
    if rule_ret[0] == '#':
        rule_ret = rule_ret[1:]
    return rule_ret

def short_sent(raw_string, tag_string, relat_list, ne_list):
    short_ret = []
    for r_i in range(len(relat_list)):
        new_relat = relat_list[r_i][:]
        relat_type = re.findall(ur',.*,', new_relat)
        assert len(relat_type) == 1
        relat_type = relat_type[0][1:-1]
        no_pare = re.findall(ur'\d+', new_relat)
        assert len(no_pare) == 2
        left_ne_no = int(no_pare[0])
        right_ne_no = int(no_pare[1])
        left_ne = ne_list[left_ne_no]
        right_ne = ne_list[right_ne_no]
        ###There should be paid attention to, how to short the sentence
        short_tag = re.findall(ur'[^'+seg_sym+'\]]*'+ur'\[\[.{0,5}<[A-Z]{3}>'+no_pare[0]+ur'\]\]'+\
                    ur'[^'+seg_sym+']*'+ur'\[\[.{0,5}<[A-Z]{3}>'+no_pare[1]+'\]\]'+ur'[^'+seg_sym+'\[]*',\
                    tag_string
        )
        mid_flag = len(short_tag)
        short_tag.extend(
                    re.findall(ur'[^'+seg_sym+'\]]*'+ur'\[\[.{0,5}<[A-Z]{3}>'+no_pare[1]+ur'\]\]'+\
                    ur'[^'+seg_sym+']*'+ur'\[\[.{0,5}<[A-Z]{3}>'+no_pare[0]+'\]\]'+ur'[^'+seg_sym+'\[]*',\
                    tag_string)
        )
        left_type = '<' + relat_type[:3] + '>'
        right_type = '<' + relat_type[4:7] + '>'
        for st_i in range(len(short_tag)):
            st_sent = short_tag[st_i]
            if st_i < mid_flag:
                left_no = left_ne_no
                right_no = right_ne_no
                lr_li = 0
                lr_ri = 1
            else:
                left_no = right_ne_no
                right_no = left_ne_no
                lr_li = 1
                lr_ri = 0
            l_type_flag = re.findall(ur'\[\[.{0,5}<[A-Z]{3}>'+no_pare[lr_li]+ur'\]\]', st_sent)
            r_type_flag = re.findall(ur'\[\[.{0,5}<[A-Z]{3}>'+no_pare[lr_ri]+ur'\]\]', st_sent)
            ###assert (len(l_type_flag) == 1 and len(r_type_flag) == 1)
            l_type_flag = l_type_flag[0]
            r_type_flag = r_type_flag[0]
            l_type = re.findall(ur'<.*>', l_type_flag)
            r_type = re.findall(ur'<.*>', r_type_flag)
            assert (len(l_type) == 1 and len(r_type) == 1)
            l_type = l_type[0]
            r_type = r_type[0]
            if len(l_type) > 5:
                if st_i < mid_flag:
                    st_sent = st_sent.replace(l_type_flag, '[['+left_type+no_pare[lr_li]+']]')
                else:
                    st_sent = st_sent.replace(l_type_flag, '[['+right_type+no_pare[lr_li]+']]')
            if len(r_type) > 5:
                if st_i < mid_flag:
                    st_sent = st_sent.replace(r_type_flag, '[['+right_type+no_pare[lr_ri]+']]')
                else:
                    st_sent = st_sent.replace(r_type_flag, '[['+left_type+no_pare[lr_ri]+']]')
            #the re.sub function can't deal with the unicode string
            short_raw = st_sent.encode('UTF-8')
            short_ne_list = re.findall('\[\[.{0,5}<[A-Z]{3}>\d+\]\]', short_raw)
            lr_i = [-1, -1]
            for sn_i in range(len(short_ne_list)): ###I think that may be awful
                sn_pos = short_ne_list[sn_i]
                sn_no = re.findall('\d+', sn_pos)
                assert len(sn_no) == 1
                sn_no = int(sn_no[0])
                if sn_no == left_no:
                    lr_i[lr_li] = sn_i
                elif sn_no == right_no:
                    if lr_i[lr_li] >= 0:
                        lr_i[lr_ri] = sn_i
                        break
            for ne_i in range(len(ne_list)):
                if re.findall('\[\[.{0,5}<[A-Z]{3}>'+str(ne_i)+'\]\]', short_raw) > 0:
                    short_raw = re.sub('\[\[.{0,5}<[A-Z]{3}>'+str(ne_i)+'\]\]', ne_list[ne_i].encode('UTF-8'), \
                                short_raw, count=1, flags=0)
            '''
            short_raw = re.sub('\[\[.{0,5}<[A-Z]{3}>'+no_pare[0].encode('UTF-8')+'\]\]', left_ne.encode('UTF-8'), \
                                short_raw, count=1, flags=0)
            short_raw = re.sub('\[\[.{0,5}<[A-Z]{3}>'+no_pare[1].encode('UTF-8')+'\]\]', right_ne.encode('UTF-8'), \
                                short_raw, count=1, flags=0)
            '''
            short_raw = short_raw.decode('UTF-8')
            new_ele = []
            new_ele.append(short_raw)
            new_ele.append(st_sent)
            new_ele.append(relat_type)
            new_ele.append(lr_i)
            short_ret.append(new_ele)
    return short_ret

def generalize():
    fp = file('./after_tag/seed_context.xxx', 'rb')
    fp_out = file('./after_tag/seed_generalize.xxx', 'wb')
    fp_r = file('./after_tag/rule_generalize.xxx', 'wb')
    part_no = 6
    count = 0
    part_lines = []
    for line in fp:
        part_lines.append(line.strip())
        count += 1
        if count == part_no:
            count = 0
            fp_out.write(part_lines[0]+'\n')
            fp_out.write(part_lines[1]+'\n')
            fp_out.write(part_lines[2]+'\n')
            fp_out.write(part_lines[3]+'\n')
            fp_out.write(part_lines[4]+'\n')
            fp_out.write(part_lines[5]+'\n')
            ne_list = ((part_lines[4].decode('UTF-8')).split())[1:]
            relat_list = ((part_lines[5].decode('UTF-8')).split())[1:]
            raw_string_long = ((part_lines[1]).strip()).decode('UTF-8')
            tag_string_long = ((part_lines[3]).strip()).decode('UTF-8')
            ###TODO...
            seg_line_long = deal_cws(raw_string_long)
            ne_pos_list_long = cal_ne_pos(tag_string_long, ne_list)
            seg_line_long = comb_ne_cws(seg_line_long, ne_pos_list_long)
            ###SOMETHING...
            short_ret_list = short_sent(raw_string_long, tag_string_long, relat_list, ne_list)
            for short_ret in short_ret_list:
                raw_string = short_ret[0]
                tag_string = short_ret[1]
                pair_type = short_ret[2]
                pair_index = short_ret[3]
                pair_str = str(pair_index[0])+'-'+str(pair_index[1])
                seg_line = deal_cws(raw_string)
                ne_pos_list = cal_ne_pos(tag_string, ne_list)
                seg_line = comb_ne_cws(seg_line, ne_pos_list)
                ###SSS
                seg_line = rm_stop_w(seg_line)
                seg_line = pos_cont_w(seg_line)
                rule_ret = gen_rule(seg_line)
                #############
                fp_out.write((u'句子划分:').encode('UTF-8'))
                for seg_part in seg_line:
                    #fp_out.write(' '+(seg_part[0]).encode('UTF-8'))
                    #if (seg_part[1][0] == '<' and seg_part[1][-1] == '>') or seg_part[1] == 'STOP':
                    #    fp_out.write('/'+(seg_part[1]).encode('UTF-8'))
                    fp_out.write(' '+(seg_part[0]).encode('UTF-8'))
                    #fp_out.write(' '+(seg_part[0]).encode('UTF-8')+'/'+(seg_part[1]).encode('UTF-8')+\
                    #            '/'+str(seg_part[2][0])+'-'+str(seg_part[2][1]))
                fp_out.write('\n')
                ###fp_out.write((u'实体下标:').encode('UTF-8'))
                ###for ne_pare in ne_pos_list:
                ###    fp_out.write(' ' + str(ne_pare[0]) + '-' + str(ne_pare[1]) + '-' + ne_pare[2])
                ###fp_out.write('\n')
                fp_out.write((u'生成规则:').encode('UTF-8')+rule_ret.encode('UTF-8')+' '+pair_type.encode('UTF-8')+\
                            ' '+pair_str.encode('UTF-8')+'\n')
                fp_r.write(rule_ret.encode('UTF-8')+' '+pair_type.encode('UTF-8')+\
                            ' '+pair_str.encode('UTF-8')+'\n')
            ###DONE...
            part_lines = []
    fp.close()
    fp_out.close()
    fp_r.close()

def load_cilin(cilin_opath = u'./tongyicilin/哈工大同义词林扩展版.txt', cilin_path = u'./tongyicilin/哈工大同义词林扩展版_new_struct.txt', n_stat_path = u'./tongyicilin/n_stat.txt'):
    extend_pattern.load_tongyicilin_new(cilin_opath, cilin_path, n_stat_path)

if __name__ == "__main__":
    start = time.clock()
    pynlpir.open()
    relat_rule()
    seed_path = u'./after_tag/seed_context.xxx'
    read_seed(seed_path)
    fetch_context()
    read_stop_table(u'./dictionary/停用表.dic')
    load_cilin() #Attention point
    generalize()
    cop_file('./after_tag/L_init.xxx', './after_tag/L.xxx')
    cop_file('./after_tag/U_init.xxx', './after_tag/U.xxx')
    pynlpir.close()
    end = time.clock()
    print 'Time to run: %f' % (end-start)
