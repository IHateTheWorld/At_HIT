# -*- coding: utf-8 -*-

import pynlpir
import sys
import re
import os
import time
from PyQt4 import QtCore, QtGui, uic
import tab_widget, home_page

#decode: str ==> unicode
#encode: unicode ==> str
encode_type = sys.getfilesystemencoding() #UTF-8 in my machine

class SearchUI(QtGui.QMainWindow, tab_widget.Ui_MainWindow):
    def __init__(self, parent=None):
        ###QtGui.QMainWindow.__init__(self)
        ###tab_widget.Ui_MainWindow.__init__(self)
        super(SearchUI, self).__init__(parent)
        self.setupUi(self)

class HomePageUI(QtGui.QMainWindow, home_page.Ui_MainWindow):
    def __init__(self, parent=None):
        super(HomePageUI, self).__init__(parent)
        self.setupUi(self)

if __name__ == "__main__":
    app = QtGui.QApplication(sys.argv)
    home_ui = HomePageUI()
    home_ui.show()
    sys.exit(app.exec_())
