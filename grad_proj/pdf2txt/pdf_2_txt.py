# -*- coding: utf-8 -*-
from pdfminer.pdfinterp import PDFResourceManager,PDFPageInterpreter
from pdfminer.converter import TextConverter
from pdfminer.layout import LAParams
from pdfminer.pdfpage import PDFPage
from cStringIO import StringIO
import sys
import os

def Pdf2Txt(path, save_name):
    rsrcmgr = PDFResourceManager()
    retstr = StringIO()
    outfp = file(save_name+'_tmp', 'w')
    codec = 'utf-8'
    imagewriter = None
    laparams = LAParams()
    device = TextConverter(rsrcmgr, retstr, codec=codec, laparams=laparams, imagewriter=imagewriter)
    fp = file(path, 'rb')
    interpreter = PDFPageInterpreter(rsrcmgr, device)
    password = ""
    maxpages = 0
    caching = True
    pagenos = set()
    for page in PDFPage.get_pages(fp, pagenos, maxpages=maxpages, password=password, caching=caching, check_extractable=True):
        interpreter.process_page(page)
    fp.close()
    device.close()
    cont_str = retstr.getvalue()
    retstr.close()
    #for line_str in cont_str: #there line_str is just a char in fact, beacause const_str is just a str not the file pointer
    #    outfp.write(line_str)
    outfp.write(cont_str)
    outfp.close()
    clean_TXT(save_name)
    return

def clean_TXT(save_name):
    fp = file(save_name+'_tmp', 'rb')
    outfp = file(save_name, 'w')
    for line in fp:
        line = line.strip()
        if len(line) == 0:
            pass
        else:
            outfp.write(line+'\n')
    fp.close()
    os.remove(save_name+'_tmp')
    outfp.close()

Path = 'medical_texts/儿童常见先天性心脏病介入治疗专家共识.pdf'
Pdf2Txt(Path,'result.txt')
