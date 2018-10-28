# -*- coding: utf-8 -*-

import os

def rm_enter(file_path):
    fp = file(os.path.join('./raw_texts', file_path), 'rb')
    fp_out = file(os.path.join('./after_manual', file_path), 'wb')
    bf_line = ''
    for line in fp:
        line = line.strip()
        if len(line) == 0:
            if len(bf_line) != 0:
                fp_out.write('\n')
        else:
            fp_out.write(line)
        bf_line = line[:]
    fp.close()
    fp_out.close()

if __name__ == "__main__":
    file_list = [u'心力衰竭诊断与治疗研究进展_戴闺柱.txt', u'冠心病诊断与治疗研究进展_胡大一.txt', u'炎症与动脉粥样硬化_程翔.txt', u'冠状动脉心肌桥研究现状_董敏.txt', u'冠心病整体防治中他汀类药物的重要地位_赵水平.txt', u'动脉粥样硬化_血管壁的慢性炎症_冯宗忱.txt', u'扩张型心肌病发病机制和治疗的研究新动向_杨英珍.txt', u'病毒性心肌炎诊断标准_修订草案_.txt', u'糖尿病心肌病发病机制的研究进展_黄娅茜.txt', u'扩张型心肌病的诊断和治疗研究进展_杨英珍.txt', u'无症状高尿酸血症合并心血管疾病诊治建议中国专家共识解读_郭立新.txt']
    for file_path in file_list:
        rm_enter(file_path)