/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>

#include "textEditor.h"

TextEditor::TextEditor(QWidget *parent) :  QTextEdit(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    isUntitled = true;

    setBackgroundColor( Qt::lightGray );

    this->setWindowTitle("Message");
}

void TextEditor::setBackgroundColor( const QColor& c )
{
        QPalette p( palette() );
        p.setColor( QPalette::Base, c );
        setPalette( p );
        viewport()->update();
}


void TextEditor::newFile()
{
    static int sequenceNumber = 1;

    isUntitled = true;
    curFile = tr("document%1.txt").arg(sequenceNumber++);
    setWindowTitle(curFile + "[*]");

    connect(document(), &QTextDocument::contentsChanged,
            this, &TextEditor::documentWasModified);
}


bool TextEditor::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("MDI"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }


    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);

    connect(document(), &QTextDocument::contentsChanged,
            this, &TextEditor::documentWasModified);

    return true;
}

bool TextEditor::save()
{
    if (isUntitled) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool TextEditor::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                    curFile);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

bool TextEditor::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("MDI"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << toPlainText();
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    return true;
}

QString TextEditor::userFriendlyCurrentFile()
{
    return strippedName(curFile);
}

void TextEditor::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void TextEditor::documentWasModified()
{
    setWindowModified(document()->isModified());
}

bool TextEditor::maybeSave()
{
    if (!document()->isModified())
        return true;
    const QMessageBox::StandardButton ret
            = QMessageBox::warning(this, tr("MDI"),
                                   tr("'%1' has been modified.\n"
                                      "Do you want to save your changes?")
                                   .arg(userFriendlyCurrentFile()),
                                   QMessageBox::Save | QMessageBox::Discard
                                   | QMessageBox::Cancel);
    switch (ret) {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return true;
}

void TextEditor::setCurrentFile(const QString &fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    document()->setModified(false);
    setWindowModified(false);
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

QString TextEditor::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}


void TextEditor::move_to_last_block(QTextCursor &cursor)
{
    bool moved=false;
    while (cursor.movePosition(QTextCursor::NextBlock)) {
        moved=true;
    }
    if (moved)
        cursor.movePosition(QTextCursor::EndOfBlock);

    QTextCursor cursor2=cursor;
    cursor2.movePosition(QTextCursor::StartOfBlock);
    if (cursor.position()-cursor2.position()<=2)
        cursor.movePosition(QTextCursor::EndOfBlock);
}



void TextEditor::displayMessage(const char *line,bool is_error,bool newline)
{
    setCurrentFont(QFont("Courier", 12));
    setPlainText(toPlainText());

    QTextCursor cursor=textCursor();
    QStringList lines=QString(line).split("\n");

    move_to_last_block(cursor);
    cursor.movePosition(QTextCursor::EndOfBlock);
    bool need_to_display_prompt=false;
    if ((cursor.columnNumber()>0)&&(cursor.block().text().mid(0,2).compare(">>")==0)) {
        need_to_display_prompt=true;
        cursor.insertBlock();
        move_to_last_block(cursor);
    }
    else {
        if (lines.count()>1) {
            cursor.insertBlock();
        }
    }

    if (is_error) {
        QTextCharFormat F=cursor.charFormat();
        F.setForeground(QBrush(QColor(255,0,0)));
        cursor.setCharFormat(F);
    }
    else {
        QTextCharFormat F=cursor.charFormat();
        F.setForeground(QBrush(QColor(0,0,0)));
        cursor.setCharFormat(F);
    }

    int j;
    for (j=0; j<lines.count(); j++) {
        if ((j>0)||(cursor.columnNumber()==0))
            cursor.insertText("  ");
        cursor.insertText(lines[j]);
        if (j<lines.count()-1)
            cursor.insertText("\n");
    }

    QTextCharFormat F=cursor.charFormat();
    F.setForeground(QBrush(QColor(0,0,0)));
    cursor.setCharFormat(F);

    if (newline) {
        //display_prompt();
        cursor.insertBlock();
    }

    move_to_last_block(cursor);
    cursor.movePosition(QTextCursor::EndOfBlock);

    setTextCursor(cursor);
}

void TextEditor::display_prompt()
{
    QTextCursor cursor=textCursor();
    if (cursor.block().text().mid(0,2).compare(">>")!=0) {
        cursor.movePosition(QTextCursor::StartOfBlock);
        cursor.insertText(">>");
    }
}

void TextEditor::displaySpaceLine()
{
    displayMessage(" ", false,true);
}
