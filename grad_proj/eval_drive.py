# -*- coding: utf-8 -*-
import ner_drive
import os
import sys
import time
import pynlpir
import re

n_recall, n_gold, n_pred = 0, 0, 0

def cal_pos(in_str):
    cur_pos = 0
    is_i = 0
    ne_length = 0
    ne_type = ''
    ne_pos_list = dict()
    while is_i < len(in_str):
        char = in_str[is_i]
        if char == '[':
            if is_i < (len(in_str) - 1):
                is_i += 1
                char = in_str[is_i]
                if char == '[':
                    is_i += 1
                    char = in_str[is_i]
                    while char != '<':
                        ne_length += 1
                        is_i += 1
                        char = in_str[is_i]
                    ne_type += char
                    while char != ']':
                        ne_type += char
                        is_i += 1
                        char = in_str[is_i]
                    is_i += 1
                    char = in_str[is_i]
                    assert char == ']'
                    ne_pos_pare = []
                    cur_pos += ne_length
                    new_pare = (cur_pos, cur_pos+ne_length)
                    assert new_pare not in ne_pos_list.keys()
                    ne_pos_list[new_pare] = ne_type
                    cur_pos -= 1
                    ne_type = ''
                    ne_length = 0
                else:
                    is_i -= 1
        is_i += 1
        cur_pos += 1
    return ne_pos_list

def retrive_raw_line(pred_line):
    raw_line = pred_line
    ne_full_pos = re.findall(r'\[\[[^\[\]]*\]\]', pred_line)
    ne_full_pos = list(set(ne_full_pos))
    for full_pos in ne_full_pos:
        raw_ne = full_pos[2:-2]
        raw_ne = re.sub(r'<.*>', '', raw_ne, count=0, flags=0)
        raw_line = raw_line.replace(full_pos, raw_ne)
    return raw_line

def retrive_gold_line(pred_line):
    raw_line = retrive_raw_line(pred_line)
    gold_line = ner_drive.analysis(raw_line, True)
    return gold_line

def eval_sent(pred_line):
    global n_recall, n_gold, n_pred
    gold_line = retrive_gold_line(pred_line)
    gold_pos_list = cal_pos(gold_line)
    pred_pos_list = cal_pos(pred_line)
    for k, v in gold_pos_list.iteritems():
        if k in pred_pos_list.keys():
            if pred_pos_list[k] in gold_pos_list[k]:
                n_recall += 1
    n_gold += len(gold_pos_list)
    n_pred += len(pred_pos_list)

def eval_ret():
    global n_recall, n_gold, n_pred
    p = float(n_recall)/n_gold
    r = float(n_recall)/n_pred
    f = 2 * p * r / (p + r)
    return (p, r, f)

def eval_one_time():
    global n_gold, n_pred, n_recall
    n_gold = 0
    n_pred = 0
    n_recall = 0
    text_list = os.listdir('./test_data/')
    for text in text_list:
        if len(re.findall(r'.*_tag$', text)) > 0:
            pass
        else:
            continue
        fp = file(os.path.join('./test_data', text), 'rb')
        for line in fp:
            pred_line = (line.strip()).decode('UTF-8')
            eval_sent(pred_line)
        fp.close()
    p, r, f = eval_ret()
    return (p, r, f, n_recall, n_gold, n_pred)

def drive_start():
    ner_drive.drive_start(tag_flag = False)
    eval_one_time()
    ner_drive.drive_end()

if __name__ == "__main__":
    drive_start()
