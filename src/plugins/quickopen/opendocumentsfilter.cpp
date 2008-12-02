/***************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact:  Qt Software Information (qt-info@nokia.com)
**
**
** Non-Open Source Usage
**
** Licensees may use this file in accordance with the Qt Beta Version
** License Agreement, Agreement version 2.2 provided with the Software or,
** alternatively, in accordance with the terms contained in a written
** agreement between you and Nokia.
**
** GNU General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU General
** Public License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the packaging
** of this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
**
** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
** http://www.gnu.org/copyleft/gpl.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt GPL Exception
** version 1.2, included in the file GPL_EXCEPTION.txt in this package.
**
***************************************************************************/
***************************************************************************/
#include "opendocumentsfilter.h"

Q_DECLARE_METATYPE(Core::IEditor*);

using namespace Core;
using namespace QuickOpen;
using namespace QuickOpen::Internal;

OpenDocumentsFilter::OpenDocumentsFilter(EditorManager *editorManager) :
    m_editorManager(editorManager)
{
    connect(m_editorManager, SIGNAL(editorOpened(Core::IEditor*)),
            this, SLOT(refreshInternally()));
    connect(m_editorManager, SIGNAL(editorsClosed(QList<Core::IEditor*>)),
            this, SLOT(refreshInternally()));
    setShortcutString("o");
    setIncludedByDefault(true);
}

QList<FilterEntry> OpenDocumentsFilter::matchesFor(const QString &entry)
{
    QList<FilterEntry> value;
    const QChar asterisk = QLatin1Char('*');
    QString pattern = QString(asterisk);
    pattern += entry;
    pattern += asterisk;
    const QRegExp regexp(pattern, Qt::CaseInsensitive, QRegExp::Wildcard);
    if (!regexp.isValid())
        return value;
    foreach (IEditor *editor, m_editors) {
        QString fileName = editor->file()->fileName();
        if (regexp.exactMatch(editor->displayName())) {
            QString visibleName;
            QVariant data;
            if (fileName.isEmpty()) {
                value.append(FilterEntry(this, editor->displayName(), qVariantFromValue(editor)));
            } else {
                QFileInfo fi(fileName);
                FilterEntry entry(this, fi.fileName(), fileName);
                entry.extraInfo = QDir::toNativeSeparators(fi.path());
                entry.resolveFileIcon = true;
                value.append(entry);
            }
        }
    }
    return value;
}

void OpenDocumentsFilter::refreshInternally()
{
    m_editors = m_editorManager->openedEditors();
}

void OpenDocumentsFilter::refresh(QFutureInterface<void> &future)
{
    Q_UNUSED(future);
    // invokeAsyncronouslyOnGuiThread
    connect(this, SIGNAL(invokeRefresh()), this, SLOT(refreshInternally()));
    emit invokeRefresh();
    disconnect(this, SIGNAL(invokeRefresh()), this, SLOT(refreshInternally()));
}

void OpenDocumentsFilter::accept(FilterEntry selection) const
{
    IEditor *editor = selection.internalData.value<IEditor *>();
    if (editor) {
        m_editorManager->setCurrentEditor(editor);
        return;
    }
    m_editorManager->openEditor(selection.internalData.toString());
    m_editorManager->ensureEditorManagerVisible();
}
