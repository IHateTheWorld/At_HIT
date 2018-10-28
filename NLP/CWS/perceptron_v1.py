#-*- coding:utf-8 -*-

from __future__ import print_function
import gzip
import sys
from copy import copy
import time

#decode : str ==> unicode
#encode : unicode ==> str
encode_type = sys.getfilesystemencoding()

punctuation_tab = [u"，", u"、", u"。", u"“", u"”", u"．", u"（", u"）", u"《", u"》", u"！", u"『", u"』", u"％", u"—",
                   u"∶", u"‘", u"’", u"●", u"／", u"；", u"▲", u"【", u"】", u"？", u"〉", u"〈", u"～", u"…", u"★",
                   u"〖", u"〗", u"◆", u"◇", u"◢", u"☆", u"＜", u"＞",
                   u",", u".", u"/", u"?", u"<", u">", u";", u":", u"'", u'"', u"{", u"}", u"[", u"]", u"~", u"!", u"@",
                   u"#", u"$", u"%", u"^", u"&", u"*", u"(", u")", u"-", u"_", u"`", u"+", u"="]
date_tab = [u"年", u"月", u"日", u"时", u"分", u"秒"]
#digit_tab = [u"一", u"二", u"三", u"四", u"五", u"六", u"七", u"八", u"九", u"十", u"〇", u"零"]

def read_dataset(filename='cip-data.train.gz', wf_name='train.txt', flag = False, is_devel=False):
    try:
        fp=gzip.open(filename, 'r')
        #wfp = open(wf_name, 'w'),
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

def output_postag(postag_list = None, wf_name = None):
    if wf_name:
        wfp = open(wf_name, 'w')
    else:
        return 
    
    for sentance in postag_list:
        print(' '.join(ch.encode(encode_type) for ch in sentance[0]), file=wfp)
        print(' '.join(tag for tag in sentance[1]), file=wfp)
    
    wfp.close()

