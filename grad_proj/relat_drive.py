# -*- coding: utf-8 -*-
import os
import sys
import time
import pynlpir
import re

from tag import *
import ner_drive

#decode: str ==> unicode
#encode: unicode ==> str
encode_type = sys.getfilesystemencoding()

def rp2re(pat_name):
    rep_dict = {u"<MED>":u"<MED>@\d+", u"<DIS>":u"<DIS>@\d+", \
                u"<SYM>":u"<SYM>@\d+", u"<TRE>":u"<TRE>@\d+"}
    in_base='./tag/manual_pattern/'
    out_base='./tag/extend_pattern'
    fp = file(os.path.join(in_base, pat_name), 'rb')
    fp_out = file(os.path.join(out_base, pat_name), 'wb')
    for pat_line in fp:
        pat_line = (pat_line.strip()).decode('UTF-8')
        pat_line = pat_line.split()
        pat = pat_line[0]
        for k, v in rep_dict.iteritems():
            pat = pat.replace(k, v)
        fp_out.write(pat.encode('UTF-8'))
        for i in range(1, len(pat_line)):
            fp_out.write(' '+pat_line[i].encode('UTF-8'))
        fp_out.write('\n')
    fp_out.close()
    fp.close()

def get_seg_line(seg_str):
    seg_list = seg_str.split()
    for seg_i in range(len(seg_list)):
        seg_list[seg_i] = seg_list[seg_i].split('/')
        if seg_list[seg_i][0] == '':
            seg_list[seg_i][0] = u' '
    return seg_list

def gen_match_str(seg_list):
    ret_str = u''
    word_flag_map = dict()
    for seg_i in range(len(seg_list)):
        seg_part = seg_list[seg_i]
        word = seg_part[0]
        flag = seg_part[1]
        if '<DIS>' in flag or '<MED>' in flag or '<SYM>' in flag or '<TRE>' in flag:
            ret_str += flag + '@' + str(seg_i)
            word_flag_map[seg_i] = word
        else:
            ret_str += word
    return (ret_str, word_flag_map)

def gen_oneD_str(seg_list):
    ret_str = u''
    for seg_i in range(len(seg_list)):
        seg_part = seg_list[seg_i]
        ret_str += seg_part[0] + '@' + str(seg_i)
    return ret_str

def init_LU():
    texts_list = os.listdir('./tag/after_tag')
    pat_list = bootstrapping_init.relat_pattern(u'./tag/extend_pattern/实体之间关系规则.pat')
    fp_L = file('./tag/after_tag/L_init.xxx', 'wb')
    fp_U = file('./tag/after_tag/U_init.xxx', 'wb')
    for text in texts_list:
        if len(re.findall(ur'.*_ner$', text)) > 0:
            pass
        else:
            continue
        fp = file(os.path.join('./tag/after_tag', text), 'rb')
        fp_out = file(os.path.join('./tag/after_tag', text+'_rrule'), 'wb')
        for line in fp:
            line = (line.strip()).decode('UTF-8')
            seg_line = get_seg_line(line)
            flag_str, word_flag_map = gen_match_str(seg_line)
            relat_list = set()
            for pat_i in range(len(pat_list)):
                pat_str = pat_list[pat_i][0]
                relat_type = pat_list[pat_i][1]
                pattern = re.compile(pat_str)
                ret_list = pattern.findall(flag_str)
                if len(ret_list) > 0:
                    ret_list = list(set(ret_list))
                    for ret in ret_list:
                        ne_str_list = re.findall(ur'<[A-Z]{3}>.{0,5}@\d+', ret)
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
                            relat_list.add('<'+word_flag_map[left_ne]+','+relat_type+','+word_flag_map[right_ne]+'>')
            fp_out.write(line.encode('UTF-8')+'\n')
            fp_out.write(' '.join(y.encode('UTF-8') for y in relat_list)+'\n')
            if len(relat_list) > 0:
                fp_L.write((line+'\n').encode('UTF-8'))
                fp_L.write(' '.join(y.encode('UTF-8') for y in relat_list)+'\n')
                fp_L.write((u'《'+text.decode(encode_type)+u'》\n').encode('UTF-8'))
            else:
                fp_U.write((line+'\n').encode('UTF-8'))
                fp_U.write((u'《'+text.decode(encode_type)+u'》\n').encode('UTF-8'))
        fp.close()
        fp_out.close()
    fp_L.close()
    fp_U.close()

