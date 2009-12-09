#include "gnashpluginfactory.h"
#include "gnashwidget.h"
#include <QUrl>

using namespace std;

QObject *GnashPluginFactory::create(QString const &mimeType, QUrl const &url, QStringList const &argumentNames, QStringList const &argumentValues) const
{
	if(mimeType != QLatin1String("application/x-shockwave-flash")
			&& mimeType != QLatin1String("application/futuresplash")) {
		if(_otherPlugins)
			return _otherPlugins->create(mimeType, url, argumentNames, argumentValues);
		return 0;
	}

	GnashWidget *v=new GnashWidget(0, true /* autoplay */);
	v->setSwf(url.toString());
	return v;
}

QList<GnashPluginFactory::Plugin> GnashPluginFactory::plugins() const
{
	QList<Plugin> plugins;

	MimeType swf;
	swf.description=tr("Flash animation");
	swf.fileExtensions=QStringList() << QLatin1String("swf") << QLatin1String("SWF") << QLatin1String("Swf");
	swf.name=QLatin1String("application/x-shockwave-flash");

	MimeType futuresplash;
	futuresplash.description=tr("Flash animation");
	futuresplash.fileExtensions=QStringList() << QLatin1String("spl") << QLatin1String("SPL") << QLatin1String("Spl");
	futuresplash.name=QLatin1String("application/futuresplash");

	Plugin p;
	p.description=tr("Gnash Flash Player");
	p.name=tr("Gnash Flash Player");
	p.mimeTypes=QList<MimeType>() << swf << futuresplash;
	if(_otherPlugins)
		plugins += _otherPlugins->plugins();
	return plugins;
}
