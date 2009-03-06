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

QList<HistoryEntry> mostVisted(HistoryManager *manager, int count)
{
    QList<HistoryEntry> m_history = manager->history();
    QHash<QString, int> urlCount;
    for (int i = 0; i < m_history.count(); ++i) {
        QUrl url = m_history.at(i).url;
        int c = urlCount.value(url.toString());
        urlCount.insert(url.toString(),  ++c);
    }
    QMap<int, QString> sort;
    QHash<QString, int>::iterator i = urlCount.begin();
    while (i != urlCount.end()) {
        sort.insert(i.value(), i.key());
        ++i;
    }
    HistoryFilterModel *m_historyFilterModel = manager->historyFilterModel();
    QList<HistoryEntry> mostVisited;
    QMapIterator<int, QString> i2(sort);
    i2.toBack();
    while (i2.hasPrevious()) {
        i2.previous();
        QString url = i2.value();
        int offset = m_historyFilterModel->historyLocation(url);
        mostVisited.append(m_history[offset]);
        if (--count == 0)
            break;
    }
    return mostVisited;
}


int main(int argc, char **argv)
{
    QApplication application(argc, argv);
    QCoreApplication::setOrganizationDomain(QLatin1String("arora-browser.org"));
    QCoreApplication::setApplicationName(QLatin1String("Arora"));
    QString version = QLatin1String("0.5");

    HistoryManager history;
    QList<HistoryEntry> mostVisited = mostVisted(&history, 10);

    QString badhtml;
    badhtml += "<title>Top Sites</title>\n";
    badhtml += "<h3>Arora's Getto Top Sites Page</h3>\n";
    badhtml += "<ol>\n";
    for (int i = 0; i < mostVisited.count(); ++i) {
        QString link = mostVisited[i].url;
        QString title = mostVisited[i].title;
        if (title.isEmpty())
            title = link;
        badhtml += QString("<li><a href=\"%1\">%2</a></li>\n").arg(link).arg(title);
    }
    badhtml += "</ol>\n";
    QTextStream out(stdout);
    out << badhtml;
    return 0;
}

