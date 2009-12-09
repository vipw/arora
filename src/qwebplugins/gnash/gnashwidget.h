#ifndef GNASHWIDGET_H
#define GNASHWIDGET_H

#include <QX11EmbedContainer>
#include <QUrl>
#include <QStringList>

class QProcess;

class GnashWidget : public QX11EmbedContainer {
	Q_OBJECT

public:
	GnashWidget(const QUrl& url,
                const QStringList& argNames, const QStringList& argValues);
	~GnashWidget();

public slots:
	void shutdown();

private:
	QProcess* _player;
};

#endif // GNASHWIDGET_H
