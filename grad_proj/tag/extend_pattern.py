# -*- coding: utf-8 -*-

import time
import os
import re
import pynlpir
import math

rep_dict = {u"\[\[<MED>\d+\]\]":u"<MED>", u"\[\[.{0,5}<DIS>.{0,5}\d+\]\]":u"<DIS>", \
                u"\[\[.{0,5}<SYM>.{0,5}\d+\]\]":u"<SYM>", u"\[\[<TRE>\d+\]\]":u"<TRE>"}
tycl_rep = {u"<MED>":u"药物", u"<DIS>":u"疾病", u"<SYM>":u"症状", u"<TRE>":u"治疗"}
stop_pos = ("punctuation-mark")

para_list = [0.1, 0.65, 0.8, 0.9, 0.96, 0.5]
cilin_original = []
cilin = []
stat_list = []
n_stat = dict()
seg_index = (1, 2, 4, 5)
id_count = 0
word_count = 0
stat_count = 0

def orginal_rela_pat(pat_name, in_base='./manual_pattern/', out_base='./extend_pattern'):
    global rep_dict
    fp = file(os.path.join(in_base, pat_name), 'rb')
    fp_out = file(os.path.join(out_base, pat_name), 'wb')
    for pat_line in fp:
        pat_line = (pat_line.strip()).decode('UTF-8')
        pat_line = pat_line.split()
        pat = pat_line[0]
        for k,v in rep_dict.iteritems():
            pat = pat.replace(k, v)
        fp_out.write(pat.encode('UTF-8'))
        for i in range(1, len(pat_line)):
            fp_out.write(' '+pat_line[i].encode('UTF-8'))
        fp_out.write('\n')
    fp_out.close()
    fp.close()

def restore_rela_pat(pat_name):
    global rep_dict
    fp = file(os.path.join('./manual_pattern', pat_name), 'rb')
    fp_out = file(os.path.join('./extend_pattern', pat_name+'_ex'), 'wb')
    for pat_line in fp:
        pat_line = (pat_line.strip()).decode('UTF-8')
        pat_line = pat_line.split()
        pat = pat_line[0]
        for k,v in rep_dict.iteritems():
            pat = pat.replace(v, k) #there is the point different
        fp_out.write(pat.encode('UTF-8'))
        for i in range(1, len(pat_line)):
            fp_out.write(' '+pat_line[i].encode('UTF-8'))
        fp_out.write('\n')
    fp.close()
    fp_out.close()

def load_tongyicilin_new(cilin_opath, cilin_path, n_stat_path):
    global cilin, stat_list, cilin_original
    global word_count, stat_count, id_count
    fp = file(cilin_opath, 'rb')
    for line in fp:
        line = (line.strip()).decode('UTF-8')
        line = line.split()
        cilin_original.append([])
        cilin_original[-1].append(line[0])
        cilin_original[-1].append(line[1:])
    fp.close()
    id_count = len(cilin_original)
    fp = file(cilin_path, 'rb')
    for line in fp:
        line = (line.strip()).decode('UTF-8')
        line = line.split()
        cilin.append([])
        cilin[-1].append(line[0])
        cilin[-1].append(line[1:])
    fp.close()
    word_count = len(cilin)
    fp = file(n_stat_path)
    for line in fp:
        line = (line.strip()).decode('UTF-8')
        line = line.split()
        stat_list.append([])
        stat_list[-1].append(line[0])
        stat_list[-1].append(int(line[1]))
    fp.close()
    stat_count = len(stat_list)

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
        return ToD_list[loc][1]
    else:
        return None

def sear_num_new(word):
    global cilin
    global word_count
    return sear_interface(cilin, word_count, word)

def sear_stat(id):
    global stat_list
    global stat_count
    return sear_interface(stat_list, stat_count, id)

def sear_words(id):
    global cilin_original
    global id_count
    return sear_interface(cilin_original, id_count, id)

