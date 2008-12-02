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
#ifndef PERFORCEPLUGIN_H
#define PERFORCEPLUGIN_H

#include "perforcesettings.h"

#include <coreplugin/editormanager/ieditorfactory.h>
#include <coreplugin/iversioncontrol.h>
#include <coreplugin/icorelistener.h>
#include <projectexplorer/ProjectExplorerInterfaces>
#include <extensionsystem/iplugin.h>

#ifdef USE_P4_API
#include "workbenchclientuser.h"
#else

#endif

#include <QtCore/QObject>
#include <QtCore/QProcess>
#include <QtCore/QStringList>

QT_BEGIN_NAMESPACE
class QFile;
class QAction;
class QTemporaryFile;
class QTextCodec;
QT_END_NAMESPACE

namespace Core {
    class IEditorFactory;
}

namespace Perforce {
namespace Internal {
class PerforceOutputWindow;
class SettingsPage;
class PerforceVersionControl;
class PerforcePlugin;

// Just a proxy for PerforcePlugin
class CoreListener : public Core::ICoreListener
{
    Q_OBJECT
public:
    CoreListener(PerforcePlugin *plugin) : m_plugin(plugin) { }
    bool editorAboutToClose(Core::IEditor *editor);
    bool coreAboutToClose() { return true; }
private:
    PerforcePlugin *m_plugin;
};

struct PerforceResponse
{
    bool error;
    QString command;
    QString stdOut;
    QString stdErr;
    QString message;
};

class PerforcePlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    PerforcePlugin();
    ~PerforcePlugin();

    QStringList basicP4Args() const;
    inline SettingsPage *settingsPage() const { return m_settingsPage; }

    bool initialize(const QStringList &arguments, QString *error_message);
    void extensionsInitialized();

    bool managesDirectory(const QString &directory) const;
    QString findTopLevelForDirectory(const QString &directory) const;
    bool vcsOpen(const QString &fileName);
    bool vcsAdd(const QString &fileName);
    bool vcsDelete(const QString &filename);
    // Displays the message for the submit mesage
    bool editorAboutToClose(Core::IEditor *editor);

    void p4Diff(const QStringList &files, QString diffname = QString());

    Core::IEditor *openPerforceSubmitEditor(const QString &fileName, const QStringList &depotFileNames);

    static Core::ICore *coreInstance() {Q_ASSERT(m_coreInstance); return m_coreInstance;}
    static PerforcePlugin *perforcePluginInstance() {Q_ASSERT(m_perforcePluginInstance); return m_perforcePluginInstance;}

    PerforceSettings settings() const;
    void setSettings(const PerforceSettings &s);

    // Map a perforce name "//xx" to its real name in the file system
    QString fileNameFromPerforceName(const QString& perforceName, QString *errorMessage) const;

public slots:
    void describe(const QString &source, const QString &n);

private slots:;
    void openCurrentFile();
    void addCurrentFile();
    void deleteCurrentFile();
    void revertCurrentFile();
    void printOpenedFileList();
    void diffCurrentFile();
    void diffCurrentProject();
    void diffAllOpened();
    void submit();
    void describeChange();
    void annotateCurrentFile();
    void annotate();
    void filelogCurrentFile();
    void filelog();

    void updateActions();
    void submitCurrentLog();
    void printPendingChanges();
    void slotDiff(const QStringList &files);

#ifdef USE_P4_API
    void resolve();
#endif

private:
    QStringList environment() const;

    Core::IEditor *showOutputInEditor(const QString& title, const QString output,
                                      int editorType,
                                      QTextCodec *codec = 0);
    // args are passed as command line arguments
    // extra args via a tempfile and the option -x "temp-filename"
    PerforceResponse runP4Cmd(const QStringList &args,
                              const QStringList &extraArgs = QStringList(),
                              bool showStdOutInOutputWindow = false,
                              bool showStdErrInOutputWindow = true,
                              QTextCodec *outputCodec = 0) const;

    void openFiles(const QStringList &files);

    QString clientFilePath(const QString &serverFilePath);
    QString currentFileName();
    bool checkP4Command() const;
    void showOutput(const QString &output, bool popup = false) const;
    void annotate(const QString &fileName);
    void filelog(const QString &fileName);

    ProjectExplorer::ProjectExplorerPlugin *m_projectExplorer;
    PerforceOutputWindow *m_perforceOutputWindow;
    SettingsPage *m_settingsPage;
    QList<Core::IEditorFactory*> m_editorFactories;

    QAction *m_editAction;
    QAction *m_addAction;
    QAction *m_deleteAction;
    QAction *m_openedAction;
    QAction *m_revertAction;
    QAction *m_diffCurrentAction;
    QAction *m_diffProjectAction;
    QAction *m_diffAllAction;
    QAction *m_resolveAction;
    QAction *m_submitAction;
    QAction *m_pendingAction;
    QAction *m_describeAction;
    QAction *m_annotateCurrentAction;
    QAction *m_annotateAction;
    QAction *m_filelogCurrentAction;
    QAction *m_filelogAction;
    QAction *m_submitCurrentLogAction;
    QAction *m_diffSelectedFiles;

    QAction *m_undoAction;
    QAction *m_redoAction;

    QTemporaryFile *m_changeTmpFile;

    static const char * const PERFORCE_MENU;
    static const char * const EDIT;
    static const char * const ADD;
    static const char * const DELETE_FILE;
    static const char * const OPENED;
    static const char * const REVERT;
    static const char * const DIFF_ALL;
    static const char * const DIFF_PROJECT;
    static const char * const DIFF_CURRENT;
    static const char * const RESOLVE;
    static const char * const SUBMIT;
    static const char * const PENDING_CHANGES;
    static const char * const DESCRIBE;
    static const char * const ANNOTATE_CURRENT;
    static const char * const ANNOTATE;
    static const char * const FILELOG_CURRENT;
    static const char * const FILELOG;
    static const char * const SEPARATOR1;
    static const char * const SEPARATOR2;
    static const char * const SEPARATOR3;

    static Core::ICore *m_coreInstance;
    static PerforcePlugin *m_perforcePluginInstance;
    QString pendingChangesData();

#ifdef USE_P4_API
    void runP4APICmd(const QString &cmd, const QStringList &args = QStringList());
    WorkbenchClientUser *m_workbenchClientUser;
    bool m_enableP4APIActions;
#endif

    CoreListener *m_coreListener;
    Core::IEditorFactory *m_submitEditorFactory;
    PerforceVersionControl *m_versionControl;
    PerforceSettings m_settings;

    friend class PerforceOutputWindow;
};

} // namespace Perforce
} // namespace Internal

#endif // PERFORCEPLUGIN_H
