# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'tab_widget.ui'
#
# Created: Wed Jun 21 22:12:27 2017
#      by: PyQt4 UI code generator 4.9.6
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui
from highlighter import *

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
        self.pushButton1.setGeometry(QtCore.QRect(130, 90, 111, 31))
        self.pushButton1.setStyleSheet(_fromUtf8("font: 75 italic 12pt \"Agency FB\";"))
        self.pushButton1.setObjectName(_fromUtf8("pushButton1"))
        self.textEdit1 = QtGui.QTextEdit(self.tab_1)
        self.textEdit1.setGeometry(QtCore.QRect(40, 200, 721, 321))
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
        self.label3 = QtGui.QLabel(self.tab_1)
        self.label3.setGeometry(QtCore.QRect(40, 130, 721, 31))
        self.label3.setText(_fromUtf8(""))
        self.label3.setObjectName(_fromUtf8("label3"))
        self.label3_2 = QtGui.QLabel(self.tab_1)
        self.label3_2.setGeometry(QtCore.QRect(40, 180, 81, 21))
        self.label3_2.setObjectName(_fromUtf8("label3_2"))
        self.tabWidget1.addTab(self.tab_1, _fromUtf8(""))
        self.tab_2 = QtGui.QWidget()
        self.tab_2.setObjectName(_fromUtf8("tab_2"))
        self.textEdit2 = QtGui.QTextEdit(self.tab_2)
        self.textEdit2.setGeometry(QtCore.QRect(30, 130, 721, 321))
        self.textEdit2.setStyleSheet(_fromUtf8("font: 12pt \"Agency FB\";"))
        self.textEdit2.setReadOnly(True)
        self.textEdit2.setObjectName(_fromUtf8("textEdit2"))
        self.comboBox1 = QtGui.QComboBox(self.tab_2)
        self.comboBox1.setGeometry(QtCore.QRect(30, 70, 491, 31))
        self.comboBox1.setObjectName(_fromUtf8("comboBox1"))
        self.pushButton2 = QtGui.QPushButton(self.tab_2)
        self.pushButton2.setGeometry(QtCore.QRect(660, 70, 75, 31))
        self.pushButton2.setStyleSheet(_fromUtf8("font: 75 italic 12pt \"Agency FB\";"))
        self.pushButton2.setObjectName(_fromUtf8("pushButton2"))
        self.label2 = QtGui.QLabel(self.tab_2)
        self.label2.setGeometry(QtCore.QRect(30, 21, 151, 31))
        self.label2.setObjectName(_fromUtf8("label2"))
        self.label = QtGui.QLabel(self.tab_2)
        self.label.setGeometry(QtCore.QRect(40, 470, 111, 21))
        self.label.setObjectName(_fromUtf8("label"))
        self.label_2 = QtGui.QLabel(self.tab_2)
        self.label_2.setGeometry(QtCore.QRect(250, 470, 41, 21))
        self.label_2.setStyleSheet(_fromUtf8("background-color: rgb(255, 0, 0);"))
        self.label_2.setObjectName(_fromUtf8("label_2"))
        self.label_3 = QtGui.QLabel(self.tab_2)
        self.label_3.setGeometry(QtCore.QRect(170, 470, 41, 21))
        self.label_3.setStyleSheet(_fromUtf8("background-color: rgb(0, 255, 0);"))
        self.label_3.setObjectName(_fromUtf8("label_3"))
        self.label_4 = QtGui.QLabel(self.tab_2)
        self.label_4.setGeometry(QtCore.QRect(320, 470, 41, 21))
        self.label_4.setStyleSheet(_fromUtf8("background-color: rgb(255, 255, 0);"))
        self.label_4.setObjectName(_fromUtf8("label_4"))
        self.label_5 = QtGui.QLabel(self.tab_2)
        self.label_5.setGeometry(QtCore.QRect(390, 470, 81, 21))
        self.label_5.setStyleSheet(_fromUtf8("background-color: rgb(143, 143, 143);"))
        self.label_5.setObjectName(_fromUtf8("label_5"))
        self.pushButton = QtGui.QPushButton(self.tab_2)
        self.pushButton.setGeometry(QtCore.QRect(560, 70, 75, 31))
        self.pushButton.setObjectName(_fromUtf8("pushButton"))
        self.label_6 = QtGui.QLabel(self.tab_2)
        self.label_6.setGeometry(QtCore.QRect(20, 500, 131, 21))
        self.label_6.setObjectName(_fromUtf8("label_6"))
        self.label_7 = QtGui.QLabel(self.tab_2)
        self.label_7.setGeometry(QtCore.QRect(160, 500, 71, 20))
        self.label_7.setText(_fromUtf8(""))
        self.label_7.setObjectName(_fromUtf8("label_7"))
        self.label_8 = QtGui.QLabel(self.tab_2)
        self.label_8.setGeometry(QtCore.QRect(240, 500, 61, 21))
        self.label_8.setText(_fromUtf8(""))
        self.label_8.setObjectName(_fromUtf8("label_8"))
        self.label_9 = QtGui.QLabel(self.tab_2)
        self.label_9.setGeometry(QtCore.QRect(313, 500, 61, 21))
        self.label_9.setText(_fromUtf8(""))
        self.label_9.setObjectName(_fromUtf8("label_9"))
        self.label_10 = QtGui.QLabel(self.tab_2)
        self.label_10.setGeometry(QtCore.QRect(393, 500, 71, 20))
        self.label_10.setText(_fromUtf8(""))
        self.label_10.setObjectName(_fromUtf8("label_10"))
        self.tabWidget1.addTab(self.tab_2, _fromUtf8(""))
        self.tab = QtGui.QWidget()
        self.tab.setObjectName(_fromUtf8("tab"))
        self.textEdit = QtGui.QTextEdit(self.tab)
        self.textEdit.setGeometry(QtCore.QRect(20, 80, 741, 331))
        self.textEdit.setStyleSheet(_fromUtf8("font: 12pt \"Agency FB\";"))
        self.textEdit.setObjectName(_fromUtf8("textEdit"))
        self.pushButton_2 = QtGui.QPushButton(self.tab)
        self.pushButton_2.setGeometry(QtCore.QRect(20, 10, 75, 31))
        self.pushButton_2.setObjectName(_fromUtf8("pushButton_2"))
        self.lineEdit = QtGui.QLineEdit(self.tab)
        self.lineEdit.setGeometry(QtCore.QRect(120, 10, 451, 31))
        self.lineEdit.setObjectName(_fromUtf8("lineEdit"))
        self.pushButton_3 = QtGui.QPushButton(self.tab)
        self.pushButton_3.setGeometry(QtCore.QRect(30, 420, 75, 31))
        self.pushButton_3.setObjectName(_fromUtf8("pushButton_3"))
        self.label_11 = QtGui.QLabel(self.tab)
        self.label_11.setGeometry(QtCore.QRect(30, 460, 711, 21))
        self.label_11.setObjectName(_fromUtf8("label_11"))
        self.label_12 = QtGui.QLabel(self.tab)
        self.label_12.setGeometry(QtCore.QRect(100, 490, 54, 12))
        self.label_12.setText(_fromUtf8(""))
        self.label_12.setObjectName(_fromUtf8("label_12"))
        self.label_13 = QtGui.QLabel(self.tab)
        self.label_13.setGeometry(QtCore.QRect(170, 490, 54, 12))
        self.label_13.setText(_fromUtf8(""))
        self.label_13.setObjectName(_fromUtf8("label_13"))
        self.label_14 = QtGui.QLabel(self.tab)
        self.label_14.setGeometry(QtCore.QRect(230, 490, 54, 12))
        self.label_14.setText(_fromUtf8(""))
        self.label_14.setObjectName(_fromUtf8("label_14"))
        self.pushButton_4 = QtGui.QPushButton(self.tab)
        self.pushButton_4.setGeometry(QtCore.QRect(640, 30, 111, 31))
        self.pushButton_4.setObjectName(_fromUtf8("pushButton_4"))
        self.pushButton_5 = QtGui.QPushButton(self.tab)
        self.pushButton_5.setGeometry(QtCore.QRect(390, 420, 51, 23))
        self.pushButton_5.setObjectName(_fromUtf8("pushButton_5"))
        self.pushButton_6 = QtGui.QPushButton(self.tab)
        self.pushButton_6.setGeometry(QtCore.QRect(460, 420, 51, 23))
        self.pushButton_6.setObjectName(_fromUtf8("pushButton_6"))
        self.pushButton_7 = QtGui.QPushButton(self.tab)
        self.pushButton_7.setGeometry(QtCore.QRect(530, 420, 51, 23))
        self.pushButton_7.setObjectName(_fromUtf8("pushButton_7"))
        self.pushButton_8 = QtGui.QPushButton(self.tab)
        self.pushButton_8.setGeometry(QtCore.QRect(670, 420, 75, 23))
        self.pushButton_8.setObjectName(_fromUtf8("pushButton_8"))
        self.pushButton_9 = QtGui.QPushButton(self.tab)
        self.pushButton_9.setGeometry(QtCore.QRect(600, 420, 51, 23))
        self.pushButton_9.setObjectName(_fromUtf8("pushButton_9"))
        self.checkBox = QtGui.QCheckBox(self.tab)
        self.checkBox.setGeometry(QtCore.QRect(120, 50, 131, 16))
        self.checkBox.setStyleSheet(_fromUtf8("font: 10pt \"Agency FB\";"))
        self.checkBox.setObjectName(_fromUtf8("checkBox"))
        self.label_15 = QtGui.QLabel(self.tab)
        self.label_15.setGeometry(QtCore.QRect(310, 490, 54, 12))
        self.label_15.setText(_fromUtf8(""))
        self.label_15.setObjectName(_fromUtf8("label_15"))
        self.label_16 = QtGui.QLabel(self.tab)
        self.label_16.setGeometry(QtCore.QRect(400, 490, 54, 12))
        self.label_16.setText(_fromUtf8(""))
        self.label_16.setObjectName(_fromUtf8("label_16"))
        self.label_17 = QtGui.QLabel(self.tab)
        self.label_17.setGeometry(QtCore.QRect(490, 490, 54, 12))
        self.label_17.setText(_fromUtf8(""))
        self.label_17.setObjectName(_fromUtf8("label_17"))
        self.tabWidget1.addTab(self.tab, _fromUtf8(""))
        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QtGui.QMenuBar(MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 834, 23))
        self.menubar.setObjectName(_fromUtf8("menubar"))
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QtGui.QStatusBar(MainWindow)
        self.statusbar.setObjectName(_fromUtf8("statusbar"))
        MainWindow.setStatusBar(self.statusbar)

        self.retranslateUi(MainWindow)
        self.tabWidget1.setCurrentIndex(0)
        QtCore.QObject.connect(self.pushButton1, QtCore.SIGNAL(_fromUtf8("clicked()")), MainWindow.click_ok)
        QtCore.QObject.connect(self.pushButton3, QtCore.SIGNAL(_fromUtf8("clicked()")), MainWindow.click_search)
        QtCore.QObject.connect(self.checkBox1, QtCore.SIGNAL(_fromUtf8("stateChanged(int)")), MainWindow.check_tips)
        QtCore.QObject.connect(self.comboBox1, QtCore.SIGNAL(_fromUtf8("currentIndexChanged(QString)")), MainWindow.comboBox_index_change)
        QtCore.QObject.connect(self.pushButton2, QtCore.SIGNAL(_fromUtf8("clicked()")), MainWindow.click_file_load)
        QtCore.QObject.connect(self.pushButton, QtCore.SIGNAL(_fromUtf8("clicked()")), MainWindow.click_tag_data)
        QtCore.QObject.connect(self.pushButton_2, QtCore.SIGNAL(_fromUtf8("clicked()")), MainWindow.click_load_tag_file)
        QtCore.QObject.connect(self.pushButton_4, QtCore.SIGNAL(_fromUtf8("clicked()")), MainWindow.click_save_chang)
        QtCore.QObject.connect(self.pushButton_3, QtCore.SIGNAL(_fromUtf8("clicked()")), MainWindow.click_cal_ret)
        QtCore.QObject.connect(self.pushButton_5, QtCore.SIGNAL(_fromUtf8("clicked()")), MainWindow.click_med)
        QtCore.QObject.connect(self.pushButton_6, QtCore.SIGNAL(_fromUtf8("clicked()")), MainWindow.click_dis)
        QtCore.QObject.connect(self.pushButton_7, QtCore.SIGNAL(_fromUtf8("clicked()")), MainWindow.click_sym)
        QtCore.QObject.connect(self.pushButton_8, QtCore.SIGNAL(_fromUtf8("clicked()")), MainWindow.click_clear)
        QtCore.QObject.connect(self.pushButton_9, QtCore.SIGNAL(_fromUtf8("clicked()")), MainWindow.click_tre)
        QtCore.QObject.connect(self.checkBox, QtCore.SIGNAL(_fromUtf8("stateChanged(int)")), MainWindow.check_box_pretreat)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)
        MainWindow.setTabOrder(self.pushButton1, self.lineEdit1)
        MainWindow.setTabOrder(self.lineEdit1, self.textEdit1)
        MainWindow.setTabOrder(self.textEdit1, self.tabWidget1)
        self.filename = u''
        self.highlighter1 = MyHighlighter(self.textEdit1)
        self.highlight_data_1= dict()
        self.highlighter2 = MyHighlighter(self.textEdit2)
        self.highlight_data_2 = dict()
        self.search_sent = u''
        self.tag_file_name = u''

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(_translate("MainWindow", "医学文献内容检索系统v1.0", None))
        self.label1.setText(_translate("MainWindow", "<html><head/><body><p><span style=\" font-size:20pt; font-style:italic;\">输入查询语句</span></p></body></html>", None))
        self.pushButton1.setText(_translate("MainWindow", "分析查询语句", None))
        self.pushButton3.setText(_translate("MainWindow", " 查找", None))
        self.checkBox1.setText(_translate("MainWindow", "提示性提问", None))
        self.label3_2.setText(_translate("MainWindow", "查找结果：", None))
        self.tabWidget1.setTabText(self.tabWidget1.indexOf(self.tab_1), _translate("MainWindow", "查询", None))
        self.pushButton2.setText(_translate("MainWindow", "高亮显示", None))
        self.label2.setText(_translate("MainWindow", "<html><head/><body><p><span style=\" font-size:20pt;\">选取文件：</span></p></body></html>", None))
        self.label.setText(_translate("MainWindow", "提示颜色信息：", None))
        self.label_2.setText(_translate("MainWindow", "疾病", None))
        self.label_3.setText(_translate("MainWindow", "药物", None))
        self.label_4.setText(_translate("MainWindow", "症状", None))
        self.label_5.setText(_translate("MainWindow", "手术和检查", None))
        self.pushButton.setText(_translate("MainWindow", "标注数据", None))
        self.label_6.setText(_translate("MainWindow", "文献中实体个数：", None))
        self.tabWidget1.setTabText(self.tabWidget1.indexOf(self.tab_2), _translate("MainWindow", "命名实体", None))
        self.pushButton_2.setText(_translate("MainWindow", "载入文件", None))
        self.pushButton_3.setText(_translate("MainWindow", "计算结果", None))
        self.label_11.setText(_translate("MainWindow", "测试结果：    P                    R                    F                    n_recall                    n_gold                    n_pred", None))
        self.pushButton_4.setText(_translate("MainWindow", "保存修改文件", None))
        self.pushButton_5.setText(_translate("MainWindow", "<MED>", None))
        self.pushButton_6.setText(_translate("MainWindow", "<DIS>", None))
        self.pushButton_7.setText(_translate("MainWindow", "<SYM>", None))
        self.pushButton_8.setText(_translate("MainWindow", "CLEAR", None))
        self.pushButton_9.setText(_translate("MainWindow", "<TRE>", None))
        self.checkBox.setText(_translate("MainWindow", "命名实体预处理", None))
        self.tabWidget1.setTabText(self.tabWidget1.indexOf(self.tab), _translate("MainWindow", "标注+测试", None))
        self.set_combox_items(item_list=[], is_init=True)
        retrieve.drive_start()
        self.setWindowIcon(QtGui.QIcon('images/heart.ico'))

    def set_combox_items(self, item_list, is_init=False):
        self.comboBox1.clear()
        if is_init == True:
            item_list = os.listdir('./tag/medical_texts')
            for item_i in range(len(item_list)):
                item_list[item_i] = item_list[item_i].decode(encode_type)
                # self.comboBox1.setItemText(item_i, _translate("MainWindow", _fromUtf8(item_list[item_i]), None))
        self.comboBox1.insertItems(0, item_list)
        self.filename = unicode(self.comboBox1.currentText().toUtf8(), 'utf-8', 'ignore')
        self.filename = os.path.join('./tag/medical_texts', self.filename)
        self.load_file()

    def click_ok(self):
        self.search_sent = unicode(self.lineEdit1.text().toUtf8(), 'utf-8', 'ignore')
        seg_sent = ner_drive.analysis(self.search_sent)
        self.label3.setText(_translate("MainWindow", seg_sent, None))
        match_ret = retrieve.match_pt(seg_sent)
        self.highlighter1.setHighlightData(self.highlight_data_1)
        self.highlighter1.init_ret_count()
        self.highlighter1.rehighlight()

    def click_search(self):
        self.search_sent = unicode(self.lineEdit1.text().toUtf8(), 'utf-8', 'ignore')
        # if len(self.search_sent) == 0:
            # self.search_sent = u'轻中度高血压应该使用什么药物治疗？'
        seg_sent = ner_drive.analysis(self.search_sent)
        self.label3.setText(_translate("MainWindow", seg_sent, None))
        match_ret = retrieve.match_pt(seg_sent)
        self.textEdit1.setPlainText("="*10+(u"共查到%d个结果！" % len(match_ret))+"="*10)
        match_no = 0
        for match_i in match_ret:
            match_no += 1
            self.textEdit1.append(">"*17+(u"第%d个结果：" % match_no))
            raw_string, ne_flags = retrieve.get_raw_str(match_i[0])
            #self.highlight_data_1.update(ne_flags) #there may have some problems
            self.highlight_data_1 = retrieve.comb2dict(ne_flags, self.highlight_data_1)
            file_info = match_i[1]
            self.textEdit1.append(u"原句："+raw_string)
            self.textEdit1.append(u"出处："+file_info)
        #print self.highlight_data_1
        self.highlighter1.setHighlightData(self.highlight_data_1)
        self.highlighter1.init_ret_count()
        self.highlighter1.rehighlight()

    def clean_flag_num(self):
        self.label_7.setText(_translate("MainWindow", '0', None))
        self.label_8.setText(_translate("MainWindow", '0', None))
        self.label_9.setText(_translate("MainWindow", '0', None))
        self.label_10.setText(_translate("MainWindow", '0', None))
        self.highlight_data_2 = dict()
        self.highlighter2.setHighlightData(self.highlight_data_2)
        self.highlighter2.rehighlight()

    def click_file_load(self):
        text = unicode(self.comboBox1.currentText().toUtf8(), 'utf-8', 'ignore')
        count_dict = retrieve.ner_one_file_count(text)
        for k, v in count_dict.iteritems():
            if k == '<MED>':
                self.label_7.setText(_translate("MainWindow", str(len(v)), None))
            elif k == '<DIS>':
                self.label_8.setText(_translate("MainWindow", str(len(v)), None))
            elif k == '<SYM>':
                self.label_9.setText(_translate("MainWindow", str(len(v)), None))
            elif k == '<TRE>':
                self.label_10.setText(_translate("MainWindow", str(len(v)), None))
        self.highlighter2.setHighlightData(count_dict)
        self.highlighter2.init_ret_count()
        self.highlighter2.rehighlight()
        ret_count = self.highlighter2.return_count()

    def load_file(self):
        fh = None
        try:
            fh = QtCore.QFile(self.filename)
            if not fh.open(QtCore.QIODevice.ReadOnly): #There is a big problem
                raise IOError(str(fh.errorString()))
            stream = QtCore.QTextStream(fh)
            stream.setCodec("UTF-8")
            self.textEdit2.setPlainText(stream.readAll())
            self.textEdit2.document().setModified(False)
        except EnvironmentError as e:
            QtGui.QMessageBox.warning(self, u"文件加载错误",
                    u"Failed to load {0}: {1}".format(self.filename, e))
        finally:
            if fh is not None:
                fh.close()

    def check_tips(self):
        if self.checkBox1.isChecked():
            print 'checked...'
        else:
            print 'not checked...'

    def comboBox_index_change(self):
        self.filename = unicode(self.comboBox1.currentText().toUtf8(), 'utf-8', 'ignore')
        self.filename = os.path.join('./tag/medical_texts', self.filename)
        self.load_file()
        self.clean_flag_num()

    def click_tag_data(self):
        self.filename = unicode(self.comboBox1.currentText().toUtf8(), 'utf-8', 'ignore')
        self.filename = os.path.join('./tag/after_tag', self.filename+'_ner')
        self.load_file()
        self.clean_flag_num()

    def documentWasModified(self):
        is_modified = self.textEdit.document().isModified()
        return is_modified #is the BOOL type

    def select_data(self):
        textCursor = self.textEdit.textCursor()
        lineContent = unicode(textCursor.selectedText())
        return lineContent

    def click_load_tag_file(self):
        self.tag_file_name = QtGui.QFileDialog.getOpenFileName(self,u"加载标注文件窗口","C:\Users\Administrator\Desktop","All Files (*);;Text Files (*.txt)")
        self.lineEdit.setText(self.tag_file_name)
        # if os.path.isfile(self.tag_file_name):
            # text = open(self.tag_file_name).read()
            # self.textEdit.setText(text)
        print self.checkBox.isChecked()
        if (self.checkBox.isChecked()) == False:
            fh = None
            try:
                fh = QtCore.QFile(self.tag_file_name)
                if not fh.open(QtCore.QIODevice.ReadOnly): #There is a big problem
                    raise IOError(str(fh.errorString()))
                stream = QtCore.QTextStream(fh)
                stream.setCodec("UTF-8")
                self.textEdit.setPlainText(stream.readAll())
                self.textEdit.document().setModified(False)
            except EnvironmentError as e:
                QtGui.QMessageBox.warning(self, u"文件加载错误",
                        u"Failed to load {0}: {1}".format(self.tag_file_name, e))
            finally:
                if fh is not None:
                    fh.close()
        else:
            file_path = unicode(self.tag_file_name.toUtf8(), 'utf-8', 'ignore')
            fp_in = file(file_path, 'rb')
            self.textEdit.setPlainText(u'')
            for line in fp_in:
                line = (line.strip()).decode('UTF-8')
                if line[0] == u'\ufeff': #Attention point
                    line = line[1:]
                tag_line = ner_drive.analysis(line, True)
                self.textEdit.append(tag_line)
            fp_in.close()

    def click_save_chang(self):
        file_name = unicode(self.tag_file_name.toUtf8(), 'utf-8', 'ignore')
        if len(re.findall(r'.*_tag$', file_name)) > 0:
            file_path =  QtGui.QFileDialog.getSaveFileName(self,u'保存文件',self.tag_file_name ,"All Files (*);;Text Files (*.txt)")
        else:
            file_path =  QtGui.QFileDialog.getSaveFileName(self,u'保存文件',self.tag_file_name+'_tag' ,"All Files (*);;Text Files (*.txt)")
        file_path = unicode(file_path.toUtf8(), 'utf-8', 'ignore')
        fp = open(file_path, "w")
        fp.write((unicode((self.textEdit.toPlainText()).toUtf8(), 'utf-8', 'ignore')).encode('UTF-8'))
        fp.close()

    def tran_fun(self, tran_str):
        sd = self.select_data()
        textCursor = self.textEdit.textCursor()
        textCursor.removeSelectedText()
        pos = textCursor.position()
        textCursor.setPosition(pos)
        new_str = u'[[' + sd + tran_str + ']]'
        textCursor.insertText(new_str)

    def clear_fun(self):
        sd = self.select_data()
        textCursor = self.textEdit.textCursor()
        textCursor.removeSelectedText()
        pos = textCursor.position()
        textCursor.setPosition(pos)
        sd = sd.replace('[[', '')
        sd = re.sub('<.*>]]', '', sd, count=0, flags=0)
        textCursor.insertText(sd)

    def click_cal_ret(self):
        p, r, f, n_recall, n_gold, n_pred = eval_drive.eval_one_time()
        self.label_12.setText(_translate("MainWindow", ('%.2f%%'%(p*100)), None))
        self.label_13.setText(_translate("MainWindow", ('%.2f%%'%(r*100)), None))
        self.label_14.setText(_translate("MainWindow", ('%.2f%%'%(f*100)), None))
        self.label_15.setText(_translate("MainWindow", str(n_recall), None))
        self.label_16.setText(_translate("MainWindow", str(n_gold), None))
        self.label_17.setText(_translate("MainWindow", str(n_pred), None))

    def click_med(self):
        self.tran_fun('<MED>')
    
    def click_dis(self):
        self.tran_fun('<DIS>')
    
    def click_sym(self):
        self.tran_fun('<SYM>')
    
    def click_tre(self):
        self.tran_fun('<TRE>')
    
    def click_clear(self):
        self.clear_fun()
    
    def check_box_pretreat(self):
        if self.checkBox.isChecked():
            print 'checked...'
        else:
            print 'not checked...'

class SearchUI(QtGui.QMainWindow, Ui_MainWindow):
    def __init__(self, parent=None):
        super(SearchUI, self).__init__(parent)
        self.setupUi(self)

