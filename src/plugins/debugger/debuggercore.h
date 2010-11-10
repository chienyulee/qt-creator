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

#ifndef DEBUGGERCORE_H
#define DEBUGGERCORE_H

#include "debugger_global.h"
#include "debuggerconstants.h"

#include <QtCore/QObject>

QT_BEGIN_NAMESPACE
class QIcon;
class QMessageBox;
class QWidget;
QT_END_NAMESPACE

namespace CPlusPlus {
class Snapshot;
}

namespace Debugger {

class DebuggerEngine;
class DebuggerRunControl;
class DebuggerStartParameters;

namespace Internal {

class BreakHandler;
class SnapshotHandler;

// This is the "internal" interface of the debugger plugin that's
// used by debugger views and debugger engines. The interface is
// implemented in DebuggerPluginPrivate.

class DebuggerCore : public QObject
{
    Q_OBJECT

public:
    DebuggerCore() {}
    virtual ~DebuggerCore() {}

    static DebuggerCore *instance();

    virtual QVariant sessionValue(const QString &name) = 0;
    virtual void setSessionValue(const QString &name, const QVariant &value) = 0;
    virtual QVariant configValue(const QString &name) const = 0;
    virtual void setConfigValue(const QString &name, const QVariant &value) = 0;
    virtual void updateState(DebuggerEngine *engine) = 0;
    virtual void showQtDumperLibraryWarning(const QString &details) = 0;
    virtual QIcon locationMarkIcon() const = 0;
    virtual const CPlusPlus::Snapshot &cppCodeModelSnapshot() const = 0;
    virtual bool isRegisterViewVisible() const = 0;
    virtual bool hasSnapshots() const = 0;
    virtual void openTextEditor(const QString &titlePattern, const QString &contents) = 0;
    virtual Internal::BreakHandler *breakHandler() const = 0;
    virtual Internal::SnapshotHandler *snapshotHandler() const = 0;
    virtual DebuggerEngine *currentEngine() const = 0;
    virtual bool isActiveDebugLanguage(int language) const = 0;

public slots: // FIXME: Slot needed?
    virtual void clearCppCodeModelSnapshot() = 0;
    virtual void ensureLogVisible() = 0;

    // void runTest(const QString &fileName);
    virtual void showMessage(const QString &msg, int channel, int timeout = -1) = 0;
    virtual void gotoLocation(const QString &fileName, int lineNumber = -1,
        bool setMarker = false) = 0;

    virtual void resetLocation() = 0;
    virtual void readSettings() = 0;
    virtual void writeSettings() const = 0;

    virtual bool isReverseDebugging() const = 0;
    virtual void createNewDock(QWidget *widget) = 0;
    virtual void runControlStarted(DebuggerRunControl *runControl) = 0;
    virtual void runControlFinished(DebuggerRunControl *runControl) = 0;
    virtual void displayDebugger(DebuggerEngine *engine, bool updateEngine) = 0;
    virtual DebuggerLanguages activeLanguages() const = 0;

    virtual QMessageBox *showMessageBox(int icon, const QString &title,
        const QString &text, int buttons = 0) = 0;

    virtual bool initialize(const QStringList &arguments, QString *errorMessage) = 0;
    virtual QWidget *mainWindow() const = 0;
};

DebuggerCore *debuggerCore();

} // namespace Internal
} // namespace Debugger

#endif // DEBUGGERPLUGIN_H
