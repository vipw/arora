/*
 * Copyright 2009 Benjamin C. Meyer <ben@meyerhome.net>
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

#ifndef RSSBUTTON_H
#define RSSBUTTON_H

#include <qabstractbutton.h>

#include <qurl.h>

class Feed {
public:
    Feed(const QString &t, const QString &h)
        :title(t), href(h) {}
    QString title;
    QString href;
};

class WebView;
class RssButton : public QAbstractButton
{
    Q_OBJECT

public:
    RssButton(QWidget *parent = 0);
    void setWebView(WebView *webView);
    void paintEvent(QPaintEvent *event);

protected:
    void mousePressEvent(QMouseEvent *event);

private slots:
    void loadFinished(bool ok);
    void triggeredAction();

private:
    QList<Feed> m_feeds;
    QString m_script;
    WebView *m_webView;

};

#endif // RSSBUTTON_H

