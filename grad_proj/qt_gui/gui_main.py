# -*- coding: utf-8 -*-

import pynlpir
import sys
import re
import os
import time
from PyQt4 import QtCore, QtGui, uic
import tab_widget

#decode: str ==> unicode
#encode: unicode ==> str
encode_type = sys.getfilesystemencoding() #UTF-8 in my machine

class SearchUI(QtGui.QMainWindow, tab_widget.Ui_MainWindow):
    def __init__(self, parent=None):
        ###QtGui.QMainWindow.__init__(self)
        ###tab_widget.Ui_MainWindow.__init__(self)
        super(SearchUI, self).__init__(parent)
        self.setupUi(self)

if __name__ == "__main__":
    app = QtGui.QApplication(sys.argv)
    search_ui = SearchUI()
    search_ui.show()
    sys.exit(app.exec_())
