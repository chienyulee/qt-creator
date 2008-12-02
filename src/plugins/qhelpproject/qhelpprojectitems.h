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
#ifndef QHELPPROJECTITEMS_H
#define QHELPPROJECTITEMS_H

#include <QtGui/QIcon>

#include <projectexplorer/ProjectExplorerInterfaces>

namespace QHelpProjectPlugin {
namespace Internal {

class QHelpProjectFile : public QObject,
    public ProjectExplorer::ProjectFileInterface
{
    Q_OBJECT
    Q_INTERFACES(ProjectExplorer::ProjectFileInterface ProjectExplorer::ProjectItemInterface)

public:
    QHelpProjectFile(const QString &fileName);
    QString fullName() const;

    //ProjectExplorer::ProjectItemInterface
    ProjectItemKind kind() const;

    QString name() const;
    QIcon icon() const;

private:
    QString m_file;
    static QIcon m_icon;
};

class QHelpProjectFolder : public QObject,
    public ProjectExplorer::ProjectFolderInterface
{
    Q_OBJECT
    Q_INTERFACES(ProjectExplorer::ProjectFolderInterface ProjectExplorer::ProjectItemInterface)

public:
    QHelpProjectFolder();
    ~QHelpProjectFolder();

    void setName(const QString &name);
    
    //ProjectExplorer::ProjectItemInterface
    ProjectItemKind kind() const;

    QString name() const;
    QIcon icon() const;

private:
    QString m_name;
    static QIcon m_icon;
};

}
}
}

#endif
