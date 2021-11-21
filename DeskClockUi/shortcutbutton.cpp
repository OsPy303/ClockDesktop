/*
    ClockDesktop
    Copyright (C) Root_

    This file is part of ClockDesktop.
    ClockDesktop is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ClockDesktop is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ClockDesktop.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "shortcutbutton.h"
#include <QIcon>
#include <QMouseEvent>
#include <QMenu>
#include <QAction>
#include <QDebug>
#include "controller.h"

ShortcutButton::ShortcutButton(QWidget *parent)
    : QPushButton(parent)
{
    QIcon icn(":/icons/add.png");
    this->setIcon(icn);
    this->setIconSize(QSize(50, 50));
    setContentsMargins(0, 0, 0, 0);
    setFlat(true);
    resize(90, 90);
}

void ShortcutButton::setShortcut(const Shortcut &shortcut_, bool reset)
{
    if (reset)
    {
        mShortcut.clear();
        mShortcut.setIcon(":/icons/add.png");
    }
    qDebug() << "Setting shortcut icon" << shortcut_.icon();
    //mShortcut = new Shortcut;
    mShortcut.setIcon(shortcut_.icon());
    mShortcut.setId(shortcut_.id());
    mShortcut.setName(shortcut_.name());
    mShortcut.setPath(shortcut_.path());
    mShortcut.setDescription(shortcut_.description());
    mShortcut.setArgs(shortcut_.args());
    QIcon icn = mShortcut.icon().isEmpty() ? QIcon(":/icons/window.png")
                                            : QIcon(mShortcut.icon());
    this->setIcon(icn);
    this->setToolTip(mShortcut.name());
}

void ShortcutButton::newShortCut()
{
    qDebug() << "Add new shortcut";
    ShortcutDialog *dialog = new ShortcutDialog;
    QObject::connect(dialog, &ShortcutDialog::shortcutValidated, this, &ShortcutButton::onShortcutValidated);
    dialog->show();
}

void ShortcutButton::editShortcut()
{
    ShortcutDialog *dialog = new ShortcutDialog;
    dialog->setFormData(mShortcut);
    QObject::connect(dialog, &ShortcutDialog::shortcutValidated, this, &ShortcutButton::onShortcutEdit);
    dialog->show();
}

void ShortcutButton::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton and !mShortcut.path().isEmpty())
    {
        Controller::open(mShortcut);
        e->accept();
    }

    else if (e->button() == Qt::LeftButton && mShortcut.path().isEmpty())
    {
        newShortCut();
    }
}

void ShortcutButton::contextMenuEvent(QContextMenuEvent *e)
{
    QMenu menu(this);
    menu.addAction("New", this,[this](){
            newShortCut();
        })->setDisabled(!mShortcut.path().isEmpty());
    menu.addAction("Remove", this, [this](){
            this->removeShortcut();
        })->setDisabled(mShortcut.path().isEmpty());
    menu.addAction("Edit", this, [this](){
            this->editShortcut();
        })->setDisabled(mShortcut.path().isEmpty());
    menu.exec(e->globalPos());
}

void ShortcutButton::removeShortcut()
{
    qDebug() << "Removing shortcut...";
    if (Controller::removeShortcut(mShortcut.id()))
    {
        this->setShortcut(mShortcut, true);
    }
}

void ShortcutButton::onShortcutValidated(Shortcut &shortcut)
{
    if (Controller::saveShortcut(shortcut))
    {
        this->setShortcut(shortcut);
    }
}

void ShortcutButton::onShortcutEdit(Shortcut &shortcut)
{
    shortcut.setId(mShortcut.id());
    if (Controller::updateShortcut(shortcut))
    {
        this->setShortcut(shortcut);
    }
}
