# -*- coding: utf-8 -*-

import os
import json

#decode: str ==> unicode
#encode: unicode ==> str

tran_punc = {u'（':'(', u'）':')', u'０':'0', u'１':'1', u'２':'2', u'３':'3', u'４':'4', u'５':'5', u'６':'6', u'７':'7', u'８':'8', u'９':'9', ',':u'，', u'％':'%', ':':u'：', u'～':'~', u'－':'-'}

def comb_dicts(base_path, dicts_list, targ_name):
    comb_set = set()
    total_count = 0
    print "="*30
    for d in dicts_list:
        word_count = 0
        fp = file(os.path.join(base_path, d), 'rb')
        for word in fp:
            word_count += 1
            word = (word.strip()).decode("UTF-8")
            ###if word[:2] == u'俗称':
               ### print d
            for k,v in tran_punc.iteritems():
                word = word.replace(k, v)
            comb_set.add(word)
        fp.close()
        total_count += word_count
        print "%s: %d" % (d, word_count)
    print "total: %d" % total_count
    print "%s: %d" % (targ_name, len(comb_set))
    fp_out = file(targ_name, 'wb')
    comb_list = list(comb_set)
    comb_list.sort()
    for word in comb_list:
        fp_out.write(word.encode("UTF-8")+'\n')
    fp_out.close()

def clean_dict(filename):
    fp = file(filename, 'rb')
    fp_may = file(u'./疾病_可能.dic', 'wb')
    fp_out = file(filename+'_ret', 'wb')
    fp_short = file(filename+'_ret_short', 'wb')
    ### u'问题', u'医疗', u'情况', u'咨询', u'处方', u'住院', u'状态', u'史', u'的人', u'原因', u'者', u'监督', u'查体', u'装置', u'照顾', u'指导', u'证实', u'地点', u'观察', u'语音改变', u'不明确的', u'愉快', u'外貌', u'手术', u'生长时期', u'恢复期', u'差', u'发给', u'不能', u'湿型', u'干型', u'轻型', u'假体植入', u'物', u'检查', u'见', u'筛选', u'证据', u'人工喉', u'调整', u'免疫', u'为主', u'器', u'椅', u'无关', u'保留', u'动机', u'狂', u'六', u'烷', u'个体', u'部分', u'镜', u'内', u'自杀', u'后', u'凶暴', u'失败', u'管', u'复通', u'死亡', u'叹息', u'或', u'呵欠', u'锻炼', u'服务', u'因子', u'整形', u'不当', u'喘息', u'喷嚏', u'监禁', u'特性', u'法', u'多尿', u'结果', u'移植', u'测验', u'不明', u'管理', u'术', u'一种', u'处理', u'专注', u'剂', u'病人', u'的', u'心功能', u'级', u'复苏', u'放疗', u'电话', u'鼻孔', u'训练', u'效应', u'体征', u'操作', u'或系统', u'水平', u'标明', u'药', u'分离性', u'溺水', u'其他表现', u'事故', u'关心', u'耳孔', u'计数', u'纹身', u'康复', u'因素', u'疗', u'影响', u'采取', u'伤口', u'膜性', u'除外', u'药', u'试验', u'骨', u'指', u'趾', u'维护', u'失误', u'放置', u'再造', u'证词', u'铜色', u'隔离', u'措施', u'进入', u'分娩', u'反应'
    postfix = []
    ### u'其他', u'在医院', u'心理治疗', u'化学治疗', u'为', u'核型', u'取除', u'呕', u'安装', u'寻求', u'少尿', u'抽', u'敌视', u'未见', u'植入', u'确认', u'综合征'
    prefix = []
    for med in fp:
        med = (med.strip()).decode('UTF-8')
        if med[-2:] in postfix or med[-1:] in postfix or med[-3:] in postfix or \
                med[-4:] in postfix or med[:2] in prefix or med[:3] in prefix or \
                med[:4] in prefix or \
                med[:1] in prefix:
            fp_may.write(med.encode('UTF-8')+'\n')
            pass
        else:
            if len(med) < 3:
                fp_short.write(med.encode('UTF-8')+'\n')
            else:
                fp_out.write(med.encode('UTF-8')+'\n')
    fp.close()
    fp_out.close()
    fp_may.close()
    fp_short.close()

def clean_39net_sym():
    fp = file(u'./dictionary/dict/symptom_39net.txt_raw', 'rb')
    fp_out = file(u'./dictionary/dict/symptom_39net.txt', 'wb')
    for word in fp:
        if word[-4:-1] != '...':
            fp_out.write(word)
    fp.close()
    fp_out.close()