def cal_sim(id_a, id_b):
    global n_stat
    a_parts = [ord(id_a[:1]), ord(id_a[1:2]), int(id_a[2:4]), ord(id_a[4:5]), int(id_a[5:-1])]
    b_parts = [ord(id_b[:1]), ord(id_b[1:2]), int(id_b[2:4]), ord(id_b[4:5]), int(id_b[5:-1])]
    i = 0
    while i < 5:
        if a_parts[i] != b_parts[i]:
            break;
        i += 1
    if i == 0:
        return para_list[0]
    elif i < 5:
        assert id_a[:seg_index[i-1]] == id_b[:seg_index[i-1]]
        ###n = n_stat[id_a[:seg_index[i-1]]]
        n = sear_stat(id_a[:seg_index[i-1]])
        k = abs(a_parts[i]-b_parts[i])
        print n, k, para_list[i]
        ret =  para_list[i] * math.cos(n*math.pi/180) * (float(n-k+1)/n) #Attention: int a/int b when a < b, the result is 0
        return ret
    elif i == 5:
        assert id_a[-1] == id_b[-1]
        tail_ch = id_a[-1]
        if tail_ch == '=':
            return 1
        elif tail_ch == '#':
            return para_list[-1]
        else:
            assert False
    else:
        assert False

def sim(w_a, w_b):
    ret = 0.0
    ###a_list = sear_num(w_a)
    ###b_list = sear_num(w_b)
    a_list = sear_num_new(w_a)
    b_list = sear_num_new(w_b)
    print a_list
    print b_list
    if w_a == w_b:
        return 1
    for num_a in a_list:
        for num_b in b_list:
            s_ab = cal_sim(num_a, num_b)
            print "%s - %s: %f" % (num_a, num_b, s_ab)
            if s_ab > ret:
                ret = s_ab
    return ret

