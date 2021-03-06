/*
 * Copyright 2008 Benjamin C. Meyer <ben@meyerhome.net>
 * Copyright 2008 Jason A. Donenfeld <Jason@zx2c4.com>
 * Copyright 2008 Ariya Hidayat <ariya.hidayat@gmail.com>
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

/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "webview.h"

#include "bookmarks.h"
#include "browserapplication.h"
#include "browsermainwindow.h"
#include "downloadmanager.h"
#include "webpage.h"

#include <qclipboard.h>
#include <qevent.h>
#include <qmenubar.h>
#include <qwebframe.h>

#ifdef WEBKIT_TRUNK
#include <qlabel.h>
#include <qsettings.h>
#include <qtooltip.h>
#include <qwebelement.h>
#endif

#include <qdebug.h>

WebView::WebView(QWidget *parent)
    : QWebView(parent)
    , m_progress(0)
    , m_currentZoom(100)
    , m_page(new WebPage(this))
#ifdef WEBKIT_TRUNK
    , m_enableAccessKeys(true)
    , m_accessKeysPressed(false)
#endif
{
    setPage(m_page);
    connect(page(), SIGNAL(statusBarMessage(const QString&)),
            SLOT(setStatusBarText(const QString&)));
    connect(this, SIGNAL(loadProgress(int)),
            this, SLOT(setProgress(int)));
    connect(this, SIGNAL(loadFinished(bool)),
            this, SLOT(loadFinished()));
    connect(page(), SIGNAL(aboutToLoadUrl(const QUrl &)),
            this, SIGNAL(urlChanged(const QUrl &)));
    connect(page(), SIGNAL(downloadRequested(const QNetworkRequest &)),
            this, SLOT(downloadRequested(const QNetworkRequest &)));
#if QT_VERSION >= 0x040500
    connect(BrowserApplication::instance(), SIGNAL(zoomTextOnlyChanged(bool)),
            this, SLOT(applyZoom()));
#endif
    page()->setForwardUnsupportedContent(true);
    setAcceptDrops(true);

    // the zoom values (in percent) are chosen to be like in Mozilla Firefox 3
    m_zoomLevels << 30 << 50 << 67 << 80 << 90;
    m_zoomLevels << 100;
    m_zoomLevels << 110 << 120 << 133 << 150 << 170 << 200 << 240 << 300;
#ifdef WEBKIT_TRUNK
    connect(m_page, SIGNAL(loadStarted()),
            this, SLOT(hideAccessKeys()));
    connect(m_page, SIGNAL(scrollRequested(int, int, const QRect &)),
            this, SLOT(hideAccessKeys()));
#endif
}

void WebView::loadSettings()
{
#ifdef WEBKIT_TRUNK
    QSettings settings;
    settings.beginGroup(QLatin1String("WebView"));
    m_enableAccessKeys = settings.value(QLatin1String("enableAccessKeys"), m_enableAccessKeys).toBool();
#endif
    m_page->loadSettings();
}

TabWidget *WebView::tabWidget() const
{
    QObject *widget = this->parent();
    while (widget) {
        if (TabWidget *tw = qobject_cast<TabWidget*>(widget))
            return tw;
        widget = widget->parent();
    }
    return 0;
}

void WebView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = new QMenu(this);

    QWebHitTestResult r = page()->mainFrame()->hitTestContent(event->pos());

    if (!r.linkUrl().isEmpty()) {
        menu->addAction(tr("Open in New &Window"), this, SLOT(openLinkInNewWindow()));
        QAction *newTabAction = menu->addAction(tr("Open in New &Tab"), this, SLOT(openActionUrlInNewTab()));
        newTabAction->setData(r.linkUrl());
        menu->addSeparator();
        menu->addAction(tr("Save Lin&k"), this, SLOT(downloadLinkToDisk()));
        menu->addAction(tr("&Bookmark This Link"), this, SLOT(bookmarkLink()))->setData(r.linkUrl());
        menu->addSeparator();
        if (!page()->selectedText().isEmpty())
            menu->addAction(pageAction(QWebPage::Copy));
        menu->addAction(tr("&Copy Link Location"), this, SLOT(copyLinkToClipboard()));
        if (page()->settings()->testAttribute(QWebSettings::DeveloperExtrasEnabled))
            menu->addAction(pageAction(QWebPage::InspectElement));
    }

    if (!r.imageUrl().isEmpty()) {
        if (!menu->isEmpty())
            menu->addSeparator();
        menu->addAction(tr("Open Image in New &Window"), this, SLOT(openImageInNewWindow()));
        QAction *newTabAction = menu->addAction(tr("Open Image in New &Tab"), this, SLOT(openActionUrlInNewTab()));
        newTabAction->setData(r.imageUrl());
        menu->addSeparator();
        menu->addAction(tr("&Save Image"), this, SLOT(downloadImageToDisk()));
        menu->addAction(tr("&Copy Image"), this, SLOT(copyImageToClipboard()));
        menu->addAction(tr("C&opy Image Location"), this, SLOT(copyImageLocationToClipboard()))->setData(r.imageUrl().toString());
    }

#if QT_VERSION >= 0x040500
    if (menu->isEmpty()) {
        delete menu;
        menu = page()->createStandardContextMenu();
    }
#endif

    if (!menu->isEmpty()) {
        if (tabWidget()->mainWindow()->menuBar()->isHidden()) {
            menu->addSeparator();
            menu->addAction(tabWidget()->mainWindow()->showMenuBarAction());
        }

        menu->exec(mapToGlobal(event->pos()));
        delete menu;
        return;
    }
    delete menu;

    QWebView::contextMenuEvent(event);
}

void WebView::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        int numDegrees = event->delta() / 8;
        int numSteps = numDegrees / 15;
        m_currentZoom = m_currentZoom + numSteps * 10;
        applyZoom();
        event->accept();
        return;
    }
    QWebView::wheelEvent(event);
}

void WebView::resizeEvent(QResizeEvent *event)
{
    int offset = event->size().height() - event->oldSize().height();
    int currentValue = page()->mainFrame()->scrollBarValue(Qt::Vertical);
    setUpdatesEnabled(false);
    page()->mainFrame()->setScrollBarValue(Qt::Vertical, currentValue - offset);
    setUpdatesEnabled(true);
    QWebView::resizeEvent(event);
}

void WebView::openLinkInNewWindow()
{
    pageAction(QWebPage::OpenLinkInNewWindow)->trigger();
}

void WebView::downloadLinkToDisk()
{
    pageAction(QWebPage::DownloadLinkToDisk)->trigger();
}

void WebView::copyLinkToClipboard()
{
    pageAction(QWebPage::CopyLinkToClipboard)->trigger();
}
void WebView::openActionUrlInNewTab()
{
    if (QAction *action = qobject_cast<QAction*>(sender())) {
        QWebPage *page = tabWidget()->getView(TabWidget::NewNotSelectedTab, this)->page();
        QNetworkRequest request(action->data().toUrl());
        request.setRawHeader("Referer", url().toEncoded());
        page->mainFrame()->load(request);
    }
}

void WebView::openImageInNewWindow()
{
    pageAction(QWebPage::OpenImageInNewWindow)->trigger();
}

void WebView::downloadImageToDisk()
{
    pageAction(QWebPage::DownloadImageToDisk)->trigger();
}

void WebView::copyImageToClipboard()
{
    pageAction(QWebPage::CopyImageToClipboard)->trigger();
}

void WebView::copyImageLocationToClipboard()
{
    if (QAction *action = qobject_cast<QAction*>(sender())) {
        BrowserApplication::clipboard()->setText(action->data().toString());
    }
}

void WebView::bookmarkLink()
{
    if (QAction *action = qobject_cast<QAction*>(sender())) {
        AddBookmarkDialog dialog;
        dialog.setUrl(QString::fromUtf8(action->data().toUrl().toEncoded()));
        dialog.exec();
    }
}

void WebView::setProgress(int progress)
{
    m_progress = progress;
}

int WebView::levelForZoom(int zoom)
{
    int i;

    i = m_zoomLevels.indexOf(zoom);
    if (i >= 0)
        return i;

    for (i = 0 ; i < m_zoomLevels.count(); ++i)
        if (zoom <= m_zoomLevels[i])
            break;

    if (i == m_zoomLevels.count())
        return i - 1;
    if (i == 0)
        return i;

    if (zoom - m_zoomLevels[i-1] > m_zoomLevels[i] - zoom)
        return i;
    else
        return i-1;
}

void WebView::applyZoom()
{
#if QT_VERSION >= 0x040500
    setZoomFactor(qreal(m_currentZoom) / 100.0);
#else
    setTextSizeMultiplier(qreal(m_currentZoom) / 100.0);
#endif
}

void WebView::zoomIn()
{
    int i = levelForZoom(m_currentZoom);

    if (i < m_zoomLevels.count() - 1)
        m_currentZoom = m_zoomLevels[i + 1];
    applyZoom();
}

void WebView::zoomOut()
{
    int i = levelForZoom(m_currentZoom);

    if (i > 0)
        m_currentZoom = m_zoomLevels[i - 1];
    applyZoom();
}

void WebView::resetZoom()
{
    m_currentZoom = 100;
    applyZoom();
}

void WebView::loadFinished()
{
    if (100 != m_progress) {
        qWarning() << "Received finished signal while progress is still:" << progress()
                   << "Url:" << url();
    }
    m_progress = 0;
}

void WebView::loadUrl(const QUrl &url, const QString &title)
{
    if (url.scheme() == QLatin1String("javascript")) {
        QString scriptSource = url.toString().mid(11);
        QVariant result = page()->mainFrame()->evaluateJavaScript(scriptSource);
        if (result.canConvert(QVariant::String)) {
            QString newHtml = result.toString();
            setHtml(newHtml);
        }
        return;
    }
    m_initialUrl = url;
    if (!title.isEmpty())
        emit titleChanged(tr("Loading..."));
    else
        emit titleChanged(title);
    load(url);
}

QString WebView::lastStatusBarText() const
{
    return m_statusBarText;
}

QUrl WebView::url() const
{
    QUrl url = QWebView::url();
    if (!url.isEmpty())
        return url;

    return m_initialUrl;
}

void WebView::mousePressEvent(QMouseEvent *event)
{
    BrowserApplication::instance()->setEventMouseButtons(event->buttons());
    BrowserApplication::instance()->setEventKeyboardModifiers(event->modifiers());

    switch (event->button()) {
    case Qt::XButton1:
        pageAction(WebPage::Back)->trigger();
        break;
    case Qt::XButton2:
        pageAction(WebPage::Forward)->trigger();
        break;
    default:
        QWebView::mousePressEvent(event);
        break;
    }
}

void WebView::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void WebView::dragMoveEvent(QDragMoveEvent *event)
{
    if (!event->mimeData()->urls().isEmpty()) {
        event->acceptProposedAction();
    } else {
        QUrl url(event->mimeData()->text());
        if (url.isValid())
            event->acceptProposedAction();
    }
    if (!event->isAccepted())
        QWebView::dragMoveEvent(event);
}

void WebView::dropEvent(QDropEvent *event)
{
    QWebView::dropEvent(event);
    if (!event->isAccepted()
        && event->possibleActions() & Qt::CopyAction) {

        QUrl url;
        if (!event->mimeData()->urls().isEmpty())
            url = event->mimeData()->urls().first();
        if (!url.isValid())
            url = event->mimeData()->text();
        if (url.isValid()) {
            loadUrl(url);
            event->acceptProposedAction();
        }
    }
}

void WebView::mouseReleaseEvent(QMouseEvent *event)
{
    QWebView::mouseReleaseEvent(event);
    if (!event->isAccepted()
        && (BrowserApplication::instance()->eventMouseButtons() & Qt::MidButton)) {
        QUrl url(QApplication::clipboard()->text(QClipboard::Selection));
        if (!url.isEmpty() && url.isValid() && !url.scheme().isEmpty()) {
            loadUrl(url);
        }
    }
}

void WebView::setStatusBarText(const QString &string)
{
    m_statusBarText = string;
}

void WebView::downloadRequested(const QNetworkRequest &request)
{
    BrowserApplication::downloadManager()->download(request);
}

#ifdef WEBKIT_TRUNK

void WebView::keyPressEvent(QKeyEvent *event)
{
    if (m_enableAccessKeys) {
        m_accessKeysPressed = (event->modifiers() == Qt::MetaModifier
                               && event->key() == Qt::Key_Meta);

        if (!m_accessKeysPressed) {
            if (checkForAccessKey(event)) {
                hideAccessKeys();
                event->accept();
                return;
            }
            hideAccessKeys();
        }
    }

    QWebView::keyPressEvent(event);
}

void WebView::keyReleaseEvent(QKeyEvent *event)
{
    if (m_accessKeysPressed) {
        if (m_accessKeyLabels.isEmpty()) {
            showAccessKeys();
        } else {
            hideAccessKeys();
        }
        m_accessKeysPressed = false;
    }

    QWebView::keyReleaseEvent(event);
}

bool WebView::checkForAccessKey(QKeyEvent *event)
{
    if (m_accessKeyLabels.isEmpty())
        return false;

    QString text = event->text();
    if (text.isEmpty())
        return false;
    QChar key = text.at(0).toUpper();
    bool handled = false;
    if (m_accessKeyNodes.contains(key)) {
        QPoint p = m_accessKeyNodes[key].geometry().center();
        QMouseEvent pevent(QEvent::MouseButtonPress, p, Qt::LeftButton, 0, 0);
        qApp->sendEvent(this, &pevent);
        QMouseEvent revent(QEvent::MouseButtonRelease, p, Qt::LeftButton, 0, 0);
        qApp->sendEvent(this, &revent);
        handled = true;
    }
    return handled;
}

void WebView::hideAccessKeys()
{
    if (!m_accessKeyLabels.isEmpty()) {
        for (int i = 0; i < m_accessKeyLabels.count(); ++i) {
            QLabel *label = m_accessKeyLabels[i];
            label->hide();
            label->deleteLater();
        }
        m_accessKeyLabels.clear();
        m_accessKeyNodes.clear();
        update();
    }
}

void WebView::showAccessKeys()
{
    QStringList supportedElement;
    supportedElement << QLatin1String("input")
                     << QLatin1String("a")
                     << QLatin1String("area")
                     << QLatin1String("button")
                     << QLatin1String("label")
                     << QLatin1String("legend")
                     << QLatin1String("textarea");

    QList<QChar> unusedKeys;
    for (char c = 'A'; c <= 'Z'; ++c)
        unusedKeys << QLatin1Char(c);
    for (char c = '0'; c <= '9'; ++c)
        unusedKeys << QLatin1Char(c);

    QRect viewport = QRect(m_page->mainFrame()->scrollPosition(), m_page->viewportSize());
    // Priority first goes to elements with accesskey attributes
    QList<QWebElement> alreadyLabeled;
    foreach (const QString &elementType, supportedElement) {
        QWebElementSelection result = page()->mainFrame()->findAllElements(elementType);
        foreach (const QWebElement &element, result) {
            const QRect geometry = element.geometry();
            if (geometry.size().isEmpty()
                || !viewport.contains(geometry.topLeft())) {
                continue;
            }
            QString accessKeyAttribute = element.attribute(QLatin1String("accesskey")).toUpper();
            if (accessKeyAttribute.isEmpty())
                continue;
            const QChar accessKey = accessKeyAttribute.at(0);
            if (!unusedKeys.contains(accessKey))
                continue;
            unusedKeys.removeOne(accessKey);
            makeAccessKeyLabel(accessKey, element);
            alreadyLabeled.append(element);
        }
    }

    // Pick an access key first from the letters in the text and then from the
    // list of unused access keys
    foreach (const QString &elementType, supportedElement) {
        QWebElementSelection result = page()->mainFrame()->findAllElements(elementType);
        foreach (const QWebElement &element, result) {
            const QRect geometry = element.geometry();
            if (unusedKeys.isEmpty()
                || alreadyLabeled.contains(element)
                || geometry.size().isEmpty()
                || !viewport.contains(geometry.topLeft())) {
                continue;
            }
            QChar accessKey;
            QString text = element.toPlainText().toUpper();
            for (int i = 0; i < text.count(); ++i) {
                const QChar &c = text.at(i);
                if (unusedKeys.contains(c)) {
                    accessKey = c;
                    break;
                }
            }
            if (accessKey.isNull())
                accessKey = unusedKeys.takeFirst();
            unusedKeys.removeOne(accessKey);
            makeAccessKeyLabel(accessKey, element);
        }
    }
}

void WebView::makeAccessKeyLabel(const QChar &accessKey, const QWebElement &element)
{
    QLabel *label = new QLabel(this);
    label->setText(QString(QLatin1String("<qt><b>%1</b>")).arg(accessKey));

    QPalette p = QToolTip::palette();
    QColor color(Qt::yellow);
    color = color.lighter(150);
    color.setAlpha(175);
    p.setColor(QPalette::Window, color);
    label->setPalette(p);
    label->setAutoFillBackground(true);
    label->setFrameStyle(QFrame::Box | QFrame::Plain);
    QPoint point = element.geometry().center();
    point -= m_page->mainFrame()->scrollPosition();
    label->move(point);
    label->show();
    point.setX(point.x() - label->width() / 2);
    label->move(point);
    m_accessKeyLabels.append(label);
    m_accessKeyNodes[accessKey] = element;
}

#endif
