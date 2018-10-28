# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'tab_widget.ui'
#
# Created: Mon Jun 05 14:50:53 2017
#      by: PyQt4 UI code generator 4.9.6
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui
import os
import sys
import re
#decode: str ==> unicode
#encode: unicode ==> str
encode_type = sys.getfilesystemencoding()

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    def _fromUtf8(s):
        return s

try:
    _encoding = QtGui.QApplication.UnicodeUTF8
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig, _encoding)
except AttributeError:
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig)

class MyHighlighter(QtGui.QSyntaxHighlighter):
    def __init__(self, parent):
        QtGui.QSyntaxHighlighter.__init__(self, parent)
        self.parent = parent
        self.highlight_data = []
        
        self.matched_format = QtGui.QTextCharFormat()
        brush = QtGui.QBrush(QtCore.Qt.yellow, QtCore.Qt.SolidPattern)
        self.matched_format.setBackground(brush)
        self.ret_count = 0
    '''
    def highlightBlock(self, text):
        index = 0
        length = 0
        for item in self.highlight_data:
            index = text.indexOf(item, index + length)
            length = len(item)
            self.setFormat(index, length, self.matched_format)
    '''
    def highlightBlock(self, text):
        index = 0
        length = 0
        is_change = True
        while is_change == True: #Attention point...
            is_change = False
            for item in self.highlight_data:
                if item.count('\n') != 0:
                    itemList = item.split('\n')
                    for part in itemList:
                        index = text.indexOf(part, index + length)
                        if index == -1:
                            index = 0
                        else:
                            length = len(part)
                            self.setFormat(index, length, self.matched_format)
                            is_change = True
                            self.ret_count += 1
                else:
                    index = text.indexOf(item, index + length)
                    if index != -1:
                        is_change = True
                        self.ret_count += 1
                    length = len(item)
                    self.setFormat(index, length, self.matched_format)
    
    def setHighlightData(self, highlight_data):
        self.highlight_data = highlight_data
    
    def init_ret_count(self):
        self.ret_count = 0
    
    def return_count(self):
        return self.ret_count


