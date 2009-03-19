/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact:  Qt Software Information (qt-info@nokia.com)
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
** contact the sales department at qt-sales@nokia.com.
**
**************************************************************************/

#ifndef GENERICMAKESTEP_H
#define GENERICMAKESTEP_H

#include <projectexplorer/abstractprocessstep.h>

QT_BEGIN_NAMESPACE
class QListWidgetItem;

namespace Ui {
class GenericMakeStep;
}
QT_END_NAMESPACE

namespace GenericProjectManager {
namespace Internal {

class GenericProject;

class GenericMakeStep : public ProjectExplorer::AbstractProcessStep
{
    Q_OBJECT
public:
    GenericMakeStep(GenericProject *pro);
    ~GenericMakeStep();
    virtual bool init(const QString &buildConfiguration);

    virtual void run(QFutureInterface<bool> &fi);

    virtual QString name();
    virtual QString displayName();
    virtual ProjectExplorer::BuildStepConfigWidget *createConfigWidget();
    virtual bool immutable() const;
    GenericProject *project() const;
    bool buildsTarget(const QString &buildConfiguration, const QString &target) const;
    void setBuildTarget(const QString &buildConfiguration, const QString &target, bool on);
private slots:
    void slotAddToTaskWindow(const QString & fn, int type, int linenumber, const QString & description);
    void addDirectory(const QString &dir);
    void removeDirectory(const QString &dir);
protected:
    virtual void stdOut(const QString &line);
    virtual void stdError(const QString &line);
private:
    GenericProject *m_pro;
    ProjectExplorer::BuildParserInterface *m_buildParser;
    QSet<QString> m_openDirectories;
};

class GenericMakeStepConfigWidget :public ProjectExplorer::BuildStepConfigWidget
{
    Q_OBJECT
public:
    GenericMakeStepConfigWidget(GenericMakeStep *makeStep);
    virtual QString displayName() const;
    virtual void init(const QString &buildConfiguration);
private slots:
    void itemChanged(QListWidgetItem*);
    void makeLineEditTextEdited();
    void makeArgumentsLineEditTextEdited();
private:
    QString m_buildConfiguration;
    Ui::GenericMakeStep *m_ui;
    GenericMakeStep *m_makeStep;
};

class GenericMakeStepFactory : public ProjectExplorer::IBuildStepFactory
{
    virtual bool canCreate(const QString &name) const;
    virtual ProjectExplorer::BuildStep *create(ProjectExplorer::Project *pro, const QString &name) const;
    virtual QStringList canCreateForProject(ProjectExplorer::Project *pro) const;
    virtual QString displayNameForName(const QString &name) const;
};

} // namespace Internal
} // namespace GenericProjectManager

#endif // GENERICMAKESTEP_H
