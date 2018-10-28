# -*- coding: utf-8 -*-
import os
import sys
import time
import pynlpir
from svmutil import *

if not "../tag/" in sys.path: #set the path first
    sys.path.append("../tag/")
from bootstrapping_init import *

category_set = set()
feature_alphabet = dict()
feature_index = 1
label_alphabet = dict()

def train_default():
    y, x = svm_read_problem('./input_data/heart_scale')
    m = svm_train(y[:200], x[:200], '-c 4')
    p_label, p_acc, p_val = svm_predict(y[200:], x[200:], m)

def train_drive(train_file, predict_file):
    global category_set
    global label_alphabet
    for category_i in category_set:
        y_tr, x_tr = svm_read_problem(train_file[:-4]+'_'+category_i+'.xxx')
        m = svm_train(y_tr, x_tr)
        y_ts, x_ts = svm_read_problem(predict_file[:-4]+'_'+category_i+'.xxx')
        p_label, p_acc, p_val = svm_predict(y_ts, x_ts, m)
        fp_ret = file(predict_file[:-4]+'_ret_'+category_i+'.xxx', 'wb')
        for label_i in p_label:
            fp_ret.write('%d %s\n' % (int(label_i), label_alphabet[category_i][2][int(label_i)]))
        fp_ret.close()

def get_all_pair(o_train_file, train_file):
    global category_set
    fp_in = file(o_train_file, 'rb')
    part_no = 5
    count = 0
    part_lines = []
    for line in fp_in:
        part_lines.append(line.strip())
        count += 1
        if count == part_no:
            count = 0
            relat_list = ((part_lines[4].decode('UTF-8')).split())[1:]
            for new_relat in relat_list:
                relat_type = re.findall(ur',.*,', new_relat)
                assert len(relat_type) == 1
                relat_type = relat_type[0][1:8]
                category_set.add(relat_type)
            part_lines = []
    fp_in.close()
    for category_i in category_set:
        f_i = file(train_file[:-4]+'_'+category_i+'.xxx', 'wb')
        f_i.truncate()
        f_i.close()

def ne_type_map(tag_string):
    ret_list = dict()
    ret_list['MED'] = set()
    ret_list['DIS'] = set()
    ret_list['SYM'] = set()
    ret_list['TRE'] = set()
    flag_list = re.findall(r'\[\[.{0,5}<[A-Z]{3}>\d+\]\]', tag_string)
    for flag_i in flag_list:
        flag_str = re.findall(r'<.*>', flag_i)
        flag_no = re.findall(r'\d+', flag_i)
        assert len(flag_str) == 1 and len(flag_no) == 1
        flag_str = flag_str[0]
        flag_no = int(flag_no[0])
        if flag_str == '<MED>':
            ret_list['MED'].add(flag_no)
        elif flag_str == '<TRE>':
            ret_list['TRE'].add(flag_no)
        elif flag_str == '<SYM>':
            ret_list['SYM'].add(flag_no)
        elif flag_str == '<DIS>':
            ret_list['DIS'].add(flag_no)
        elif flag_str == '<SYM><DIS>':
            ret_list['SYM'].add(flag_no)
            ret_list['DIS'].add(flag_no)
        elif flag_str == '<DIS><SYM>':
            ret_list['DIS'].add(flag_no)
            ret_list['SYM'].add(flag_no)
        else: #There is also "<DIS><TRE>"
            sub_flags = re.findall('[A-Z]{3}', flag_str)
            for sub_flag in sub_flags:
                ret_list[sub_flag].add(flag_no)
            #assert False
    r_list = dict()
    for k_i, v_i in ret_list.iteritems():
        for k_y, v_y in ret_list.iteritems():
            if k_i != k_y:
                r_key = k_i+'-'+k_y
                if not r_key in r_list.iteritems():
                    r_list[r_key] = []
                for no_i in v_i:
                    for no_y in v_y:
                        if no_i != no_y:
                            r_list[r_key].append((no_i, no_y))
    return r_list

def train_no_pair_index(left_ne_no, right_ne_no, seg_line_long, ne_list):
    left_ne = ne_list[left_ne_no]
    right_ne = ne_list[right_ne_no]
    left_index = -1
    right_index = -1
    for seg_i in range(len(seg_line_long)):
        if seg_line_long[seg_i][0] == left_ne:
            left_index = seg_i
        if seg_line_long[seg_i][0] == right_ne:
            right_index = seg_i
    return (left_index, right_index)

def train_pair_index(new_relat, seg_line_long, ne_list):
    relat_type = re.findall(ur',.*,', new_relat)
    assert len(relat_type) == 1
    relat_type = relat_type[0][1:-1]
    no_pare = re.findall(ur'\d+', new_relat)
    assert len(no_pare) == 2
    left_ne_no = int(no_pare[0])
    right_ne_no = int(no_pare[1])
    left_index, right_index = train_no_pair_index(left_ne_no, right_ne_no, seg_line_long, ne_list)
    return (left_ne_no, right_ne_no, left_index, right_index, relat_type)