def cp2tag(file_list):
    for f_name in file_list:
        fp = file(os.path.join('./dictionary', f_name), 'rb')
        fp_out = file(os.path.join('../tag/dictionary', f_name), 'wb')
        for line in fp:
            fp_out.write(line)
        fp.close()
        fp_out.close()

def comb_drive():
    base_path = u'./dictionary/dict'
    #clean_dict(u'./dictionary/dict/医药名称.scel.txt_UTF-8')
    #comb_dicts(base_path, [u'symptom_39net_part1.txt', u'symptom_39net_part2.txt', u'symptom_39net_part3.txt'], u'./dictionary/dict/symptom_39net.txt_old')
    #clean_39net_sym()
    #comb_dicts(base_path, [u'download_cell.scel.txt_UTF-8', u'医药公司大全.scel.txt_UTF-8', u'常用药品生产厂家的名称.scel.txt_UTF-8', u'药品产地.scel.txt_UTF-8', u'药品生产企业词库.scel.txt_UTF-8'], u'./dictionary/药品生产企业.dic')
    #comb_dicts(base_path, [u'ffd.scel.txt_UTF-8', u'中草药.scel.txt_UTF-8', u'中药名称.scel.txt_UTF-8', u'中药名称ab.scel.txt_UTF-8', u'中药名称词库.scel.txt_UTF-8', u'中药药材.scel.txt_UTF-8', u'中药饮片.scel.txt_UTF-8', u'中药材及部分中药饮片.scel.txt_UTF-8', '中药词汇.scel.txt_UTF-8', u'部分中药材词库.scel.txt_UTF-8', u'常用中药名.scel.txt_UTF-8', u'快速输入中药名称.scel.txt_UTF-8', '药店中药饮片410个.scel.txt_UTF-8', u'独活的中药词库-健康是福-急毒药材132味.scel.txt_UTF-8', u'常用中药处方名称（唐都医院中医科丁井永）.scel.txt_UTF-8'], u'./dictionary/中草药.dic')
    #comb_dicts(base_path, [u'医疗仪器与医疗器械.scel.txt_UTF-8', u'医疗器械大全【官方推荐】.scel.txt_UTF-8', u'医疗器材及试剂词库.scel.txt_UTF-8', u'公司专用器械.scel.txt_UTF-8', u'yiyongqixie.com.txt', u'medsoso.cn.txt'], u'./dictionary/医学器械.dic')
    #comb_dicts(base_path, [u'医药化工字典.scel.txt_UTF-8', u'精神药物.scel.txt_UTF-8', u'药品对照品.scel.txt_UTF-8', u'部分的西药名称.scel.txt_UTF-8', u'维康医院西药目录.scel.txt_UTF-8', u'医院西药名称.scel.txt_UTF-8', u'医药名称.scel.txt_UTF-8', u'中外药品名称大全【官方推荐】.scel.txt_UTF-8', u'临床用药大全STZ.scel.txt_UTF-8', u'处方常用药品通用名目录.scel.txt_UTF-8', u'心血管内科常用药.txt'], u'./dictionary/药品.dic_raw')
    #comb_dicts(base_path, [u'医药化工字典.scel.txt_UTF-8', u'精神药物.scel.txt_UTF-8', u'药品对照品.scel.txt_UTF-8', u'部分的西药名称.scel.txt_UTF-8', u'维康医院西药目录.scel.txt_UTF-8', u'医院西药名称.scel.txt_UTF-8', u'临床用药大全STZ.scel.txt_UTF-8', u'处方常用药品通用名目录.scel.txt_UTF-8', u'心血管内科常用药.txt'], u'./dictionary/药品.dic_raw') # have not '医药名称.scel.txt_UTF-8', u'中外药品名称大全【官方推荐】.scel.txt_UTF-8'
    #clean_dict(u'./dictionary/药品.dic')
    comb_dicts(base_path, [u'手术分类与代码(ICD-9-CM3).scel.txt_UTF-8', u'国际标准手术编码.scel.txt_UTF-8', u'baidu百科_心脏介入手术.txt', u'a-hospital.com_胸心外科手术.txt', u'a-hospital.com_血管外科手术.txt', u'a-hospital.com_临床常用诊断技术.txt', u'科学百科诊疗方法分类_filter.txt'], u'./dictionary/手术检查.dic')
    #comb_dicts(base_path, [u'ICD-10疾病编码1.scel.txt_UTF-8', u'西医病名.scel.txt_UTF-8', u'飞华健康网_心血管内科疾病.txt'], u'./dictionary/疾病.dic')
    comb_dicts(base_path, [u'症状名.scel.txt_UTF-8', u'symptom_39net.txt', u'symptom_120net.txt', u'飞华健康网_心脏症状.txt', u'飞华健康网_症状.txt', u'心电图诊断用语.scel.txt_UTF-8', u'qiuyi.cn_sym.txt'], u'./dictionary/症状.dic')
    comb_dicts(base_path, [u'心血管内科常用药.txt', u'a-hospital.com_治疗心脑血管疾病的药品列表.txt', u'a-hospital.com_治疗病毒性心肌炎的药品列表.txt', u'a-hospital.com_其它治疗心脑血管疾病的药品列表_part1.txt', u'a-hospital.com_其它治疗心脑血管疾病的药品列表_part2.txt', u'a-hospital.com_治疗高血压的药品列表.txt', u'a-hospital.com_治疗冠心病的药品列表.txt', u'a-hospital.com_治疗心力衰竭的药品列表.txt', u'a-hospital.com_治疗心绞痛的药品列表.txt', u'a-hospital.com_治疗心肌病的药品列表.txt', u'a-hospital.com_治疗高血脂的药品列表.txt', u'a-hospital.com_治疗心功能不全的药品列表_part1.txt', u'a-hospital.com_治疗心功能不全的药品列表_part2.txt', u'a-hospital.com_治疗动脉硬化的药品列表.txt', u'a-hospital.com_治疗肺动脉高压的药品列表.txt', u'a-hospital.com_治疗心慌心悸的药品列表.txt', u'a-hospital.com_治疗中风和偏瘫的药品列表.txt', u'a-hospital.com_治疗胸闷和胸痛的药品列表.txt', u'a-hospital.com_治疗心律失常的药品列表.txt', u'a-hospital.com_治疗心律失常的药品列表_part2.txt', u'a-hospital.com_ATC代码_C心血管系统.txt', u'a-hospital.com_ATC代码_B血液系统.txt', u'a-hospital.com_心血管系统常见药剂类别.txt', u'www.baikemy.com_药品.txt', u'a-hospital.com_世界卫生组织基本药物标准清单.txt', u'a-hospital.com_中国国家基本药物目录2012版.txt', u'a-hospital.com_撤回药品列表.txt', u'a-hospital.com_2009版中国国家基本药物列表.txt', u'中华康网-药品大全.txt', u'科学百科药物分类_filter.txt'], u'./dictionary/药物.dic')
    #comb_dicts(base_path, [u'飞华健康网_心血管内科疾病.txt', u'a-hospital.com_心血管内科疾病_part1.txt', u'a-hospital.com_心血管内科疾病_part2.txt', u'a-hospital.com_心血管内科疾病_part3.txt', u'jbk.99.com.cn_心血管内科疾病大全.txt', u'cndzys.com_心血管内科相关疾病.txt', u'西医病名.scel.txt_UTF-8', u'ICD-10疾病编码1.scel.txt_UTF-8'], u'./dictionary/疾病.dic')
    comb_dicts(base_path, [u'飞华健康网_心血管内科疾病.txt', u'a-hospital.com_心血管内科疾病_part1.txt', u'a-hospital.com_心血管内科疾病_part2.txt', u'a-hospital.com_心血管内科疾病_part3.txt', u'jbk.99.com.cn_心血管内科疾病大全.txt', u'cndzys.com_心血管内科相关疾病.txt', u'疾病_manual_long.txt', u'疾病_manual_short.txt'], u'./dictionary/疾病.dic')
    #comb_dicts(base_path, [u'baidu百科_心脏介入手术.txt', u'a-hospital.com_胸心外科手术.txt', u'a-hospital.com_血管外科手术.txt', u'a-hospital.com_临床常用诊断技术.txt'], u'./dictionary/心血管相关手术.dic')
    comb_dicts(base_path, [u'中文停用词库.txt', u'四川大学机器智能实验室停用词库.txt', u'哈工大停用词表.txt'], u'./dictionary/停用表.dic')
    file_list = [u'疾病.dic', u'症状.dic', u'药物.dic', u'手术检查.dic', u'停用表.dic']
    cp2tag(file_list)

if __name__ == "__main__":
    comb_drive()
