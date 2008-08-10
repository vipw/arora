/*
 * Copyright 2008 Benjamin C. Meyer <ben@meyerhome.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#ifndef KEYBOARDSHORTCUTSDIALOGS_H
#define KEYBOARDSHORTCUTSDIALOGS_H

#include <qdialog.h>
#include "ui_keyboardshortcutsdialog.h"

class QStandardItemModel;
class QMenu;
class ActionCollection;
class QStandardItem;

class KeyboardShortcutsDialog : public QDialog, public Ui_ShortcutDialog
{
    Q_OBJECT

public:
    KeyboardShortcutsDialog(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    enum Roles {
        ActionRole = Qt::UserRole + 1,
    };

private slots:
    void generate();
    void itemChanged(QStandardItem *item);

private:
    QModelIndex insert(const QString &title, const QModelIndex &parent);
    void addAction(const QModelIndex &parent, QAction *action);
    void addCollection(ActionCollection *collection);

    QStandardItemModel *m_model;
    ActionCollection *m_collection;

};

#endif // KEYBOARDSHORTCUTSDIALOGS_H
