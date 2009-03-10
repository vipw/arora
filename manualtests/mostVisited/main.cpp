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

#include <QtGui/QtGui>
#include "history.h"

int main(int argc, char **argv)
{
    QApplication application(argc, argv);
    QCoreApplication::setOrganizationDomain(QLatin1String("arora-browser.org"));
    QCoreApplication::setApplicationName(QLatin1String("Arora"));
    QString version = QLatin1String("0.5");

    HistoryManager history;
    QList<int> mostVisitedRow = history.historyFilterModel()->mostVisited(10);

    HistoryModel *historyModel = history.historyModel();
    QString badhtml;
    badhtml += "<title>Top Sites</title>\n";
    badhtml += "<h3>Arora's Getto Top Sites Page</h3>\n";
    badhtml += "<ol>\n";
    for (int i = 0; i < mostVisitedRow.count(); ++i) {
        QModelIndex idx = historyModel->index(mostVisitedRow[i], 0);
        QString link = idx.data(HistoryModel::UrlStringRole).toString();
        QString title = idx.data(HistoryModel::TitleRole).toString();
        badhtml += QString("<li><a href=\"%1\">%2</a></li>\n").arg(link).arg(title);
    }
    badhtml += "</ol>\n";
    QTextStream out(stdout);
    out << badhtml;
    return 0;
}

