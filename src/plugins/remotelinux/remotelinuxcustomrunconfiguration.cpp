/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#include "remotelinuxcustomrunconfiguration.h"

#include "remotelinux_constants.h"
#include "remotelinuxenvironmentaspect.h"

#include <projectexplorer/runnables.h>
#include <projectexplorer/runconfigurationaspects.h>
#include <projectexplorer/target.h>

#include <qtsupport/qtoutputformatter.h>
#include <utils/pathchooser.h>

#include <QFormLayout>
#include <QLabel>

using namespace ProjectExplorer;
using namespace Utils;

namespace RemoteLinux {
namespace Internal {

class RemoteLinuxCustomRunConfigWidget : public QWidget
{
public:
    explicit RemoteLinuxCustomRunConfigWidget(RemoteLinuxCustomRunConfiguration *runConfig)
    {
        auto fl = new QFormLayout(this);

        runConfig->extraAspect<ExecutableAspect>()->addToMainConfigurationWidget(this, fl);
        runConfig->extraAspect<SymbolFileAspect>()->addToMainConfigurationWidget(this, fl);
        runConfig->extraAspect<ArgumentsAspect>()->addToMainConfigurationWidget(this, fl);
        runConfig->extraAspect<WorkingDirectoryAspect>()->addToMainConfigurationWidget(this, fl);
    }
};

RemoteLinuxCustomRunConfiguration::RemoteLinuxCustomRunConfiguration(Target *target)
    : RunConfiguration(target, runConfigId())
{
    auto exeAspect = new ExecutableAspect(this);
    exeAspect->setSettingsKey("RemoteLinux.CustomRunConfig.RemoteExecutable");
    exeAspect->setLabelText(tr("Remote Executable:"));
    exeAspect->setExecutablePathStyle(OsTypeLinux);
    addExtraAspect(exeAspect);

    auto symbolsAspect = new SymbolFileAspect(this);
    symbolsAspect->setSettingsKey("RemoteLinux.CustomRunConfig.LocalExecutable");
    symbolsAspect->setLabelText(tr("Local executable:"));
    symbolsAspect->setDisplayStyle(SymbolFileAspect::PathChooserDisplay);
    addExtraAspect(symbolsAspect);

    addExtraAspect(new ArgumentsAspect(this, "RemoteLinux.CustomRunConfig.Arguments"));
    addExtraAspect(new WorkingDirectoryAspect(this, "RemoteLinux.CustomRunConfig.WorkingDirectory"));
    addExtraAspect(new RemoteLinuxEnvironmentAspect(this));

    setDefaultDisplayName(runConfigDefaultDisplayName());
    setOutputFormatter<QtSupport::QtOutputFormatter>();
}

bool RemoteLinuxCustomRunConfiguration::isConfigured() const
{
    return !extraAspect<ExecutableAspect>()->executable().isEmpty();
}

RunConfiguration::ConfigurationState
RemoteLinuxCustomRunConfiguration::ensureConfigured(QString *errorMessage)
{
    if (!isConfigured()) {
        if (errorMessage) {
            *errorMessage = tr("The remote executable must be set "
                               "in order to run a custom remote run configuration.");
        }
        return UnConfigured;
    }
    return Configured;
}

QWidget *RemoteLinuxCustomRunConfiguration::createConfigurationWidget()
{
    return wrapWidget(new RemoteLinuxCustomRunConfigWidget(this));
}

Runnable RemoteLinuxCustomRunConfiguration::runnable() const
{
    StandardRunnable r;
    r.executable = extraAspect<ExecutableAspect>()->executable().toString();
    r.environment = extraAspect<RemoteLinuxEnvironmentAspect>()->environment();
    r.commandLineArguments = extraAspect<ArgumentsAspect>()->arguments();
    r.workingDirectory = extraAspect<WorkingDirectoryAspect>()->workingDirectory().toString();
    return r;
}

Core::Id RemoteLinuxCustomRunConfiguration::runConfigId()
{
    return "RemoteLinux.CustomRunConfig";
}

QString RemoteLinuxCustomRunConfiguration::runConfigDefaultDisplayName()
{
    QString remoteExecutable = extraAspect<ExecutableAspect>()->executable().toString();
    QString display = remoteExecutable.isEmpty()
            ? tr("Custom Executable") : tr("Run \"%1\"").arg(remoteExecutable);
    return  RunConfigurationFactory::decoratedTargetName(display, target());
}

// RemoteLinuxCustomRunConfigurationFactory

RemoteLinuxCustomRunConfigurationFactory::RemoteLinuxCustomRunConfigurationFactory()
    : FixedRunConfigurationFactory(RemoteLinuxCustomRunConfiguration::tr("Custom Executable"), true)
{
    registerRunConfiguration<RemoteLinuxCustomRunConfiguration>
            (RemoteLinuxCustomRunConfiguration::runConfigId());
    addSupportedTargetDeviceType(RemoteLinux::Constants::GenericLinuxOsType);
}

} // namespace Internal
} // namespace RemoteLinux
