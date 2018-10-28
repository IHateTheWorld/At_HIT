#-*- coding:utf-8 -*-

from __future__ import print_function
import gzip
import sys
from copy import copy
import time

#decode : str ==> unicode
#encode : unicode ==> str
encode_type = sys.getfilesystemencoding()

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
        tokens = line.strip().split()
        #print(' '.join(t for t in tokens), file=wfp)
        #print(' '.join(str(len(t.decode('utf-8'))) for t in tokens), file=wfp)
        dataset.append([])
        if flag:
            dataset[-1].append([])
            dataset[-1].append([])
        for t in tokens:
            t = t.decode('utf-8')
            if flag:
                dataset[-1][1].append(t) #words
                for c in t:
                    dataset[-1][0].append(c) #chars
            else:
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
        print(' '.join(ch.encode(encode_type) for ch in sentance), file=wfp)
    
    wfp.close()

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

class PerceptronClassifier(object):
    def __init__(self, max_iter=10, training_data=None, devel1_data=None, devel2_data=None):
        self.max_iter = max_iter
        if training_data is not None:
            self.fit(training_data, devel1_data, devel2_data)
    
    def fit(self, training_data, devel1_data=None, devel2_data=None):
        self.feature_alphabet = {'None': 0}

        instances = []
        self.next_feature_index = len(self.feature_alphabet)
        for chars, words in training_data:
            V = self.extract_features(words, add=True)
            instances.append((chars, words, V))

        self.D = len(self.feature_alphabet)
        print('number of features : %d' % self.D)
        print('Length of instances: %d' % len(instances))
        
        #print (self.feature_alphabet)
        
        self.W = [0 for j in range(self.D)]
        self.best_W = None
        self.best_acc = None
        #N = len(instances)
        #total = [0 for j in range(self.D)]
        
        for it in range(self.max_iter):
            w_correct, w_total, w_seg = 0, 0, 0
            s_errors, s_total = 0, len(instances)
            
            print('training iteration #%d' % it)
            #count_inst = 1
            for X, Y, Vy in instances:
                #print (count_inst)
                #count_inst += 1
                Vz, Z = self.predict(X)
                if Z != Y:
                    s_errors += 1
                    w_correct = calculate_2sent(w_correct, Y, Z)
                    for vy in Vy:
                        self.W[vy] += Vy[vy]
                    for vz in Vz:
                        self.W[vz] -= Vz[vz]
                else:
                    w_correct += len(Y)
                w_total += len(Y)
                w_seg += len(Z)
                #for i in range(self.D): #too slow
                 #   total[i] += self.W[i]
            #for i in range(self.D): #avg operation
             #   self.W[i] = 1.0 * total[i] / (N*(it_1))
            
            print ('###### train   #############################')
            print('Sentance (Error/Total)  : %d/%d    %f%%' % (s_errors, s_total, 100.0*(s_total-s_errors)/s_total))
            P = 1.0*w_correct/w_seg
            R = 1.0*w_correct/w_total
            F = 2*R*P/(P+R)
            print('P  (Correct/SegTotal): %d/%d    %f%%' % (w_correct, w_seg, P))
            print('R  (Correct/Total)   : %d/%d    %f%%' % (w_correct, w_total, R))
            print('F                    : %f%%' % F)
            
            print ('###### devel 1 #############################')
            F1 = self.devel(devel1_data)
            print ('###### devel 2 #############################')
            F2 = self.devel(devel2_data)
            if self.best_acc < F1:
                self.best_acc = F1
                self.best_W = copy(self.W)
        
        if self.best_W is None:
            self.best_W = copy(self.W)
    
    def devel(self, devel_data):
        self.w_correct, self.w_total, self.w_seg = 0, 0, 0
        self.s_correct, self.s_total = 0, len(devel_data)
        if devel_data is not None:
            for chars, words in devel_data:
                Vz, Z = self.predict(chars)
                if Z == words:
                    self.s_correct += 1
                    self.w_correct += len(words)
                else:
                    self.w_correct = calculate_2sent(self.w_correct, words, Z)
                self.w_total += len(words)
                self.w_seg += len(Z)
        print('Sentance (Correct/Total)  : %d/%d    %f%%' % (self.s_correct, self.s_total, 100.0*self.s_correct/self.s_total))
        P = 1.0*self.w_correct/self.w_seg
        R = 1.0*self.w_correct/self.w_total
        F = 2*R*P/(P+R)
        print('P  (Correct/SegTotal): %d/%d    %f%%' % (self.w_correct, self.w_seg, P))
        print('R  (Correct/Total)   : %d/%d    %f%%' % (self.w_correct, self.w_total, R))
        print('F                    : %f%%' % F)
        
        return F
    
    def extract_features(self, words, add=True):
        mapped_features = {}
        LW = len(words)
        for i in range(LW):
            LC = len(words[i])
            for j in range(LC):
                mapped_features = self._extract_current(mapped_features, words, i, j, add)
        return mapped_features
    
    def _extract_current(self, mapped_features, words, i, j, add=True):
        context = {}
        context['C0'] = words[i][j]
        if 0 == j: #separated
            if i > 0:
                context['W1'] = words[i-1]
                if 1 == len(words[i-1]):
                    context['W1,l1'] = words[i-1]
                else:
                    context['W1,l1'] = '<M>'
                context['L1'] = len(words[i-1])
                context['S1'] = words[i-1][0]
                context['E1'] = words[i-1][-1]
            else:
                context['W1'] = '<N>'
                context['W1,l1'] = '<N>'
                context['L1'] = 0
                context['S1'] = '<S>'
                context['E1'] = '<E>'
            if i > 1:
                context['W2'] = words[i-2]
                context['L2'] = len(words[i-2])
                context['E2'] = words[i-2][-1]
            else:
                context['W2'] = '<N>'
                context['L2'] = 0
                context['E2'] = '<E>'
            raw_features = ['W1=%s' % context['W1'],
                        'W1,W2=%s,%s' % (context['W1'], context['W2']),
                        'W1,l1=%s' % (context['W1,l1']),
                        'S1,L1=%s,%s' % (context['S1'], context['L1']),
                        'E1,L1=%s,%s' % (context['E1'], context['L1']),
                        'E1,C0=%s,%s' % (context['E1'], context['C0']),
                        'S1,E1=%s,%s' % (context['S1'], context['E1']),
                        'W1,C0=%s,%s' % (context['W1'], context['C0']),
                        'E2,W1=%s,%s' % (context['E2'], context['W1']),
                        'S1,C0=%s,%s' % (context['S1'], context['C0']),
                        'E2,E1=%s,%s' % (context['E2'], context['E1']),
                        'W2,L1=%s,%s' % (context['W2'], context['L1']),
                        'L2,W1=%s,%s' % (context['L2'], context['W1'])
                        ]
        else: #appended
            #context['C1'] = words[i][j-1]
            raw_features = ['C-1,C0=%s,%s' % (words[i][j-1], context['C0'])]
        
        for f in raw_features:
            if add and (f not in self.feature_alphabet):
                self.feature_alphabet[f] = self.next_feature_index
                self.next_feature_index += 1
            if f in self.feature_alphabet:
                if f in mapped_features:
                    mapped_features[self.feature_alphabet[f]] += 1
                else:
                    mapped_features[self.feature_alphabet[f]] = 1
            
        return mapped_features
    
    def _score(self, features):
        s = 0
        for f_index in features:
            s += self.W[f_index] * features[f_index]
        return s
    
    def predict(self, sent): #beam-search decoder
        src = [[[],{}, 0]]
        tgt = []
        B = 2 #the size of the beam
        
        for index in range(len(sent)):
            char = sent[index]
            for item, pre_features, s in src:
                item1 = copy(item)
                item1.append(char)
                tmp_features = copy(pre_features)
                tgt.append([item1, self._extract_current(tmp_features, item1, len(item1)-1, 0, False)])
                tgt[-1].append(self._score(tgt[-1][1]))
                if len(item) > 0:
                    item2 = copy(item)
                    item2[-1] += char
                    tgt.append([item2, self._extract_current(pre_features, item2, len(item2)-1, len(item2[-1])-1, False)])
                    tgt[-1].append(self._score(tgt[-1][1]))
            tgt.sort(lambda LS,RS:cmp(RS[2],LS[2]))
            if len(tgt) > B:
                src = tgt[:B]
            else:
                src = copy(tgt)
            tgt = []
            
        return (src[0][1], src[0][0])

