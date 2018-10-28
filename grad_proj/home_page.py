# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'home_page.ui'
#
# Created: Wed Jun 28 23:31:35 2017
#      by: PyQt4 UI code generator 4.9.6
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui
import tab_widget

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
        MainWindow.resize(800, 600)
        self.centralwidget = QtGui.QWidget(MainWindow)
        self.centralwidget.setObjectName(_fromUtf8("centralwidget"))
        self.pushButton = QtGui.QPushButton(self.centralwidget)
        self.pushButton.setGeometry(QtCore.QRect(300, 250, 131, 61))
        self.pushButton.setStyleSheet(_fromUtf8("font: 75 italic 14pt \"Agency FB\";"))
        self.pushButton.setObjectName(_fromUtf8("pushButton"))
        self.label = QtGui.QLabel(self.centralwidget)
        self.label.setGeometry(QtCore.QRect(230, 90, 281, 71))
        self.label.setStyleSheet(_fromUtf8("font: 75 20pt \"黑体\";"))
        self.label.setObjectName(_fromUtf8("label"))
        self.label_2 = QtGui.QLabel(self.centralwidget)
        self.label_2.setGeometry(QtCore.QRect(480, 460, 311, 16))
        self.label_2.setStyleSheet(_fromUtf8("font: 12pt \"隶书\";"))
        self.label_2.setObjectName(_fromUtf8("label_2"))
        self.label_3 = QtGui.QLabel(self.centralwidget)
        self.label_3.setGeometry(QtCore.QRect(590, 500, 54, 12))
        self.label_3.setStyleSheet(_fromUtf8("font: 12pt \"Times New Roman\";"))
        self.label_3.setObjectName(_fromUtf8("label_3"))
        self.label_4 = QtGui.QLabel(self.centralwidget)
        self.label_4.setGeometry(QtCore.QRect(530, 110, 91, 31))
        self.label_4.setStyleSheet(_fromUtf8("font: 20pt \"Times New Roman\";"))
        self.label_4.setObjectName(_fromUtf8("label_4"))
        self.label_5 = QtGui.QLabel(self.centralwidget)
        self.label_5.setGeometry(QtCore.QRect(550, 460, 16, 21))
        self.label_5.setStyleSheet(_fromUtf8("font: 20pt \"宋体\";\n"
"font: 20pt \"楷体\";"))
        self.label_5.setObjectName(_fromUtf8("label_5"))
        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QtGui.QMenuBar(MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 800, 23))
        self.menubar.setObjectName(_fromUtf8("menubar"))
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QtGui.QStatusBar(MainWindow)
        self.statusbar.setObjectName(_fromUtf8("statusbar"))
        MainWindow.setStatusBar(self.statusbar)

        self.retranslateUi(MainWindow)
        QtCore.QObject.connect(self.pushButton, QtCore.SIGNAL(_fromUtf8("clicked()")), MainWindow.click_enter)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(_translate("MainWindow", "医学文献内容检索系统v1.0", None))
        self.pushButton.setText(_translate("MainWindow", "点击进入系统", None))
        self.label.setText(_translate("MainWindow", "医学文献内容检索系统", None))
        self.label_2.setText(_translate("MainWindow", "版权所有   哈尔滨福满科技责任有限公司", None))
        self.label_3.setText(_translate("MainWindow", "2017-07", None))
        self.label_4.setText(_translate("MainWindow", "V 1.0", None))
        self.label_5.setText(_translate("MainWindow", "©", None))
        self.setWindowIcon(QtGui.QIcon('images/heart.ico'))
        self.search_ui = tab_widget.SearchUI()

    def click_enter(self):
        self.hide() #close the home_page
        self.search_ui.show()

