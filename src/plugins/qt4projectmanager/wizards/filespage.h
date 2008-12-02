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
#ifndef FILESPAGE_H
#define FILESPAGE_H

#include <QtGui/QWizard>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

namespace Core {
    namespace Utils {
        class NewClassWidget;
    }
}

namespace Qt4ProjectManager {
namespace Internal {

class FilesPage : public QWizardPage
{
    Q_OBJECT

public:
    explicit FilesPage(QWidget *parent = 0);
    virtual bool isComplete() const;

    QString className() const;
    void setClassName(const QString &suggestedClassName);

    QString baseClassName() const;
    QString sourceFileName() const;
    QString headerFileName() const;
    QString formFileName() const;

    // API of the embedded NewClassWidget
    bool namespacesEnabled() const;
    bool isBaseClassInputVisible() const;
    bool isFormInputVisible() const;
    bool formInputCheckable() const;
    bool formInputChecked() const;
    QStringList baseClassChoices() const;

    void setSuffixes(const QString &header, const QString &source,  const QString &form = QString());

public slots:
    void setBaseClassName(const QString &);
    void setNamespacesEnabled(bool b);
    void setBaseClassInputVisible(bool visible);
    void setBaseClassChoices(const QStringList &choices);
    void setFormFileInputVisible(bool visible);
    void setFormInputCheckable(bool checkable);
    void setFormInputChecked(bool checked);


private:
    Core::Utils::NewClassWidget *m_newClassWidget;
    QLabel *m_errorLabel;
};

} // namespace Internal
} // namespace Qt4ProjectManager

#endif // FILESPAGE_H
