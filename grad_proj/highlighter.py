import os
import sys
import time
import re
import ner_drive, relat_drive, retrieve, eval_drive
from PyQt4 import QtCore, QtGui
#decode: str ==> unicode
#encode: unicode ==> str
encode_type = sys.getfilesystemencoding()


class MyHighlighter(QtGui.QSyntaxHighlighter):
    def __init__(self, parent):
        QtGui.QSyntaxHighlighter.__init__(self, parent)
        self.parent = parent
        self.highlight_data = dict()
        
        self.DIS_format = QtGui.QTextCharFormat()
        DIS_brush = QtGui.QBrush(QtCore.Qt.red, QtCore.Qt.SolidPattern)
        self.DIS_format.setBackground(DIS_brush)
        self.MED_format = QtGui.QTextCharFormat()
        MED_brush = QtGui.QBrush(QtCore.Qt.green, QtCore.Qt.SolidPattern)
        self.MED_format.setBackground(MED_brush)
        self.SYM_format = QtGui.QTextCharFormat()
        SYM_brush = QtGui.QBrush(QtCore.Qt.yellow, QtCore.Qt.SolidPattern)
        self.SYM_format.setBackground(SYM_brush)
        self.TRE_format = QtGui.QTextCharFormat()
        TRE_brush = QtGui.QBrush(QtCore.Qt.gray, QtCore.Qt.SolidPattern)
        self.TRE_format.setBackground(TRE_brush)
        self.format_list = (self.DIS_format, self.MED_format, self.SYM_format, self.TRE_format)
        self.flags_list = ('<DIS>', '<MED>', '<SYM>', '<TRE>')
        self.ret_count = 0
    
    def highlightBlock(self, text):
        map_length = -1
        new_map_length = 0
        for flag_i in range(len(self.flags_list)):
            flag_str = self.flags_list[flag_i]
            if flag_str in self.highlight_data.keys():
                index_length_set = []
                for item in self.highlight_data[flag_str]:
                    index = 0
                    length = 0
                    map_length = new_map_length - 1
                    while map_length != new_map_length: #Attention point...
                        map_length = new_map_length
                        index = text.indexOf(item, index + length)
                        length = len(item)
                        if index != -1:
                            if (index, length) not in index_length_set:
                                index_length_set.append((index, length))
                                new_map_length += 1
                for (index, length) in index_length_set:
                    self.setFormat(index, length, self.format_list[flag_i])
    
    def setHighlightData(self, highlight_data):
        self.highlight_data = highlight_data.copy()
    
    def init_ret_count(self):
        self.ret_count = 0
    
    def return_count(self):
        return self.ret_count
