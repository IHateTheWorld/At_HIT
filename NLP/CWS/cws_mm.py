from __future__ import print_function
import gzip
import sys
from collections import Counter
from math import log
import time

#decode : str ==> unicode
#encode : unicode ==> str
encode_type = sys.getfilesystemencoding()

word_dict = set()
M = 6 #the max-length of the Chinese-Word

def read_dataset(filename='cip-data.train.gz', wf_name='train.txt', flag = False):
    '''
    flag == True  : read chars and words
    flag == False : only read chars
    '''
    try:
        fp=gzip.open(filename, 'r')
        #wfp = open(wf_name, 'w')
    except:
        print("Failed to open file.", file=sys.stderr)
        return
    
    dataset = []
    for line in fp:
        line = line.decode('utf-8').strip()
        tokens = line.split()
        #print(' '.join(t for t in tokens), file=wfp)   
        #print(' '.join(str(len(t.decode('utf-8'))) for t in tokens), file=wfp)
        
        if flag:
            dataset.append([])
            dataset[-1].append('')
            dataset[-1].append([])
            for t in tokens:
                dataset[-1][0] += t
                dataset[-1][1].append(t)
        else:
            dataset.append(line)
        
    #wfp.close()
    
    return dataset

def get_dictionary(train_dataset):
    for sent, seg in train_dataset:
        for word in seg:
            word_dict.add(word)
    print('Count of the word: %d' % len(word_dict))

def calculate_2sent(n_correct, sent1, sent2):
    trans1, trans2 = set(), set()
    start, end = 0, 0
    for s in sent1:
        end += len(s)
        trans1.add(tuple((start, end)))
        start = end
    start, end = 0, 0
    for s in sent2:
        end += len(s)
        trans2.add(tuple((start, end)))
        start = end
    n_correct += len(trans1 & trans2)
    
    return n_correct

def mm_devel(devel_dataset):
    w_correct, w_seg, w_total = 0, 0, 0
    s_correct, s_total = 0, len(devel_dataset)
    for sent, seg in devel_dataset:
        ret = seg_func(sent)
        if ret == seg:
            s_correct += 1
        w_correct = calculate_2sent(w_correct, ret, seg)
        w_seg += len(ret)
        w_total += len(seg)
    P = 1.0*w_correct/w_seg
    R = 1.0*w_correct/w_total
    F = 2*R*P/(P+R)
    print('Accurate(sentance)   : %d/%d    %f%%' % (s_correct, s_total, (1.0 * s_correct / s_total)))
    print('P  (Correct/SegTotal): %d/%d    %f%%' % (w_correct, w_seg, 100*P))
    print('R  (Correct/Total)   : %d/%d    %f%%' % (w_correct, w_total, 100*R))
    print('F                    : %f%%' % (100*F))

def mm_test(test_dataset, filename):
    wfp = open(filename, 'w')
    for sent in test_dataset:
        seg = seg_func(sent)
        print("\t".join(y.encode(encode_type) for y in seg), file=wfp)
    wfp.close()

def seg_func(sent):
    #left to right
    lr= []
    base = 0
    lr_out, lr_single = 0, 0
    while base < len(sent):
        if base + M > len(sent):
            extent = len(sent) - base
        else:
            extent = M
        for w_len in range(extent, 0, -1):
            seg_word = sent[base:base+w_len]
            if seg_word in word_dict:
                break
            else:
                lr_out += 1
        lr.append(seg_word)
        base += w_len
        if 1 == w_len:
            lr_single += 1
    #right to left
    rl = []
    base = len(sent)
    rl_out, rl_single = 0, 0
    while base > 0:
        if base - M < 0:
            extent = base
        else:
            extent = M
        for w_len in range(extent, 0, -1):
            seg_word = sent[base-w_len:base]
            if seg_word in word_dict:
                break
            else:
                rl_out += 1
        rl.insert(0, seg_word)
        base -= w_len
        if 1 == w_len:
            rl_single += 1
    '''
    #judge the whether lr or rl
    #compare the counter of words in words-seg
    if len(lr) < len(rl):
        return lr
    if len(rl) < len(lr):
        return rl
    #compare the counter of single-word
    if lr_single < rl_single:
        return lr
    if rl_single < lr_single:
        return rl
    #compare the counter of the words out of dictionary
    if lr_out < rl_out:
        return lr
    if rl_out < lr_out:
        return rl
    '''
    return lr

if __name__ == "__main__":
    train_dataset = read_dataset('cip-data.train.gz', 'train.txt', True)
    #output_postag(train_dataset, 'tain_postag.txt')
    #judge1_dataset = read_dataset('judge.data.1.gz', 'judge1.txt', False)
    ans1_dataset = read_dataset('judge.data.ans.1.gz', 'ans1.txt', True)
    #output_postag(ans1_dataset, 'ans1_postag.txt')
    #judge2_dataset = read_dataset('judge.data.2.gz', 'judge2.txt', False)
    ans2_dataset = read_dataset('judge.data.ans.2.gz', 'ans2.txt', True)
    #output_postag(ans2_dataset, 'ans2_postag.txt')
    final1_dataset = read_dataset('final-test.1.txt.gz', 'final1.txt', False)
    final2_dataset = read_dataset('final-test.2.txt.gz', 'final2.txt', False)
    a1_dataset = read_dataset('judge.data.1.gz', 'a1.txt', False)
    a2_dataset = read_dataset('judge.data.2.gz', 'a2.txt', False)
    
    get_dictionary(train_dataset)
    mm_devel(ans1_dataset)
    mm_devel(ans2_dataset)
    mm_test(final1_dataset, 'final1_result.txt')
    mm_test(final2_dataset, 'final2_result.txt')
    mm_test(a1_dataset, 'a1.txt')
    mm_test(a2_dataset, 'a2.txt')
