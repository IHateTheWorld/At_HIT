QT += core gui sql

SOURCES += \
    main.cpp \
    login_dialog.cpp \
    register_dialog.cpp \
    homepage_dialog.cpp \
    experience_dialog.cpp

HEADERS += \
    connect_db.h \
    login_dialog.h \
    register_dialog.h \
    homepage_dialog.h \
    global_info.h \
    experience_dialog.h

FORMS += \
    login_dialog.ui \
    register_dialog.ui \
    homepage_dialog.ui \
    experience_dialog.ui

OTHER_FILES += \
    demon.rc

RC_FILE = \
    demon.rc
