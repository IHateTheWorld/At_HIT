from __future__ import print_function
import gzip
import sys
from collections import Counter
from math import log
import time
import json

#decode : str ==> unicode
#encode : unicode ==> str
encode_type = sys.getfilesystemencoding()

def read_dataset(filename='cip-data.train.gz', wf_name='train.txt', flag = False, is_devel=False):
    try:
        fp=gzip.open(filename, 'r')
        #wfp = open(wf_name, 'w')
    except:
        print("Failed to open file.", file=sys.stderr)
        return
    
    dataset = []
    for line in fp:
        tokens = line.strip().split()
        #print(' '.join(t for t in tokens), file=wfp)
        #print(' '.join(str(len(t.decode('utf-8'))) for t in tokens), file=wfp)
        dataset.append([])
        if flag:
            dataset[-1].append([])
            dataset[-1].append([])
            for t in tokens:
                t = t.decode('utf-8')
                if is_devel:
                    dataset[-1][1].append(t)
                for i in range(len(t)):
                    dataset[-1][0].append(t[i])
                    if not is_devel:
                        if 1 == len(t):
                            dataset[-1][1].append('S')
                        elif 0 == i:
                            dataset[-1][1].append('B')
                        elif len(t)-1 == i:
                            dataset[-1][1].append('E')
                        else:
                            dataset[-1][1].append('M')
        else:
            for t in tokens:
                t = t.decode('utf-8')
                for c in t:
                    dataset[-1].append(c)
    
    #wfp.close()
    
    return dataset

class HMM(object):
    def __init__(self, epsilon=1e-5, training_data=None):
        self.epsilon = epsilon
        if training_data is not None:
            self.fit(training_data)
    
    def fit(self, training_data):
        self.unigram = Counter()
        self.bigram = Counter()
        self.cooc = Counter()
        self.wordcount = Counter()
        self.total_words = 0
        self.total_tags = 0
        
        print('building HMM model ...')
        for words, tags in training_data:
            assert len(tags) == len(words)
            self.total_words += len(words)
            #self.total_tags += len(tags) #equal to self.total_words
            self.unigram.update(tags)
            self.bigram.update(zip(tags[:-1], tags[1:]))
            self.cooc.update(zip(words, tags))
            self.wordcount.update(words)
        self.total_tags = self.total_words
        
        print('HMM model is built.')
        self.postags = ['B', 'M', 'E', 'S']
    
    def emit(self, words, i, tag):
        '''
        P(word|tag)
        '''
        lambda_1 = 0.9999999
        lambda_2 = 0.00000005
        lambda_3 = 1 - lambda_1 - lambda_2
        prob_1 = 1.0 * self.cooc[tuple((words[i], tag))] / self.unigram[tag]
        prob_2 = 1.0 * self.wordcount[words[i]] / self.total_words
        prob_3 = 1.0 * self.unigram[tag] / self.total_tags
        prob = log(lambda_1*prob_1 + lambda_2*prob_2 + lambda_2*prob_3)
        return prob
    
    def trans(self, tag, tag1):
        '''
        P(tag1|tag)
        '''
        lambda_1 = 0.9999999
        lambda_2 = 0.00000005
        lambda_3 = 1 - lambda_1 - lambda_2
        prob_1 = 1.0 * self.bigram[tuple((tag, tag1))] / self.unigram[tag]
        prob_2 = 1.0 * self.unigram[tag] / self.total_tags
        prob_3 = 1.0 * self.unigram[tag1] / self.total_tags
        prob = log(lambda_1*prob_1 + lambda_2*prob_2 + lambda_2*prob_3)
        return prob
    
def viterbi(words, hmm):
    N, T = len(words), len(hmm.postags)
    
    score = [[-float('inf') for j in range(T)] for i in range(N)]
    path = [[-1 for j in range(T)] for i in range(N)]
    
    for i, word in enumerate(words):
        if i == 0: #first word
            for j, tag in enumerate(hmm.postags):
                if tag == 'S' or tag == 'B': #not start from M or E
                    score[i][j] = hmm.emit(words, i, tag)
        else:
            for j, tag in enumerate(hmm.postags):
                best, best_t = -1e20, -1
                for k, tag0 in enumerate(hmm.postags):
                    new_score = score[i-1][k]+hmm.trans(tag0, tag)+hmm.emit(words, i, tag)
                    if(best < new_score):
                        best = new_score
                        best_t = k
                score[i][j] = best
                path[i][j] = best_t
    
    best, best_t = -1e20, -1
    for j, tag in enumerate(hmm.postags):
    #for j in [3, 2]: #3 stand for 'S', 2 stand for 'E'
        if best < score[len(words)- 1][j]:
            best = score[len(words)- 1][j]
            best_t = j
    
    result = [best_t]
    for i in range(len(words)-1, 0, -1):
        best_t = path[i][best_t]
        result.append(best_t)
    
    result = [hmm.postags[t] for t in reversed(result)]
    return result

