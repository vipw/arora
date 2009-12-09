#include "gnashwidget.h"
#include <QFileInfo>
#include <QApplication>
#include <QProcess>
#include <QDebug>

GnashWidget::GnashWidget(const QUrl& url, const QStringList& argNames,
                         const QStringList& argValues)
	: QX11EmbedContainer(0)
	, _player(0)
{
	Q_UNUSED(argNames);
	Q_UNUSED(argValues);

	connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(shutdown()));
	show();

	_player = new QProcess(this);

	QString program(QLatin1String("kde4-gnash"));
	QStringList args;
	args << QLatin1String("-x") << QString::number(winId());
	args << QLatin1String("-k") << QString::number(height());
	args << QLatin1String("-j") << QString::number(width());
	args << QLatin1String("-u") << url.toString() << url.toString();
	qDebug() << program << args;
	_player->start(program, args);
}

GnashWidget::~GnashWidget()
{
	shutdown();
}

void GnashWidget::shutdown()
{
	if(_player) {
		qDebug() << "stderr:\n" << _player->readAllStandardError();
		qDebug() << "stdout:\n" << _player->readAllStandardOutput();
		_player->kill();
		delete _player;
		_player = 0;
	}
}