def tycl_replace(pat_name):
    global tycl_rep
    global stop_pos
    pynlpir.open()
    fp = file(os.path.join('./manual_pattern', pat_name), 'rb')
    fp_out = file(os.path.join('./extend_pattern', pat_name+'_tycl'), 'wb')
    for pat_line in fp:
        fp_out.write('='*50+'\n')
        pat_line = (pat_line.strip()).decode('UTF-8')
        pat_line = pat_line.split()
        pat = pat_line[0]
        c_pat = pat
        for k,v in tycl_rep.iteritems():
            c_pat = c_pat.replace(k, v)
        seg_list = pynlpir.segment(pat, pos_tagging=False)
        seg_line = pynlpir.segment(pat, pos_tagging=True)
        c_seg_line = pynlpir.segment(c_pat, pos_tagging=True)
        for s_i in range(len(seg_line)-1, -1, -1):
            seg_line[s_i] = list(seg_line[s_i])
            if seg_line[s_i][1] == None:
                seg_line[s_i][1] = u'None'
                if seg_line[s_i][0] == u' ':
                    del seg_line[s_i]
            else:
                seg_line[s_i][1] = seg_line[s_i][1].replace(' ', '-')
        seg_line_str = ' '.join('/'.join(y) for y in seg_line)
        for s_i in range(len(c_seg_line)-1, -1, -1):
            c_seg_line[s_i] = list(c_seg_line[s_i])
            if c_seg_line[s_i][1] == None:
                c_seg_line[s_i][1] = u'None'
                if c_seg_line[s_i][0] == u' ':
                    del c_seg_line[s_i]
            else:
                c_seg_line[s_i][1] = c_seg_line[s_i][1].replace(' ', '-')
        c_seg_line_str = ' '.join('/'.join(y) for y in c_seg_line)
        fp_out.write(pat.encode('UTF-8')+'\n')
        fp_out.write(c_pat.encode('UTF-8')+'\n')
        fp_out.write('/'.join(y.encode('UTF-8') for y in seg_list))
        fp_out.write('\n')
        fp_out.write(seg_line_str.encode('UTF-8')+'\n')
        fp_out.write(c_seg_line_str.encode('UTF-8')+'\n')
        ss_i = 0 #the relative index of the word
        for s_i in range(len(seg_line)):
            left_sign = seg_line[s_i][0]
            right_sign = seg_line[s_i][1]
            if left_sign == "MED" or left_sign == "DIS" or left_sign == "SYM" or left_sign == "TRE" or \
               left_sign == "<":
               continue
            elif right_sign == None or right_sign == "punctuation-mark" or right_sign == "numeral" or \
               right_sign == "particle":
                ss_i += 1
                continue
            else:
                rep_word = seg_list[s_i]
                rep_ret = set()
                ids_list = sear_num_new(rep_word)
                if ids_list != None:
                    for id_i in ids_list:
                        one_part = sear_words(id_i)
                        assert one_part != None
                        if len(one_part) > 1:
                            #print rep_word, one_part
                            tmp_part = one_part[:]
                            tmp_part.remove(rep_word)
                            #TODO... the replace operations
                            for tmp_i in range(len(tmp_part)-1, -1, -1):
                                rep_w = tmp_part[tmp_i]
                                new_str = seg_list[:s_i]
                                new_str.append(rep_w)
                                new_str.extend(seg_list[s_i+1:])
                                new_str = ''.join(new_str)
                                c_new_str = new_str
                                for k,v in tycl_rep.iteritems():
                                    c_new_str = c_new_str.replace(k, v)
                                fp_out.write('*** '+c_new_str.encode('UTF-8')+'\t')
                                new_seg = pynlpir.segment(new_str, pos_tagging=True)
                                c_new_seg = pynlpir.segment(c_new_str, pos_tagging=True)
                                #something to do
                                for n_i in range(len(c_new_seg)-1, -1, -1):
                                    c_new_seg[n_i] = list(c_new_seg[n_i])
                                    if c_new_seg[n_i][1] == None:
                                        c_new_seg[n_i][1] = u'None'
                                        if c_new_seg[n_i][0] == u' ':
                                            del c_new_seg[n_i]
                                    else:
                                        c_new_seg[n_i][1] = c_new_seg[n_i][1].replace(' ', '-')
                                c_new_seg_str = ' '.join('/'.join(y) for y in c_new_seg)
                                fp_out.write(c_new_seg_str.encode('UTF-8')+'\n')
                                #something done
                                is_continue = False
                                if len(c_new_seg) == len(c_seg_line):
                                    if c_new_seg[ss_i][1] == c_seg_line[ss_i][1]: #only compare the pos of the word
                                        pass
                                    else:
                                        tmp_part.remove(rep_w)
                                        continue
                                    for front_i in range(ss_i-1, -1, -1):
                                        if c_new_seg[front_i][0] == c_seg_line[front_i][0] and c_new_seg[front_i][1] == c_seg_line[front_i][1]:
                                            pass
                                        else:
                                            tmp_part.remove(rep_w)
                                            is_continue = True
                                            break
                                    if is_continue == True:
                                        continue
                                    for back_i in range(ss_i+1, len(c_seg_line)):
                                        if c_new_seg[back_i][0] == c_seg_line[back_i][0] and c_new_seg[back_i][1] == c_seg_line[back_i][1]:
                                            pass
                                        else:
                                            tmp_part.remove(rep_w)
                                            break
                                else:
                                    tmp_part.remove(rep_w)
                                    continue
                            #DONE
                            for ti in tmp_part:
                                rep_ret.add(ti)
                fp_out.write('>>>>>> %s\n' % rep_word.encode('UTF-8'))
                #for ret_i in rep_ret:
                fp_out.write(' '.join(y.encode('UTF-8') for y in rep_ret))
                fp_out.write('\n')
                ss_i += 1
        '''
        for i in range(1, len(pat_line)):
            fp_out.write(' '+pat_line[i].encode('UTF-8'))
        '''
    fp.close()
    fp_out.close()
    pynlpir.close()

if __name__ == "__main__":
    start = time.clock()
    cilin_opath = u'./tongyicilin/哈工大同义词林扩展版.txt'
    cilin_path = u'./tongyicilin/哈工大同义词林扩展版_new_struct.txt'
    n_stat_path = u'./tongyicilin/n_stat.txt'
    load_tongyicilin_new(cilin_opath, cilin_path, n_stat_path)
    pat_name = u'实体之间关系规则.pat'
    ###orginal_rela_pat(pat_name)
    tycl_replace(pat_name)
    restore_rela_pat(pat_name)
    end = time.clock()
    print "Time to run: %f" % (end-start)
