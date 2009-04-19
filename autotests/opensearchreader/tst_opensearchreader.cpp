/*
 * Copyright 2009 Jakub Wieczorek <faw217@gmail.com>
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

#include <QtTest/QtTest>

#include "opensearchreader.h"
#include "opensearchengine.h"

class tst_OpenSearchReader : public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void validFile();
    void validByteArray();
    void invalidFile();
    void invalidByteArray();
    void urlParameters();
    void emptyByteArray();
    void infinitiveLoops();
};

// This will be called before the first test function is executed.
// It is only called once.
void tst_OpenSearchReader::initTestCase()
{
}

// This will be called after the last test function is executed.
// It is only called once.
void tst_OpenSearchReader::cleanupTestCase()
{
}

// This will be called before each test function is executed.
void tst_OpenSearchReader::init()
{
}

// This will be called after every test function.
void tst_OpenSearchReader::cleanup()
{
}

void tst_OpenSearchReader::validFile()
{
    QFile file(QLatin1String("valid.xml"));
    OpenSearchReader reader;
    OpenSearchEngine *engine = reader.read(&file);

    QVERIFY(engine->isValid());
    QCOMPARE(engine->name(), QLatin1String("Wikipedia (en)"));
    QCOMPARE(engine->description(), QLatin1String("Full text search in the English Wikipedia"));
    QCOMPARE(engine->searchUrl(), QLatin1String("http://en.wikipedia.org/bar"));
    QCOMPARE(engine->suggestionsUrl(), QLatin1String("http://en.wikipedia.org/foo"));
    QCOMPARE(engine->imageUrl().toString(), QLatin1String("http://en.wikipedia.org/favicon.ico"));

    delete engine;
}

void tst_OpenSearchReader::validByteArray()
{
    QByteArray data;
    data.append("<OpenSearchDescription xmlns=\"http://a9.com/-/spec/opensearch/1.1/\">");
    data.append("<ShortName>Google</ShortName>");
    data.append("<Description>Google Web Search</Description>");
    data.append("<Url method=\"get\" type=\"text/html\""
                " template=\"http://www.google.com/search?bar\" />");
    data.append("<Url method=\"get\" type=\"application/x-suggestions+json\""
                " template=\"http://suggestqueries.google.com/complete/foo\" />");
    data.append("<Image width=\"16\" height=\"16\">http://www.google.com/favicon.ico</Image>");
    data.append("</OpenSearchDescription>");

    OpenSearchReader reader;
    OpenSearchEngine *engine = reader.read(data);

    QVERIFY(engine->isValid());
    QCOMPARE(engine->name(), QLatin1String("Google"));
    QCOMPARE(engine->description(), QLatin1String("Google Web Search"));
    QCOMPARE(engine->searchUrl(), QLatin1String("http://www.google.com/search?bar"));
    QCOMPARE(engine->suggestionsUrl(), QLatin1String("http://suggestqueries.google.com/complete/foo"));
    QCOMPARE(engine->imageUrl().toString(), QLatin1String("http://www.google.com/favicon.ico"));

    delete engine;
}

void tst_OpenSearchReader::invalidFile()
{
    QFile file(QLatin1String("invalid.xml"));
    OpenSearchReader reader;
    OpenSearchEngine *engine = reader.read(&file);

    QVERIFY(!engine->isValid());
    QCOMPARE(engine->name(), QLatin1String("Wikipedia (en)"));
    QCOMPARE(engine->description(), QString());

    delete engine;
}

void tst_OpenSearchReader::invalidByteArray()
{
    QByteArray data;
    data.append("<OpenSearchDescription>");
    data.append("<ShortName>Google</ShortName>");
    data.append("<Description>Google Web Search</Description>");
    data.append("<Url method=\"get\" type=\"text/html\""
    " template=\"http://www.google.com/search?bar\" />");
    data.append("</OpenSearchDescription>");

    OpenSearchReader reader;
    OpenSearchEngine *engine = reader.read(data);

    QVERIFY(!engine->isValid()); // lacking in namespace URI
    QCOMPARE(engine->name(), QString());
    QCOMPARE(engine->description(), QString());
    QCOMPARE(engine->searchUrl(), QString());

    delete engine;
}

void tst_OpenSearchReader::urlParameters()
{
    QFile file(QLatin1String("urlparams.xml"));
    OpenSearchReader reader;
    OpenSearchEngine *engine = reader.read(&file);

    QVERIFY(engine->isValid());
    QCOMPARE(engine->name(), QLatin1String("GitHub"));
    QCOMPARE(engine->description(), QLatin1String("Search GitHub"));
    QCOMPARE(engine->searchUrl(), QLatin1String("http://github.com/search"));
    QCOMPARE(engine->suggestionsUrl(), QString());

    QCOMPARE(engine->searchParameters().count(), 2);
    QVERIFY(engine->searchParameters().contains(QLatin1String("q")));
    QCOMPARE(engine->searchParameters().value(QLatin1String("q")), QLatin1String("{searchTerms}"));
    QCOMPARE(engine->searchParameters().value(QLatin1String("b")), QLatin1String("foo"));

    delete engine;
}

void tst_OpenSearchReader::emptyByteArray()
{
    OpenSearchReader reader;
    OpenSearchEngine *engine = reader.read(QByteArray());

    QVERIFY(!engine->isValid());

    delete engine;
}

void tst_OpenSearchReader::infinitiveLoops()
{
    QString xml("<foo><bar></bar></foo>");
    OpenSearchReader reader;

    OpenSearchEngine *engine = reader.read(xml);
    QVERIFY(!engine->isValid());
}

QTEST_MAIN(tst_OpenSearchReader)

#include "tst_opensearchreader.moc"
