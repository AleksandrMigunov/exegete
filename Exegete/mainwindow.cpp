/*
Copyright (C) 2021 Aleksandr Migunov

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QPrinter>
#include <QPrintDialog>
#include <QInputDialog>
#include <QFont>
#include <QFontDialog>
#include <QColor>
#include <QColorDialog>
#include <QPalette>
#include <QDateTime>
#include <QPushButton>
#include <QTextDocument>
#include <QTextDocumentWriter>
#include <QPrintPreviewDialog>
#include <QPainter>
#include <QSettings>
#include <QDesktopWidget>
#include <QCloseEvent>
#include <QDir>
#include <QTextCodec>
#include <QByteArray>
#include "converter.h"
#include "dialogfind.h"
#include "dialogreplace.h"
#include "dialogreplaceall.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ZoomList = ZoomOriginal = ZoomTargetText = ZoomBaseText = 0;
    ZoomAdditional1 = ZoomAdditional2 = 0;
    EncodingList = EncodingOriginal = EncodingTargetText = EncodingBaseText = EncodingHTML = "UTF-8";
    EncodingAdditional1 = EncodingAdditional2 = "UTF-8";
    ParatextFileEditing = false;
    hideLabelDirs();
    showDirs = false;
    m_settings = new QSettings("exegete_settings.ini", QSettings::IniFormat, this);
    readSettings();

    if (!FilesNotLoadAuto) {
        open_files_at_start();
    }

    textEdit_focused("List");
    set_zoom_at_start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// /////////////////////////////////////////
//Functions at start and exit
// /////////////////////////////////////////

//reading settings at start
void MainWindow::readSettings() {
    QDesktopWidget desk;
    QRect rect = desk.availableGeometry();
    m_settings->beginGroup("MainWindow");
    QPoint pos = m_settings->value("pos", QPoint(0, 0)).toPoint();
    QSize size = m_settings->value("size", rect.size()).toSize();
    if (pos.x() < 0) {
        pos.setX(0);
    }
    if (pos.y() < 0) {
        pos.setY(0);
    }
    if (size.width() > rect.width()) {
        size.setWidth(rect.width());
    }
    if (size.height() > rect.height()) {
        size.setHeight(rect.height());
    }

    resize(size);
    move(pos);

    FilenameList = m_settings->value("list", list).toString();
    FilenameOriginal = m_settings->value("original", original).toString();
    FilenameTargetText = m_settings->value("target_text", target_text).toString();
    FilenameBaseText = m_settings->value("base_text", base_text).toString();
    FilenameAdditional1 = m_settings->value("additional_1", additional_1).toString();
    FilenameAdditional2 = m_settings->value("additional_2", additional_2).toString();

    language = m_settings->value("language", lang).toString();
    TimeAuto = m_settings->value("time_setting", time_auto).toBool();
    TitleAuto = m_settings->value("title_setting", title_auto).toBool();
    FilesNotLoadAuto = m_settings->value("files_not_loading", files_not_load_auto).toBool();

    ZoomList = m_settings->value("zoom_list", zoom_list).toInt();
    ZoomOriginal = m_settings->value("zoom_original", zoom_original).toInt();
    ZoomTargetText = m_settings->value("zoom_target_text", zoom_target_text).toInt();
    ZoomBaseText = m_settings->value("zoom_base_text", zoom_base_text).toInt();
    ZoomAdditional1 = m_settings->value("zoom_additional_1", zoom_additional_1).toInt();
    ZoomAdditional2 = m_settings->value("zoom_additional_2", zoom_additional_2).toInt();
    m_settings->endGroup();
}

//writing settings at finish
void MainWindow::writeSettings() {
    m_settings->beginGroup("MainWindow");
    m_settings->setValue("pos", pos());
    m_settings->setValue("size", size());

    m_settings->setValue("list", FilenameList);
    m_settings->setValue("original", FilenameOriginal);
    m_settings->setValue("target_text", FilenameTargetText);
    m_settings->setValue("base_text", FilenameBaseText);
    m_settings->setValue("additional_1", FilenameAdditional1);
    m_settings->setValue("additional_2", FilenameAdditional2);

    m_settings->setValue("language", language);
    m_settings->setValue("time_setting", TimeAuto);
    m_settings->setValue("title_setting", TitleAuto);
    m_settings->setValue("files_not_loading", FilesNotLoadAuto);

    m_settings->setValue("zoom_list", ZoomList);
    m_settings->setValue("zoom_original", ZoomOriginal);
    m_settings->setValue("zoom_target_text", ZoomTargetText);
    m_settings->setValue("zoom_base_text", ZoomBaseText);
    m_settings->setValue("zoom_additional_1", ZoomAdditional1);
    m_settings->setValue("zoom_additional_2", ZoomAdditional2);

    m_settings->endGroup();
    m_settings->sync();
}

//setting zoom in or zoom out at start
void MainWindow::set_zoom_at_start() {
    if (ZoomList > 0) {
        for (int n = 0; n < ZoomList; n++) {
            ui->textEdit_List->zoomIn();
        }
    }
    else if (ZoomList < 0) {
        for (int n = 0; n > ZoomList; n--) {
            ui->textEdit_List->zoomOut();
        }
    }

    if (ZoomOriginal > 0) {
        for (int n = 0; n < ZoomOriginal; n++) {
            ui->textEdit_Original->zoomIn();
        }
    }
    else if (ZoomOriginal < 0) {
        for (int n = 0; n > ZoomOriginal; n--) {
            ui->textEdit_Original->zoomOut();
        }
    }

    if (ZoomTargetText > 0) {
        for (int n = 0; n < ZoomTargetText; n++) {
            ui->textEdit_TargetText->zoomIn();
        }
    }
    else if (ZoomTargetText < 0) {
        for (int n = 0; n > ZoomTargetText; n--) {
            ui->textEdit_TargetText->zoomOut();
        }
    }

    if (ZoomBaseText > 0) {
        for (int n = 0; n < ZoomBaseText; n++) {
            ui->textEdit_BaseText->zoomIn();
        }
    }
    else if (ZoomBaseText < 0) {
        for (int n = 0; n > ZoomBaseText; n--) {
            ui->textEdit_BaseText->zoomOut();
        }
    }

    if (ZoomAdditional1 > 0) {
        for (int n = 0; n < ZoomAdditional1; n++) {
            ui->textEdit_Additional1->zoomIn();
        }
    }
    else if (ZoomAdditional1 < 0) {
        for (int n = 0; n > ZoomAdditional1; n--) {
            ui->textEdit_Additional1->zoomOut();
        }
    }

    if (ZoomAdditional2 > 0) {
        for (int n = 0; n < ZoomAdditional2; n++) {
            ui->textEdit_Additional2->zoomIn();
        }
    }
    else if (ZoomAdditional2 < 0) {
        for (int n = 0; n > ZoomAdditional2; n--) {
            ui->textEdit_Additional2->zoomOut();
        }
    }
}

//closing the program
void MainWindow::closeEvent(QCloseEvent *event) {
    if (ParatextFileEditing == false) {
        close_list();
    }

    QMessageBox* messagebox = new QMessageBox (QMessageBox::Question,
                                               tr("Confirm"), tr("Exit program?"),
                                               QMessageBox::Yes | QMessageBox::No, this);
    messagebox->setButtonText(QMessageBox::Yes, tr("Yes"));
    messagebox->setButtonText(QMessageBox::No, tr("No"));
    int n = messagebox->exec();
    delete messagebox;

    if (n == QMessageBox::Yes) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

//setting highlighted textEdit
//all the methods that affect textEdits will call this method in the beginning
void MainWindow::textEdit_focused(QString textEditName) {
    //setting the chosen textEdit and corresponding label as highlighted
    if (textEditName == "List") {
        ui->textEdit_List->setFrameStyle(QFrame::WinPanel | QFrame::Plain);
        ui->label_List->setStyleSheet("color: rgb(255, 255, 255);background-color: rgb(0, 0, 255);");
//        ui->label_dir_list->setStyleSheet("color: rgb(255, 255, 255);background-color: rgb(0, 0, 255);");
    }
    else if (textEditName == "Original") {
        ui->textEdit_Original->setFrameStyle(QFrame::WinPanel | QFrame::Plain);
        ui->label_Original->setStyleSheet("color: rgb(255, 255, 255);background-color: rgb(0, 0, 255);");
//        ui->label_dir_original->setStyleSheet("color: rgb(255, 255, 255);background-color: rgb(0, 0, 255);");
    }
    else if (textEditName == "TargetText") {
        ui->textEdit_TargetText->setFrameStyle(QFrame::WinPanel | QFrame::Plain);
        ui->label_TargetText->setStyleSheet("color: rgb(255, 255, 255);background-color: rgb(0, 0, 255);");
//        ui->label_dir_target->setStyleSheet("color: rgb(255, 255, 255);background-color: rgb(0, 0, 255);");
    }
    else if (textEditName == "BaseText") {
        ui->textEdit_BaseText->setFrameStyle(QFrame::WinPanel | QFrame::Plain);
        ui->label_BaseText->setStyleSheet("color: rgb(255, 255, 255);background-color: rgb(0, 0, 255);");
//        ui->label_dir_base->setStyleSheet("color: rgb(255, 255, 255);background-color: rgb(0, 0, 255);");
    }

    else if (textEditName == "Additional1") {
        ui->textEdit_Additional1->setFrameStyle(QFrame::WinPanel | QFrame::Plain);
        ui->label_Additional1->setStyleSheet("color: rgb(255, 255, 255);background-color: rgb(0, 0, 255);");
//        ui->label_dir_add1->setStyleSheet("color: rgb(255, 255, 255);background-color: rgb(0, 0, 255);");
    }
    else if (textEditName == "Additional2") {
        ui->textEdit_Additional2->setFrameStyle(QFrame::WinPanel | QFrame::Plain);
        ui->label_Additional2->setStyleSheet("color: rgb(255, 255, 255);background-color: rgb(0, 0, 255);");
//        ui->label_dir_add2->setStyleSheet("color: rgb(255, 255, 255);background-color: rgb(0, 0, 255);");
    }

    //unsetting highlightening of others
    if (textEditName != "List") {
        ui->textEdit_List->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
        ui->label_List->setStyleSheet("color: rgb(0, 0, 0);background-color: rgb(240, 240, 240);");
//        ui->label_dir_list->setStyleSheet("color: rgb(0, 0, 0);background-color: rgb(240, 240, 240);");
    }
    if (textEditName != "Original") {
        ui->textEdit_Original->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
        ui->label_Original->setStyleSheet("color: rgb(0, 0, 0);background-color: rgb(240, 240, 240);");
//        ui->label_dir_original->setStyleSheet("color: rgb(0, 0, 0);background-color: rgb(240, 240, 240);");
    }
    if (textEditName != "TargetText") {
        ui->textEdit_TargetText->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
        ui->label_TargetText->setStyleSheet("color: rgb(0, 0, 0);background-color: rgb(240, 240, 240);");
//        ui->label_dir_target->setStyleSheet("color: rgb(0, 0, 0);background-color: rgb(240, 240, 240);");
    }
    if (textEditName != "BaseText") {
        ui->textEdit_BaseText->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
        ui->label_BaseText->setStyleSheet("color: rgb(0, 0, 0);background-color: rgb(240, 240, 240);");
//        ui->label_dir_base->setStyleSheet("color: rgb(0, 0, 0);background-color: rgb(240, 240, 240);");
    }
    if (textEditName != "Additional1") {
        ui->textEdit_Additional1->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
        ui->label_Additional1->setStyleSheet("color: rgb(0, 0, 0);background-color: rgb(240, 240, 240);");
//        ui->label_dir_add1->setStyleSheet("color: rgb(0, 0, 0);background-color: rgb(240, 240, 240);");
    }
    if (textEditName != "Additional2") {
        ui->textEdit_Additional2->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
        ui->label_Additional2->setStyleSheet("color: rgb(0, 0, 0);background-color: rgb(240, 240, 240);");
//        ui->label_dir_add2->setStyleSheet("color: rgb(0, 0, 0);background-color: rgb(240, 240, 240);");
    }

}

//opening files that were previously opened when the program starts
//their paths and names are stored in FilenameList, FilenameOriginal, FilenameTargetText, and FilenameBaseText
void MainWindow::open_files_at_start() {
    //opening list
    if (FilenameList != "") {
        open_list(FilenameList);
    }

    else {
        if (TitleAuto) {
            on_actionInsert_list_title_triggered();
        }
    }

    //opening original
    if (FilenameOriginal != "") {
        open_original(FilenameOriginal);
    }

    //opening target text
    if (FilenameTargetText != "") {
        open_target_text(FilenameTargetText);
    }

    //opening base text
    if (FilenameBaseText != "") {
        open_base_text(FilenameBaseText);
    }

    //opening additional text 1
    if (FilenameAdditional1 != "") {
        open_additional_1(FilenameAdditional1);
    }

    //opening additional text 2
    if (FilenameAdditional2 != "") {
        open_additional_2(FilenameAdditional2);
    }

}

//opening list file
void MainWindow::open_list(QString file_list) {
    QFile FileList(file_list);
    if(FileList.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream in(&FileList);

        if (EncodingList == "UTF-8") in.setCodec("UTF-8");
        else if (EncodingList == "Unicode") in.setCodec("Unicode");
        else if (EncodingList == "Windows-1251") in.setCodec("Windows-1251");
        else if (EncodingList == "KOI8-R") in.setCodec("KOI8-R");
        else if (EncodingList == "IBM866") in.setCodec("IBM866");
        else if (EncodingList == "ISO 8859-5") in.setCodec("ISO 8859-5");
        else if (EncodingList == "Windows-1250") in.setCodec("Windows-1250");
        else if (EncodingList == "Windows-1252") in.setCodec("Windows-1252");
        else if (EncodingList == "Windows-1253") in.setCodec("Windows-1253");
        else if (EncodingList == "Windows-1254") in.setCodec("Windows-1254");
        else if (EncodingList == "Windows-1255") in.setCodec("Windows-1255");
        else if (EncodingList == "Windows-1256") in.setCodec("Windows-1256");
        else if (EncodingList == "Windows-1257") in.setCodec("Windows-1257");
        else if (EncodingList == "ISO 8859-1") in.setCodec("ISO 8859-1");
        else if (EncodingList == "ISO 8859-2") in.setCodec("ISO 8859-2");
        else if (EncodingList == "ISO 8859-3") in.setCodec("ISO 8859-3");
        else if (EncodingList == "ISO 8859-4") in.setCodec("ISO 8859-4");
        else if (EncodingList == "ISO 8859-9") in.setCodec("ISO 8859-9");
        else if (EncodingList == "ISO 8859-10") in.setCodec("ISO 8859-10");
        else if (EncodingList == "ISO 8859-13") in.setCodec("ISO 8859-13");
        else if (EncodingList == "ISO 8859-14") in.setCodec("ISO 8859-14");
        else if (EncodingList == "ISO 8859-15") in.setCodec("ISO 8859-15");
        else if (EncodingList == "ISO 8859-16") in.setCodec("ISO 8859-16");
        else if (EncodingList == "ISO 8859-6") in.setCodec("ISO 8859-6");
        else if (EncodingList == "ISO 8859-7") in.setCodec("ISO 8859-7");
        else if (EncodingList == "ISO 8859-8") in.setCodec("ISO 8859-8");
        else in.setCodec("UTF-8");

        QString text = in.readAll();
        FileList.close();

        if (FilenameList.endsWith(".html") || FilenameList.endsWith(".htm")) {
            ui->textEdit_List->setAcceptRichText(true);
        }
        else {
            ui->textEdit_List->setAcceptRichText(false);
        }
        ui->textEdit_List->setText(text);
        TextList = text;

        //moving cursor to the end of list
        if (!(FilenameList.endsWith(".html") || FilenameList.endsWith(".htm"))) {
            QTextCursor cursor = ui->textEdit_List->textCursor();
            int pos = text.length();
            cursor.setPosition(pos);
            ui->textEdit_List->setTextCursor(cursor);
            ui->textEdit_List->insertPlainText("\n");
            TextList = text + "\n";
        }
        if (TimeAuto) {
            on_actionInsert_time_of_work_start_triggered();
        }

        ui->label_dir_list->setText(FilenameList);
    }
}

//opening original file
void MainWindow::open_original(QString file_original) {
    QFile FileOriginal(file_original);
    if(FileOriginal.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream in(&FileOriginal);

        if (EncodingOriginal == "UTF-8") in.setCodec("UTF-8");
        else if (EncodingOriginal == "Unicode") in.setCodec("Unicode");
        else if (EncodingOriginal == "Windows-1251") in.setCodec("Windows-1251");
        else if (EncodingOriginal == "KOI8-R") in.setCodec("KOI8-R");
        else if (EncodingOriginal == "IBM866") in.setCodec("IBM866");
        else if (EncodingOriginal == "ISO 8859-5") in.setCodec("ISO 8859-5");
        else if (EncodingOriginal == "Windows-1250") in.setCodec("Windows-1250");
        else if (EncodingOriginal == "Windows-1252") in.setCodec("Windows-1252");
        else if (EncodingOriginal == "Windows-1253") in.setCodec("Windows-1253");
        else if (EncodingOriginal == "Windows-1254") in.setCodec("Windows-1254");
        else if (EncodingOriginal == "Windows-1255") in.setCodec("Windows-1255");
        else if (EncodingOriginal == "Windows-1256") in.setCodec("Windows-1256");
        else if (EncodingOriginal == "Windows-1257") in.setCodec("Windows-1257");
        else if (EncodingOriginal == "ISO 8859-1") in.setCodec("ISO 8859-1");
        else if (EncodingOriginal == "ISO 8859-2") in.setCodec("ISO 8859-2");
        else if (EncodingOriginal == "ISO 8859-3") in.setCodec("ISO 8859-3");
        else if (EncodingOriginal == "ISO 8859-4") in.setCodec("ISO 8859-4");
        else if (EncodingOriginal == "ISO 8859-9") in.setCodec("ISO 8859-9");
        else if (EncodingOriginal == "ISO 8859-10") in.setCodec("ISO 8859-10");
        else if (EncodingOriginal == "ISO 8859-13") in.setCodec("ISO 8859-13");
        else if (EncodingOriginal == "ISO 8859-14") in.setCodec("ISO 8859-14");
        else if (EncodingOriginal == "ISO 8859-15") in.setCodec("ISO 8859-15");
        else if (EncodingOriginal == "ISO 8859-16") in.setCodec("ISO 8859-16");
        else if (EncodingOriginal == "ISO 8859-6") in.setCodec("ISO 8859-6");
        else if (EncodingOriginal == "ISO 8859-7") in.setCodec("ISO 8859-7");
        else if (EncodingOriginal == "ISO 8859-8") in.setCodec("ISO 8859-8");
        else in.setCodec("UTF-8");

        QString text = in.readAll();

        FileOriginal.close();
        ui->textEdit_Original->setText(text);

        ui->label_dir_original->setText(FilenameOriginal);
    }
}

//opening target text
void MainWindow::open_target_text(QString file_target_text) {
    QFile FileTargetText(file_target_text);
    if(FileTargetText.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream in(&FileTargetText);

        if (EncodingTargetText == "UTF-8") in.setCodec("UTF-8");
        else if (EncodingTargetText == "Unicode") in.setCodec("Unicode");
        else if (EncodingTargetText == "Windows-1251") in.setCodec("Windows-1251");
        else if (EncodingTargetText == "KOI8-R") in.setCodec("KOI8-R");
        else if (EncodingTargetText == "IBM866") in.setCodec("IBM866");
        else if (EncodingTargetText == "ISO 8859-5") in.setCodec("ISO 8859-5");
        else if (EncodingTargetText == "Windows-1250") in.setCodec("Windows-1250");
        else if (EncodingTargetText == "Windows-1252") in.setCodec("Windows-1252");
        else if (EncodingTargetText == "Windows-1253") in.setCodec("Windows-1253");
        else if (EncodingTargetText == "Windows-1254") in.setCodec("Windows-1254");
        else if (EncodingTargetText == "Windows-1255") in.setCodec("Windows-1255");
        else if (EncodingTargetText == "Windows-1256") in.setCodec("Windows-1256");
        else if (EncodingTargetText == "Windows-1257") in.setCodec("Windows-1257");
        else if (EncodingTargetText == "ISO 8859-1") in.setCodec("ISO 8859-1");
        else if (EncodingTargetText == "ISO 8859-2") in.setCodec("ISO 8859-2");
        else if (EncodingTargetText == "ISO 8859-3") in.setCodec("ISO 8859-3");
        else if (EncodingTargetText == "ISO 8859-4") in.setCodec("ISO 8859-4");
        else if (EncodingTargetText == "ISO 8859-9") in.setCodec("ISO 8859-9");
        else if (EncodingTargetText == "ISO 8859-10") in.setCodec("ISO 8859-10");
        else if (EncodingTargetText == "ISO 8859-13") in.setCodec("ISO 8859-13");
        else if (EncodingTargetText == "ISO 8859-14") in.setCodec("ISO 8859-14");
        else if (EncodingTargetText == "ISO 8859-15") in.setCodec("ISO 8859-15");
        else if (EncodingTargetText == "ISO 8859-16") in.setCodec("ISO 8859-16");
        else if (EncodingTargetText == "ISO 8859-6") in.setCodec("ISO 8859-6");
        else if (EncodingTargetText == "ISO 8859-7") in.setCodec("ISO 8859-7");
        else if (EncodingTargetText == "ISO 8859-8") in.setCodec("ISO 8859-8");
        else in.setCodec("UTF-8");

        QString text = in.readAll();

        if (FilenameTargetText.endsWith(".sfm") || FilenameTargetText.endsWith(".SFM")) {
            QString text_converted = converter_to_html(text);
            ui->textEdit_TargetText->setHtml(text_converted);
        }
        else {
            ui->textEdit_TargetText->setText(text);
        }
        ui->label_dir_target->setText(FilenameTargetText);
    }
}


//opening base text
void MainWindow::open_base_text(QString file_base_text) {
    QFile FileBaseText(file_base_text);
    if(FileBaseText.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream in(&FileBaseText);

        if (EncodingBaseText == "UTF-8") in.setCodec("UTF-8");
        else if (EncodingBaseText == "Unicode") in.setCodec("Unicode");
        else if (EncodingBaseText == "Windows-1251") in.setCodec("Windows-1251");
        else if (EncodingBaseText == "KOI8-R") in.setCodec("KOI8-R");
        else if (EncodingBaseText == "IBM866") in.setCodec("IBM866");
        else if (EncodingBaseText == "ISO 8859-5") in.setCodec("ISO 8859-5");
        else if (EncodingBaseText == "Windows-1250") in.setCodec("Windows-1250");
        else if (EncodingBaseText == "Windows-1252") in.setCodec("Windows-1252");
        else if (EncodingBaseText == "Windows-1253") in.setCodec("Windows-1253");
        else if (EncodingBaseText == "Windows-1254") in.setCodec("Windows-1254");
        else if (EncodingBaseText == "Windows-1255") in.setCodec("Windows-1255");
        else if (EncodingBaseText == "Windows-1256") in.setCodec("Windows-1256");
        else if (EncodingBaseText == "Windows-1257") in.setCodec("Windows-1257");
        else if (EncodingBaseText == "ISO 8859-1") in.setCodec("ISO 8859-1");
        else if (EncodingBaseText == "ISO 8859-2") in.setCodec("ISO 8859-2");
        else if (EncodingBaseText == "ISO 8859-3") in.setCodec("ISO 8859-3");
        else if (EncodingBaseText == "ISO 8859-4") in.setCodec("ISO 8859-4");
        else if (EncodingBaseText == "ISO 8859-9") in.setCodec("ISO 8859-9");
        else if (EncodingBaseText == "ISO 8859-10") in.setCodec("ISO 8859-10");
        else if (EncodingBaseText == "ISO 8859-13") in.setCodec("ISO 8859-13");
        else if (EncodingBaseText == "ISO 8859-14") in.setCodec("ISO 8859-14");
        else if (EncodingBaseText == "ISO 8859-15") in.setCodec("ISO 8859-15");
        else if (EncodingBaseText == "ISO 8859-16") in.setCodec("ISO 8859-16");
        else if (EncodingBaseText == "ISO 8859-6") in.setCodec("ISO 8859-6");
        else if (EncodingBaseText == "ISO 8859-7") in.setCodec("ISO 8859-7");
        else if (EncodingBaseText == "ISO 8859-8") in.setCodec("ISO 8859-8");
        else in.setCodec("UTF-8");

        QString text = in.readAll();

        FileBaseText.close();

        if (FilenameBaseText.endsWith(".sfm") || FilenameBaseText.endsWith(".SFM")) {
            QString text_converted = converter_to_html(text);
            ui->textEdit_BaseText->setHtml(text_converted);
        }
        else {
            ui->textEdit_BaseText->setText(text);
        }
        ui->label_dir_base->setText(FilenameBaseText);
    }
}

//opening additional text 1
void MainWindow::open_additional_1(QString file_additional_1)
{
    QFile FileAdditional1(file_additional_1);
    if(FileAdditional1.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream in(&FileAdditional1);

        if (EncodingAdditional1 == "UTF-8") in.setCodec("UTF-8");
        else if (EncodingAdditional1 == "Unicode") in.setCodec("Unicode");
        else if (EncodingAdditional1 == "Windows-1251") in.setCodec("Windows-1251");
        else if (EncodingAdditional1 == "KOI8-R") in.setCodec("KOI8-R");
        else if (EncodingAdditional1 == "IBM866") in.setCodec("IBM866");
        else if (EncodingAdditional1 == "ISO 8859-5") in.setCodec("ISO 8859-5");
        else if (EncodingAdditional1 == "Windows-1250") in.setCodec("Windows-1250");
        else if (EncodingAdditional1 == "Windows-1252") in.setCodec("Windows-1252");
        else if (EncodingAdditional1 == "Windows-1253") in.setCodec("Windows-1253");
        else if (EncodingAdditional1 == "Windows-1254") in.setCodec("Windows-1254");
        else if (EncodingAdditional1 == "Windows-1255") in.setCodec("Windows-1255");
        else if (EncodingAdditional1 == "Windows-1256") in.setCodec("Windows-1256");
        else if (EncodingAdditional1 == "Windows-1257") in.setCodec("Windows-1257");
        else if (EncodingAdditional1 == "ISO 8859-1") in.setCodec("ISO 8859-1");
        else if (EncodingAdditional1 == "ISO 8859-2") in.setCodec("ISO 8859-2");
        else if (EncodingAdditional1 == "ISO 8859-3") in.setCodec("ISO 8859-3");
        else if (EncodingAdditional1 == "ISO 8859-4") in.setCodec("ISO 8859-4");
        else if (EncodingAdditional1 == "ISO 8859-9") in.setCodec("ISO 8859-9");
        else if (EncodingAdditional1 == "ISO 8859-10") in.setCodec("ISO 8859-10");
        else if (EncodingAdditional1 == "ISO 8859-13") in.setCodec("ISO 8859-13");
        else if (EncodingAdditional1 == "ISO 8859-14") in.setCodec("ISO 8859-14");
        else if (EncodingAdditional1 == "ISO 8859-15") in.setCodec("ISO 8859-15");
        else if (EncodingAdditional1 == "ISO 8859-16") in.setCodec("ISO 8859-16");
        else if (EncodingAdditional1 == "ISO 8859-6") in.setCodec("ISO 8859-6");
        else if (EncodingAdditional1 == "ISO 8859-7") in.setCodec("ISO 8859-7");
        else if (EncodingAdditional1 == "ISO 8859-8") in.setCodec("ISO 8859-8");
        else in.setCodec("UTF-8");

        QString text = in.readAll();

        FileAdditional1.close();

        if (FilenameAdditional1.endsWith(".sfm") || FilenameAdditional1.endsWith(".SFM")) {
            QString text_converted = converter_to_html(text);
            ui->textEdit_Additional1->setHtml(text_converted);
        }
        else {
            ui->textEdit_Additional1->setText(text);
        }
        ui->label_dir_add1->setText(FilenameAdditional1);
    }
}

//opening additional text 2
void MainWindow::open_additional_2(QString file_additional_2)
{
    QFile FileAdditional2(file_additional_2);
    if(FileAdditional2.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream in(&FileAdditional2);

        if (EncodingAdditional2 == "UTF-8") in.setCodec("UTF-8");
        else if (EncodingAdditional2 == "Unicode") in.setCodec("Unicode");
        else if (EncodingAdditional2 == "Windows-1251") in.setCodec("Windows-1251");
        else if (EncodingAdditional2 == "KOI8-R") in.setCodec("KOI8-R");
        else if (EncodingAdditional2 == "IBM866") in.setCodec("IBM866");
        else if (EncodingAdditional2 == "ISO 8859-5") in.setCodec("ISO 8859-5");
        else if (EncodingAdditional2 == "Windows-1250") in.setCodec("Windows-1250");
        else if (EncodingAdditional2 == "Windows-1252") in.setCodec("Windows-1252");
        else if (EncodingAdditional2 == "Windows-1253") in.setCodec("Windows-1253");
        else if (EncodingAdditional2 == "Windows-1254") in.setCodec("Windows-1254");
        else if (EncodingAdditional2 == "Windows-1255") in.setCodec("Windows-1255");
        else if (EncodingAdditional2 == "Windows-1256") in.setCodec("Windows-1256");
        else if (EncodingAdditional2 == "Windows-1257") in.setCodec("Windows-1257");
        else if (EncodingAdditional2 == "ISO 8859-1") in.setCodec("ISO 8859-1");
        else if (EncodingAdditional2 == "ISO 8859-2") in.setCodec("ISO 8859-2");
        else if (EncodingAdditional2 == "ISO 8859-3") in.setCodec("ISO 8859-3");
        else if (EncodingAdditional2 == "ISO 8859-4") in.setCodec("ISO 8859-4");
        else if (EncodingAdditional2 == "ISO 8859-9") in.setCodec("ISO 8859-9");
        else if (EncodingAdditional2 == "ISO 8859-10") in.setCodec("ISO 8859-10");
        else if (EncodingAdditional2 == "ISO 8859-13") in.setCodec("ISO 8859-13");
        else if (EncodingAdditional2 == "ISO 8859-14") in.setCodec("ISO 8859-14");
        else if (EncodingAdditional2 == "ISO 8859-15") in.setCodec("ISO 8859-15");
        else if (EncodingAdditional2 == "ISO 8859-16") in.setCodec("ISO 8859-16");
        else if (EncodingAdditional2 == "ISO 8859-6") in.setCodec("ISO 8859-6");
        else if (EncodingAdditional2 == "ISO 8859-7") in.setCodec("ISO 8859-7");
        else if (EncodingAdditional2 == "ISO 8859-8") in.setCodec("ISO 8859-8");
        else in.setCodec("UTF-8");

        QString text = in.readAll();

        FileAdditional2.close();

        if (FilenameAdditional2.endsWith(".sfm") || FilenameAdditional2.endsWith(".SFM")) {
            QString text_converted = converter_to_html(text);
            ui->textEdit_Additional2->setHtml(text_converted);
        }
        else {
            ui->textEdit_Additional2->setText(text);
        }
        ui->label_dir_add2->setText(FilenameAdditional2);
    }
}

//closing list file
void MainWindow::close_list() {
    if (ParatextFileEditing == true) {
        return;
    }
//checking if the text of list was changed
//if time is inserted automatically, the time of end of work will be added
    QString str = ui->textEdit_List->toPlainText();
    if ((!(str.isEmpty() || str.isNull())) && TimeAuto) {
        on_actionInsert_time_of_work_end_triggered();
        str = str + "Work finished"; //this is to make sure that the time will be saved
    }
    if ((!(str.isEmpty() || str.isNull())) && (str != TextList)) {
        QMessageBox* messagebox = new QMessageBox (QMessageBox::Warning,
                                                   tr("Warning"), tr("Do you want to save the list file?"),
                                                   QMessageBox::Yes | QMessageBox::No, this);
        messagebox->setButtonText(QMessageBox::Yes, tr("Yes"));
        messagebox->setButtonText(QMessageBox::No, tr("No"));
        int n = messagebox->exec();
        delete messagebox;
        if (n == QMessageBox::Yes) {
            if (FilenameList.isEmpty() || FilenameList.isNull()) {
                on_actionSave_list_as_triggered();
            }
            else {
                on_actionSave_list_triggered();
            }
        }
    }
}


// /////////////////////////////////////////////////////////////////
//Setting windows highlighted depending on window selected
// /////////////////////////////////////////////////////////////////

//list window selected
void MainWindow::on_textEdit_List_selectionChanged()
{
    textEdit_focused("List");
}

//original window selected
void MainWindow::on_textEdit_Original_selectionChanged()
{
    textEdit_focused("Original");
}

//target text window selected
void MainWindow::on_textEdit_TargetText_selectionChanged()
{
    textEdit_focused("TargetText");
}

//base text window selected
void MainWindow::on_textEdit_BaseText_selectionChanged()
{
    textEdit_focused("BaseText");
}

void MainWindow::on_textEdit_Additional1_selectionChanged()
{
    textEdit_focused("Additional1");
}

void MainWindow::on_textEdit_Additional2_selectionChanged()
{
    textEdit_focused("Additional2");
}


// ///////////////////////////////////////////////////////////////////////////////////
//MENU FILE OPTIONS
// ///////////////////////////////////////////////////////////////////////////////////

// ///////////////////////////////////////////////////////////////
//Option "Open"
// ///////////////////////////////////////////////////////////////

//opening original file
void MainWindow::on_actionOpen_original_triggered()
{
    textEdit_focused("Original");

    QString file = QFileDialog::getOpenFileName(this, tr("Open the original file"), "",
                                              tr("All files for opening (*.txt *.sfm *.SFM *.html *.htm);;\
Text files (*.txt);;Paratext files (*.sfm *.SFM);;HTML files (*.html *.htm)"));

    if(!file.isEmpty()) {
        FilenameOriginal = file;
        open_original(FilenameOriginal);
    }
}

//opening list file
void MainWindow::on_actionOpen_list_triggered()
{
    textEdit_focused("List");
    if (ParatextFileEditing == false) {
        close_list(); //closing previously opened list file
    } else {
        on_actionClose_Paratext_file_triggered();
    }

    ParatextFileEditing = false;

    //opening (another) list file
    QString file = QFileDialog::getOpenFileName(this, tr("Open the list file"), "",
                                                tr("All files for opening (*.txt *.html *.htm);;\
Text files (*.txt);;HTML files (*.html *.htm)"));

    if(!file.isEmpty())
    {
        FilenameList = file;
        open_list(FilenameList);
    }
}

//opening target text file
void MainWindow::on_actionOpen_target_text_triggered()
{
    textEdit_focused("TargetText");

    QString file = QFileDialog::getOpenFileName(this, tr("Open the target text file"), "",
                                                tr("All files for opening (*.txt *.sfm *.SFM *.html *.htm);;\
Text files (*.txt);;Paratext files (*.sfm *.SFM);;HTML files (*.html *.htm)"));

    if(!file.isEmpty())
    {
        FilenameTargetText = file;
        open_target_text(FilenameTargetText);
    }
}

//opening base text file
void MainWindow::on_actionOpen_base_text_triggered()
{
    textEdit_focused("BaseText");

    QString file = QFileDialog::getOpenFileName(this, tr("Open the base text file"), "",
                                                tr("All files for opening (*.txt *.sfm *.SFM *.html *.htm);;\
Text files (*.txt);;Paratext files (*.sfm *.SFM);;HTML files (*.html *.htm)"));

    if(!file.isEmpty())
    {
        FilenameBaseText = file;
        open_base_text(FilenameBaseText);
    }
}

//opening additional text 1
void MainWindow::on_actionOpen_additional_text_1_triggered()
{
    textEdit_focused("Additional1");

    QString file = QFileDialog::getOpenFileName(this, tr("Open the base text file"), "",
                                                tr("All files for opening (*.txt *.sfm *.SFM *.html *.htm);;\
Text files (*.txt);;Paratext files (*.sfm *.SFM);;HTML files (*.html *.htm)"));

    if(!file.isEmpty())
    {
        FilenameAdditional1 = file;
        open_additional_1(FilenameAdditional1);
    }
}

//opening additional text 2
void MainWindow::on_actionOpen_additional_text_2_triggered()
{
    textEdit_focused("Additional2");

    QString file = QFileDialog::getOpenFileName(this, tr("Open the base text file"), "",
                                                tr("All files for opening (*.txt *.sfm *.SFM *.html *.htm);;\
Text files (*.txt);;Paratext files (*.sfm *.SFM);;HTML files (*.html *.htm)"));

    if(!file.isEmpty())
    {
        FilenameAdditional2 = file;
        open_additional_2(FilenameAdditional2);
    }
}



// ///////////////////////////////////////////////////////////////
//Option "Close"
// ///////////////////////////////////////////////////////////////
//closing original
void MainWindow::on_actionClose_original_triggered()
{
    textEdit_focused("Original");
    ui->textEdit_Original->setText("");
    ui->label_dir_original->setText("");
}

//closing list
void MainWindow::on_actionClose_list_triggered()
{
    textEdit_focused("List");
    if (ParatextFileEditing == false) {
        close_list();
    } else {
        on_actionClose_Paratext_file_triggered();
    }
    ui->textEdit_List->setText("");    
    ui->label_dir_list->setText("");
}

//closing target text
void MainWindow::on_actionClose_target_text_triggered()
{
    textEdit_focused("TargetText");
    ui->textEdit_TargetText->setText("");
    ui->label_dir_target->setText("");
}

//closing base text
void MainWindow::on_actionClose_base_text_triggered()
{
    textEdit_focused("BaseText");
    ui->textEdit_BaseText->setText("");
    ui->label_dir_base->setText("");
}

//closing additional text 1
void MainWindow::on_actionClose_additional_text_1_triggered()
{
    textEdit_focused("Additional1");
    ui->textEdit_Additional1->setText("");
    ui->label_dir_add1->setText("");
}

//closing additional text 2
void MainWindow::on_actionClose_additional_text_2_triggered()
{
    textEdit_focused("Additional2");
    ui->textEdit_Additional2->setText("");
    ui->label_dir_add2->setText("");
}

// //////////////////////////////////////
//Option "New list"
// //////////////////////////////////////

//creating new list file
void MainWindow::on_actionNew_list_triggered() {
    textEdit_focused("List");
    if (ParatextFileEditing == false) {
        close_list();
    } else {
        on_actionClose_Paratext_file_triggered();
    }

    ParatextFileEditing = false;

    FilenameList = "";

    ui->textEdit_List->setAcceptRichText(true);
    ui->textEdit_List->setText("");

    if (TitleAuto) {
         on_actionInsert_list_title_triggered();
    }


    if (TimeAuto) {
        on_actionInsert_time_of_work_start_triggered();
    }

    ui->label_dir_list->setText("");
}

// ////////////////////////////////////////////
//Option "Save list"
// ////////////////////////////////////////////
//saving list in existing file
void MainWindow::on_actionSave_list_triggered()
{
    textEdit_focused("List");

    if (ParatextFileEditing == true) {
        QMessageBox::warning(this, tr("Warning"),
                                 tr("For saving Paratext file, use a special option."));
        return;
    }

    QFile FileList(FilenameList);

    if (FilenameList.isEmpty() || FilenameList.isNull()) {
        on_actionSave_list_as_triggered();
    }

    if(FileList.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream out(&FileList);

        if (EncodingList == "UTF-8") out.setCodec("UTF-8");
        else if (EncodingList == "Unicode") out.setCodec("Unicode");
        else if (EncodingList == "Windows-1251") out.setCodec("Windows-1251");
        else if (EncodingList == "KOI8-R") out.setCodec("KOI8-R");
        else if (EncodingList == "IBM866") out.setCodec("IBM866");
        else if (EncodingList == "ISO 8859-5") out.setCodec("ISO 8859-5");
        else if (EncodingList == "Windows-1250") out.setCodec("Windows-1250");
        else if (EncodingList == "Windows-1252") out.setCodec("Windows-1252");
        else if (EncodingList == "Windows-1253") out.setCodec("Windows-1253");
        else if (EncodingList == "Windows-1254") out.setCodec("Windows-1254");
        else if (EncodingList == "Windows-1255") out.setCodec("Windows-1255");
        else if (EncodingList == "Windows-1256") out.setCodec("Windows-1256");
        else if (EncodingList == "Windows-1257") out.setCodec("Windows-1257");
        else if (EncodingList == "ISO 8859-1") out.setCodec("ISO 8859-1");
        else if (EncodingList == "ISO 8859-2") out.setCodec("ISO 8859-2");
        else if (EncodingList == "ISO 8859-3") out.setCodec("ISO 8859-3");
        else if (EncodingList == "ISO 8859-4") out.setCodec("ISO 8859-4");
        else if (EncodingList == "ISO 8859-9") out.setCodec("ISO 8859-9");
        else if (EncodingList == "ISO 8859-10") out.setCodec("ISO 8859-10");
        else if (EncodingList == "ISO 8859-13") out.setCodec("ISO 8859-13");
        else if (EncodingList == "ISO 8859-14") out.setCodec("ISO 8859-14");
        else if (EncodingList == "ISO 8859-15") out.setCodec("ISO 8859-15");
        else if (EncodingList == "ISO 8859-16") out.setCodec("ISO 8859-16");
        else if (EncodingList == "ISO 8859-6") out.setCodec("ISO 8859-6");
        else if (EncodingList == "ISO 8859-7") out.setCodec("ISO 8859-7");
        else if (EncodingList == "ISO 8859-8") out.setCodec("ISO 8859-8");
        else out.setCodec("UTF-8");


        if (ui->textEdit_List->acceptRichText()) {
            out << ui->textEdit_List->toHtml();
        }
        else {
            out << ui->textEdit_List->toPlainText();

        }

        FileList.flush();
        FileList.close();

        TextList = ui->textEdit_List->toPlainText();
    }
}

// /////////////////////////////////////////////////////
//Option "Save list as"
// /////////////////////////////////////////////////////
//saving list in a new file
void MainWindow::on_actionSave_list_as_triggered()
{
    textEdit_focused("List");

    if (ParatextFileEditing == true) {
        QMessageBox::warning(this, tr("Warning"),
                                 tr("For saving Paratext file, use a special option."));
        return;
    }

    QStringList items;
    items << tr("Plain text (txt)") << tr("Rich text (HTML)");

    bool ok;
    QString item = QInputDialog::getItem(this, tr("Saving file"), tr("Choose how to save file:"),
                                         items, 0, false, &ok);

    if (ok && !item.isEmpty()) {

        QString file;

        bool isHTML = false;

        if (item == tr("Rich text (HTML)")) {
            ui->textEdit_List->setAcceptRichText(true);
            file = QFileDialog::getSaveFileName(this, tr("Save the list file"), "",
                                                tr("HTML files (*.html)"));
        }
        else {
            ui->textEdit_List->setAcceptRichText(false);
            file = QFileDialog::getSaveFileName(this, tr("Save the list file"), "",
                                                tr("Text files (*.txt)"));
        }

        if(!file.isEmpty())
        {
            if (isHTML == true) {
                file = file + ".html";
            }
            else {
                file = file + ".txt";
            }
            QDir direct;
            QString dir = direct.filePath(file);
            FilenameList = dir;
            on_actionSave_list_triggered();
            ui->label_dir_list->setText(FilenameList);
        }
    }
}

// ////////////////////////////////////////////////////////
//Option "Print"
// ////////////////////////////////////////////////////////
//printing original
void MainWindow::on_actionPrint_original_triggered()
{
    textEdit_focused("Original");

    QPrinter printer (QPrinter::HighResolution);
    printer.setPrinterName(tr("Printer name"));
    QPrintDialog dialog(&printer, this);
    if(dialog.exec() == QDialog::Rejected) return;
    ui->textEdit_Original->print(&printer);
}

//printing list
void MainWindow::on_actionPrint_list_triggered()
{
    textEdit_focused("List");

    QPrinter printer (QPrinter::HighResolution);
    printer.setPrinterName(tr("Printer name"));
    QPrintDialog dialog(&printer, this);
    if(dialog.exec() == QDialog::Rejected) return;
    ui->textEdit_List->print(&printer);
}

//printing target text
void MainWindow::on_actionPrint_target_text_triggered()
{
    textEdit_focused("TargetText");

    QPrinter printer (QPrinter::HighResolution);
    printer.setPrinterName(tr("Printer name"));
    QPrintDialog dialog(&printer, this);
    if(dialog.exec() == QDialog::Rejected) return;
    ui->textEdit_TargetText->print(&printer);
}

//printing base text
void MainWindow::on_actionPrint_base_text_triggered()
{
    textEdit_focused("BaseText");

    QPrinter printer (QPrinter::HighResolution);
    printer.setPrinterName(tr("Printer name"));
    QPrintDialog dialog(&printer, this);
    if(dialog.exec() == QDialog::Rejected) return;
    ui->textEdit_BaseText->print(&printer);
}

//printing additional text 1
void MainWindow::on_actionPrint_additional_text_1_triggered()
{
    textEdit_focused("Additional1");

    QPrinter printer (QPrinter::HighResolution);
    printer.setPrinterName(tr("Printer name"));
    QPrintDialog dialog(&printer, this);
    if(dialog.exec() == QDialog::Rejected) return;
    ui->textEdit_Additional1->print(&printer);
}

//printing additional text 2
void MainWindow::on_actionPrint_additional_text_2_triggered()
{
    textEdit_focused("Additional2");

    QPrinter printer (QPrinter::HighResolution);
    printer.setPrinterName(tr("Printer name"));
    QPrintDialog dialog(&printer, this);
    if(dialog.exec() == QDialog::Rejected) return;
    ui->textEdit_Additional2->print(&printer);
}

// //////////////////////////////////////////////////////
//Option "Print preview"
// //////////////////////////////////////////////////////

//print preview of original
void MainWindow::on_actionPrint_preview_original_triggered()
{
    textEdit_focused("Original");

    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this, Qt::Window);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)), SLOT(paintPreviewOriginal(QPrinter*)));
    preview.exec();
}

void MainWindow::paintPreviewOriginal(QPrinter *printer) {
    ui->textEdit_Original->print(printer);
}

//print preview of list
void MainWindow::on_actionPrint_preview_list_triggered()
{
    textEdit_focused("List");

    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this, Qt::Window);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)), SLOT(paintPreviewList(QPrinter*)));
    preview.exec();
}

void MainWindow::paintPreviewList(QPrinter *printer) {
    ui->textEdit_List->print(printer);
}

//print preview of target text
void MainWindow::on_actionPrint_preview_target_text_triggered()
{
    textEdit_focused("TargetText");

    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this, Qt::Window);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)), SLOT(paintPreviewTargetText(QPrinter*)));
    preview.exec();
}

void MainWindow::paintPreviewTargetText(QPrinter *printer) {
    ui->textEdit_TargetText->print(printer);
}

//print preview of base text
void MainWindow::on_actionPrint_preview_base_text_triggered()
{
    textEdit_focused("BaseText");

    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this, Qt::Window);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)), SLOT(paintPreviewBaseText(QPrinter*)));
    preview.exec();
}

void MainWindow::paintPreviewBaseText(QPrinter *printer) {
    ui->textEdit_BaseText->print(printer);
}

//print preview of additional text 1
void MainWindow::on_actionPrint_preview_additional_text_1_triggered()
{
    textEdit_focused("Additional1");

    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this, Qt::Window);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)), SLOT(paintPreviewAdditional1(QPrinter*)));
    preview.exec();
}

void MainWindow::paintPreviewAdditional1(QPrinter *printer)
{
    ui->textEdit_Additional1->print(printer);
}

//print preview of additional text 2
void MainWindow::on_actionPrint_preview_additional_text_2_triggered()
{
    textEdit_focused("Additional2");

    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this, Qt::Window);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)), SLOT(paintPreviewAdditional2(QPrinter*)));
    preview.exec();
}

void MainWindow::paintPreviewAdditional2(QPrinter *printer)
{
    ui->textEdit_Additional2->print(printer);
}


// /////////////////////////////////////////////////////////////
//Option "Convert to PDF"
// /////////////////////////////////////////////////////////////

//converting to PDF
void MainWindow::convert_to_PDF(QString writing, QString document_name) {
    QTextDocument document;
    document.setHtml(writing);

    QString Filename;
    if (document_name == "Original") {
        Filename = FilenameOriginal;
    }
    else if (document_name == "List") {
        Filename = FilenameList;
    }
    else if (document_name == "TargetText") {
        Filename = FilenameTargetText;
    }
    else if (document_name == "BaseText") {
        Filename = FilenameBaseText;
    }
    else if (document_name == "Additional1") {
        Filename = FilenameAdditional1;
    }
    else if (document_name == "Additional2") {
        Filename = FilenameAdditional2;
    }
    else return;

    if (Filename.endsWith(".txt")) {
        Filename = Filename.replace(".txt", ".pdf");
    }
    else if (Filename.endsWith(".html")) {
        Filename = Filename.replace(".html", ".pdf");
    }
    else if (Filename.endsWith(".htm")) {
        Filename = Filename.replace(".htm", ".pdf");
    }
    else if (Filename.endsWith(".sfm")) {
        Filename = Filename.replace(".sfm", ".pdf");
    }
    else if (Filename.endsWith(".SFM")) {
        Filename = Filename.replace(".SFM", ".pdf");
    }
    else {
        Filename = Filename + ".pdf";
    }

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFileName(Filename);
    printer.setPaperSize(QPrinter::A4);
    printer.setOutputFormat(QPrinter::PdfFormat);
    document.print(&printer);
    QMessageBox::information(this, tr("Conversion to PDF"), tr("Created file") + " " + Filename);
}

//converting original to PDF
void MainWindow::on_actionConvert_original_to_PDF_triggered()
{
    textEdit_focused("Original");

    QString writing = ui->textEdit_Original->toHtml();
    QString writing_2 = ui->textEdit_Original->toPlainText();
    if (writing_2.isEmpty() || writing_2.isNull()) {
        QMessageBox::warning(this, tr("Warning"), tr("The quesions file is not open"));
        return;
    }

    convert_to_PDF(writing, "Original");
}

//converting list to PDF
void MainWindow::on_actionConvert_list_to_PDF_triggered()
{
    textEdit_focused("List");

    QString writing = ui->textEdit_List->toHtml();
    QString writing_2 = ui->textEdit_List->toPlainText();
    if (writing_2.isEmpty() || writing_2.isNull()) {
        QMessageBox::warning(this, tr("Warning"), tr("The list is empty"));
        return;
    }

    if (FilenameList.isEmpty() || FilenameList.isNull()) {
        bool bOK;
        FilenameList = QInputDialog::getText(this, tr("Choose file name"), tr("Enter file name:"),
                                                 QLineEdit::Normal, "", &bOK);
        if (!bOK) return;
    }

    convert_to_PDF(writing, "List");
}

//converting target text to PDF
void MainWindow::on_actionConvert_target_text_to_PDF_triggered()
{
    textEdit_focused("TargetText");

    QString writing = ui->textEdit_TargetText->toHtml();
    QString writing_2 = ui->textEdit_TargetText->toPlainText();
    if (writing_2.isEmpty() || writing_2.isNull()) {
        QMessageBox::warning(this, tr("Warning"), tr("The target text is not open"));
        return;
    }

    convert_to_PDF(writing, "TargetText");
}

//converting base text to PDF
void MainWindow::on_actionConvert_base_text_to_PDF_triggered()
{
    textEdit_focused("BaseText");

    QString writing = ui->textEdit_BaseText->toHtml();
    QString writing_2 = ui->textEdit_BaseText->toPlainText();
    if (writing_2.isEmpty() || writing_2.isNull()) {
        QMessageBox::warning(this, tr("Warning"), tr("The base text is not open"));
        return;
    }

    convert_to_PDF(writing, "BaseText");
}

//converting additional text 1 to PDF
void MainWindow::on_actionConvert_additional_text_1_to_PDF_triggered()
{
    textEdit_focused("Additional1");

    QString writing = ui->textEdit_Additional1->toHtml();
    QString writing_2 = ui->textEdit_Additional1->toPlainText();
    if (writing_2.isEmpty() || writing_2.isNull()) {
        QMessageBox::warning(this, tr("Warning"), tr("The text is not open"));
        return;
    }

    convert_to_PDF(writing, "Additional1");
}

//converting additional text 2 to PDF
void MainWindow::on_actionConvert_additional_text_2_to_PDF_triggered()
{
    textEdit_focused("Additional2");

    QString writing = ui->textEdit_Additional2->toHtml();
    QString writing_2 = ui->textEdit_Additional2->toPlainText();
    if (writing_2.isEmpty() || writing_2.isNull()) {
        QMessageBox::warning(this, tr("Warning"), tr("The text is not open"));
        return;
    }

    convert_to_PDF(writing, "Additional2");
}

// ////////////////////////////////////////////////////////
//Option "Convert to ODF"
// ////////////////////////////////////////////////////////
//converting to ODT
void MainWindow::convert_to_ODT(QString writing, QString document_name) {
    QTextDocument document;
    document.setHtml(writing);

    QString Filename;
    if (document_name == "Original") {
        Filename = FilenameOriginal;
    }
    else if (document_name == "List") {
        Filename = FilenameList;
    }
    else if (document_name == "TargetText") {
        Filename = FilenameTargetText;
    }
    else if (document_name == "BaseText") {
        Filename = FilenameBaseText;
    }
    else if (document_name == "Additional1") {
        Filename = FilenameAdditional1;
    }
    else if (document_name == "Additional2") {
        Filename = FilenameAdditional2;
    }
    else return;

    if (Filename.endsWith(".txt")) {
        Filename = Filename.replace(".txt", ".odt");
    }
    else if (Filename.endsWith(".html")) {
        Filename = Filename.replace(".html", ".odt");
    }
    else if (Filename.endsWith(".htm")) {
        Filename = Filename.replace(".htm", ".odt");
    }
    else if (Filename.endsWith(".sfm")) {
        Filename = Filename.replace(".sfm", ".odt");
    }
    else if (Filename.endsWith(".SFM")) {
        Filename = Filename.replace(".SFM", ".odt");
    }
    else {
        Filename = Filename + ".odt";
    }

    QTextDocumentWriter writer;
    writer.setFormat("odf");
    writer.setFileName(Filename);
    writer.write(&document);
    QMessageBox::information(this, tr("Conversion to ODT"), tr("Created file") + " " + Filename);
}

//converting original to ODT
void MainWindow::on_actionConvert_original_to_ODT_triggered()
{
    textEdit_focused("Original");

    QString writing = ui->textEdit_Original->toHtml();
    QString writing_2 = ui->textEdit_Original->toPlainText();
    if (writing_2.isEmpty() || writing_2.isNull()) {
        QMessageBox::warning(this, tr("Warning"), tr("The quesions file is not open"));
        return;
    }

    convert_to_ODT(writing, "Original");
}

//converting list to ODT
void MainWindow::on_actionConvert_list_to_ODT_triggered()
{
    textEdit_focused("List");

    QString writing = ui->textEdit_List->toHtml();
    QString writing_2 = ui->textEdit_List->toPlainText();
    if (writing_2.isEmpty() || writing_2.isNull()) {
        QMessageBox::warning(this, tr("Warning"), tr("The list is empty"));
        return;
    }

    if (FilenameList.isEmpty() || FilenameList.isNull()) {
        bool bOK;
        FilenameList = QInputDialog::getText(this, tr("Choose file name"), tr("Enter file name:"),
                                                 QLineEdit::Normal, "", &bOK);
        if (!bOK) return;
    }

    convert_to_ODT(writing, "List");
}

//converting target text to ODT
void MainWindow::on_actionConvert_target_text_to_ODT_triggered()
{
    textEdit_focused("TargetText");

    QString writing = ui->textEdit_TargetText->toHtml();
    QString writing_2 = ui->textEdit_TargetText->toPlainText();
    if (writing_2.isEmpty() || writing_2.isNull()) {
        QMessageBox::warning(this, tr("Warning"), tr("The target text is not open"));
        return;
    }

    convert_to_ODT(writing, "TargetText");
}

//converting base text to ODT
void MainWindow::on_actionConvert_base_text_to_ODT_triggered()
{
    textEdit_focused("BaseText");

    QString writing = ui->textEdit_BaseText->toHtml();
    QString writing_2 = ui->textEdit_BaseText->toPlainText();
    if (writing_2.isEmpty() || writing_2.isNull()) {
        QMessageBox::warning(this, tr("Warning"), tr("The base text is not open"));
        return;
    }

    convert_to_ODT(writing, "BaseText");
}

//converting additional text 1 to ODT
void MainWindow::on_actionConvert_additional_text_1_to_ODT_triggered()
{
    textEdit_focused("Additional1");

    QString writing = ui->textEdit_Additional1->toHtml();
    QString writing_2 = ui->textEdit_Additional1->toPlainText();
    if (writing_2.isEmpty() || writing_2.isNull()) {
        QMessageBox::warning(this, tr("Warning"), tr("The text is not open"));
        return;
    }

    convert_to_ODT(writing, "Additional1");
}

//converting additional text 2 to ODT
void MainWindow::on_actionConvert_additional_text_2_to_ODT_triggered()
{
    textEdit_focused("Additional2");

    QString writing = ui->textEdit_Additional2->toHtml();
    QString writing_2 = ui->textEdit_Additional2->toPlainText();
    if (writing_2.isEmpty() || writing_2.isNull()) {
        QMessageBox::warning(this, tr("Warning"), tr("The text is not open"));
        return;
    }

    convert_to_ODT(writing, "Additional2");
}


// ////////////////////////////////////////////////////
//Option "Exit"
// ////////////////////////////////////////////////////
//exiting the program
void MainWindow::on_actionExit_triggered()
{
    textEdit_focused("List");
    close(); //this will call exit event
}

// //////////////////////////////////////////////////////////////////////////////
//MENU EDIT
// //////////////////////////////////////////////////////////////////////////////

// /////////////////////////////////////////////////////////////////////////////
//Option "Undo"
// ////////////////////////////////////////////////////////////////////////////
//undo in list
void MainWindow::on_actionUndo_triggered()
{
    textEdit_focused("List");
    ui->textEdit_List->undo();
}

// /////////////////////////////////////////////////////////////////////////////
//Option "Redo"
// ////////////////////////////////////////////////////////////////////////////
//redo in list
void MainWindow::on_actionRedo_triggered()
{
    textEdit_focused("List");
    ui->textEdit_List->redo();
}

// ///////////////////////////////////////////////////////////////////////////
//Option "Cut"
// ///////////////////////////////////////////////////////////////////////////
//cut from list
void MainWindow::on_actionCut_triggered()
{
    textEdit_focused("List");
    ui->textEdit_List->cut();
}

// //////////////////////////////////////////////////////////////////////////
//Option "Copy"
// //////////////////////////////////////////////////////////////////////////
//copy from original
void MainWindow::on_actionCopy_from_original_triggered()
{
    textEdit_focused("Original");
    ui->textEdit_Original->copy();
}

//copy from list
void MainWindow::on_actionCopy_from_list_triggered()
{
    textEdit_focused("List");
    ui->textEdit_List->copy();
}

//copy from target text
void MainWindow::on_actionCopy_from_target_text_triggered()
{
    textEdit_focused("TargetText");
    ui->textEdit_TargetText->copy();
}

//copy from base text
void MainWindow::on_actionCopy_from_base_text_triggered()
{
    textEdit_focused("BaseText");
    ui->textEdit_BaseText->copy();
}

//copy from additional text 1
void MainWindow::on_actionCopy_from_additional_text_1_triggered()
{
    textEdit_focused("Additional1");
    ui->textEdit_Additional1->copy();
}

//copy from additional text 2
void MainWindow::on_actionCopy_from_additional_text_2_triggered()
{
    textEdit_focused("Additional2");
    ui->textEdit_Additional2->copy();
}

// //////////////////////////////////////////////////////////////
//Option "Paste"
// //////////////////////////////////////////////////////////////
//paste into list
void MainWindow::on_actionPaste_triggered()
{
    textEdit_focused("List");
    ui->textEdit_List->paste();
}

// ///////////////////////////////////////////////////////////////
//Option "Copy and paste"
// ///////////////////////////////////////////////////////////////
//copy and paste from original
void MainWindow::on_actionCopy_and_paste_from_original_triggered()
{
    textEdit_focused("List");

    ui->textEdit_Original->copy();
    ui->textEdit_List->paste();
    ui->textEdit_List->insertPlainText("\n");
}

//copy and paste from list
void MainWindow::on_actionCopy_and_paste_from_list_triggered()
{
    textEdit_focused("List");

    ui->textEdit_List->copy();
    ui->textEdit_List->paste();
    ui->textEdit_List->insertPlainText("\n");
}

//copy and paste from target text
void MainWindow::on_actionCopy_and_paste_from_target_text_triggered()
{
    textEdit_focused("List");

    ui->textEdit_TargetText->copy();
    ui->textEdit_List->paste();
    ui->textEdit_List->insertPlainText("\n");
}

//copy and paste from base text
void MainWindow::on_actionCopy_and_paste_from_base_text_triggered()
{
    textEdit_focused("List");

    ui->textEdit_BaseText->copy();
    ui->textEdit_List->paste();
    ui->textEdit_List->insertPlainText("\n");
}

//copy and paste from additional text 1
void MainWindow::on_actionCopy_and_paste_from_additional_text_1_triggered()
{
    textEdit_focused("List");

    ui->textEdit_Additional1->copy();
    ui->textEdit_List->paste();
    ui->textEdit_List->insertPlainText("\n");
}

//copy and paste from additional text 2
void MainWindow::on_actionCopy_and_paste_from_additional_text_2_triggered()
{
    textEdit_focused("List");

    ui->textEdit_Additional2->copy();
    ui->textEdit_List->paste();
    ui->textEdit_List->insertPlainText("\n");
}

// /////////////////////////////////////////////////////////////////
//Option "Delete"
// /////////////////////////////////////////////////////////////////
//deleting from list
void MainWindow::on_actionDelete_triggered()
{
    textEdit_focused("List");

    QTextCursor cursor = ui->textEdit_List->textCursor();
    cursor.removeSelectedText();
}

// /////////////////////////////////////////////////////////////////
//Option "Select all"
// /////////////////////////////////////////////////////////////////
//selecting all from original
void MainWindow::on_actionSelect_all_from_original_triggered()
{
    textEdit_focused("Original");
    ui->textEdit_Original->selectAll();
}

//selecting all from list
void MainWindow::on_actionSelect_all_from_list_triggered()
{
    textEdit_focused("List");
    ui->textEdit_List->selectAll();
}

//selecting all from target text
void MainWindow::on_actionSelect_all_from_target_text_triggered()
{
    textEdit_focused("TargetText");
    ui->textEdit_TargetText->selectAll();
}

//selecting all from base text
void MainWindow::on_actionSelect_all_from_base_text_triggered()
{
    textEdit_focused("BaseText");
    ui->textEdit_BaseText->selectAll();
}

//selecting all from additional text 1
void MainWindow::on_actionSelect_all_from_additional_text_1_triggered()
{
    textEdit_focused("Additional1");
    ui->textEdit_Additional1->selectAll();
}

//selecting all from additional text 2
void MainWindow::on_actionSelect_all_from_additional_text_2_triggered()
{
    textEdit_focused("Additional2");
    ui->textEdit_Additional2->selectAll();
}

// ///////////////////////////////////////////////////////////////////
//Searching
// ///////////////////////////////////////////////////////////////////
//search
void MainWindow::search_word(QString document_name) {
    DialogFind dialogfindDocument;
    dialogfindDocument.exec();
    dialogfindDocument.show(); //in this way, this dialog will be shown when search continues

    bool Found = false;

    QFlags<QTextDocument::FindFlag> searchOptions;

    if (dialogfindDocument.CaseSensitive == true) {
        if (dialogfindDocument.WholeWord == true) {
            if (dialogfindDocument.SearchDirection == "Forward") {
                searchOptions = QTextDocument::FindCaseSensitively | QTextDocument::FindWholeWords;
            }
            else if (dialogfindDocument.SearchDirection == "Backward") {
                searchOptions = QTextDocument::FindBackward | QTextDocument::FindCaseSensitively |
                        QTextDocument::FindWholeWords;
            }
        }
        else {
            if (dialogfindDocument.SearchDirection == "Forward") {
                searchOptions = QTextDocument::FindCaseSensitively;
            }
            else if (dialogfindDocument.SearchDirection == "Backward") {
                searchOptions = QTextDocument::FindBackward | QTextDocument::FindCaseSensitively;
            }
        }
    }

    else {
        if (dialogfindDocument.WholeWord == true) {
            if (dialogfindDocument.SearchDirection == "Forward") {
                searchOptions = QTextDocument::FindWholeWords;
            }
            else if (dialogfindDocument.SearchDirection == "Backward") {
                searchOptions = QTextDocument::FindBackward | QTextDocument::FindWholeWords;
            }

        }
        else {
            if (dialogfindDocument.SearchDirection == "Forward") {
                searchOptions = 0;
            }
            else if (dialogfindDocument.SearchDirection == "Backward") {
                searchOptions = QTextDocument::FindBackward;
            }
        }
    }

    if (document_name == "Original") {
        Found = ui->textEdit_Original->find(dialogfindDocument.search, searchOptions);
    }
    else if (document_name == "List") {
        Found = ui->textEdit_List->find(dialogfindDocument.search, searchOptions);
    }
    else if (document_name == "TargetText") {
        Found = ui->textEdit_TargetText->find(dialogfindDocument.search, searchOptions);
    }
    else if (document_name == "BaseText") {
        Found = ui->textEdit_BaseText->find(dialogfindDocument.search, searchOptions);
    }
    else if (document_name == "Additional1") {
        Found = ui->textEdit_Additional1->find(dialogfindDocument.search, searchOptions);
    }
    else if (document_name == "Additional2") {
        Found = ui->textEdit_Additional2->find(dialogfindDocument.search, searchOptions);
    }
    else return;

    if ((!(dialogfindDocument.search.isEmpty() || dialogfindDocument.search.isNull())) & !Found) {
        QMessageBox* notFound = new QMessageBox(QMessageBox::Warning, tr("Not found"),
                                                tr("The word was not found"));
        notFound->exec();
        delete notFound;
    }

    if ((!(dialogfindDocument.search.isEmpty() || dialogfindDocument.search.isNull())) & Found) {
        bool continueSearch = true;
        bool foundNew = true;

        while (continueSearch & foundNew) {
            QMessageBox* findNext =
                    new QMessageBox(QMessageBox::Question, tr("Continue?"), tr("Find next?"),
                                    QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

            findNext->setButtonText(QMessageBox::Yes, tr("Yes"));
            findNext->setButtonText(QMessageBox::No, tr("No"));
            findNext->setButtonText(QMessageBox::Cancel, tr("Cancel"));

            int n = findNext->exec();
            delete findNext;

            if (n == QMessageBox::Yes) {
                if (document_name == "Original") {
                    foundNew = ui->textEdit_Original->find(dialogfindDocument.search, searchOptions);
                }
                else if (document_name == "List") {
                    foundNew = ui->textEdit_List->find(dialogfindDocument.search, searchOptions);
                }
                else if (document_name == "TargetText") {
                    foundNew = ui->textEdit_TargetText->find(dialogfindDocument.search, searchOptions);
                }
                else if (document_name == "BaseText") {
                    foundNew = ui->textEdit_BaseText->find(dialogfindDocument.search, searchOptions);
                }
                else if (document_name == "Additional1") {
                    foundNew = ui->textEdit_Additional1->find(dialogfindDocument.search, searchOptions);
                }
                else if (document_name == "Additional2") {
                    foundNew = ui->textEdit_Additional2->find(dialogfindDocument.search, searchOptions);
                }
                else return;
            }
            else if ((n == QMessageBox::No) || (n == QMessageBox::Cancel)) {
                continueSearch = false;
            }
        }

        if (!foundNew) {
            QMessageBox* noNewFound = new QMessageBox(QMessageBox::Warning, tr("End of search"),
                                                      tr("End of search is reached"));
            noNewFound->exec();
            delete noNewFound;
        }
    }
}


// //////////////////////////////////////////////////////////////////
//Option "Find"
// //////////////////////////////////////////////////////////////////
//searching in original
void MainWindow::on_actionFind_in_original_triggered() {
    textEdit_focused("Original");

    QString str = ui->textEdit_Original->toPlainText();
    if (str.isEmpty() || str.isNull()) {
        QMessageBox* warning = new QMessageBox(QMessageBox::Warning, tr("Warning"), tr("File is not open"));
        warning->exec();
        delete warning;
        return;
    }
    search_word("Original");
}

//searching in list
void MainWindow::on_actionFind_in_list_triggered() {
    textEdit_focused("List");

    QString str = ui->textEdit_List->toPlainText();
    if (str.isEmpty() || str.isNull()) {
        QMessageBox* warning = new QMessageBox(QMessageBox::Warning, tr("Warning"), tr("The list is empty"));
        warning->exec();
        delete warning;
        return;
    }

    search_word("List");
}

//searching in target text
void MainWindow::on_actionFind_in_target_text_triggered() {
    textEdit_focused("TargetText");

    QString str = ui->textEdit_TargetText->toPlainText();
    if (str.isEmpty() || str.isNull()) {
        QMessageBox* warning = new QMessageBox(QMessageBox::Warning, tr("Warning"), tr("File is not open"));
        warning->exec();
        delete warning;
        return;
    }

    search_word("TargetText");
}

//searching in base text
void MainWindow::on_actionFind_in_base_text_triggered() {
    textEdit_focused("BaseText");

    QString str = ui->textEdit_BaseText->toPlainText();
    if (str.isEmpty() || str.isNull()) {
        QMessageBox* warning = new QMessageBox(QMessageBox::Warning, tr("Warning"), tr("File is not open"));
        warning->exec();
        delete warning;
        return;
    }

    search_word("BaseText");
}

//searching in additional text 1
void MainWindow::on_actionFind_in_additional_text_1_triggered()
{
    textEdit_focused("Additional1");

    QString str = ui->textEdit_Additional1->toPlainText();
    if (str.isEmpty() || str.isNull()) {
        QMessageBox* warning = new QMessageBox(QMessageBox::Warning, tr("Warning"), tr("File is not open"));
        warning->exec();
        delete warning;
        return;
    }

    search_word("Additional1");
}

//searching in additional text 2
void MainWindow::on_actionFind_in_additional_text_2_triggered()
{
    textEdit_focused("Additional2");

    QString str = ui->textEdit_Additional2->toPlainText();
    if (str.isEmpty() || str.isNull()) {
        QMessageBox* warning = new QMessageBox(QMessageBox::Warning, tr("Warning"), tr("File is not open"));
        warning->exec();
        delete warning;
        return;
    }

    search_word("Additional2");
}

// /////////////////////////////////////////////////////
//Option "Find and replace"
// /////////////////////////////////////////////////////
//searching and replacing in list
void MainWindow::on_actionFind_and_replace_triggered()
{
    textEdit_focused("List");

    QString str = ui->textEdit_List->toPlainText();
    if (str.isEmpty() || str.isNull()) {
        QMessageBox* warning = new QMessageBox(QMessageBox::Warning, tr("Warning"), tr("The list is empty"));
        warning->exec();
        delete warning;
        return;
    }

    DialogReplace dialogreplace;
    dialogreplace.exec();
    dialogreplace.show(); //in this way, this dialog will be shown when search continues
    bool Found = false;

    QFlags<QTextDocument::FindFlag> searchOptions;

    if (dialogreplace.CaseSensitive == true) {
        if (dialogreplace.WholeWord == true) {
            if (dialogreplace.SearchDirection == "Forward") {
                searchOptions = QTextDocument::FindCaseSensitively | QTextDocument::FindWholeWords;
            }
            else if (dialogreplace.SearchDirection == "Backward") {
                searchOptions = QTextDocument::FindBackward | QTextDocument::FindCaseSensitively |
                        QTextDocument::FindWholeWords;
            }
        }
        else {
            if (dialogreplace.SearchDirection == "Forward") {
                searchOptions = QTextDocument::FindCaseSensitively;
            }
            else if (dialogreplace.SearchDirection == "Backward") {
                searchOptions = QTextDocument::FindBackward | QTextDocument::FindCaseSensitively;
            }
        }
    }

    else {
        if (dialogreplace.WholeWord == true) {
            if (dialogreplace.SearchDirection == "Forward") {
                searchOptions = QTextDocument::FindWholeWords;
            }
            else if (dialogreplace.SearchDirection == "Backward") {
                searchOptions = QTextDocument::FindBackward | QTextDocument::FindWholeWords;
            }
        }
        else {
            if (dialogreplace.SearchDirection == "Forward") {
                searchOptions = 0;
            }
            else if (dialogreplace.SearchDirection == "Backward") {
                searchOptions = QTextDocument::FindBackward;
            }
        }
    }

    Found = ui->textEdit_List->find(dialogreplace.search, searchOptions);

    if ((!(dialogreplace.search.isEmpty() || dialogreplace.search.isNull())) & !Found) {
        QMessageBox* notFound = new QMessageBox(QMessageBox::Warning, tr("Not found"),
                                                tr("The word was not found"));
        notFound->exec();
        delete notFound;
    }

    else if (!(dialogreplace.search.isEmpty() || dialogreplace.search.isNull())) {
        ui->textEdit_List->cut();
        ui->textEdit_List->insertPlainText(dialogreplace.replace);
    }

    if ((!(dialogreplace.search.isEmpty() || dialogreplace.search.isNull())) & Found) {
        bool continueSearch = true;
        bool foundNew = true;

        while (continueSearch & foundNew) {
            QMessageBox* findNext =
                    new QMessageBox(QMessageBox::Question, tr("Continue?"), tr("Find and replace next?"),
                                    QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

            findNext->setButtonText(QMessageBox::Yes, tr("Yes"));
            findNext->setButtonText(QMessageBox::No, tr("No"));
            findNext->setButtonText(QMessageBox::Cancel, tr("Cancel"));

            int n = findNext->exec();
            delete findNext;

            if (n == QMessageBox::Yes) {
                foundNew = ui->textEdit_List->find(dialogreplace.search, searchOptions);

                if (foundNew) {
                    ui->textEdit_List->cut();
                    ui->textEdit_List->insertPlainText(dialogreplace.replace);
                }
            }

            else if ((n == QMessageBox::No) || (n == QMessageBox::Cancel)) {
                continueSearch = false;
            }
        }

        if (!foundNew) {
            QMessageBox* noNewFound = new QMessageBox(QMessageBox::Warning, tr("End of search"),
                                                      tr("End of search is reached"));
            noNewFound->exec();
            delete noNewFound;
        }
    }

}

// /////////////////////////////////////////////////////////
//Option "Find and replace all"
// /////////////////////////////////////////////////////////
//searching and replacing all in list
void MainWindow::on_actionFind_and_replace_all_triggered()
{
    textEdit_focused("List");

    QString str = ui->textEdit_List->toPlainText();
    if (str.isEmpty() || str.isNull()) {
        QMessageBox* warning = new QMessageBox(QMessageBox::Warning, tr("Warning"), tr("The list is empty"));
        warning->exec();
        delete warning;
        return;
    }

    DialogReplaceAll dialogreplaceall;
    dialogreplaceall.exec();
    dialogreplaceall.show();

    if (!(dialogreplaceall.search.isEmpty() || dialogreplaceall.search.isNull())) {
        QString text = ui->textEdit_List->toPlainText();
        text.replace(dialogreplaceall.search, dialogreplaceall.replace);
        ui->textEdit_List->setText(text);
        QMessageBox::information(this, tr("Found and replaced"), tr("Find and replace all are done"));
    }
}

// ////////////////////////////////////////////////////////////////////
//MENU VIEW
// ////////////////////////////////////////////////////////////////////

// ////////////////////////////////////////////////////////
//Option "Zoom in"
// ////////////////////////////////////////////////////////
//zoom in all windows
void MainWindow::on_actionZoom_in_all_windows_triggered()
{
    ui->textEdit_Original->zoomIn();
    ui->textEdit_List->zoomIn();
    ui->textEdit_TargetText->zoomIn();
    ui->textEdit_BaseText->zoomIn();
    ui->textEdit_Additional1->zoomIn();
    ui->textEdit_Additional2->zoomIn();

    ZoomOriginal++;
    ZoomList++;
    ZoomTargetText++;
    ZoomBaseText++;
    ZoomAdditional1++;
    ZoomAdditional2++;
}

//zoom in original
void MainWindow::on_actionZoom_in_original_triggered()
{
    textEdit_focused("Original");

    ui->textEdit_Original->zoomIn();
    ZoomOriginal++;
}

//zoom in list
void MainWindow::on_actionZoom_in_list_triggered()
{
    textEdit_focused("List");

    ui->textEdit_List->zoomIn();
    ZoomList++;
}

//zoom in target text
void MainWindow::on_actionZoom_in_target_text_triggered()
{
    textEdit_focused("TargetText");

    ui->textEdit_TargetText->zoomIn();
    ZoomTargetText++;
}

//zoom in base text
void MainWindow::on_actionZoom_in_base_text_triggered()
{
    textEdit_focused("BaseText");

    ui->textEdit_BaseText->zoomIn();
    ZoomBaseText++;
}

//zoom in additional text 1
void MainWindow::on_actionZoom_in_additional_text_1_triggered()
{
    textEdit_focused("Additional1");

    ui->textEdit_Additional1->zoomIn();
    ZoomAdditional1++;
}

//zoom in additional text 2
void MainWindow::on_actionZoom_in_additional_text_2_triggered()
{
    textEdit_focused("Additional2");

    ui->textEdit_Additional2->zoomIn();
    ZoomAdditional2++;
}

// ///////////////////////////////////////////////////
//Option "Zoom out"
// //////////////////////////////////////////////////
//zoom out all windows
void MainWindow::on_actionZoom_out_all_windows_triggered()
{
    ui->textEdit_Original->zoomOut();
    ui->textEdit_List->zoomOut();
    ui->textEdit_TargetText->zoomOut();
    ui->textEdit_BaseText->zoomOut();
    ui->textEdit_Additional1->zoomOut();
    ui->textEdit_Additional2->zoomOut();

    ZoomOriginal--;
    ZoomList--;
    ZoomTargetText--;
    ZoomBaseText--;
    ZoomAdditional1--;
    ZoomAdditional2--;
}

//zoom out original
void MainWindow::on_actionZoom_out_original_triggered()
{
    textEdit_focused("Original");

    ui->textEdit_Original->zoomOut();
    ZoomOriginal--;
}

//zoom out list
void MainWindow::on_actionZoom_out_list_triggered()
{
    textEdit_focused("List");

    ui->textEdit_List->zoomOut();
    ZoomList--;
}

//zoom out target text
void MainWindow::on_actionZoom_out_target_text_triggered()
{
    textEdit_focused("TargetText");

    ui->textEdit_TargetText->zoomOut();
    ZoomTargetText--;
}

//zoom out base text
void MainWindow::on_actionZoom_out_base_text_triggered()
{
    textEdit_focused("BaseText");

    ui->textEdit_BaseText->zoomOut();
    ZoomBaseText--;
}

//zoom out additional text 1
void MainWindow::on_actionZoom_out_additional_text_1_triggered()
{
    textEdit_focused("Additional1");

    ui->textEdit_Additional1->zoomOut();
    ZoomAdditional1--;
}

//zoom out additional text 2
void MainWindow::on_actionZoom_out_additional_text_2_triggered()
{
    textEdit_focused("Additional2");

    ui->textEdit_Additional2->zoomOut();
    ZoomAdditional2--;
}

// //////////////////////////////////////////////
//Option "Font"
// //////////////////////////////////////////////
//changing font in original
void MainWindow::on_actionFont_of_original_triggered()
{
    textEdit_focused("Original");

    bool ok;
    QFont font = QFontDialog::getFont(&ok, this);
    if (ok) {
        ui->textEdit_Original->setFont(font);
    } else return;
}

//changing font in list
void MainWindow::on_actionFont_of_list_triggered()
{
    textEdit_focused("List");

    bool ok;
    QFont font = QFontDialog::getFont(&ok, this);
    if (ok) {
        ui->textEdit_List->setFont(font);
    } else return;
}

//changing font in target text
void MainWindow::on_actionFont_of_target_text_triggered()
{
    textEdit_focused("TargetText");

    bool ok;
    QFont font = QFontDialog::getFont(&ok, this);
    if (ok) {
        ui->textEdit_TargetText->setFont(font);
    } else return;
}

//changing font in base text
void MainWindow::on_actionFont_of_base_text_triggered()
{
    textEdit_focused("BaseText");

    bool ok;
    QFont font = QFontDialog::getFont(&ok, this);
    if (ok) {
        ui->textEdit_BaseText->setFont(font);
    } else return;
}

//changing font in additional text 1
void MainWindow::on_actionFont_of_additional_text_1_triggered()
{
    textEdit_focused("Additional1");

    bool ok;
    QFont font = QFontDialog::getFont(&ok, this);
    if (ok) {
        ui->textEdit_Additional1->setFont(font);
    } else return;
}

//changing font in additional text 2
void MainWindow::on_actionFont_of_additional_text_2_triggered()
{
    textEdit_focused("Additional2");

    bool ok;
    QFont font = QFontDialog::getFont(&ok, this);
    if (ok) {
        ui->textEdit_Additional2->setFont(font);
    } else return;
}

// ///////////////////////////////////////////////////////////
//Option "Text color"
// ///////////////////////////////////////////////////////////
//text color in original
void MainWindow::on_actionText_color_of_original_triggered()
{
    textEdit_focused("Original");

    QColor color = QColorDialog::getColor(Qt::white, this, tr("Choose Color"));
    if(color.isValid()) {
        ui->textEdit_Original->setTextColor(color);
    }
}

//text color in list
void MainWindow::on_actionText_color_of_list_triggered()
{
    textEdit_focused("List");

    QColor color = QColorDialog::getColor(Qt::white, this, tr("Choose Color"));
    if(color.isValid()) {
        ui->textEdit_List->setTextColor(color);
    }
}

//text color in target text
void MainWindow::on_actionText_color_of_target_text_triggered()
{
    textEdit_focused("TargetText");

    QColor color = QColorDialog::getColor(Qt::white, this, tr("Choose Color"));
    if(color.isValid()) {
        ui->textEdit_TargetText->setTextColor(color);
    }
}

//text color in base text
void MainWindow::on_actionText_color_of_base_text_triggered()
{
    textEdit_focused("BaseText");

    QColor color = QColorDialog::getColor(Qt::white, this, tr("Choose Color"));
    if(color.isValid()) {
        ui->textEdit_BaseText->setTextColor(color);
    }
}

//text color in additional text 1
void MainWindow::on_actionText_color_of_additional_text_1_triggered()
{
    textEdit_focused("Additional1");

    QColor color = QColorDialog::getColor(Qt::white, this, tr("Choose Color"));
    if(color.isValid()) {
        ui->textEdit_Additional1->setTextColor(color);
    }
}

//text color in additional text 2
void MainWindow::on_actionText_color_of_additional_text_2_triggered()
{
    textEdit_focused("Additional2");

    QColor color = QColorDialog::getColor(Qt::white, this, tr("Choose Color"));
    if(color.isValid()) {
        ui->textEdit_Additional2->setTextColor(color);
    }
}


// ////////////////////////////////////////////////////////////
//Option "Background color"
// ////////////////////////////////////////////////////////////
//text background color in original
void MainWindow::on_actionBackground_color_of_original_triggered()
{
    textEdit_focused("Original");

    QColor color = QColorDialog::getColor(Qt::white, this, tr("Choose Color"));
    if(color.isValid()) {
        ui->textEdit_Original->setTextBackgroundColor(color);
    }
}

//text background color in list
void MainWindow::on_actionBackground_color_of_list_triggered()
{
    textEdit_focused("List");

    QColor color = QColorDialog::getColor(Qt::white, this, tr("Choose Color"));
    if(color.isValid()) {
        ui->textEdit_List->setTextBackgroundColor(color);
    }
}

//text background color in target text
void MainWindow::on_actionBackground_color_of_target_text_triggered()
{
    textEdit_focused("TargetText");

    QColor color = QColorDialog::getColor(Qt::white, this, tr("Choose Color"));
    if(color.isValid()) {
        ui->textEdit_TargetText->setTextBackgroundColor(color);
    }
}

//text background color in base text
void MainWindow::on_actionBackground_color_of_base_text_triggered()
{
    textEdit_focused("BaseText");

    QColor color = QColorDialog::getColor(Qt::white, this, tr("Choose Color"));
    if(color.isValid()) {
        ui->textEdit_BaseText->setTextBackgroundColor(color);
    }
}

//text background color in additional text 1
void MainWindow::on_actionBackground_color_of_additional_text_1_triggered()
{
    textEdit_focused("Additional1");

    QColor color = QColorDialog::getColor(Qt::white, this, tr("Choose Color"));
    if(color.isValid()) {
        ui->textEdit_Additional1->setTextBackgroundColor(color);
    }
}

//text background color in additional text 2
void MainWindow::on_actionBackground_color_of_additional_text_2_triggered()
{
    textEdit_focused("Additional2");

    QColor color = QColorDialog::getColor(Qt::white, this, tr("Choose Color"));
    if(color.isValid()) {
        ui->textEdit_Additional2->setTextBackgroundColor(color);
    }
}

// /////////////////////////////////////////////////
//Option "Palette color"
// /////////////////////////////////////////////////
//window background color in all windows
void MainWindow::on_actionPalette_color_of_all_windows_triggered()
{
    QColor color = QColorDialog::getColor(Qt::white, this, tr("Choose Color"));
    QPalette palette;
    palette.setColor(QPalette::Base, color);
    if(color.isValid()) {
        ui->textEdit_Original->setPalette(palette);
        ui->textEdit_List->setPalette(palette);
        ui->textEdit_TargetText->setPalette(palette);
        ui->textEdit_BaseText->setPalette(palette);
    }
}

//window background color in original
void MainWindow::on_actionPalette_color_of_original_triggered()
{
    textEdit_focused("Original");

    QColor color = QColorDialog::getColor(Qt::white, this, tr("Choose Color"));
    QPalette palette;
    palette.setColor(QPalette::Base, color);
    if(color.isValid()) {
        ui->textEdit_Original->setPalette(palette);
    }
}

//window background color in list
void MainWindow::on_actionPalette_color_of_list_triggered()
{
    textEdit_focused("List");

    QColor color = QColorDialog::getColor(Qt::white, this, tr("Choose Color"));
    QPalette palette;
    palette.setColor(QPalette::Base, color);
    if(color.isValid()) {
        ui->textEdit_List->setPalette(palette);
    }
}

//window background color in target text
void MainWindow::on_actionPalette_color_of_target_text_triggered()
{
    textEdit_focused("TargetText");

    QColor color = QColorDialog::getColor(Qt::white, this, tr("Choose Color"));
    QPalette palette;
    palette.setColor(QPalette::Base, color);
    if(color.isValid()) {
        ui->textEdit_TargetText->setPalette(palette);
    }
}

//window background color in base text
void MainWindow::on_actionPalette_color_of_base_text_triggered()
{
    textEdit_focused("BaseText");

    QColor color = QColorDialog::getColor(Qt::white, this, tr("Choose Color"));
    QPalette palette;
    palette.setColor(QPalette::Base, color);
    if(color.isValid()) {
        ui->textEdit_BaseText->setPalette(palette);
    }
}

//window background color in additional text 1
void MainWindow::on_actionPalette_color_of_additional_text_1_triggered()
{
    textEdit_focused("Additional1");

    QColor color = QColorDialog::getColor(Qt::white, this, tr("Choose Color"));
    QPalette palette;
    palette.setColor(QPalette::Base, color);
    if(color.isValid()) {
        ui->textEdit_Additional1->setPalette(palette);
    }
}

//window background color in additional text 2
void MainWindow::on_actionPalette_color_of_additional_text_2_triggered()
{
    textEdit_focused("Additional2");

    QColor color = QColorDialog::getColor(Qt::white, this, tr("Choose Color"));
    QPalette palette;
    palette.setColor(QPalette::Base, color);
    if(color.isValid()) {
        ui->textEdit_Additional2->setPalette(palette);
    }
}

// //////////////////////////////////////////////////////////
//Formatted text
// //////////////////////////////////////////////////////////

void MainWindow::formatted(QString document_name) {
    QString file = "";

    if (document_name == "TargetText") {
        file = FilenameTargetText;
    }
    else if (document_name == "BaseText") {
        file = FilenameBaseText;
    }
    else if (document_name == "Original") {
        file = FilenameOriginal;
    }
    else if (document_name == "Additional1") {
        file = FilenameAdditional1;
    }
    else if (document_name == "Additional2") {
        file = FilenameAdditional2;
    }
    else return;

    if (file.isEmpty() || file.isNull()) {
        file = QFileDialog::getOpenFileName(this, tr("Open the file"), "",
                                            "Text files (*.txt);;Paratext files (*.sfm *.SFM);;\
HTML files (*.html *.htm)");
    }

    if(!file.isEmpty())
    {
        QFile FileText(file);
        if(FileText.open(QFile::ReadOnly | QFile::Text))
        {
            QTextStream in(&FileText);
            in.setCodec("UTF-8");
            QString text = in.readAll();
            FileText.close();
            QString text_converted = converter_to_html(text);

            if (document_name == "TargetText") {
                ui->textEdit_TargetText->setHtml(text_converted);
                FilenameTargetText = file;
            }
            else if (document_name == "BaseText") {
                ui->textEdit_BaseText->setHtml(text_converted);
                FilenameBaseText = file;
            }
            else if (document_name == "Original") {
                ui->textEdit_Original->setHtml(text_converted);
                FilenameOriginal = file;
            }
            else if (document_name == "Additional1") {
                ui->textEdit_Additional1->setHtml(text_converted);
                FilenameAdditional1 = file;
            }
            else if (document_name == "Additional2") {
                ui->textEdit_Additional2->setHtml(text_converted);
                FilenameAdditional2 = file;
            }
        }
    }
}

// /////////////////////////////////////////////////////
//Unformatted text
// /////////////////////////////////////////////////////

void MainWindow::unformatted(QString document_name)
{
    QString file = "";

    if (document_name == "TargetText") {
        file = FilenameTargetText;
    }
    else if (document_name == "BaseText") {
        file = FilenameBaseText;
    }
    else if (document_name == "Original") {
        file = FilenameOriginal;
    }
    else if (document_name == "Additional1") {
        file = FilenameAdditional1;
    }
    else if (document_name == "Additional2") {
        file = FilenameAdditional2;
    }
    else return;

    if (file.isEmpty() || file.isNull()) {
        file = QFileDialog::getOpenFileName(this, tr("Open the file"), "",
                                            "Text files (*.txt);;Paratext files (*.sfm *.SFM);;\
HTML files (*.html *.htm)");
    }

    if(!file.isEmpty())
    {
        QFile FileText(file);
        if(FileText.open(QFile::ReadOnly | QFile::Text))
        {
            QTextStream in(&FileText);
            in.setCodec("UTF-8");
            QString text = in.readAll();

            FileText.close();

            if (document_name == "TargetText") {
                ui->textEdit_TargetText->setPlainText(text);
                FilenameTargetText = file;
            }
            else if (document_name == "BaseText") {
                ui->textEdit_BaseText->setPlainText(text);
                FilenameBaseText = file;
            }
            else if (document_name == "Original") {
                ui->textEdit_Original->setPlainText(text);
                FilenameOriginal = file;
            }
            else if (document_name == "Additional1") {
                ui->textEdit_Additional1->setPlainText(text);
                FilenameAdditional1 = file;
            }
            else if (document_name == "Additional2") {
                ui->textEdit_Additional2->setPlainText(text);
                FilenameAdditional2 = file;
            }
        }
    }
}

// ///////////////////////////////////////////////////////
//Option "Target text"
// ///////////////////////////////////////////////////////
//changing target text to formatted
void MainWindow::on_actionFormatted_target_text_triggered()
{
    textEdit_focused("TargetText");
    formatted("TargetText");
}

//changing target text to unformatted
void MainWindow::on_actionUnformatted_target_text_triggered()
{
    textEdit_focused("TargetText");
    unformatted("TargetText");
}

// ////////////////////////////////////////////////////
//Option "Base text"
// ////////////////////////////////////////////////////
//changing base text to formatted
void MainWindow::on_actionFormatted_base_text_triggered()
{
    textEdit_focused("BaseText");
    formatted ("BaseText");
}

//changing base text to unformatted
void MainWindow::on_actionUnformatted_base_text_triggered()
{
    textEdit_focused("BaseText");
    unformatted("BaseText");
}

// //////////////////////////////////////////////////////
//Option "Original"
// //////////////////////////////////////////////////////
//changing original to formatted according to Transcelerator
void MainWindow::on_actionFormatted_Transcelerator_triggered()
{
    textEdit_focused("Original");

    QString file = FilenameOriginal;

    if (FilenameOriginal.isEmpty() || FilenameOriginal.isNull()) {
        file = QFileDialog::getOpenFileName(this, tr("Open the file"), "",
                                            "Text files (*.txt);;Paratext files (*.sfm *.SFM);;\
HTML files (*.html *.htm)");
    }

    if(!FilenameOriginal.isEmpty())
    {
        QFile FileText(file);
        if(FileText.open(QFile::ReadOnly | QFile::Text))
        {
            QTextStream in(&FileText);
            in.setCodec("UTF-8");
            QString text = in.readAll();
            FileText.close();
            QString text_converted = converter_from_transcelerator(text);

            ui->textEdit_Original->setHtml(text_converted);
            FilenameOriginal = file;
        }
    }
}

//changing original to formatted
void MainWindow::on_actionFormatted_original_triggered()
{
    textEdit_focused("Original");
    formatted ("Original");
}

//changing original to unformatted
void MainWindow::on_actionUnformatted_original_triggered()
{
    textEdit_focused("Original");
    unformatted("Original");
}

// ////////////////////////////////////////////////////////
//Option "Additional text 1"
// ////////////////////////////////////////////////////////
//changing additional text 1 to formatted
void MainWindow::on_actionFormatted_additional_text_1_triggered()
{
    textEdit_focused("Additional1");
    formatted ("Additional1");
}

//changing additional text 2 to unformatted
void MainWindow::on_actionUnformatted_additional_text_1_triggered()
{
    textEdit_focused("Additional1");
    unformatted ("Additional1");
}

//changing additional text 2 to formatted
void MainWindow::on_actionFormatted_additional_text_2_triggered()
{
    textEdit_focused("Additional2");
    formatted ("Additional2");
}

//changing additional text 2 to unformatted
void MainWindow::on_actionUnformatted_additional_text_2_triggered()
{
    textEdit_focused("Additional2");
    unformatted ("Additional2");
}


// ////////////////////////////////////////////////////////
//Option "Encoding"
// ////////////////////////////////////////////////////////
//encoding of original
void MainWindow::on_actionEncoding_of_original_triggered()
{
    textEdit_focused("Original");
    change_encoding("Original");
    open_original(FilenameOriginal);
}

//encoding of list
void MainWindow::on_actionEncoding_of_list_triggered()
{
    textEdit_focused("List");
    change_encoding("List");
    open_list(FilenameList);
}

//encoding of target text
void MainWindow::on_actionEncoding_of_target_text_triggered()
{
    textEdit_focused("TargetText");
    change_encoding("TargetText");
    open_target_text(FilenameTargetText);
}

//encoding of base text
void MainWindow::on_actionEncoding_of_base_text_triggered()
{
    textEdit_focused("BaseText");
    change_encoding("BaseText");
    open_base_text(FilenameBaseText);
}

//encoding of additional text 1
void MainWindow::on_actionEncoding_of_additional_text_1_triggered()
{
    textEdit_focused("Additional1");
    change_encoding("Additional1");
    open_additional_1(FilenameAdditional1);
}

//encoding of additional text 2
void MainWindow::on_actionEncoding_of_additional_text_2_triggered()
{
    textEdit_focused("Additional2");
    change_encoding("Additional2");
    open_additional_2(FilenameAdditional2);
}

//changing encoding
void MainWindow::change_encoding(QString document_name) {
    QStringList items;
    items << "UTF-8" << "Unicode (UTF-16)" << tr("Windows-1251 (Cyrillic)") << tr("KOI8-R (Cyrillic)") <<
             tr("CP866 / IBM866 (Cyrillic)") << tr("ISO 8859-5 (Cyrillic)") <<
             tr("Windows-1250 (Central European)") << tr("Windows-1252 (Western)") <<
             tr("Windows-1253 (Greek)") << tr("Windows-1254 (Turkish)") <<
             tr("Windows-1255 (Hebrew)") << tr("Windows-1256 (Arabic)") << tr("Windows-1257 (Baltic)") <<
             "ISO 8859-1 (Latin-1)" << "ISO 8859-2 (Latin-2)" <<
             "ISO 8859-3 (Latin-3)" << "ISO 8859-4 (Latin-4)" <<
             "ISO 8859-9 (Latin-5)" << "ISO 8859-10 (Latin-6)" <<
             "ISO 8859-13 (Latin-7)" << "ISO 8859-14 (Latin-8)" <<
             "ISO 8859-15 (Latin-9)" << "ISO 8859-16 (Latin-10)" <<
             tr("ISO 8859-6 (Arabic)") << tr("ISO 8859-7 (Greek)") << tr("ISO 8859-8 (Hebrew)");

    bool ok;
    QString item = QInputDialog::getItem(this, tr("Changing encoding"), tr("Choose encoding:"),
                                         items, 0, false, &ok);

    if (ok && !item.isEmpty()) {
        QString EncodingDocument;

        if (item == "UTF-8") EncodingDocument = "UTF-8";
        else if (item == "Unicode (UTF-16)") EncodingDocument = "Unicode";
        else if (item == tr("Windows-1251 (Cyrillic)")) EncodingDocument = "Windows-1251";
        else if (item == tr("KOI8-R (Cyrillic)")) EncodingDocument = "KOI8-R";
        else if (item == tr("CP866 / IBM866 (Cyrillic)")) EncodingDocument = "IBM866";
        else if (item == tr("ISO 8859-5 (Cyrillic)")) EncodingDocument = "ISO 8859-5";
        else if (item == tr("Windows-1250 (Central European)")) EncodingDocument = "Windows-1250";
        else if (item == tr("Windows-1252 (Western)")) EncodingDocument = "Windows-1252";
        else if (item == tr("Windows-1253 (Greek)")) EncodingDocument = "Windows-1253";
        else if (item == tr("Windows-1254 (Turkish)")) EncodingDocument = "Windows-1254";
        else if (item == tr("Windows-1255 (Hebrew)")) EncodingDocument = "Windows-1255";
        else if (item == tr("Windows-1256 (Arabic)")) EncodingDocument = "Windows-1256";
        else if (item == tr("Windows-1257 (Baltic)")) EncodingDocument = "Windows-1257";
        else if (item == "ISO 8859-1 (Latin-1)") EncodingDocument = "ISO 8859-1";
        else if (item == "ISO 8859-2 (Latin-2)") EncodingDocument = "ISO 8859-2";
        else if (item == "ISO 8859-3 (Latin-3)") EncodingDocument = "ISO 8859-3";
        else if (item == "ISO 8859-4 (Latin-4)") EncodingDocument = "ISO 8859-4";
        else if (item == "ISO 8859-9 (Latin-5)") EncodingDocument = "ISO 8859-9";
        else if (item == "ISO 8859-10 (Latin-6)") EncodingDocument = "ISO 8859-10";
        else if (item == "ISO 8859-13 (Latin-7)") EncodingDocument = "ISO 8859-13";
        else if (item == "ISO 8859-14 (Latin-8)") EncodingDocument = "ISO 8859-14";
        else if (item == "ISO 8859-15 (Latin-9)") EncodingDocument = "ISO 8859-15";
        else if (item == "ISO 8859-16 (Latin-10)") EncodingDocument = "ISO 8859-16";
        else if (item == tr("ISO 8859-6 (Arabic)")) EncodingDocument = "ISO 8859-6";
        else if (item == tr("ISO 8859-7 (Greek)")) EncodingDocument = "ISO 8859-7";
        else if (item == tr("ISO 8859-8 (Hebrew)")) EncodingDocument = "ISO 8859-8";
        else return;

        if (document_name == "List") EncodingList = EncodingDocument;
        else if (document_name == "Original") EncodingOriginal = EncodingDocument;
        else if (document_name == "TargetText") EncodingTargetText = EncodingDocument;
        else if (document_name == "BaseText") EncodingBaseText = EncodingDocument;
        else if (document_name == "Additional1") EncodingAdditional1 = EncodingDocument;
        else if (document_name == "Additional2") EncodingAdditional2 = EncodingDocument;
        else if (document_name == "HTML") EncodingHTML = EncodingDocument;

        else return;
    }
}


// ///////////////////////////////////////////////////////////////
//MENU INSERT
// ///////////////////////////////////////////////////////////////

// //////////////////////////////////////////////////////
//Option "Insert book title"
// //////////////////////////////////////////////////////
//inserting book title into list
void MainWindow::on_actionInsert_book_title_triggered()
{
    textEdit_focused("List");

    QString book_title = QInputDialog::getText(this, tr("Book title"), tr("Enter book title:"),
                                               QLineEdit::Normal);
    ui->textEdit_List->insertPlainText(book_title + "\n");
}

// /////////////////////////////////////////////////////
//Option "Insert number"
// /////////////////////////////////////////////////////
//inserting chapter number into list
void MainWindow::on_actionInsert_chapter_number_triggered()
{
    textEdit_focused("List");

    int chapter_number = QInputDialog::getInt(this, tr("Chapter number"), tr("Enter chapter number:"),
                                              QLineEdit::Normal, 1);
    ui->textEdit_List->insertPlainText(tr("Chapter") + " " + QString::number(chapter_number) + "\n");
}

//inserting chapters numbers into list
void MainWindow::on_actionInsert_chapters_numbers_triggered()
{
    textEdit_focused("List");

    int chapter_number_first = QInputDialog::getInt(this, tr("First chapter number"),
                                                    tr("Enter first chapter number:"), QLineEdit::Normal, 1);
    int chapter_number_last = QInputDialog::getInt(this, tr("Last chapter number"),
                                                   tr("Enter last chapter number:"), QLineEdit::Normal, 2);
    ui->textEdit_List->insertPlainText(tr("Chapters") + " " + QString::number(chapter_number_first) + "-"
                                         + QString::number(chapter_number_last) + "\n");
}

//inserting verse number into list
void MainWindow::on_actionInsert_verse_number_triggered()
{
    textEdit_focused("List");

    int verse_number = QInputDialog::getInt(this, tr("Verse number"), tr("Enter verse number:"),
                                            QLineEdit::Normal, 1);
    ui->textEdit_List->insertPlainText(tr("Verse") + " " + QString::number(verse_number) + "\n");
}

//inserting verses numbers into list
void MainWindow::on_actionInsert_verses_numbers_triggered()
{
    textEdit_focused("List");

    int verse_number_first = QInputDialog::getInt(this, tr("First verse number"), tr("Enter first verse number:"),
                                                  QLineEdit::Normal, 1);
    int verse_number_last = QInputDialog::getInt(this, tr("Last verse number"), tr("Enter last verse number:"),
                                                 QLineEdit::Normal, 2);
    ui->textEdit_List->insertPlainText(tr("Verses") + " " + QString::number(verse_number_first) + "-"
                                         + QString::number(verse_number_last) + "\n");
}

// ////////////////////////////////////////////////////////
//Option "Insert question number"
// ////////////////////////////////////////////////////////
//inserting question number into list
void MainWindow::on_actionInsert_question_number_triggered()
{
    textEdit_focused("List");

    int question_number = QInputDialog::getInt(this, tr("Question number"), tr("Enter question number:"),
                                               QLineEdit::Normal, 1);
    ui->textEdit_List->insertPlainText(tr("Question") + " " + QString::number(question_number) + "\n");
}

// //////////////////////////////////////////////////////////
//Option "Insert time"
// //////////////////////////////////////////////////////////
//inserting time of work start
void MainWindow::on_actionInsert_time_of_work_start_triggered()
{
    textEdit_focused("List");

    QDateTime work_start = QDateTime::currentDateTime();
    ui->textEdit_List->insertPlainText(tr("Work began at:") + " " + work_start.toString("dd/MM/yyyy hh:mm:ss")
                                         + "\n");
}

//inserting time of work end
void MainWindow::on_actionInsert_time_of_work_end_triggered()
{
    textEdit_focused("List");

    QDateTime work_end = QDateTime::currentDateTime();
    ui->textEdit_List->insertPlainText(tr("Work ended at:") + " " + work_end.toString("dd/MM/yyyy hh:mm:ss")
                                         + "\n");
}

// ////////////////////////////////////////////////////////////
//Option "Insert list title"
// ////////////////////////////////////////////////////////////
//inserting list title
void MainWindow::on_actionInsert_list_title_triggered()
{
    textEdit_focused("List");

    ui->textEdit_List->insertHtml(tr("<b>The list of comments of exegetical checking</b><br>") + "\n");
}


// //////////////////////////////////////////////////////////////////////////////
//MENU TEXTUAL COMMENTS
// //////////////////////////////////////////////////////////////////////////////

//inserting "Word is missing" into list of comments
void MainWindow::on_actionWord_is_missing_triggered()
{
    textEdit_focused("List");

    ui->textEdit_List->insertPlainText(tr("Word is missing: "));
}

//inserting "Words are missing" into list of comments
void MainWindow::on_actionWords_are_missing_triggered()
{
    textEdit_focused("List");

    ui->textEdit_List->insertPlainText(tr("Words are missing: "));
}

//inserting "Word should be replaced" into list of comments
void MainWindow::on_actionWord_should_be_replaced_triggered()
{
    textEdit_focused("List");

    ui->textEdit_List->insertPlainText(tr("Word should be replaced: "));
}

//inserting "Words should be replaced" into list of comments
void MainWindow::on_actionWords_should_be_replaced_triggered()
{
    textEdit_focused("List");

     ui->textEdit_List->insertPlainText(tr("Words should be replaced: "));

}

//inserting "Translation has added word(s)" into list of comments
void MainWindow::on_actionTranslation_has_added_word_s_triggered()
{
    textEdit_focused("List");

    ui->textEdit_List->insertPlainText(tr("Translation has added word(s): "));
}

//inserting "In original" into list of comments
void MainWindow::on_actionIn_original_triggered()
{
    textEdit_focused("List");

    ui->textEdit_List->insertPlainText(tr("In oiginal: "));
}

//inserting "It would be more accurately to translate" into list of comments
void MainWindow::on_actionIt_would_be_more_accurate_to_translate_triggered()
{
    textEdit_focused("List");

    ui->textEdit_List->insertPlainText(tr("It would be more accurate to translate: "));
}

// //////////////////////////////////////////////////////////////////////////////
//MENU OTHER VERSIONS
// //////////////////////////////////////////////////////////////////////////////

//inserting "In Chechen" into list of comments
void MainWindow::on_actionIn_Chechen_triggered()
{
    textEdit_focused("List");

    ui->textEdit_List->insertPlainText(tr("In Chechen: "));
}

//inserting "In Avar" into list of comments
void MainWindow::on_actionIn_Avar_triggered()
{
    textEdit_focused("List");

    ui->textEdit_List->insertPlainText(tr("In Avar: "));
}

//inserting "In Adyghe" into list of comments
void MainWindow::on_actionIn_Adyghe_triggered()
{
    textEdit_focused("List");

    ui->textEdit_List->insertPlainText(tr("In Adyghe: "));
}

//inserting "In Kabardian" into list of comments
void MainWindow::on_actionIn_Kabardian_triggered()
{
    textEdit_focused("List");

    ui->textEdit_List->insertPlainText(tr("In Kabardian: "));
}

//inserting "In CARS" into list of comments
void MainWindow::on_actionIn_CARS_triggered()
{
    textEdit_focused("List");

    ui->textEdit_List->insertPlainText(tr("In CARS: "));
}

//inserting "In CRV (RBO)" into list of comments
void MainWindow::on_actionIn_CRV_triggered()
{
    textEdit_focused("List");

    ui->textEdit_List->insertPlainText(tr("In CRV (RBO): "));
}

//inserting "In CASS (Kassian)" into list of comments
void MainWindow::on_actionIn_CASS_triggered()
{
    textEdit_focused("List");

    ui->textEdit_List->insertPlainText(tr("In CASS (Kassian): "));
}

//inserting "In RBT (Kulakov)" into list of comments
void MainWindow::on_actionIn_RBT_Kulakov_triggered()
{
    textEdit_focused("List");

    ui->textEdit_List->insertPlainText(tr("In RBT (Kulakov): "));
}

//inserting "In NRT (Biblica)" into list of comments
void MainWindow::on_actionIn_NRT_Biblica_triggered()
{
    textEdit_focused("List");

    ui->textEdit_List->insertPlainText(tr("In NRT (Biblica): "));
}

//inserting "In Synodal" into list of comments
void MainWindow::on_actionIn_Synodal_triggered()
{
    textEdit_focused("List");

    ui->textEdit_List->insertPlainText(tr("In Synodal: "));
}


// //////////////////////////////////////////////////////////////////////////////
//MENU LANGUAGE COMMENTS
// //////////////////////////////////////////////////////////////////////////////

void MainWindow::on_actionIs_the_pronoun_reference_clear_triggered()
{
    textEdit_focused("List");

    ui->textEdit_List->insertPlainText(tr("Is the pronoun reference clear?"));
}

void MainWindow::on_actionWould_it_be_better_to_turn_it_to_a_pronoun_triggered()
{
    textEdit_focused("List");

    ui->textEdit_List->insertPlainText(tr("Would it be better to turn it to a pronoun?"));
}

void MainWindow::on_actionIs_this_sentence_too_complicated_or_long_triggered()
{
    textEdit_focused("List");

    ui->textEdit_List->insertPlainText(tr("Is this sentence too complicated or long?"));
}

void MainWindow::on_actionWould_it_be_better_to_break_up_this_long_complicated_sentence_triggered()
{
    textEdit_focused("List");

    ui->textEdit_List->insertPlainText(tr("Would it be better to break up this long/complicated sentence?"));
}

void MainWindow::on_actionWould_it_be_better_to_restructure_the_sentence_triggered()
{
    textEdit_focused("List");

    ui->textEdit_List->insertPlainText(tr("Would it be better to restructure the sentence?"));
}

void MainWindow::on_actionHow_does_this_verse_connect_to_the_preceding_text_triggered()
{
    textEdit_focused("List");

    ui->textEdit_List->insertPlainText(tr("How does this verse connect to the preceding text?"));
}

void MainWindow::on_actionHow_is_this_figurative_expression_understood_triggered()
{
    textEdit_focused("List");

    ui->textEdit_List->insertPlainText(tr("How is this figurative expression understood?"));
}



// ////////////////////////////////////////////////////////////////////////
//MENU OPTIONS
// ////////////////////////////////////////////////////////////////////////

// ////////////////////////////////////////////////////////////////
//Option "Interface language"
// ////////////////////////////////////////////////////////////////
//setting English language of interface
void MainWindow::on_actionEnglish_triggered()
{
    language = "English";
    QMessageBox::information(this, tr("Language change"),
                             tr("Interface language will be changed after the program restarts."));
}

//setting Russian language of interface
void MainWindow::on_actionRussian_triggered()
{
    language = "Russian";
    QMessageBox::information(this, tr("Language change"),
                             tr("Interface language will be changed after the program restarts."));
}

// ///////////////////////////////////////////////////////////////
//Option "Insert time automatically"
// ///////////////////////////////////////////////////////////////
//setting automatic insertion of time
void MainWindow::on_actionInsert_time_automatically_triggered()
{
    textEdit_focused("List");

    QMessageBox* messagebox = new QMessageBox (QMessageBox::Question,
                                               tr("Insert time"),
                                               tr("Insert time of beginning and end of work automatically?"),
                                               QMessageBox::Yes | QMessageBox::No, this);
    messagebox->setButtonText(QMessageBox::Yes, tr("Yes"));
    messagebox->setButtonText(QMessageBox::No, tr("No"));
    int n = messagebox->exec();
    delete messagebox;

    if (n == QMessageBox::Yes) {
        TimeAuto = true;
        QMessageBox::information(this, tr("Insert time"),
                                 tr("Time of beginning and end of work will be inserted automatically."));
     } else {
        TimeAuto = false;
        QMessageBox::information(this, tr("Insert time"),
                                 tr("Time of beginning and end of work will not be inserted automatically."));
    }
}

// //////////////////////////////////////////////////////////////////////
//Option "Insert title automatically"
// //////////////////////////////////////////////////////////////////////
//setting option to insert title into new list file automatically
void MainWindow::on_actionInsert_title_automatically_triggered()
{
    textEdit_focused("List");

    QMessageBox* messagebox = new QMessageBox (QMessageBox::Question,
                                               tr("Insert title"),
                                               tr("Insert title into new list file automatically?"),
                                               QMessageBox::Yes | QMessageBox::No, this);
    messagebox->setButtonText(QMessageBox::Yes, tr("Yes"));
    messagebox->setButtonText(QMessageBox::No, tr("No"));
    int n = messagebox->exec();
    delete messagebox;

    if (n == QMessageBox::Yes) {
        TitleAuto = true;
        QMessageBox::information(this, tr("Insert title"),
                                 tr("Title will be inserted into new list file automatically."));
     } else {
        TitleAuto = false;
        QMessageBox::information(this, tr("Insert title"),
                                 tr("Title will not be inserted into new list file automatically."));
    }
}

// /////////////////////////////////////////////////////
//Option "Load files automatically"
// /////////////////////////////////////////////////////
//setting option to load or not load files automatically at start
void MainWindow::on_actionLoad_files_automatically_triggered()
{
    QMessageBox* messagebox = new QMessageBox (QMessageBox::Question,
                                               tr("Files loading"),
                                               tr("By default, files are loaded automatically\
 at start of the program. Do you want to stop automatic loading?"),
                                               QMessageBox::Yes | QMessageBox::No, this);
    messagebox->setButtonText(QMessageBox::Yes, tr("Yes"));
    messagebox->setButtonText(QMessageBox::No, tr("No"));
    int n = messagebox->exec();
    delete messagebox;

    if (n == QMessageBox::Yes) {
        FilesNotLoadAuto = true;
        QMessageBox::information(this, tr("Files loading"),
                                 tr("Files will not be loaded automatically at start."));
     } else {
        FilesNotLoadAuto = false;
        QMessageBox::information(this, tr("Files loading"),
                                 tr("Files will be loaded automatically at start."));
    }
}


// //////////////////////////////////////////////////////
//Option "USFM -> HTML"
// //////////////////////////////////////////////////////

//method for USFM -> HTML (without formatting)
void MainWindow::USFM_to_HTML(QString document_name) {
    QString file = "";

    if (document_name == "TargetText") {
        file = FilenameTargetText;
    }
    else if (document_name == "BaseText") {
        file = FilenameBaseText;
    }
    else if (document_name == "Original") {
        file = FilenameOriginal;
    }
    else return;

    if (file.isEmpty() || file.isNull()) {
        file = QFileDialog::getOpenFileName(this, tr("Open the file"), "",
                                            "Paratext files (*.sfm *.SFM)");
    }

    if(!file.isEmpty() && (file.endsWith(".sfm") || (file.endsWith(".SFM"))))
    {
        QFile FileText(file);
        if(FileText.open(QFile::ReadOnly | QFile::Text))
        {
            QTextStream in(&FileText);

            if (EncodingHTML == "UTF-8") in.setCodec("UTF-8");
            else if (EncodingHTML == "Unicode") in.setCodec("Unicode");
            else if (EncodingHTML == "Windows-1251") in.setCodec("Windows-1251");
            else if (EncodingHTML == "KOI8-R") in.setCodec("KOI8-R");
            else if (EncodingHTML == "IBM866") in.setCodec("IBM866");
            else if (EncodingHTML == "ISO 8859-5") in.setCodec("ISO 8859-5");
            else if (EncodingHTML == "Windows-1250") in.setCodec("Windows-1250");
            else if (EncodingHTML == "Windows-1252") in.setCodec("Windows-1252");
            else if (EncodingHTML == "Windows-1253") in.setCodec("Windows-1253");
            else if (EncodingHTML == "Windows-1254") in.setCodec("Windows-1254");
            else if (EncodingHTML == "Windows-1255") in.setCodec("Windows-1255");
            else if (EncodingHTML == "Windows-1256") in.setCodec("Windows-1256");
            else if (EncodingHTML == "Windows-1257") in.setCodec("Windows-1257");
            else if (EncodingHTML == "ISO 8859-1") in.setCodec("ISO 8859-1");
            else if (EncodingHTML == "ISO 8859-2") in.setCodec("ISO 8859-2");
            else if (EncodingHTML == "ISO 8859-3") in.setCodec("ISO 8859-3");
            else if (EncodingHTML == "ISO 8859-4") in.setCodec("ISO 8859-4");
            else if (EncodingHTML == "ISO 8859-9") in.setCodec("ISO 8859-9");
            else if (EncodingHTML == "ISO 8859-10") in.setCodec("ISO 8859-10");
            else if (EncodingHTML == "ISO 8859-13") in.setCodec("ISO 8859-13");
            else if (EncodingHTML == "ISO 8859-14") in.setCodec("ISO 8859-14");
            else if (EncodingHTML == "ISO 8859-15") in.setCodec("ISO 8859-15");
            else if (EncodingHTML == "ISO 8859-16") in.setCodec("ISO 8859-16");
            else if (EncodingHTML == "ISO 8859-6") in.setCodec("ISO 8859-6");
            else if (EncodingHTML == "ISO 8859-7") in.setCodec("ISO 8859-7");
            else if (EncodingHTML == "ISO 8859-8") in.setCodec("ISO 8859-8");
            else in.setCodec("UTF-8");

            QString text = in.readAll();
            FileText.close();

            QString text_converted = converter_to_html(text);

            if (document_name == "TargetText") {
                ui->textEdit_TargetText->setPlainText(text_converted);
                FilenameTargetText = file;
            }
            else if (document_name == "BaseText") {
                ui->textEdit_BaseText->setPlainText(text_converted);
                FilenameBaseText = file;
            }
            else if (document_name == "Original") {
                ui->textEdit_Original->setPlainText(text_converted);
                FilenameOriginal = file;
            }
        }
    }
}

//USFM to HTML of original
void MainWindow::on_actionOriginalUSFM_HTML_triggered()
{
    textEdit_focused("Original");
    USFM_to_HTML("Original");
}

//USFM to HTML of target text
void MainWindow::on_actionTarget_textUSFM_HTML_triggered()
{
    textEdit_focused("TargetText");
    USFM_to_HTML("TargetText");
}

//USFM to HTML of base text
void MainWindow::on_actionBase_textUSFM_HTML_triggered()
{
    textEdit_focused("BaseText");
    USFM_to_HTML("BaseText");
}

//USFM to HTML of additional text 1
void MainWindow::on_actionAdditional_text_1_USFM_HTML_triggered()
{
    textEdit_focused("Additional1");
    USFM_to_HTML("Additional1");
}

//USFM to HTML of additional text 2
void MainWindow::on_actionAdditional_text_2_USFM_HTML_triggered()
{
    textEdit_focused("Additional2");
    USFM_to_HTML("Additional2");
}


//changing encoding for USFM -> HTML
void MainWindow::on_actionEncoding_for_USFM_HTML_triggered()
{
    change_encoding("HTML");
}



// ///////////////////////////////////////////////////////////////////
//MENU ABOUT
// ///////////////////////////////////////////////////////////////////

//about program
void MainWindow::on_actionAbout_triggered()
{
    QString about_text;
    about_text  = tr("Program for exegetical checking of Bible texts") + "\n\n";

    about_text += tr("Copyright (C) 2021 Aleksandr Migunov") + "\n\n";

    about_text += "This program is free software; you can redistribute it and/or modify \
it under the terms of the GNU General Public License as published by \
the Free Software Foundation; either version 3 of the License, or \
(at your option) any later version. \n\n";

    about_text += "This program is distributed in the hope that it will be useful, \
but WITHOUT ANY WARRANTY; without even the implied warranty of \
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the \
GNU General Public License for more details. \n\n";

    about_text += "You should have received a copy of the GNU General Public License \
along with this program; if not, write to the Free Software \
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.";

    QMessageBox::about(this, tr("About Program"), about_text);
}

//about Qt
void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(0);
}


// ////////////////////////////////////////////////////////////////////
//TOOLBAR
// ////////////////////////////////////////////////////////////////////

//setting bold text
void MainWindow::on_actionTextBold_triggered()
{
    textEdit_focused("List");

    if (TextBold == false) {
        ui->textEdit_List->setFontWeight(QFont::Bold);
        TextBold = true;
    }
    else {
        ui->textEdit_List->setFontWeight(QFont::Normal);
        TextBold = false;
    }
}

//setting italic text
void MainWindow::on_actionTextItalic_triggered()
{
    textEdit_focused("List");

    if (TextItalic == false) {
        ui->textEdit_List->setFontItalic(true);
        TextItalic = true;
    }
    else {
        ui->textEdit_List->setFontItalic(false);
        TextItalic = false;
    }
}

//setting underlined text
void MainWindow::on_actionTextUnderline_triggered()
{
    textEdit_focused("List");

    if (TextUnderline == false) {
        ui->textEdit_List->setFontUnderline(true);
        TextUnderline = true;
    }
    else {
        ui->textEdit_List->setFontUnderline(false);
        TextUnderline = false;
    }
}

//setting left alignment
void MainWindow::on_actionAlignLeft_triggered()
{
    textEdit_focused("List");

    ui->textEdit_List->setAlignment(Qt::AlignLeft);
}

//setting center alignment
void MainWindow::on_actionAlignCenter_triggered()
{
    textEdit_focused("List");

    ui->textEdit_List->setAlignment(Qt::AlignCenter);
}

//setting justify alignment
void MainWindow::on_actionAlignJustify_triggered()
{
    textEdit_focused("List");

    ui->textEdit_List->setAlignment(Qt::AlignJustify);
}

//setting right alignment
void MainWindow::on_actionAlignRight_triggered()
{
    textEdit_focused("List");

    ui->textEdit_List->setAlignment(Qt::AlignRight);
}

// ////////////////////////////////////////////////////////
//CONTEXT MENUS
// ////////////////////////////////////////////////////////

//setting contect menu for original
void MainWindow::on_textEdit_Original_customContextMenuRequested()
{
    textEdit_focused("Original");

    QAction *actionOpen = new QAction(tr("Open"), this);
    connect(actionOpen, SIGNAL(triggered()), this, SLOT(on_actionOpen_original_triggered()));

    QAction *actionClose = new QAction(tr("Close"), this);
    connect(actionClose, SIGNAL(triggered()), this, SLOT(on_actionClose_original_triggered()));

    QAction *actionPrint = new QAction(tr("Print"), this);
    connect(actionPrint, SIGNAL(triggered()), this, SLOT(on_actionPrint_original_triggered()));

    QAction *actionPrintPreview = new QAction(tr("Print Preview"), this);
    connect(actionPrintPreview, SIGNAL(triggered()), this, SLOT(on_actionPrint_preview_original_triggered()));

    QAction *actionConvertToPDF = new QAction(tr("Convert To PDF"), this);
    connect(actionConvertToPDF, SIGNAL(triggered()), this, SLOT(on_actionConvert_original_to_PDF_triggered()));

    QAction *actionConvertToODT = new QAction(tr("Convert To ODT"), this);
    connect(actionConvertToODT, SIGNAL(triggered()), this, SLOT(on_actionConvert_original_to_ODT_triggered()));

    QAction *actionFormattedTranscelerator = new QAction(tr("Formatted (Transcelerator)"), this);
    connect(actionFormattedTranscelerator, SIGNAL(triggered()), this,
            SLOT(on_actionFormatted_Transcelerator_triggered()));

    QAction *actionFormatted = new QAction(tr("Formatted (USFM/Paratext)"), this);
    connect(actionFormatted, SIGNAL(triggered()), this, SLOT(on_actionFormatted_original_triggered()));

    QAction *actionUnformatted = new QAction(tr("Unformatted (USFM/Paratext)"), this);
    connect(actionUnformatted, SIGNAL(triggered()), this, SLOT(on_actionUnformatted_original_triggered()));

    QAction *actionEncoding = new QAction(tr("Encoding"), this);
    connect(actionEncoding, SIGNAL(triggered()), this, SLOT(on_actionEncoding_of_original_triggered()));

    QAction *actionCopy = new QAction(tr("Copy"), this);
    connect(actionCopy, SIGNAL(triggered()), this, SLOT(on_actionCopy_from_original_triggered()));

    QAction *actionCopyPaste = new QAction(tr("Copy and Paste into List"), this);
    connect(actionCopyPaste, SIGNAL(triggered()), this, SLOT(on_actionCopy_and_paste_from_original_triggered()));

    QAction *actionSelectAll = new QAction(tr("Select All"), this);
    connect(actionSelectAll, SIGNAL(triggered()), this, SLOT(on_actionSelect_all_from_original_triggered()));

    QAction *actionFind = new QAction(tr("Find"), this);
    connect(actionFind, SIGNAL(triggered()), this, SLOT(on_actionFind_in_original_triggered()));

    QAction *actionZoomIn = new QAction(tr("Zoom In"), this);
    connect(actionZoomIn, SIGNAL(triggered()), this, SLOT(on_actionZoom_in_original_triggered()));

    QAction *actionZoomOut = new QAction(tr("Zoom Out"), this);
    connect(actionZoomOut, SIGNAL(triggered()), this, SLOT(on_actionZoom_out_original_triggered()));

    QMenu *contextMenu = new QMenu(this);
    contextMenu->addAction(actionOpen);
    contextMenu->addAction(actionClose);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionPrint);
    contextMenu->addAction(actionPrintPreview);
    contextMenu->addAction(actionConvertToPDF);
    contextMenu->addAction(actionConvertToODT);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionFormattedTranscelerator);
    contextMenu->addAction(actionFormatted);
    contextMenu->addAction(actionUnformatted);
    contextMenu->addAction(actionEncoding);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionCopy);
    contextMenu->addAction(actionCopyPaste);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionSelectAll);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionFind);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionZoomIn);
    contextMenu->addAction(actionZoomOut);
    contextMenu->exec(QCursor::pos());
}

//setting context menu for list
void MainWindow::on_textEdit_List_customContextMenuRequested()
{
    textEdit_focused("List");

    QAction *actionNew = new QAction(tr("New"), this);
    connect(actionNew, SIGNAL(triggered()), this, SLOT(on_actionNew_list_triggered()));

    QAction *actionOpen = new QAction(tr("Open"), this);
    connect(actionOpen, SIGNAL(triggered()), this, SLOT(on_actionOpen_list_triggered()));

    QAction *actionSave = new QAction(tr("Save"), this);
    connect(actionSave, SIGNAL(triggered()), this, SLOT(on_actionSave_list_triggered()));

    QAction *actionSaveAs = new QAction(tr("Save As"), this);
    connect(actionSaveAs, SIGNAL(triggered()), this, SLOT(on_actionSave_list_as_triggered()));

    QAction *actionPrint = new QAction(tr("Print"), this);
    connect(actionPrint, SIGNAL(triggered()), this, SLOT(on_actionPrint_list_triggered()));

    QAction *actionPrintPreview = new QAction(tr("Print Preview"), this);
    connect(actionPrintPreview, SIGNAL(triggered()), this, SLOT(on_actionPrint_preview_list_triggered()));

    QAction *actionConvertToPDF = new QAction(tr("Convert To PDF"), this);
    connect(actionConvertToPDF, SIGNAL(triggered()), this, SLOT(on_actionConvert_list_to_PDF_triggered()));

    QAction *actionConvertToODT = new QAction(tr("Convert To ODT"), this);
    connect(actionConvertToODT, SIGNAL(triggered()), this, SLOT(on_actionConvert_list_to_ODT_triggered()));

    QAction *actionEncoding = new QAction(tr("Encoding"), this);
    connect(actionEncoding, SIGNAL(triggered()), this, SLOT(on_actionEncoding_of_list_triggered()));

    QAction *actionClose = new QAction(tr("Close"), this);
    connect(actionClose, SIGNAL(triggered()), this, SLOT(on_actionClose_list_triggered()));

    QAction *actionUndo = new QAction(tr("Undo"), this);
    connect(actionUndo, SIGNAL(triggered()), this, SLOT(on_actionUndo_triggered()));

    QAction *actionRedo = new QAction(tr("Redo"), this);
    connect(actionRedo, SIGNAL(triggered()), this, SLOT(on_actionRedo_triggered()));

    QAction *actionCut = new QAction(tr("Cut"), this);
    connect(actionCut, SIGNAL(triggered()), this, SLOT(on_actionCut_triggered()));

    QAction *actionPaste = new QAction(tr("Paste"), this);
    connect(actionPaste, SIGNAL(triggered()), this, SLOT(on_actionPaste_triggered()));

    QAction *actionDelete = new QAction(tr("Delete"), this);
    connect(actionDelete, SIGNAL(triggered()), this, SLOT(on_actionDelete_triggered()));

    QAction *actionSelectAll = new QAction(tr("Select All"), this);
    connect(actionSelectAll, SIGNAL(triggered()), this, SLOT(on_actionSelect_all_from_list_triggered()));

    QAction *actionCopy = new QAction(tr("Copy"), this);
    connect(actionCopy, SIGNAL(triggered()), this, SLOT(on_actionCopy_from_original_triggered()));

    QAction *actionCopyPaste = new QAction(tr("Copy and Paste"), this);
    connect(actionCopyPaste, SIGNAL(triggered()), this, SLOT(on_actionCopy_and_paste_from_list_triggered()));

    QAction *actionFind = new QAction(tr("Find"), this);
    connect(actionFind, SIGNAL(triggered()), this, SLOT(on_actionFind_in_list_triggered()));

    QAction *actionFindReplace = new QAction(tr("Find and Replace"), this);
    connect(actionFindReplace, SIGNAL(triggered()), this, SLOT(on_actionFind_and_replace_triggered()));

    QAction *actionFindReplaceAll = new QAction(tr("Find and Replace All"), this);
    connect(actionFindReplaceAll, SIGNAL(triggered()), this, SLOT(on_actionFind_and_replace_all_triggered()));

    QAction *actionZoomIn = new QAction(tr("Zoom In"), this);
    connect(actionZoomIn, SIGNAL(triggered()), this, SLOT(on_actionZoom_in_list_triggered()));

    QAction *actionZoomOut = new QAction(tr("Zoom Out"), this);
    connect(actionZoomOut, SIGNAL(triggered()), this, SLOT(on_actionZoom_out_list_triggered()));

    QMenu *contextMenu = new QMenu(this);
    contextMenu->addAction(actionNew);
    contextMenu->addAction(actionOpen);
    contextMenu->addAction(actionSave);
    contextMenu->addAction(actionSaveAs);
    contextMenu->addAction(actionClose);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionPrint);
    contextMenu->addAction(actionPrintPreview);
    contextMenu->addAction(actionConvertToPDF);
    contextMenu->addAction(actionConvertToODT);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionEncoding);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionUndo);
    contextMenu->addAction(actionRedo);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionCut);
    contextMenu->addAction(actionCopy);
    contextMenu->addAction(actionPaste);
    contextMenu->addAction(actionCopyPaste);
    contextMenu->addAction(actionDelete);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionSelectAll);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionFind);
    contextMenu->addAction(actionFindReplace);
    contextMenu->addAction(actionFindReplaceAll);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionZoomIn);
    contextMenu->addAction(actionZoomOut);
    contextMenu->exec(QCursor::pos());
}

//setting context menu for target text
void MainWindow::on_textEdit_TargetText_customContextMenuRequested()
{
    textEdit_focused("TargetText");

    QAction *actionOpen = new QAction(tr("Open"), this);
    connect(actionOpen, SIGNAL(triggered()), this, SLOT(on_actionOpen_target_text_triggered()));

    QAction *actionClose = new QAction(tr("Close"), this);
    connect(actionClose, SIGNAL(triggered()), this, SLOT(on_actionClose_target_text_triggered()));

    QAction *actionPrint = new QAction(tr("Print"), this);
    connect(actionPrint, SIGNAL(triggered()), this, SLOT(on_actionPrint_target_text_triggered()));

    QAction *actionPrintPreview = new QAction(tr("Print Preview"), this);
    connect(actionPrintPreview, SIGNAL(triggered()), this, SLOT(on_actionPrint_preview_target_text_triggered()));

    QAction *actionConvertToPDF = new QAction(tr("Convert To PDF"), this);
    connect(actionConvertToPDF, SIGNAL(triggered()), this, SLOT(on_actionConvert_target_text_to_PDF_triggered()));

    QAction *actionConvertToODT = new QAction(tr("Convert To ODT"), this);
    connect(actionConvertToODT, SIGNAL(triggered()), this, SLOT(on_actionConvert_target_text_to_ODT_triggered()));

    QAction *actionFormatted = new QAction(tr("Formatted (USFM/Paratext)"), this);
    connect(actionFormatted, SIGNAL(triggered()), this, SLOT(on_actionFormatted_target_text_triggered()));

    QAction *actionUnformatted = new QAction(tr("Unformatted (USFM/Paratext)"), this);
    connect(actionUnformatted, SIGNAL(triggered()), this, SLOT(on_actionUnformatted_target_text_triggered()));

    QAction *actionEncoding = new QAction(tr("Encoding"), this);
    connect(actionEncoding, SIGNAL(triggered()), this, SLOT(on_actionEncoding_of_target_text_triggered()));

    QAction *actionCopy = new QAction(tr("Copy"), this);
    connect(actionCopy, SIGNAL(triggered()), this, SLOT(on_actionCopy_from_target_text_triggered()));

    QAction *actionCopyPaste = new QAction(tr("Copy and Paste into List"), this);
    connect(actionCopyPaste, SIGNAL(triggered()), this, SLOT(on_actionCopy_and_paste_from_target_text_triggered()));

    QAction *actionSelectAll = new QAction(tr("Select All"), this);
    connect(actionSelectAll, SIGNAL(triggered()), this, SLOT(on_actionSelect_all_from_target_text_triggered()));

    QAction *actionFind = new QAction(tr("Find"), this);
    connect(actionFind, SIGNAL(triggered()), this, SLOT(on_actionFind_in_target_text_triggered()));

    QAction *actionZoomIn = new QAction(tr("Zoom In"), this);
    connect(actionZoomIn, SIGNAL(triggered()), this, SLOT(on_actionZoom_in_target_text_triggered()));

    QAction *actionZoomOut = new QAction(tr("Zoom Out"), this);
    connect(actionZoomOut, SIGNAL(triggered()), this, SLOT(on_actionZoom_out_target_text_triggered()));

    QMenu *contextMenu = new QMenu(this);
    contextMenu->addAction(actionOpen);
    contextMenu->addAction(actionClose);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionPrint);
    contextMenu->addAction(actionPrintPreview);
    contextMenu->addAction(actionConvertToPDF);
    contextMenu->addAction(actionConvertToODT);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionFormatted);
    contextMenu->addAction(actionUnformatted);
    contextMenu->addAction(actionEncoding);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionCopy);
    contextMenu->addAction(actionCopyPaste);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionSelectAll);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionFind);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionZoomIn);
    contextMenu->addAction(actionZoomOut);
    contextMenu->exec(QCursor::pos());
}

//setting context menu for base text
void MainWindow::on_textEdit_BaseText_customContextMenuRequested()
{
    textEdit_focused("BaseText");

    QAction *actionOpen = new QAction(tr("Open"), this);
    connect(actionOpen, SIGNAL(triggered()), this, SLOT(on_actionOpen_base_text_triggered()));

    QAction *actionClose = new QAction(tr("Close"), this);
    connect(actionClose, SIGNAL(triggered()), this, SLOT(on_actionClose_base_text_triggered()));

    QAction *actionPrint = new QAction(tr("Print"), this);
    connect(actionPrint, SIGNAL(triggered()), this, SLOT(on_actionPrint_base_text_triggered()));

    QAction *actionPrintPreview = new QAction(tr("Print Preview"), this);
    connect(actionPrintPreview, SIGNAL(triggered()), this, SLOT(on_actionPrint_preview_base_text_triggered()));

    QAction *actionConvertToPDF = new QAction(tr("Convert To PDF"), this);
    connect(actionConvertToPDF, SIGNAL(triggered()), this, SLOT(on_actionConvert_base_text_to_PDF_triggered()));

    QAction *actionConvertToODT = new QAction(tr("Convert To ODT"), this);
    connect(actionConvertToODT, SIGNAL(triggered()), this, SLOT(on_actionConvert_base_text_to_ODT_triggered()));

    QAction *actionFormatted = new QAction(tr("Formatted (USFM/Paratext)"), this);
    connect(actionFormatted, SIGNAL(triggered()), this, SLOT(on_actionFormatted_base_text_triggered()));

    QAction *actionUnformatted = new QAction(tr("Unformatted (USFM/Paratext)"), this);
    connect(actionUnformatted, SIGNAL(triggered()), this, SLOT(on_actionUnformatted_base_text_triggered()));

    QAction *actionEncoding = new QAction(tr("Encoding"), this);
    connect(actionEncoding, SIGNAL(triggered()), this, SLOT(on_actionEncoding_of_base_text_triggered()));

    QAction *actionCopy = new QAction(tr("Copy"), this);
    connect(actionCopy, SIGNAL(triggered()), this, SLOT(on_actionCopy_from_base_text_triggered()));

    QAction *actionCopyPaste = new QAction(tr("Copy and Paste into List"), this);
    connect(actionCopyPaste, SIGNAL(triggered()), this, SLOT(on_actionCopy_and_paste_from_base_text_triggered()));

    QAction *actionSelectAll = new QAction(tr("Select All"), this);
    connect(actionSelectAll, SIGNAL(triggered()), this, SLOT(on_actionSelect_all_from_base_text_triggered()));

    QAction *actionFind = new QAction(tr("Find"), this);
    connect(actionFind, SIGNAL(triggered()), this, SLOT(on_actionFind_in_base_text_triggered()));

    QAction *actionZoomIn = new QAction(tr("Zoom In"), this);
    connect(actionZoomIn, SIGNAL(triggered()), this, SLOT(on_actionZoom_in_base_text_triggered()));

    QAction *actionZoomOut = new QAction(tr("Zoom Out"), this);
    connect(actionZoomOut, SIGNAL(triggered()), this, SLOT(on_actionZoom_out_base_text_triggered()));

    QMenu *contextMenu = new QMenu(this);
    contextMenu->addAction(actionOpen);
    contextMenu->addAction(actionClose);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionPrint);
    contextMenu->addAction(actionPrintPreview);
    contextMenu->addAction(actionConvertToPDF);
    contextMenu->addAction(actionConvertToODT);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionFormatted);
    contextMenu->addAction(actionUnformatted);
    contextMenu->addAction(actionEncoding);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionCopy);
    contextMenu->addAction(actionCopyPaste);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionSelectAll);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionFind);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionZoomIn);
    contextMenu->addAction(actionZoomOut);
    contextMenu->exec(QCursor::pos());

}

void MainWindow::on_textEdit_Additional1_customContextMenuRequested()
{
    textEdit_focused("Additional1");

    QAction *actionOpen = new QAction(tr("Open"), this);
    connect(actionOpen, SIGNAL(triggered()), this, SLOT(on_actionOpen_additional_text_1_triggered()));

    QAction *actionClose = new QAction(tr("Close"), this);
    connect(actionClose, SIGNAL(triggered()), this, SLOT(on_actionClose_additional_text_1_triggered()));

    QAction *actionPrint = new QAction(tr("Print"), this);
    connect(actionPrint, SIGNAL(triggered()), this, SLOT(on_actionPrint_additional_text_1_triggered()));

    QAction *actionPrintPreview = new QAction(tr("Print Preview"), this);
    connect(actionPrintPreview, SIGNAL(triggered()), this, SLOT(on_actionPrint_preview_additional_text_1_triggered()));

    QAction *actionConvertToPDF = new QAction(tr("Convert To PDF"), this);
    connect(actionConvertToPDF, SIGNAL(triggered()), this, SLOT(on_actionConvert_additional_text_1_to_PDF_triggered()));

    QAction *actionConvertToODT = new QAction(tr("Convert To ODT"), this);
    connect(actionConvertToODT, SIGNAL(triggered()), this, SLOT(on_actionConvert_additional_text_1_to_ODT_triggered()));

    QAction *actionFormatted = new QAction(tr("Formatted (USFM/Paratext)"), this);
    connect(actionFormatted, SIGNAL(triggered()), this, SLOT(on_actionFormatted_additional_text_1_triggered()));

    QAction *actionUnformatted = new QAction(tr("Unformatted (USFM/Paratext)"), this);
    connect(actionUnformatted, SIGNAL(triggered()), this, SLOT(on_actionUnformatted_additional_text_1_triggered()));

    QAction *actionEncoding = new QAction(tr("Encoding"), this);
    connect(actionEncoding, SIGNAL(triggered()), this, SLOT(on_actionEncoding_of_additional_text_1_triggered()));

    QAction *actionCopy = new QAction(tr("Copy"), this);
    connect(actionCopy, SIGNAL(triggered()), this, SLOT(on_actionCopy_from_additional_text_1_triggered()));

    QAction *actionCopyPaste = new QAction(tr("Copy and Paste into List"), this);
    connect(actionCopyPaste, SIGNAL(triggered()), this, SLOT(on_actionCopy_and_paste_from_additional_text_1_triggered()));

    QAction *actionSelectAll = new QAction(tr("Select All"), this);
    connect(actionSelectAll, SIGNAL(triggered()), this, SLOT(on_actionSelect_all_from_additional_text_1_triggered()));

    QAction *actionFind = new QAction(tr("Find"), this);
    connect(actionFind, SIGNAL(triggered()), this, SLOT(on_actionFind_in_additional_text_1_triggered()));

    QAction *actionZoomIn = new QAction(tr("Zoom In"), this);
    connect(actionZoomIn, SIGNAL(triggered()), this, SLOT(on_actionZoom_in_additional_text_1_triggered()));

    QAction *actionZoomOut = new QAction(tr("Zoom Out"), this);
    connect(actionZoomOut, SIGNAL(triggered()), this, SLOT(on_actionZoom_out_additional_text_1_triggered()));

    QMenu *contextMenu = new QMenu(this);
    contextMenu->addAction(actionOpen);
    contextMenu->addAction(actionClose);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionPrint);
    contextMenu->addAction(actionPrintPreview);
    contextMenu->addAction(actionConvertToPDF);
    contextMenu->addAction(actionConvertToODT);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionFormatted);
    contextMenu->addAction(actionUnformatted);
    contextMenu->addAction(actionEncoding);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionCopy);
    contextMenu->addAction(actionCopyPaste);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionSelectAll);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionFind);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionZoomIn);
    contextMenu->addAction(actionZoomOut);
    contextMenu->exec(QCursor::pos());
}

void MainWindow::on_textEdit_Additional2_customContextMenuRequested()
{
    textEdit_focused("Additional2");

    QAction *actionOpen = new QAction(tr("Open"), this);
    connect(actionOpen, SIGNAL(triggered()), this, SLOT(on_actionOpen_additional_text_2_triggered()));

    QAction *actionClose = new QAction(tr("Close"), this);
    connect(actionClose, SIGNAL(triggered()), this, SLOT(on_actionClose_additional_text_2_triggered()));

    QAction *actionPrint = new QAction(tr("Print"), this);
    connect(actionPrint, SIGNAL(triggered()), this, SLOT(on_actionPrint_additional_text_2_triggered()));

    QAction *actionPrintPreview = new QAction(tr("Print Preview"), this);
    connect(actionPrintPreview, SIGNAL(triggered()), this, SLOT(on_actionPrint_preview_additional_text_2_triggered()));

    QAction *actionConvertToPDF = new QAction(tr("Convert To PDF"), this);
    connect(actionConvertToPDF, SIGNAL(triggered()), this, SLOT(on_actionConvert_additional_text_2_to_PDF_triggered()));

    QAction *actionConvertToODT = new QAction(tr("Convert To ODT"), this);
    connect(actionConvertToODT, SIGNAL(triggered()), this, SLOT(on_actionConvert_additional_text_2_to_ODT_triggered()));

    QAction *actionFormatted = new QAction(tr("Formatted (USFM/Paratext)"), this);
    connect(actionFormatted, SIGNAL(triggered()), this, SLOT(on_actionFormatted_additional_text_2_triggered()));

    QAction *actionUnformatted = new QAction(tr("Unformatted (USFM/Paratext)"), this);
    connect(actionUnformatted, SIGNAL(triggered()), this, SLOT(on_actionUnformatted_additional_text_2_triggered()));

    QAction *actionEncoding = new QAction(tr("Encoding"), this);
    connect(actionEncoding, SIGNAL(triggered()), this, SLOT(on_actionEncoding_of_additional_text_2_triggered()));

    QAction *actionCopy = new QAction(tr("Copy"), this);
    connect(actionCopy, SIGNAL(triggered()), this, SLOT(on_actionCopy_from_additional_text_2_triggered()));

    QAction *actionCopyPaste = new QAction(tr("Copy and Paste into List"), this);
    connect(actionCopyPaste, SIGNAL(triggered()), this, SLOT(on_actionCopy_and_paste_from_additional_text_2_triggered()));

    QAction *actionSelectAll = new QAction(tr("Select All"), this);
    connect(actionSelectAll, SIGNAL(triggered()), this, SLOT(on_actionSelect_all_from_additional_text_2_triggered()));

    QAction *actionFind = new QAction(tr("Find"), this);
    connect(actionFind, SIGNAL(triggered()), this, SLOT(on_actionFind_in_additional_text_2_triggered()));

    QAction *actionZoomIn = new QAction(tr("Zoom In"), this);
    connect(actionZoomIn, SIGNAL(triggered()), this, SLOT(on_actionZoom_in_additional_text_2_triggered()));

    QAction *actionZoomOut = new QAction(tr("Zoom Out"), this);
    connect(actionZoomOut, SIGNAL(triggered()), this, SLOT(on_actionZoom_out_additional_text_2_triggered()));

    QMenu *contextMenu = new QMenu(this);
    contextMenu->addAction(actionOpen);
    contextMenu->addAction(actionClose);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionPrint);
    contextMenu->addAction(actionPrintPreview);
    contextMenu->addAction(actionConvertToPDF);
    contextMenu->addAction(actionConvertToODT);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionFormatted);
    contextMenu->addAction(actionUnformatted);
    contextMenu->addAction(actionEncoding);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionCopy);
    contextMenu->addAction(actionCopyPaste);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionSelectAll);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionFind);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionZoomIn);
    contextMenu->addAction(actionZoomOut);
    contextMenu->exec(QCursor::pos());
}


// ////////////////////////////////////////////////////////////
//Options "Paratext file editing"
// ////////////////////////////////////////////////////////////

void MainWindow::on_actionOpen_Paratext_file_for_editing_triggered()
{
    textEdit_focused("List");
    if (ParatextFileEditing == false) {
        close_list(); //closing previously opened list file
    }

    //opening Paratext file for editing
    QString file = QFileDialog::getOpenFileName(this, tr("Open Paratext file for editing"), "",
                                                tr("Paratext files (*.sfm *.SFM)"));

    if(!file.isEmpty())
    {
        ParatextFileName = file;
        QFile FileList(ParatextFileName);
        if(FileList.open(QFile::ReadOnly | QFile::Text))
        {
            QTextStream in(&FileList);
            in.setCodec("UTF-8");

            QString text = in.readAll();
            FileList.close();

            ui->textEdit_List->setText(text);
            ParatextFileEditing = true;
            ui->label_dir_list->setText(ParatextFileName);
        }
    }
}

void MainWindow::on_actionSave_Paratext_file_triggered()
{
    textEdit_focused("List");

    if (ParatextFileEditing == false) {
        return;
    }

    QMessageBox* messagebox = new QMessageBox (QMessageBox::Question,
                                               tr("Confirm"),
                                               tr("This is a Paratext file. Do you really want to save it with the same name?"),
                                               QMessageBox::Yes | QMessageBox::No, this);
    messagebox->setButtonText(QMessageBox::Yes, tr("Yes"));
    messagebox->setButtonText(QMessageBox::No, tr("No"));
    int n = messagebox->exec();
    delete messagebox;

    if (n == QMessageBox::No) {
        on_actionSave_Paratext_file_as_triggered();
    } else {
        QFile FileList(ParatextFileName);

        if(FileList.open(QFile::WriteOnly | QFile::Text))
        {
            QTextStream out(&FileList);

            out.setCodec("UTF-8");
            out << ui->textEdit_List->toPlainText();

            FileList.flush();
            FileList.close();

            QMessageBox::information(this, tr("File was saved"),
                                     tr("Paratext file was saved with the same name."));

        }
    }
}

void MainWindow::on_actionSave_Paratext_file_as_triggered()
{
    textEdit_focused("List");

    if (ParatextFileEditing == false) {
        return;
    }

    QString file = "";

    ui->textEdit_List->setAcceptRichText(false);
    file = QFileDialog::getSaveFileName(this, tr("Save Paratext file"), "",
                                        tr("Paratext files (*.sfm *.SFM)"));

    if(!file.isEmpty())
    {
        file = file + ".SFM";
        QDir direct;
        QString dir = direct.filePath(file);
        ParatextFileName = dir;
        QFile FileList(ParatextFileName);

        if(FileList.open(QFile::WriteOnly | QFile::Text))
        {
            QTextStream out(&FileList);

            out.setCodec("UTF-8");
            out << ui->textEdit_List->toPlainText();

            FileList.flush();
            FileList.close();

            ui->label_dir_list->setText(ParatextFileName);

            QMessageBox::information(this, tr("File was saved"),
                                     tr("Paratext file was saved as") + " " + ParatextFileName);
        }
    }
}

void MainWindow::on_actionClose_Paratext_file_triggered()
{
    textEdit_focused("List");
    ui->textEdit_List->setText("");
    ParatextFileEditing = false;
    ui->label_dir_list->setText("");
}



void MainWindow::on_actionFormatted_Paratext_file_triggered()
{
    textEdit_focused("List");
    if (ParatextFileEditing == false) {
        return;
    }

    QString file = ParatextFileName;

    if(!file.isEmpty())
    {
        QFile FileText(file);
        if(FileText.open(QFile::ReadOnly | QFile::Text))
        {
            QTextStream in(&FileText);
            in.setCodec("UTF-8");
            QString text = in.readAll();
            FileText.close();
            QString text_converted = converter_to_html(text);
            ui->textEdit_List->setHtml(text_converted);

        }
    }
}

void MainWindow::on_actionUnformatted_Paratext_file_triggered()
{
    textEdit_focused("List");
    if (ParatextFileEditing == false) {
        return;
    }

    QString file = ParatextFileName;

    if(!file.isEmpty())
    {
        QFile FileText(file);
        if(FileText.open(QFile::ReadOnly | QFile::Text))
        {
            QTextStream in(&FileText);
            in.setCodec("UTF-8");
            QString text = in.readAll();

            FileText.close();
            ui->textEdit_List->setPlainText(text);

        }
    }
}

// ////////////////////////////////////////////////////////////
//Option "Show/hide file paths and names"
// ////////////////////////////////////////////////////////////

void MainWindow::hideLabelDirs() {
    ui->label_dir_list->hide();
    ui->label_dir_original->hide();
    ui->label_dir_target->hide();
    ui->label_dir_base->hide();
    ui->label_dir_add1->hide();
    ui->label_dir_add2->hide();
}

void MainWindow::showLabelDirs() {
    ui->label_dir_list->show();
    ui->label_dir_original->show();
    ui->label_dir_target->show();
    ui->label_dir_base->show();
    ui->label_dir_add1->show();
    ui->label_dir_add2->show();
}

void MainWindow::on_actionShow_hide_file_paths_and_names_triggered()
{
    if (showDirs == false) {
        showLabelDirs();
        showDirs = true;
    }
    else {
        hideLabelDirs();
        showDirs = false;
    }
}