def beam_search_decoder(sentance, classifier):
    return classifier.predict(sentance)[1]

def final_test(final_dataset, wf_name, classifier):
    wfp=open(wf_name, 'w')
    for test_data_x in final_dataset:
        #pred_y = beam_search_decoder(test_data_x, classifier)
        pred_y = classifier.predict(test_data_x)[1]
        print(" ".join(y.encode(encode_type) for y in pred_y), file=wfp)

if __name__ == "__main__":
    start = time.clock()
    train_dataset = read_dataset('cip-data.train.gz', 'train.txt', True)
    #output_postag(train_dataset, 'tain_v2.txt')
    #judge1_dataset = read_dataset('judge.data.1.gz', 'judge1.txt', False)
    ans1_dataset = read_dataset('judge.data.ans.1.gz', 'ans1.txt', True)
    #output_postag(ans1_dataset, 'ans1_v2.txt')
    #judge2_dataset = read_dataset('judge.data.2.gz', 'judge2.txt', False)
    ans2_dataset = read_dataset('judge.data.ans.2.gz', 'ans2.txt', True)
    #output_postag(ans2_dataset, 'ans2_v2.txt')
    final1_dataset = read_dataset('final-test.1.txt.gz', 'final1.txt', False)
    #output_postag(final1_dataset, 'final1_v2.txt')
    final2_dataset = read_dataset('final-test.2.txt.gz', 'final2.txt', False)
    #output_postag(final2_dataset, 'final2_v2.txt')
    end = time.clock()
    print ('time to read data from file: %f' % (end-start))
    
    start = time.clock()
    perceptron = PerceptronClassifier(max_iter=5, training_data=train_dataset, devel1_data=ans1_dataset, devel2_data=ans2_dataset)
    #perceptron = PerceptronClassifier(max_iter=5, training_data=ans2_dataset, devel1_data=ans1_dataset, devel2_data=ans2_dataset)
    end = time.clock()
    print ('time to fit perceptron: %f' % (end-start))
    
    start = time.clock()
    final_test(final_dataset=final1_dataset, wf_name='final1_result.txt', classifier=perceptron)
    final_test(final_dataset=final2_dataset, wf_name='final2_result.txt', classifier=perceptron)
    end = time.clock()
    print ('time to generate final results: %f' % (end-start))