def short_sent(seg_str, relat_list):
    short_ret = []
    seg_list = get_seg_line(seg_str)
    oneD_str = gen_oneD_str(seg_list)
    for r_i in range(len(relat_list)):
        relat_parts = relat_list[r_i].split(',')
        relat_type = relat_parts[1]
        left_ne = relat_parts[0][1:]
        right_ne = relat_parts[2][:-1]
        short_tag = re.findall(ur'[^'+bootstrapping_init.seg_sym+']*'+left_ne+'@\d+'+\
                    ur'[^'+bootstrapping_init.seg_sym+']*'+right_ne+'@\d+'+ur'[^'+bootstrapping_init.seg_sym+']*',\
                    oneD_str
        )
        mid_flag = len(short_tag)
        short_tag.extend(
                    re.findall(ur'[^'+bootstrapping_init.seg_sym+']*'+right_ne+'@\d+'+\
                    ur'[^'+bootstrapping_init.seg_sym+']*'+left_ne+'@\d+'+ur'[^'+bootstrapping_init.seg_sym+']*',\
                    oneD_str)
        )
        for short_i in short_tag:
            if short_i[0] == '@':
                for ch_i in range(1, len(short_i)):
                    char = short_i[ch_i]
                    if char >= '0' and char <= '9':
                        continue
                    else:
                        short_i = short_i[ch_i:]
                        break
            head_tail = re.findall(r'\d+', short_i)
            assert len(head_tail) > 2
            head_no = int(head_tail[0])
            tail_no = int(head_tail[-1])
            new_short = []
            new_short.append(seg_list[head_no:tail_no+1])
            new_short.append(relat_list[r_i])
            short_ret.append(new_short)
    return short_ret

def generalize_sample(seg_list):
    for seg_i in range(len(seg_list)):
        word = seg_list[seg_i][0]
        pos = seg_list[seg_i][1]
        ret = bootstrapping_init.sear_interface(bootstrapping_init.stop_table, len(bootstrapping_init.stop_table), word)
        if ret == True:
            seg_list[seg_i][1] = 'STOP'
        elif ret == None:
            if seg_list[seg_i][1] in bootstrapping_init.stop_pos:
                seg_list[seg_i][1] = 'STOP'
            elif seg_list[seg_i][1] == 'None':
                seg_list[seg_i][1] = 'STOP'
            else:
                if pos == 'noun' or pos == 'verb': #Maybe add something
                    num_ret = extend_pattern.sear_num_new(word)
                    if num_ret != None:
                        if len(num_ret) > 1:
                            cilin_str = '|'.join(num_ret)
                            #cilin_str = '(?:'+cilin_str+')'
                        else:
                            cilin_str = num_ret[0]
                        seg_list[seg_i][1] = cilin_str
        else:
            assert False
    return seg_list

def get_sample(tag_file='./tag/after_tag/L_init.xxx'):
    fp = file(tag_file, 'rb')
    fp_rg = file('./tag/after_tag/rule_generalize.xxx', 'wb')
    part_no = 3
    count = 0
    part_lines = []
    for line in fp:
        part_lines.append(line.strip())
        count += 1
        if count == part_no:
            seg_str = part_lines[0].decode('UTF-8')
            relat_list = (part_lines[1].decode('UTF-8')).split()
            file_info = part_lines[2]
            short_ret = short_sent(seg_str, relat_list)
            fp_rg.write((seg_str+'\n').encode('UTF-8'))
            for short_i in short_ret:
                short_sample = ner_drive.fetch_ner_result(short_i[0])
                short_gene = generalize_sample(short_i[0])
                short_gene_str = ner_drive.fetch_ner_result(short_gene)
                fp_rg.write(('>>> '+short_sample+'\n').encode('UTF-8'))
                fp_rg.write(('>>> '+short_gene_str+'\n').encode('UTF-8'))
                fp_rg.write(('>>> '+short_i[1]+'\n').encode('UTF-8'))
            count = 0
            part_lines = []
    fp.close()
    fp_rg.close()

def drive_start():
    pat_name = u'实体之间关系规则.pat'
    rp2re(pat_name)
    bootstrapping_init.read_stop_table(u'./tag/dictionary/停用表.dic')
    cilin_opath = u'./tag/tongyicilin/哈工大同义词林扩展版.txt'
    cilin_path = u'./tag/tongyicilin/哈工大同义词林扩展版_new_struct.txt'
    n_stat_path = u'./tag/tongyicilin/n_stat.txt'
    extend_pattern.load_tongyicilin_new(cilin_opath, cilin_path, n_stat_path)
    init_LU()
    get_sample()

if __name__ == "__main__":
    start = time.clock()
    drive_start()
    end = time.clock()
    print "Time to run: %f" % (end-start)
