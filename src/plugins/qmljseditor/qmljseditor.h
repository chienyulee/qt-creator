/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** Commercial Usage
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://qt.nokia.com/contact.
**
**************************************************************************/

#ifndef QMLJSEDITOR_H
#define QMLJSEDITOR_H

#include "qmljseditor_global.h"

#include <qmljs/qmljsdocument.h>
#include <qmljs/qmljsscanner.h>
#include <texteditor/basetexteditor.h>

#include <QtCore/QWaitCondition>
#include <QtCore/QModelIndex>
#include <QtCore/QMutex>
#include <QtCore/QThread>

QT_BEGIN_NAMESPACE
class QComboBox;
class QTimer;
QT_END_NAMESPACE

namespace Core {
class ICore;
}

namespace QmlJS {
    class ModelManagerInterface;
    class IContextPane;
}

namespace QmlJSEditor {
class Highlighter;

namespace Internal {

class QmlJSTextEditor;
class QmlOutlineModel;

class QmlJSEditorEditable : public TextEditor::BaseTextEditorEditable
{
    Q_OBJECT

public:
    QmlJSEditorEditable(QmlJSTextEditor *);
    Core::Context context() const;

    bool duplicateSupported() const { return true; }
    Core::IEditor *duplicate(QWidget *parent);
    QString id() const;
    bool isTemporary() const { return false; }
    virtual bool open(const QString & fileName);
    virtual QString preferredMode() const;

private:
    Core::Context m_context;
};

struct Declaration
{
    QString text;
    int startLine;
    int startColumn;
    int endLine;
    int endColumn;

    Declaration()
        : startLine(0),
        startColumn(0),
        endLine(0),
        endColumn(0)
    { }
};

class Range
{
public:
    Range(): ast(0) {}

public: // attributes
    QmlJS::AST::Node *ast;
    QTextCursor begin;
    QTextCursor end;
};

class SemanticInfo
{
public:
    SemanticInfo() {}

    int revision() const;

    // Returns the declaring member
    QmlJS::AST::Node *declaringMember(int cursorPosition) const;
    QmlJS::AST::Node *declaringMemberNoProperties(int cursorPosition) const;

    // Returns the AST node under cursor
    QmlJS::AST::Node *nodeUnderCursor(int cursorPosition) const;

    // Returns the list of nodes that enclose the given position.
    QList<QmlJS::AST::Node *> astPath(int cursorPosition) const;

public: // attributes
    QmlJS::Document::Ptr document;
    QmlJS::Snapshot snapshot;
    QList<Range> ranges;
    QHash<QString, QList<QmlJS::AST::SourceLocation> > idLocations;
    QList<Declaration> declarations;

    // these are in addition to the parser messages in the document
    QList<QmlJS::DiagnosticMessage> semanticMessages;
};

class SemanticHighlighter: public QThread
{
    Q_OBJECT

public:
    SemanticHighlighter(QObject *parent = 0);
    virtual ~SemanticHighlighter();

    void abort();

    struct Source
    {
        QmlJS::Snapshot snapshot;
        QString fileName;
        QString code;
        int line;
        int column;
        int revision;
        bool force;

        Source()
            : line(0), column(0), revision(0), force(false)
        { }

        Source(const QmlJS::Snapshot &snapshot,
               const QString &fileName,
               const QString &code,
               int line, int column,
               int revision)
            : snapshot(snapshot), fileName(fileName),
              code(code), line(line), column(column),
              revision(revision), force(false)
        { }

        void clear()
        {
            snapshot = QmlJS::Snapshot();
            fileName.clear();
            code.clear();
            line = 0;
            column = 0;
            revision = 0;
            force = false;
        }
    };

