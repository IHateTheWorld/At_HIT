#!/usr/bin/env python
# result should be UTF8 encoded.
import sys
def get_result(line):
    ret = set()
    start_id = 0
    for token in line.strip().split():
        end_id = start_id + len(token.decode('utf-8'))
        ret.add((start_id, end_id, token))
        start_id = end_id
    return ret

n_recall, n_gold, n_pred = 0, 0, 0
for gold_line, pred_line in zip(open(sys.argv[1], 'r'), open(sys.argv[2], 'r')):
    gold_res = get_result(gold_line)
    pred_res = get_result(pred_line)
    for res in pred_res:
        if res in gold_res:
            n_recall += 1
    n_gold += len(gold_res)
    n_pred += len(pred_res)

p = float(n_recall)/n_gold
r = float(n_recall)/n_pred
f = 2 * p * r / (p + r)
print '#recall', n_recall
print '#gold', n_gold
print '#predict', n_pred
print 'f:', f