def calculate_2sent(n_correct, sent1, sent2):
    trans1, trans2 = set(), set()
    start, end = 0, 0
    for s in sent1:
        end += len(s)
        trans1.add(tuple((start, end)))
        start = end
    tmp_end1 = end
    start, end = 0, 0
    for s in sent2:
        end += len(s)
        trans2.add(tuple((start, end)))
        start = end
    assert tmp_end1 == end
    n_correct += len(trans1 & trans2)
    
    return n_correct

def generate_seg(pred_tag, sent):
    assert len(pred_tag) == len(sent)
    seg = []
    #assert pred_tag[-1] == 'S' or pred_tag[-1] == 'E'
    for i in range(len(pred_tag)):
        if 0 == i:
            assert pred_tag[0] == 'S' or pred_tag[0] == 'B'
            seg.append(sent[0])
        elif pred_tag[i] == 'B' or pred_tag[i] == 'S':
            seg.append(sent[i])
        else:
            seg[-1] += sent[i]
    return seg

def devel(devel_dataset, classifier):
    w_correct, w_total, w_seg = 0, 0, 0
    for devel_data_x, devel_data_y in devel_dataset:
        pred_y = viterbi(devel_data_x, classifier)
        assert len(pred_y) == len(devel_data_x)
        seg = generate_seg(pred_y, devel_data_x)
        w_correct = calculate_2sent(w_correct, seg, devel_data_y)
        w_seg += len(seg)
        w_total += len(devel_data_y)
    P = 1.0*w_correct/w_seg
    R = 1.0*w_correct/w_total
    F = 2*R*P/(P+R)
    print('P  (Correct/SegTotal): %d/%d    %f%%' % (w_correct, w_seg, 100*P))
    print('R  (Correct/Total)   : %d/%d    %f%%' % (w_correct, w_total, 100*R))
    print('F                    : %f%%' % (100*F))

def final_test(final_dataset, wf_name, classifier):
    wfp=open(wf_name, 'w')
    for test_data_x in final_dataset:
        pred_y = viterbi(test_data_x, classifier)
        assert len(pred_y) == len(test_data_x)
        seg = generate_seg(pred_y, test_data_x)
        print("\t".join(y.encode(encode_type) for y in seg), file=wfp)
        #print("\t".join(y.encode(encode_type) for y in pred_y), file=wfp)
    wfp.close()

if __name__ == "__main__":
    train_dataset = read_dataset('cip-data.train.gz', 'train.txt', True, False)
    #output_postag(train_dataset, 'tain_postag.txt')
    #judge1_dataset = read_dataset('judge.data.1.gz', 'judge1.txt', False)
    ans1_dataset = read_dataset('judge.data.ans.1.gz', 'ans1.txt', True, True)
    #output_postag(ans1_dataset, 'ans1_postag.txt')
    #judge2_dataset = read_dataset('judge.data.2.gz', 'judge2.txt', False)
    ans2_dataset = read_dataset('judge.data.ans.2.gz', 'ans2.txt', True, True)
    #output_postag(ans2_dataset, 'ans2_postag.txt')
    final1_dataset = read_dataset('final-test.1.txt.gz', 'final1.txt', False, False)
    final2_dataset = read_dataset('final-test.2.txt.gz', 'final2.txt', False, False)
    
    start = time.clock()
    hmm = HMM(training_data=train_dataset)
    end = time.clock()
    print ('time to fit the HMM: %f' % (end-start))
    
    print ('###### devel 1 ##############################')
    start = time.clock()
    devel(ans1_dataset, hmm)
    end = time.clock()
    print ('time to 1th devel: %f' % (end-start))
    print ('###### devel 2 ##############################')
    start = time.clock()
    devel(ans2_dataset, hmm)
    end = time.clock()
    print ('time to 2th devel: %f' % (end-start))
    start = time.clock()
    final_test(final_dataset=final1_dataset, wf_name='final1_result.txt', classifier=hmm)
    end = time.clock()
    print ('time to 2th final_test: %f' % (end-start))
    start = time.clock()
    final_test(final_dataset=final2_dataset, wf_name='final2_result.txt', classifier=hmm)
    end = time.clock()
    print ('time to 2th final_test: %f' % (end-start))
