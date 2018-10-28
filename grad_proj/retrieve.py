# -*- coding: utf-8 -*-
import os
import sys
import time
import pynlpir
import re

from tag import *
import ner_drive
import relat_drive

#decode: str ==> unicode
#encode: unicode ==> str
encode_type = sys.getfilesystemencoding()

pt_list = []
tag_data_dict = {}

def load_tag_data():
    global tag_data_dict
    fp = file('./tag/after_tag/L_init.xxx', 'rb')
    part_no = 3
    count = 0
    part_lines = []
    for line in fp:
        part_lines.append(line.strip())
        count += 1
        if count == part_no:
            seg_str = part_lines[0].decode('UTF-8')
            relat_list = (part_lines[1].decode('UTF-8')).split()
            file_info = part_lines[2].decode('UTF-8')
            for r_i in range(len(relat_list)):
                relat_parts = relat_list[r_i].split(',')
                relat_type = relat_parts[1]
                left_ne = relat_parts[0][1:]
                right_ne = relat_parts[2][:-1]
                if relat_type not in tag_data_dict.keys():
                    tag_data_dict[relat_type] = set()
                tag_data_dict[relat_type].add((left_ne, right_ne, seg_str, file_info))
            count = 0
            part_lines = []
    fp.close()

def load_problem_templat():
    global pt_list
    fp = file('./qt_gui/problem_templates.pt', 'rb')
    for line in fp:
        line = (line.strip()).decode('UTF-8')
        new_pt = line.split()
        new_pt[2] = int(new_pt[2])
        pt_list.append(new_pt)
    fp.close()

def get_pt_match_str(seg_sent):
    seg_list = relat_drive.get_seg_line(seg_sent)
    ret_str = u''
    ne_flags = []
    for seg_i in range(len(seg_list)):
        seg_part = seg_list[seg_i]
        word = seg_part[0]
        flag = seg_part[1]
        if '<DIS>' in flag or '<MED>' in flag or '<SYM>' in flag or '<TRE>' in flag:
            ret_str += flag
            ne_flags.append(word)
        else:
            ret_str += word
    return (ret_str, ne_flags)

def get_raw_str(seg_sent):
    seg_list = relat_drive.get_seg_line(seg_sent)
    ret_str = u''
    ne_flags = dict()
    for seg_i in range(len(seg_list)):
        seg_part = seg_list[seg_i]
        word = seg_part[0]
        flag = seg_part[1]
        if '<DIS>' in flag or '<MED>' in flag or '<SYM>' in flag or '<TRE>' in flag:
            if '<SYM><DIS>' == flag: #there should be wrong
                flag = '<DIS>'
            if flag not in ne_flags.keys():
                ne_flags[flag] = set()
            ne_flags[flag].add(word)
        ret_str += word
    return (ret_str, ne_flags)

def match_pt(seg_sent):
    global pt_list
    global tag_data_dict
    match_str, ne_flags = get_pt_match_str(seg_sent)
    match_sent = set()
    if len(ne_flags) == 1:
        assert len(ne_flags) == 1
        ne_flag = ne_flags[0]
        for pt_i in pt_list:
            pt_rule = pt_i[0]
            pt_type = pt_i[1]
            pt_target = pt_i[2]
            if pt_target == 0:
                pt_target = 1
            elif pt_target == 1:
                pt_target = 0
            match_ret = re.findall(pt_rule, match_str)
            if len(match_ret) > 0:
                type_list = tag_data_dict[pt_type]
                for type_i in type_list:
                    if type_i[pt_target] == ne_flag:
                        match_sent.add((type_i[2], type_i[3]))
    return match_sent

def comb2dict(from_dict, to_dict):
    for k, v in from_dict.iteritems():
        if k not in to_dict.keys():
            to_dict[k] = v
        else:
            for v_i in v:
                if v_i not in to_dict[k]:
                    to_dict[k].add(v_i)
    return to_dict

def ner_one_file_count(text):
    count_dict = dict()
    fp = file(os.path.join('./tag/after_tag', text+'_ner'), 'rb')
    for line in fp:
        line = (line.strip()).decode('UTF-8')
        ret_str, ne_flags = get_raw_str(line)
        count_dict = comb2dict(ne_flags, count_dict)
    fp.close()
    return count_dict

def ner_file_count():
    texts_list = os.listdir('./tag/medical_texts')
    for text in texts_list:
        count_dict = ner_one_file_count(text)
        fp_out = file(os.path.join('./tag/after_tag', text[:-4]+'.xxx'), 'wb')
        for k, v in cout_dict:
            fp_out.write(k.encode('UTF-8'))
            for v_i in v:
                fp_out.write((' '+v_i).encode('UTF-8'))
            fp_out.write('\n')
        fp_out.close()

def drive_start():
    ner_drive.drive_start(tag_flag = False)
    load_problem_templat()
    load_tag_data()

def retrieve_drive():
    ner_drive.drive_end()

if __name__ == "__main__":
    start = time.clock()
    drive_start()
    sent = u'轻中度高血压应该使用什么药物治疗？'
    seg_sent = ner_drive.analysis(sent)
    match_ret = match_pt(seg_sent)
    drive_end()
    end = time.clock()
    print "Time to run: %f" % (end-start)
