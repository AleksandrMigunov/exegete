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

#ifndef DIALOGREPLACEALL_H
#define DIALOGREPLACEALL_H

#include <QDialog>

namespace Ui {
class DialogReplaceAll;
}

class DialogReplaceAll : public QDialog
{
    Q_OBJECT

public:
    explicit DialogReplaceAll(QWidget *parent = 0);
    ~DialogReplaceAll();

    QString search;
    QString replace;
    bool BeginSearch;

private slots:
    void on_lineEdit_Find_editingFinished();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_lineEdit_Replace_editingFinished();

private:
    Ui::DialogReplaceAll *ui;
};

#endif // DIALOGREPLACEALL_H
