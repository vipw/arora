#ifndef _GNASHPLUGINFACTORY_H_
#define _GNASHPLUGINFACTORY_H_ 1

#include <QWebPluginFactory>
#include <QList>

/** @class GnashPluginFactory GnashPluginFactory.h <GnashPluginFactory.h>
 * Plugin factory that loads the Gnash player on demand
 */
class GnashPluginFactory:public QWebPluginFactory {
	Q_OBJECT
public:
	/**
	 * Construct a new GnashPluginFactory
	 * @param otherPlugins Other plugins to fall back to for media types not handled by Gnash
	 * @param parent Parent object
	 */
	GnashPluginFactory(QWebPluginFactory *otherPlugins=0, QObject *parent=0):QWebPluginFactory(parent),_otherPlugins(otherPlugins) {}
	/**
	 * Create the Gnash player widget
	 * @param mimeType MIME Type of content to be played
	 * @param url URL to be played
	 * @param argumentNames argument names
	 * @param argumentValues argument values
	 * @return Gnash player widget or 0 on error
	 */
	QObject *create(QString const &mimeType, QUrl const &url, QStringList const &argumentNames, QStringList const &argumentValues) const;
	/**
	 * Get a list of plugins
	 * @return list of plugins
	 */
	QList<Plugin> plugins() const;
protected:
	QWebPluginFactory *	_otherPlugins;
};

#endif