class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName(_fromUtf8("MainWindow"))
        MainWindow.resize(834, 598)
        MainWindow.setFocusPolicy(QtCore.Qt.StrongFocus)
        self.centralwidget = QtGui.QWidget(MainWindow)
        self.centralwidget.setObjectName(_fromUtf8("centralwidget"))
        self.tabWidget1 = QtGui.QTabWidget(self.centralwidget)
        self.tabWidget1.setEnabled(True)
        self.tabWidget1.setGeometry(QtCore.QRect(6, -1, 791, 561))
        self.tabWidget1.setFocusPolicy(QtCore.Qt.NoFocus)
        self.tabWidget1.setStyleSheet(_fromUtf8("font: 75 italic 12pt \"Agency FB\";"))
        self.tabWidget1.setObjectName(_fromUtf8("tabWidget1"))
        self.tab_1 = QtGui.QWidget()
        self.tab_1.setObjectName(_fromUtf8("tab_1"))
        self.lineEdit1 = QtGui.QLineEdit(self.tab_1)
        self.lineEdit1.setGeometry(QtCore.QRect(40, 50, 600, 30))
        self.lineEdit1.setObjectName(_fromUtf8("lineEdit1"))
        self.label1 = QtGui.QLabel(self.tab_1)
        self.label1.setGeometry(QtCore.QRect(40, 10, 171, 31))
        self.label1.setObjectName(_fromUtf8("label1"))
        self.pushButton1 = QtGui.QPushButton(self.tab_1)
        self.pushButton1.setGeometry(QtCore.QRect(140, 90, 111, 31))
        self.pushButton1.setStyleSheet(_fromUtf8("font: 75 italic 12pt \"Agency FB\";"))
        self.pushButton1.setObjectName(_fromUtf8("pushButton1"))
        self.pushButton2 = QtGui.QPushButton(self.tab_1)
        self.pushButton2.setGeometry(QtCore.QRect(670, 140, 75, 31))
        self.pushButton2.setStyleSheet(_fromUtf8("font: 75 italic 12pt \"Agency FB\";"))
        self.pushButton2.setObjectName(_fromUtf8("pushButton2"))
        self.textEdit1 = QtGui.QTextEdit(self.tab_1)
        self.textEdit1.setGeometry(QtCore.QRect(30, 180, 721, 321))
        self.textEdit1.setStyleSheet(_fromUtf8("font: 12pt \"Agency FB\";"))
        self.textEdit1.setReadOnly(True)
        self.textEdit1.setObjectName(_fromUtf8("textEdit1"))
        self.pushButton3 = QtGui.QPushButton(self.tab_1)
        self.pushButton3.setGeometry(QtCore.QRect(40, 90, 75, 31))
        self.pushButton3.setObjectName(_fromUtf8("pushButton3"))
        self.checkBox1 = QtGui.QCheckBox(self.tab_1)
        self.checkBox1.setEnabled(True)
        self.checkBox1.setGeometry(QtCore.QRect(650, 60, 101, 16))
        self.checkBox1.setFocusPolicy(QtCore.Qt.NoFocus)
        self.checkBox1.setStyleSheet(_fromUtf8("font: 75 12pt \"Agency FB\";"))
        self.checkBox1.setCheckable(True)
        self.checkBox1.setChecked(False)
        self.checkBox1.setObjectName(_fromUtf8("checkBox1"))
        self.comboBox1 = QtGui.QComboBox(self.tab_1)
        self.comboBox1.setGeometry(QtCore.QRect(40, 140, 491, 31))
        self.comboBox1.setObjectName(_fromUtf8("comboBox1"))
        self.tabWidget1.addTab(self.tab_1, _fromUtf8(""))
        self.tab_2 = QtGui.QWidget()
        self.tab_2.setObjectName(_fromUtf8("tab_2"))
        self.tabWidget1.addTab(self.tab_2, _fromUtf8(""))
        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QtGui.QMenuBar(MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 834, 23))
        self.menubar.setObjectName(_fromUtf8("menubar"))
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QtGui.QStatusBar(MainWindow)
        self.statusbar.setObjectName(_fromUtf8("statusbar"))
        MainWindow.setStatusBar(self.statusbar)
        self.filename = u''
        self.highlighter = MyHighlighter(self.textEdit1)
        self.search_sent = u''
        self.load_relat_index('../tag/after_tag/L.xxx')

        self.retranslateUi(MainWindow)
        self.tabWidget1.setCurrentIndex(0)
        QtCore.QObject.connect(self.pushButton1, QtCore.SIGNAL(_fromUtf8("clicked()")), MainWindow.click_ok)
        QtCore.QObject.connect(self.pushButton2, QtCore.SIGNAL(_fromUtf8("clicked()")), MainWindow.click_file_load)
        QtCore.QObject.connect(self.pushButton3, QtCore.SIGNAL(_fromUtf8("clicked()")), MainWindow.click_search)
        QtCore.QObject.connect(self.checkBox1, QtCore.SIGNAL(_fromUtf8("stateChanged(int)")), MainWindow.check_tips)
        QtCore.QObject.connect(self.comboBox1, QtCore.SIGNAL(_fromUtf8("currentIndexChanged(QString)")), MainWindow.comboBox_index_change)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)
        MainWindow.setTabOrder(self.pushButton1, self.pushButton2)
        MainWindow.setTabOrder(self.pushButton2, self.lineEdit1)
        MainWindow.setTabOrder(self.lineEdit1, self.textEdit1)
        MainWindow.setTabOrder(self.textEdit1, self.tabWidget1)

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(_translate("MainWindow", "MainWindow", None))
        self.label1.setText(_translate("MainWindow", "<html><head/><body><p><span style=\" font-size:20pt; font-style:italic;\">输入查询语句</span></p></body></html>", None))
        self.pushButton1.setText(_translate("MainWindow", "分析查询语句", None))
        self.pushButton2.setText(_translate("MainWindow", "读取文件", None))
        self.pushButton3.setText(_translate("MainWindow", " 查找", None))
        self.checkBox1.setText(_translate("MainWindow", "提示性提问", None))
        self.tabWidget1.setTabText(self.tabWidget1.indexOf(self.tab_1), _translate("MainWindow", "查询", None))
        self.tabWidget1.setTabText(self.tabWidget1.indexOf(self.tab_2), _translate("MainWindow", "其他", None))
        self.set_combox_items(item_list=[], is_init=True)

    def load_relat_index(self, tag_data_path):
        fp_in = file(tag_data_path, 'rb')
        fp_out = file('relat_index.xxx', 'wb')
        part_no = 6
        count = 0
        part_lines = []
        for a_line in fp_in:
            part_lines.append(a_line.strip())
            count += 1
            if count == part_no:
                count = 0
                ###TODO...
                raw_string = part_lines[1].decode('UTF-8')
                file_info = ((part_lines[2].decode('UTF-8')).split())[1]
                ne_list = ((part_lines[4].decode('UTF-8')).split())[1:]
                relat_list = ((part_lines[5].decode('UTF-8')).split())[1:]
                for relat_i in relat_list:
                    fp_out.write('='*50+'\n')
                    relat_type = re.findall(r',.*,', relat_i)
                    assert len(relat_type) == 1
                    relat_type = relat_type[0][1:-1]
                    lr_ne_flag = re.findall(r'\d+', relat_i)
                    assert len(lr_ne_flag) == 2
                    left_no = int(lr_ne_flag[0])
                    right_no = int(lr_ne_flag[1])
                    left_ne = ne_list[left_no]
                    right_ne = ne_list[right_no]
                    fp_out.write((relat_type+' '+left_ne+' '+right_ne+' '+file_info).encode('UTF-8')+'\n')
                    fp_out.write(raw_string.encode('UTF-8')+'\n')
                ###DONE...
                part_lines = []
        fp_in.close()
        fp_out.close()

    def set_combox_items(self, item_list, is_init=False):
        self.comboBox1.clear()
        if is_init == True:
            item_list = os.listdir('../tag/medical_texts')
            for item_i in range(len(item_list)):
                item_list[item_i] = item_list[item_i].decode(encode_type)
                # self.comboBox1.setItemText(item_i, _translate("MainWindow", _fromUtf8(item_list[item_i]), None))
        self.comboBox1.insertItems(0, item_list)
        self.filename = unicode(self.comboBox1.currentText().toUtf8(), 'utf-8', 'ignore')
        self.filename = os.path.join('../tag/medical_texts', self.filename)
        self.click_file_load()

    def click_ok(self):
        self.search_sent = unicode(self.lineEdit1.text().toUtf8(), 'utf-8', 'ignore')
        ###TODO...
        ###DONE...

    def click_file_load(self):
        fh = None
        try:
            fh = QtCore.QFile(self.filename)
            if not fh.open(QtCore.QIODevice.ReadOnly):
                raise IOError(str(fh.errorString()))
            stream = QtCore.QTextStream(fh)
            stream.setCodec("UTF-8")
            self.textEdit1.setPlainText(stream.readAll())
            self.textEdit1.document().setModified(False)
        except EnvironmentError as e:
            QtGui.QMessageBox.warning(self, u"File Load Error",
                    u"Failed to load {0}: {1}".format(self.filename, e))
        finally:
            if fh is not None:
                fh.close()

    def click_search(self):
        self.search_sent = unicode(self.lineEdit1.text().toUtf8(), 'utf-8', 'ignore')
        ###TODO...
        highlight_data = []
        highlight_data.append(self.search_sent)
        self.highlighter.setHighlightData(highlight_data)
        self.highlighter.init_ret_count()
        self.highlighter.rehighlight()
        ret_count = self.highlighter.return_count()
        print 'result_count: %d' % ret_count
        ###DONE...

    def check_tips(self):
        if self.checkBox1.isChecked():
            print 'checked...'
        else:
            print 'not checked...'

    def comboBox_index_change(self):
        self.filename = unicode(self.comboBox1.currentText().toUtf8(), 'utf-8', 'ignore')
        self.filename = os.path.join('../tag/medical_texts', self.filename)
        self.click_file_load()
        self.highlighter.init_ret_count()
        self.highlighter.rehighlight()
        ret_count = self.highlighter.return_count()
        print 'result_count: %d' % ret_count
