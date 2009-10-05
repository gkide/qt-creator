/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef S60DEVICERUNCONFIGURATION_H
#define S60DEVICERUNCONFIGURATION_H

#include "launcher.h"

#include <projectexplorer/runconfiguration.h>
#include <projectexplorer/toolchain.h>

#include <QtGui/QWidget>

#include <QtCore/QProcess>

QT_BEGIN_NAMESPACE
class QLabel;
class QLineEdit;
class QComboBox;
QT_END_NAMESPACE

namespace Debugger {
    class DebuggerStartParameters;
}

namespace Qt4ProjectManager {
namespace Internal {

class S60DeviceRunConfiguration : public ProjectExplorer::RunConfiguration
{
    Q_OBJECT
public:
    enum SigningMode {
        SignSelf,
        SignCustom
    };

    explicit S60DeviceRunConfiguration(ProjectExplorer::Project *project, const QString &proFilePath);
    ~S60DeviceRunConfiguration();

    QString type() const;
    bool isEnabled() const;
    QWidget *configurationWidget();
    void save(ProjectExplorer::PersistentSettingsWriter &writer) const;
    void restore(const ProjectExplorer::PersistentSettingsReader &reader);

    QString serialPortName() const;
    void setSerialPortName(const QString &name);
    QString targetName() const;
    QString basePackageFilePath() const;
    SigningMode signingMode() const;
    void setSigningMode(SigningMode mode);
    QString customSignaturePath() const;
    void setCustomSignaturePath(const QString &path);
    QString customKeyPath() const;
    void setCustomKeyPath(const QString &path);

    QString packageFileName() const;
    QString localExecutableFileName() const;

    ProjectExplorer::ToolChain::ToolChainType toolChainType() const;

signals:
    void targetInformationChanged();

private slots:
    void invalidateCachedTargetInformation();

private:
    void updateTarget();

    QString m_proFilePath;
    QString m_targetName;
    QString m_baseFileName;
    bool m_cachedTargetInformationValid;
    QString m_serialPortName;
    SigningMode m_signingMode;
    QString m_customSignaturePath;
    QString m_customKeyPath;
};

class S60DeviceRunConfigurationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit S60DeviceRunConfigurationWidget(S60DeviceRunConfiguration *runConfiguration,
                                      QWidget *parent = 0);

private slots:
    void nameEdited(const QString &text);
    void updateTargetInformation();
    void updateSerialDevices();
    void setSerialPort(int index);
    void selfSignToggled(bool toggle);
    void customSignatureToggled(bool toggle);
    void signaturePathChanged(const QString &path);
    void keyPathChanged(const QString &path);

private:
    S60DeviceRunConfiguration *m_runConfiguration;
    QComboBox *m_serialPorts;
    QLineEdit *m_nameLineEdit;
    QLabel *m_sisxFileLabel;
};

class S60DeviceRunConfigurationFactory : public ProjectExplorer::IRunConfigurationFactory
{
    Q_OBJECT
public:
    explicit S60DeviceRunConfigurationFactory(QObject *parent);
    ~S60DeviceRunConfigurationFactory();
    bool canRestore(const QString &type) const;
    QStringList availableCreationTypes(ProjectExplorer::Project *pro) const;
    // used to translate the types to names to display to the user
    QString displayNameForType(const QString &type) const;
    QSharedPointer<ProjectExplorer::RunConfiguration> create(ProjectExplorer::Project *project, const QString &type);
};

/* S60DeviceRunControlBase: Builds and signs package and starts launcher
 * to deploy. Subclasses can configure the launcher to run or start a debugger. */

class S60DeviceRunControlBase : public ProjectExplorer::RunControl
{
    Q_OBJECT
public:
    explicit S60DeviceRunControlBase(const QSharedPointer<ProjectExplorer::RunConfiguration> &runConfiguration);
    ~S60DeviceRunControlBase() {}
    virtual void start();
    virtual void stop();
    virtual bool isRunning() const;

protected:
    virtual void initLauncher(const QString &executable, trk::Launcher *) = 0;
    virtual void handleLauncherFinished() = 0;
    void processFailed(const QString &program, QProcess::ProcessError errorCode);

protected slots:
    void printApplicationOutput(const QString &output);

private slots:
    void readStandardError();
    void readStandardOutput();
    void makesisProcessFailed();
    void makesisProcessFinished();
    void signsisProcessFailed();
    void signsisProcessFinished();
    void printCopyingNotice();
    void printCreateFileFailed(const QString &filename, const QString &errorMessage);
    void printWriteFileFailed(const QString &filename, const QString &errorMessage);
    void printCopyProgress(int progress);
    void printInstallingNotice();
    void launcherFinished();

private:
    QString m_serialPortName;
    QString m_serialPortFriendlyName;
    QString m_targetName;
    QString m_baseFileName;
    QString m_workingDirectory;
    QString m_toolsDirectory;
    QString m_executableFileName;
    QString m_qtDir;
    bool m_useCustomSignature;
    QString m_customSignaturePath;
    QString m_customKeyPath;
    QProcess *m_makesis;
    QProcess *m_signsis;
    QString m_makesisTool;
    QString m_packageFile;

    trk::Launcher *m_launcher;
};

// Configure launcher to run the application
class S60DeviceRunControl : public S60DeviceRunControlBase
{
    Q_OBJECT
public:
    explicit S60DeviceRunControl(const QSharedPointer<ProjectExplorer::RunConfiguration> &runConfiguration);

protected:
    virtual void initLauncher(const QString &executable, trk::Launcher *);
    virtual void handleLauncherFinished();

private slots:
    void printStartingNotice();
    void printRunNotice(uint pid);
    void printRunFailNotice(const QString &errorMessage);

private:
};

class S60DeviceDebugRunControl : public S60DeviceRunControlBase
{
    Q_DISABLE_COPY(S60DeviceDebugRunControl)
    Q_OBJECT
public:
    explicit S60DeviceDebugRunControl(const QSharedPointer<ProjectExplorer::RunConfiguration> &runConfiguration);
    virtual ~S60DeviceDebugRunControl();

    virtual void stop();

protected:
    virtual void initLauncher(const QString &executable, trk::Launcher *);
    virtual void handleLauncherFinished();

private slots:
    void debuggingFinished();
private:
    QSharedPointer<Debugger::DebuggerStartParameters> m_startParams;
};

} // namespace Internal
} // namespace Qt4ProjectManager

#endif // S60DEVICERUNCONFIGURATION_H