def gen_vect_str(relat_type, seg_line_long, left_index, right_index, left_type, right_type, is_train, pre_relat_type=None):
    global feature_alphabet
    global feature_index
    global label_alphabet
    vect_str = u''
    if relat_type != None:
        assert pre_relat_type != None
        if not pre_relat_type in label_alphabet.keys():
            label_alphabet[pre_relat_type] = [0, dict(), dict()]
        if relat_type in label_alphabet[pre_relat_type][1].keys():
            vect_str += str(label_alphabet[pre_relat_type][1][relat_type])+' '
        else:
            new_index = label_alphabet[pre_relat_type][0] + 1
            label_alphabet[pre_relat_type][0] = new_index
            label_alphabet[pre_relat_type][1][relat_type] = new_index
            label_alphabet[pre_relat_type][2][new_index] = relat_type
            vect_str += str(new_index)+' '
    else:
        vect_str += '1 ' #Arbitrary
    ###TODO...
    new_feature = [#"T1=%s" % left_type,
                   #"T2=%s" % right_type,
                   "NE1=%s" % seg_line_long[left_index][0],
                   "NE2=%s" % seg_line_long[right_index][0],
                   ]
    for lr_i in range(left_index+1, right_index):
        word = seg_line_long[lr_i][0]
        tag_flag = seg_line_long[lr_i][1]
        if tag_flag == 'noun':
            new_feature.append("M_N=%s" % word)
        elif tag_flag == 'verb':
            new_feature.append("M_V=%s" % word)
    for lr_i in range(left_index-1, -1, -1):
        word = seg_line_long[lr_i][0]
        tag_flag = seg_line_long[lr_i][1]
        if tag_flag == 'noun':
            new_feature.append("S_N=%s" % word)
        elif tag_flag == 'verb':
            new_feature.append("S_V=%s" % word)
        elif tag_flag == 'punctuation-mark':
            break
    for lr_i in range(right_index+1, len(seg_line_long)):
        word = seg_line_long[lr_i][0]
        tag_flag = seg_line_long[lr_i][1]
        if tag_flag == 'noun':
            new_feature.append("E_N=%s" % word)
        elif tag_flag == 'verb':
            new_feature.append("E_V=%s" % word)
        elif tag_flag == 'punctuation-mark':
            break
    if is_train == True:
        for feature_part in new_feature:
            if feature_part in feature_alphabet.keys():
                vect_str += str(feature_alphabet[feature_part])+':1'+' '
            else:
                vect_str += str(feature_index)+':1'+' '
                feature_alphabet[feature_part] = feature_index
                feature_index += 1
        vect_str += '\n'
    else:
        for feature_part in new_feature:
            if feature_part in feature_alphabet.keys():
                vect_str += str(feature_alphabet[feature_part])+':1'+' '
            else:
                pass
        vect_str += '\n'
    ###DONE...
    return vect_str

def output_label_feature():
    global label_alphabet
    global feature_alphabet
    fp_label = file('./input_data/lables.xxx', 'wb')
    fp_feature = file('./input_data/features.xxx', 'wb')
    ###TODO...
    for k,v in label_alphabet.iteritems():
        fp_label.write(k)
        for k_i,v_i in v[1].iteritems():
            fp_label.write(' %s:%d' % (k_i, v_i))
        fp_label.write('\n')
    for k,v in feature_alphabet.iteritems():
        fp_feature.write('%s %d\n' % (k.encode('UTF-8'), v))
    ###DONE...
    fp_label.close()
    fp_feature.close()

