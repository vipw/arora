/**
 * Copyright (c) 2009, John Wimer
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following condition
 * is met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "gnashplugin.h"
#include "gnashwidget.h"
#include "qdebug.h"

GnashPlugin::GnashPlugin()
{
	qDebug() << "in GnashPlugin::ctor";
}

GnashPlugin::~GnashPlugin()
{
	qDebug() << "in GnashPlugin::dtor";
}

QWidget *GnashPlugin::create(const QString &mimeType, const QUrl &url,
		                     const QStringList &argNames,
                             const QStringList &argValues)
{
	Q_UNUSED(mimeType);
	qDebug() << "in GnashPlugin::create";

	return new GnashWidget(url, argNames, argValues);
}

QWebPluginFactory::Plugin GnashPlugin::metaPlugin()
{
	qDebug() << "in GnashPlugin::metaPlugin";
	QWebPluginFactory::Plugin plugin;
	plugin.name = QLatin1String("GnashPlugin");

	QWebPluginFactory::MimeType mimeTypeSwf;
	mimeTypeSwf.fileExtensions << QLatin1String("swf");
	mimeTypeSwf.name = QLatin1String("application/x-shockwave-flash");
	plugin.mimeTypes.append(mimeTypeSwf);

	// This is only for click2flash compatiblity, gnash doesn't play spl files
	QWebPluginFactory::MimeType mimeTypeFutureSplash;
	mimeTypeFutureSplash.fileExtensions << QLatin1String("spl");
	mimeTypeFutureSplash.name = QLatin1String("application/futuresplash");
	plugin.mimeTypes.append(mimeTypeFutureSplash);

	return plugin;
}

void GnashPlugin::configure()
{
}

bool GnashPlugin::isAnonymous()
{
	return false;
}