    void rehighlight(const Source &source);
    void setModelManager(QmlJS::ModelManagerInterface *modelManager);

Q_SIGNALS:
    void changed(const QmlJSEditor::Internal::SemanticInfo &semanticInfo);

protected:
    virtual void run();

private:
    bool isOutdated();
    SemanticInfo semanticInfo(const Source &source);

private:
    QMutex m_mutex;
    QWaitCondition m_condition;
    bool m_done;
    Source m_source;
    SemanticInfo m_lastSemanticInfo;
    QmlJS::ModelManagerInterface *m_modelManager;
};

class QMLJSEDITOR_EXPORT QmlJSTextEditor : public TextEditor::BaseTextEditor
{
    Q_OBJECT

public:
    QmlJSTextEditor(QWidget *parent = 0);
    ~QmlJSTextEditor();

    virtual void unCommentSelection();

    SemanticInfo semanticInfo() const;
    int editorRevision() const;
    bool isOutdated() const;

    QmlOutlineModel *outlineModel() const;
    QModelIndex outlineModelIndex();

public slots:
    void followSymbolUnderCursor();
    virtual void setFontSettings(const TextEditor::FontSettings &);

signals:
    void outlineModelIndexChanged(const QModelIndex &index);
    void selectedElementsChanged(QList<int> offsets, const QString &wordAtCursor);

private slots:
    void onDocumentUpdated(QmlJS::Document::Ptr doc);
    void modificationChanged(bool);

    void updateDocument();
    void updateDocumentNow();
    void jumpToOutlineElement(int index);
    void updateOutlineNow();
    void updateOutlineIndexNow();
    void updateFileName();

    void updateUses();
    void updateUsesNow();

    // refactoring ops
    void renameIdUnderCursor();

    void semanticRehighlight();
    void forceSemanticRehighlight();
    void updateSemanticInfo(const QmlJSEditor::Internal::SemanticInfo &semanticInfo);
    void onCursorPositionChanged();

protected:
    void contextMenuEvent(QContextMenuEvent *e);
    bool event(QEvent *e);
    void wheelEvent(QWheelEvent *event);
    TextEditor::BaseTextEditorEditable *createEditableInterface();
    void createToolBar(QmlJSEditorEditable *editable);
    TextEditor::BaseTextEditor::Link findLinkAt(const QTextCursor &cursor, bool resolveTarget = true);

    //// brace matching
    virtual bool contextAllowsAutoParentheses(const QTextCursor &cursor, const QString &textToInsert = QString()) const;
    virtual bool contextAllowsElectricCharacters(const QTextCursor &cursor) const;
    virtual bool isInComment(const QTextCursor &cursor) const;
    virtual QString insertMatchingBrace(const QTextCursor &tc, const QString &text, QChar la, int *skippedChars) const;
    virtual QString insertParagraphSeparator(const QTextCursor &tc) const;

private:
    virtual bool isElectricCharacter(QChar ch) const;
    virtual void indentBlock(QTextDocument *doc, QTextBlock block, QChar typedChar);
    bool isClosingBrace(const QList<QmlJS::Token> &tokens) const;

    void setSelectedElement();
    QString wordUnderCursor() const;

    SemanticHighlighter::Source currentSource(bool force = false);
    QModelIndex indexForPosition(unsigned cursorPosition, const QModelIndex &rootIndex = QModelIndex()) const;

    const Core::Context m_context;

    QTimer *m_updateDocumentTimer;
    QTimer *m_updateUsesTimer;
    QTimer *m_semanticRehighlightTimer;
    QTimer *m_updateOutlineTimer;
    QTimer *m_updateOutlineIndexTimer;
    QComboBox *m_outlineCombo;
    QmlOutlineModel *m_outlineModel;
    QModelIndex m_outlineModelIndex;
    QmlJS::ModelManagerInterface *m_modelManager;
    QTextCharFormat m_occurrencesFormat;
    QTextCharFormat m_occurrencesUnusedFormat;
    QTextCharFormat m_occurrenceRenameFormat;

    SemanticHighlighter *m_semanticHighlighter;
    SemanticInfo m_semanticInfo;

    QmlJS::IContextPane *m_contextPane;
    int m_oldCursorPosition;
};

} // namespace Internal
} // namespace QmlJSEditor

#endif // QMLJSEDITOR_H