class PerceptronClassifier(object):
    def __init__(self, max_iter=10, training_data=None, devel1_data=None, devel2_data=None):
        self.max_iter = max_iter
        if training_data is not None:
            self.fit(training_data, devel1_data, devel2_data)
    
    def fit(self, training_data, devel1_data=None, devel2_data=None):
        self.feature_alphabet = {'None': 0}
        self.label_alphabet = {}
    
        instances = []
        next_label_index = len(self.label_alphabet)
        self.next_feature_index = len(self.feature_alphabet)
        for words, tags in training_data:
            L = len(words)
            prev = '<s>'
            for i in range(L):
                X = self.extract_features(words, i, prev, add=True)
                if tags[i] not in self.label_alphabet:
                    self.label_alphabet[tags[i]] = next_label_index
                    Y = next_label_index
                    next_label_index += 1
                else:
                    Y = self.label_alphabet[tags[i]]
                instances.append((X, Y))
                prev = tags[i]
        
        self.labels = [-1 for k in self.label_alphabet]
        for k in self.label_alphabet:
            self.labels[self.label_alphabet[k]] = k
        
        self.D, self.T = len(self.feature_alphabet), len(self.label_alphabet)
        print('number of features : %d' % self.D)
        print('number of labels: %d' % self.T)
        
        self.W = [[0 for j in range(self.D)] for i in range(self.T)]
        self.best_W = None
        self.best_acc = None
        total = [[0 for j in range(self.D)] for i in range(self.T)]
        
        print ('Count of instances: %d' % len(instances))
        for it in range(self.max_iter):
            print('training iteration #%d' % it)
            #count_inst = 1
            for X, Y in instances:
                #print (count_inst)
                #count_inst += 1
                Z = self._predict(X) #is _predict not predict
                if Z != Y:
                    for x in X:
                        self.W[Y][x] += 1
                        self.W[Z][x] -= 1
                '''
                for i in range(self.T): #too slow
                    for j in range(self.D):
                        total[i][j] += self.W[i][j]
            for i in range(self.D): #avg operation
                self.W[i] = 1.0 * total[i] / (N*(it_1))
            '''
            
            print ('###### devel 1 ##############################')
            start = time.clock()
            F1 = self.devel(devel1_data)
            end = time.clock()
            print ('time to 1th devel: %f' % (end-start))
            print ('###### devel 2 ##############################')
            start = time.clock()
            F2 = self.devel(devel2_data)
            end = time.clock()
            print ('time to 2th devel: %f' % (end-start))
            if self.best_acc < F1:
                self.best_acc = F1
                self.best_W = copy(self.W)
        
        if self.best_W is None:
            self.best_W = copy(self.W)
    
    def devel(self, devel_data):
        w_correct, w_total, w_seg = 0, 0, 0
        if devel_data is not None:
            for sent, words in devel_data:
                prev = '<s>'
                poss = []
                for i in range(len(sent)):
                    Z = self.predict(sent, i, prev)
                    poss.append(self.labels[Z])
                    #Y = self.label_alphabet[tags[i]]
                    prev = self.labels[Z]
                seg = generate_seg(poss, sent)
                w_correct = calculate_2sent(w_correct, seg, words)
                w_seg += len(seg)
                w_total += len(words)
                
        print (w_correct, w_seg, w_total)
        P = 1.0*w_correct/w_seg
        R = 1.0*w_correct/w_total
        F = 2*R*P/(P+R)
        print('P  (Correct/SegTotal): %d/%d    %f%%' % (w_correct, w_seg, 100*P))
        print('R  (Correct/Total)   : %d/%d    %f%%' % (w_correct, w_total, 100*R))
        print('F                    : %f%%' % (100*F))
        
        return F
    
    def check_ch(self, cur_ch):
        if cur_ch == '<s>' or cur_ch == '<e>':
            return cur_ch
        '''
        if cur_ch in punctuation_tab:
            return 'PUN'
        '''
        if cur_ch in date_tab: #date
            return 'DAT'
        if (cur_ch >= "0" and cur_ch <= "9") or (cur_ch >= u"０" and cur_ch <= u"９"): #digital
        #if (cur_ch >= "0" and cur_ch <= "9") or (cur_ch >= u"０" and cur_ch <= u"９") or (cur_ch in digit_tab): #digital
            return 'DIG'
        if (cur_ch >= "a" and cur_ch <= "z") or (cur_ch >= "A" and cur_ch <= "Z") or (cur_ch >= u'ａ' and cur_ch <= u'ｚ') or (cur_ch >= u'Ａ' and cur_ch <= u'Ｚ'): #letter
            return 'LET'
        return 'OTHER' #other
    
    def extract_features(self, words, i, prev_tag=None, add=True):
        L = len(words)
        context = ['<s>' if i- 2 < 0 else words[i- 2],
                   '<s>' if i- 1 < 0 else words[i- 1],
                   words[i],
                   '<e>' if i+ 1 >= L else words[i+ 1],
                   '<e>' if i+ 2 >= L else words[i+ 1]]
        types = [self.check_ch(context[0]), self.check_ch(context[1]), self.check_ch(context[2]),
                 self.check_ch(context[3]), self.check_ch(context[4])]
        raw_features = [#'U1=%s' % context[0],
                    'U2=%s' % context[1],
                    'U3=%s' % context[2],
                    'U4=%s' % context[3],
                    #'U5=%s' % context[4],
                    #'B1,2=%s/%s' % (context[0], context[1]), # Your code here, extract the bigram raw feature,
                    'B2,3=%s/%s' % (context[1], context[2]), # Your code here, extract the bigram raw feature,
                    'B3,4=%s/%s' % (context[2], context[3]), # Your code here, extract the bigram raw feature,
                    #'B4,5=%s/%s' % (context[3], context[4]), # Your code here, extract the bigram raw feature,
                    'J2,4=%s/%s' % (context[1], context[3]),
                    #'J1,3=%s/%s' % (context[0], context[2]),
                    #'J3,5=%s/%s' % (context[2], context[4]),
                    #'T3=%s' % types[2],
                    'T2,3,4=%s/%s/%s' % (types[1], types[2], types[3])
                    ]
        
        if context[2] in punctuation_tab:
            raw_features.append('PUN')
        
        '''
        if prev_tag is not None:
            raw_features.append('B=%s' % prev_tag)
        '''

        mapped_features = []
        for f in raw_features:
            if add and (f not in self.feature_alphabet):
                self.feature_alphabet[f] = self.next_feature_index
                self.next_feature_index += 1
            if f in self.feature_alphabet:
                mapped_features.append(self.feature_alphabet[f])
            
        return mapped_features
    
    def _score(self, features, t):
        s = 0
        for i in features:
            s += self.W[t][i]
        
        return s
    
    def _predict(self, features):
        pred_scores = [self._score(features, y) for y in range(self.T)]
        best_score, best_y = None, None
        for y in range(self.T):
            if best_score < pred_scores[y]:
                best_score = pred_scores[y]
                best_y = y
        
        return best_y
    
    def predict(self, words, i, prev_tag=None):
        X = self.extract_features(words, i, prev_tag, False)
        y = self._predict(X)
        return y

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
            #assert pred_tag[0] == 'S' or pred_tag[0] == 'B'
            seg.append(sent[0])
        elif pred_tag[i] == 'B' or pred_tag[i] == 'S':
            seg.append(sent[i])
        else:
            seg[-1] += sent[i]
    return seg

def greedy_search(words, classifier):
    prev = '<s>'
    ret=[]
    for i in range(len(words)):
        label_index = classifier.predict(words, i, prev)
        label = classifier.labels[label_index]
        if 0 == i:
            ret.append(words[i])
        elif 'S' == label or 'B' == label:
            ret.append(words[i])
        else:
            ret[-1] += words[i]
        prev = label
    return ret

def final_test(final_dataset, wf_name, classifier):
    wfp=open(wf_name, 'w')
    for test_data_x in final_dataset:
        pred_y = greedy_search(test_data_x, classifier)
        print("\t".join(y.encode(encode_type) for y in pred_y), file=wfp)

if __name__ == "__main__":
    start = time.clock()
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
    end = time.clock()
    print ('time to read data from file: %f' % (end-start))
    
    perceptron = PerceptronClassifier(max_iter=5, training_data=train_dataset, devel1_data=ans1_dataset, devel2_data=ans2_dataset)
    
    start = time.clock()
    final_test(final_dataset=final1_dataset, wf_name='final1_result.txt', classifier=perceptron)
    final_test(final_dataset=final2_dataset, wf_name='final2_result.txt', classifier=perceptron)
    end = time.clock()
    print ('time to generate final results: %f' % (end-start))