def get_train_vect(o_train_file, train_file):
    global category_set
    fp_in = file(o_train_file, 'rb')
    fp_out = file(train_file, 'wb')
    part_no = 5
    count = 0
    part_lines = []
    for line in fp_in:
        part_lines.append(line.strip())
        count += 1
        if count == part_no:
            count = 0
            ###TODO...
            raw_string_long = part_lines[1].decode('UTF-8')
            tag_string_long = part_lines[2].decode('UTF-8')
            ne_list = ((part_lines[3].decode('UTF-8')).split())[1:]
            relat_list = ((part_lines[4].decode('UTF-8')).split())[1:]
            seg_line_long = deal_cws(raw_string_long)
            ne_pos_list_long = cal_ne_pos(tag_string_long, ne_list)
            seg_line_long = comb_ne_cws(seg_line_long, ne_pos_list_long)
            for seg_part in seg_line_long:
                #fp_out.write(' '+(seg_part[0]).encode('UTF-8'))
                #if (seg_part[1][0] == '<' and seg_part[1][-1] == '>') or seg_part[1] == 'STOP':
                #    fp_out.write('/'+(seg_part[1]).encode('UTF-8'))
                fp_out.write(' '+(seg_part[0]).encode('UTF-8'))
                #fp_out.write(' '+(seg_part[0]).encode('UTF-8')+'/'+(seg_part[1]).encode('UTF-8')+\
                #            '/'+str(seg_part[2][0])+'-'+str(seg_part[2][1]))
            fp_out.write('\n')
            ne_map = ne_type_map(tag_string_long)
            for relat_i in relat_list:
                left_no, right_no, left_index, right_index, relat_type = train_pair_index(relat_i, seg_line_long, ne_list)
                pre_relat_type = relat_type[:7]
                assert (left_no, right_no) in ne_map[pre_relat_type]
                ne_map[pre_relat_type].remove((left_no, right_no))
                left_type = relat_type[0:3]
                right_type = relat_type[4:7]
                if left_index != -1 and right_index != -1: #the problem come from the word segment
                    fp_out_i = file(train_file[:-4]+'_'+pre_relat_type+'.xxx', 'ab')
                    vect_str = gen_vect_str(relat_type, seg_line_long, left_index, right_index, left_type, right_type, True, pre_relat_type)
                    fp_out_i.write(vect_str.encode('UTF-8'))
                    fp_out_i.close()
            for category_i in category_set:
                left_type = category_i[0:3]
                right_type = category_i[4:7]
                fp_out_i = file(train_file[:-4]+'_'+category_i+'.xxx', 'ab')
                for type_no_pair in ne_map[category_i]:
                    left_ne_no = type_no_pair[0]
                    right_ne_no = type_no_pair[1]
                    left_index, right_index = train_no_pair_index(left_ne_no, right_ne_no, seg_line_long, ne_list)
                    if left_index != -1 and right_index != -1: #the problem come from the word segment
                        vect_str = gen_vect_str('xxx-xxx', seg_line_long, left_index, right_index, left_type, right_type, True, category_i)
                        fp_out_i.write(vect_str.encode('UTF-8'))
                fp_out_i.close()
            ###DONE...
            part_lines = []
    fp_in.close()
    fp_out.close()
    output_label_feature()

def init_predict_file(predict_file):
    global category_set
    for category_i in category_set:
        fp_out_i = file(predict_file[:-4]+'_'+category_i+'.xxx', 'wb')
        fp_out_i.truncate()
        fp_out_i.close()

def get_predict_vect(o_predict_file, predict_file):
    global category_set
    fp_in = file(o_predict_file, 'rb')
    fp_out = file(predict_file, 'wb')
    part_no = 4
    count = 0
    part_lines = []
    init_predict_file(predict_file)
    for line in fp_in:
        part_lines.append(line.strip())
        count += 1
        if count == part_no:
            count = 0
            ###TODO...
            raw_string_long = part_lines[1].decode('UTF-8')
            tag_string_long = part_lines[2].decode('UTF-8')
            ne_list = ((part_lines[3].decode('UTF-8')).split())[1:]
            seg_line_long = deal_cws(raw_string_long)
            ne_pos_list_long = cal_ne_pos(tag_string_long, ne_list)
            seg_line_long = comb_ne_cws(seg_line_long, ne_pos_list_long)
            for seg_part in seg_line_long:
                #fp_out.write(' '+(seg_part[0]).encode('UTF-8'))
                #if (seg_part[1][0] == '<' and seg_part[1][-1] == '>') or seg_part[1] == 'STOP':
                #    fp_out.write('/'+(seg_part[1]).encode('UTF-8'))
                fp_out.write(' '+(seg_part[0]).encode('UTF-8'))
                #fp_out.write(' '+(seg_part[0]).encode('UTF-8')+'/'+(seg_part[1]).encode('UTF-8')+\
                #            '/'+str(seg_part[2][0])+'-'+str(seg_part[2][1]))
            fp_out.write('\n')
            ne_map = ne_type_map(tag_string_long)
            for category_i in category_set:
                left_type = category_i[0:3]
                right_type = category_i[4:7]
                fp_out_i = file(predict_file[:-4]+'_'+category_i+'.xxx', 'ab')
                for type_no_pair in ne_map[category_i]:
                    left_ne_no = type_no_pair[0]
                    right_ne_no = type_no_pair[1]
                    left_index, right_index = train_no_pair_index(left_ne_no, right_ne_no, seg_line_long, ne_list)
                    if left_index != -1 and right_index != -1: #the problem come from the word segment
                        vect_str = gen_vect_str(None, seg_line_long, left_index, right_index, left_type, right_type, False)
                        fp_out_i.write(vect_str.encode('UTF-8'))
                fp_out_i.close()
            ###DONE...
            part_lines = []
    fp_in.close()
    fp_out.close()

if __name__ == "__main__":
    start = time.clock()
    ###train_default()
    pynlpir.open()
    ###TODO...
    print 'Sentence to vector...'
    o_train_file = './raw_data/L_test.xxx'
    train_file = './input_data/L_test.xxx'
    o_predict_file = './raw_data/U_test.xxx'
    predict_file = './input_data/U_test.xxx'
    get_all_pair(o_train_file, train_file)
    get_train_vect(o_train_file, train_file)
    get_predict_vect(o_predict_file, predict_file)
    ###DONE...
    print 'Training and predicting...'
    train_drive(train_file, predict_file)
    pynlpir.close()
    end = time.clock()
    print 'Time to run: %f' % (end-start)
