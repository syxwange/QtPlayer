/********************************************************************************
** Form generated from reading UI file 'QtPlayer.ui'
**
** Created by: Qt User Interface Compiler version 5.12.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTPLAYER_H
#define UI_QTPLAYER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QtPlayerClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *QtPlayerClass)
    {
        if (QtPlayerClass->objectName().isEmpty())
            QtPlayerClass->setObjectName(QString::fromUtf8("QtPlayerClass"));
        QtPlayerClass->resize(600, 400);
        menuBar = new QMenuBar(QtPlayerClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        QtPlayerClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(QtPlayerClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        QtPlayerClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(QtPlayerClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        QtPlayerClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(QtPlayerClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        QtPlayerClass->setStatusBar(statusBar);

        retranslateUi(QtPlayerClass);

        QMetaObject::connectSlotsByName(QtPlayerClass);
    } // setupUi

    void retranslateUi(QMainWindow *QtPlayerClass)
    {
        QtPlayerClass->setWindowTitle(QApplication::translate("QtPlayerClass", "QtPlayer", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QtPlayerClass: public Ui_QtPlayerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTPLAYER_H
