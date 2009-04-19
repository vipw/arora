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

#include "opensearchwriter.h"

#include "opensearchengine.h"

#include <qbuffer.h>
#include <qdebug.h>
#include <qiodevice.h>

OpenSearchWriter::OpenSearchWriter()
    : QXmlStreamWriter()
{
    setAutoFormatting(true);
}

bool OpenSearchWriter::write(QIODevice *device, OpenSearchEngine *engine)
{
    if (!engine)
        return false;

    if (!device->isOpen())
        device->open(QIODevice::WriteOnly);

    setDevice(device);
    write(engine);
    return true;
}

bool OpenSearchWriter::write(QByteArray *array, OpenSearchEngine *engine)
{
    if (!engine)
        return false;

    QBuffer buffer(array);
    write(&buffer, engine);
    return true;
}

void OpenSearchWriter::write(OpenSearchEngine *engine)
{
    writeStartDocument();
    writeStartElement(QLatin1String("OpenSearchDescription"));
    writeDefaultNamespace(QLatin1String("http://a9.com/-/spec/opensearch/1.1/"));

    if (!engine->name().isEmpty())
        writeTextElement(QLatin1String("ShortName"), engine->name());

    if (!engine->description().isEmpty())
        writeTextElement(QLatin1String("Description"), engine->description());

    if (!engine->searchUrl().isEmpty()) {
        writeStartElement(QLatin1String("Url"));
        writeAttribute(QLatin1String("method"), QLatin1String("get"));
        writeAttribute(QLatin1String("template"), engine->searchUrl());

        if (!engine->searchParameters().empty()) {
            writeNamespace(QLatin1String("http://a9.com/-/spec/opensearch/extensions/parameters/1.0/"), QLatin1String("p"));

            QHash<QString, QString>::const_iterator end = engine->searchParameters().constEnd();
            QHash<QString, QString>::const_iterator i = engine->searchParameters().constBegin();
            for (; i != end; ++i) {
                writeStartElement(QLatin1String("p:Parameter"));
                writeAttribute(QLatin1String("name"), i.key());
                writeAttribute(QLatin1String("value"), i.value());
                writeEndElement();
            }
        }

        writeEndElement();
    }

    if (!engine->suggestionsUrl().isEmpty()) {
        writeStartElement(QLatin1String("Url"));
        writeAttribute(QLatin1String("method"), QLatin1String("get"));
        writeAttribute(QLatin1String("type"), QLatin1String("application/x-suggestions+json"));
        writeAttribute(QLatin1String("template"), engine->suggestionsUrl());

        if (!engine->suggestionsParameters().empty()) {
            writeNamespace(QLatin1String("http://a9.com/-/spec/opensearch/extensions/parameters/1.0/"), QLatin1String("p"));

            QHash<QString, QString>::const_iterator end = engine->suggestionsParameters().constEnd();
            QHash<QString, QString>::const_iterator i = engine->suggestionsParameters().constBegin();
            for (; i != end; ++i) {
                writeStartElement(QLatin1String("p:Parameter"));
                writeAttribute(QLatin1String("name"), i.key());
                writeAttribute(QLatin1String("value"), i.value());
                writeEndElement();
            }
        }

        writeEndElement();
    }

    if (!engine->imageUrl().isEmpty())
        writeTextElement(QLatin1String("Image"), engine->imageUrl().toString());

    writeEndElement();
    writeEndDocument();
}
