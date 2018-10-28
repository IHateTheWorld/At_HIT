# -*- coding: utf-8 -*-

import math
import time

para_list = [0.1, 0.65, 0.8, 0.9, 0.96, 0.5]
cilin = []
stat_list = []
n_stat = dict()
seg_index = (1, 2, 4, 5)
word_count = 0
stat_count = 0

def new_struct(cilin_path):
    words_dict = {}
    fp = file(cilin_path, 'rb')
    for line in fp:
        line = (line.strip()).decode('UTF-8')
        line = line.split()
        line_0 = line[0]
        for i in range(1, len(line)):
            if line[i] not in words_dict.keys():
                words_dict[line[i]] = []
            words_dict[line[i]].append(line_0)
    fp.close()
    words_list = sorted(words_dict.items(), key=lambda d:d[0])
    fp_out = file(u'哈工大同义词林扩展版_new_struct.txt', 'wb')
    for word, id_list in words_list:
        fp_out.write('%s' % word.encode('UTF-8'))
        for id in id_list:
            fp_out.write(' %s' % id.encode('UTF-8'))
        fp_out.write('\n')
    fp_out.close()

def load_tongyicilin(cilin_path):
    global cilin
    global seg_index
    fp = file(cilin_path, 'rb')
    for line in fp:
        line = (line.strip()).decode('UTF-8')
        line = line.split()
        line_0 = line[0]
        l_list = [ord(line_0[1:2])-ord('a')+1, int(line_0[2:4]), ord(line_0[4:5])-ord('A')+1, int(line_0[5:-1])]
        for i in range(4):
            key = line_0[:seg_index[i]]
            value = l_list[i]
            if key not in n_stat.keys():
                n_stat[key] = value
            else:
                if n_stat[key] < value:
                    n_stat[key] = value
        cilin.append(line)
    n_stat_list = sorted(n_stat.items(), key=lambda d:d[0])
    '''
    fp_out = file('n_stat.txt', 'wb')
    for nsl in n_stat_list:
        fp_out.write('%s %s\n' % (nsl[0],str(nsl[1])))
    fp_out.close()
    '''
    fp.close()

def load_tongyicilin_new(cilin_path, n_stat_path):
    global cilin, stat_list
    global word_count, stat_count
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

def sear_num(word):
    global cilin
    ret_list = []
    for i in range(len(cilin)):
        l_list = cilin[i]
        for j in range(1, len(l_list)):
            if l_list[j] == word:
                ret_list.append(l_list[0])
                break
    return ret_list

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
                new_low -= 1
            new_low += 1
            while new_high <= high and w_i < len(ToD_list[new_high][0]) and ToD_list[new_high][0][w_i] == target:
                if w_i == (len(ToD_list[new_high][0]) - 1):
                    loc = new_high
                new_high += 1
            new_high -= 1
            low = new_low
            high = new_high
        else:
            return None
    return ToD_list[loc][1]

def sear_num_new(word):
    global cilin
    global word_count
    return sear_interface(cilin, word_count, word)

def sear_stat(id):
    global stat_list
    global stat_count
    return sear_interface(stat_list, stat_count, id)

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

if __name__ == "__main__":
    start = time.clock()
    cilin_path = u'哈工大同义词林扩展版.txt_mini'
    new_struct(cilin_path) #that function is too slow
    ###load_tongyicilin(cilin_path)
    # cilin_path = u'哈工大同义词林扩展版_new_struct.txt'
    # n_stat_path = u'n_stat.txt'
    # load_tongyicilin_new(cilin_path, n_stat_path)
    # w_a = u'人民'
    # w_b = u'成年人'
    # ret = sim(w_a, w_b)
    # print 'ret: %f' % ret
    # end = time.clock()
    # print 'Time to run: %f' % (end-start)
