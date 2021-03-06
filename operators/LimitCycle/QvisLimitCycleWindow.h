/*****************************************************************************
*
* Copyright (c) 2000 - 2015, Lawrence Livermore National Security, LLC
* Produced at the Lawrence Livermore National Laboratory
* LLNL-CODE-442911
* All rights reserved.
*
* This file is  part of VisIt. For  details, see https://visit.llnl.gov/.  The
* full copyright notice is contained in the file COPYRIGHT located at the root
* of the VisIt distribution or at http://www.llnl.gov/visit/copyright.html.
*
* Redistribution  and  use  in  source  and  binary  forms,  with  or  without
* modification, are permitted provided that the following conditions are met:
*
*  - Redistributions of  source code must  retain the above  copyright notice,
*    this list of conditions and the disclaimer below.
*  - Redistributions in binary form must reproduce the above copyright notice,
*    this  list of  conditions  and  the  disclaimer (as noted below)  in  the
*    documentation and/or other materials provided with the distribution.
*  - Neither the name of  the LLNS/LLNL nor the names of  its contributors may
*    be used to endorse or promote products derived from this software without
*    specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR  IMPLIED WARRANTIES, INCLUDING,  BUT NOT  LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND  FITNESS FOR A PARTICULAR  PURPOSE
* ARE  DISCLAIMED. IN  NO EVENT  SHALL LAWRENCE  LIVERMORE NATIONAL  SECURITY,
* LLC, THE  U.S.  DEPARTMENT OF  ENERGY  OR  CONTRIBUTORS BE  LIABLE  FOR  ANY
* DIRECT,  INDIRECT,   INCIDENTAL,   SPECIAL,   EXEMPLARY,  OR   CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT  LIMITED TO, PROCUREMENT OF  SUBSTITUTE GOODS OR
* SERVICES; LOSS OF  USE, DATA, OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER
* CAUSED  AND  ON  ANY  THEORY  OF  LIABILITY,  WHETHER  IN  CONTRACT,  STRICT
* LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN ANY  WAY
* OUT OF THE  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
* DAMAGE.
*
*****************************************************************************/

#ifndef QVIS_INTEGRALCURVE_WINDOW_H
#define QVIS_INTEGRALCURVE_WINDOW_H

#include <QvisOperatorWindow.h>
#include <AttributeSubject.h>

// Forward declarations
class LimitCycleAttributes;

class QTabWidget;
class QLabel;
class QCheckBox;
class QLineEdit;
class QSpinBox;
class QVBox;
class QButtonGroup;
class QRadioButton;
class QComboBox;
class QGroupBox;
class QPushButton;
class QListWidget;
class QListWidgetItem;

class QvisLineWidthWidget;
class QvisVariableButton;

// ****************************************************************************
// Class: QvisLimitCyclePlotWindow
//
// Purpose: 
//   Defines QvisLimitCyclePlotWindow class.
//
// Programmer: Brad Whitlock
// Creation:   Mon Oct 21 14:12:49 PST 2002
//
// Modifications:
//   Brad Whitlock, Wed Dec 22 13:03:49 PST 2004
//   Added support for coloring by vorticity and ribbons.
//
//   Brad Whitlock, Mon Jan 3 15:32:24 PST 2005
//   Added override of ProcessOldVersions.
//
//   Hank Childs, Sat Mar  3 09:11:44 PST 2007
//   Added useWholeBox.
//
//   Dave Pugmire, Thu Nov 15 12:09:08 EST 2007
//   Add streamline direction option.
//
//   Dave Pugmire, Mon Aug 4 2:49:38 EDT 2008
//   Added termination, algorithm and integration options.
//
//   Dave Pugmire, Wed Aug 6 15:16:23 EST 2008
//   Add accurate distance calculate option.
//
//   Dave Pugmire, Tue Aug 19 17:18:03 EST 2008
//   Removed the accurate distance calculation option.
//
//   Dave Pugmire, Thu Feb  5 12:20:15 EST 2009
//   Added workGroupSize for the masterSlave algorithm.
//
//   Dave Pugmire, Mon Feb 23, 09:11:34 EST 2009
//   Added number of steps as a termination criterion.
//
//   Dave Pugmire, Tue Mar 10 12:41:11 EDT 2009
//   Add pathline GUI.
//    
//   Hank Childs, Sat May  2 22:10:26 PDT 2009
//   Added option for specifying seed points as a point list.
//
//   Dave Pugmire, Wed Jun 10 16:26:25 EDT 2009
//   Add color by variable.
//
//   Dave Pugmire, Mon Nov 23 09:38:53 EST 2009
//   Add min/max options to color table.
//
//   Dave Pugmire, Tue Dec 29 14:37:53 EST 2009
//   Add lots of appearance options to the streamlines plots.
//
//   Dave Pugmire, Wed Jan 20 09:28:59 EST 2010
//   Add ramp opacity, show head geom.
//
//   Dave Pugmire, Tue Feb 16 09:08:32 EST 2010
//   Add display head geom as cone.
//
//   Allen Sanderson, Sun Mar  7 12:49:56 PST 2010
//   Change layout of window for 2.0 interface changes.
//
//   Dave Pugmire, Thu Jun 10 10:44:02 EDT 2010
//   New seed sources.
//
//   Dave Pugmire, Wed Jun 23 16:44:36 EDT 2010
//   Repace fill interior checkbox with radio button.
//
//   Hank Childs, Wed Sep 29 20:22:36 PDT 2010
//   Add methods for the maximum time step (DoPri), which is distinct from
//   the step length (Adams/Bashforth).
//
//   Hank Childs, Thu Sep 30 01:22:24 PDT 2010
//   Add widgets for choosing size type between fraction of bbox and absolute.
//
//   Hank Childs, Fri Oct  1 21:13:56 PDT 2010
//   Add size type option for absTol.
//
//   Hank Childs, Wed Oct  6 20:27:09 PDT 2010
//   Add options for different termination types.
//
//   Hank Childs, Sun Dec  5 04:59:00 PST 2010
//   Add new data members for warnings for stiffness and critical points.
//
//   Dave Pugmire, Fri Jan 28 14:49:50 EST 2011
//   Add vary tube radius by variable.
//
//   Dave Pugmire, Mon Feb 21 08:17:42 EST 2011
//   Add color by correlation distance.
//
//   Dave Pugmire, Thu Mar 15 11:23:18 EDT 2012
//   Add named selections as a seed source.
//
// ****************************************************************************

class QvisLimitCycleWindow : public QvisOperatorWindow
{
    Q_OBJECT

  public:
    QvisLimitCycleWindow(const int type,
                            LimitCycleAttributes *subj,
                            const QString &caption = QString::null,
                            const QString &shortName = QString::null,
                            QvisNotepadArea *notepad = 0);
    virtual ~QvisLimitCycleWindow();
    virtual void CreateWindowContents();

  protected:
    void CreateIntegrationTab(QWidget *);
    void CreateAppearanceTab(QWidget *);
    void CreateAdvancedTab(QWidget *);   

    void UpdateWindow(bool doAll);
    void GetCurrentValues(int which_widget);
    void UpdateSourceAttributes();
    void TurnOffSourceAttributes();
    void UpdateAlgorithmAttributes();
    void UpdateFieldAttributes();
    void UpdateIntegrationAttributes();

  private slots:
  // Integral Curves
    void sourceTypeChanged(int val);
    void lineStartProcessText();
    void lineEndProcessText();
    void planeOriginProcessText();
    void planeNormalProcessText();
    void planeUpAxisProcessText();
    void sampleDensity0Changed(int val);
    void sampleDensity1Changed(int val);
    void sampleDistance0ProcessText();
    void sampleDistance1ProcessText();

    void samplingTypeChanged(int);
    void randomSeedChanged(int);
    void numberOfRandomSamplesChanged(int);
    void fillChanged(int);

    void fieldTypeChanged(int val);
    void fieldConstantProccessText();
    void velocitySourceProcessText();
//    void forceNodalChanged(bool);

    void directionTypeChanged(int val);
    void integrationTypeChanged(int val);
    void maxStepLengthProcessText();
    void limitMaxTimeStepChanged(bool);
    void maxTimeStepProcessText();

    void relTolProcessText();
    void absTolProcessText();
    void absTolSizeTypeChanged(int);

    void limitMaxTimeChanged(bool);
    void limitMaxDistanceChanged(bool);

    void maxStepsProcessText();
    void maxTimeProcessText();
    void maxDistanceProcessText();

  // Appearance
    void cycleToleranceProcessText();
    void maxIterationsProcessText();
    void showPartialResultsChanged(bool);
    void showReturnDistancesChanged(bool);

    void dataValueChanged(int val);
    void dataVariableChanged(const QString &var);

    void correlationDistanceMinDistTypeChanged(int);
    void processCorrelationDistanceAngTolEditText();
    void processCorrelationDistanceMinDistEditText();

  // Advanced
    void parallelAlgorithmChanged(int val);
    void maxSLCountChanged(int val);
    void maxDomainCacheChanged(int val);
    void workGroupSizeChanged(int val);

    void icButtonGroupChanged(int val);
    void pathlineOverrideStartingTimeFlagChanged(bool val);
    void pathlineOverrideStartingTimeProcessText();
    void pathlinePeriodProcessText();
    void pathlineCMFEButtonGroupChanged(int val);

    void issueWarningForMaxStepsChanged(bool);
    void issueWarningForStepsizeChanged(bool);
    void issueWarningForStiffnessChanged(bool);
    void issueWarningForCriticalPointsChanged(bool);
    void criticalPointThresholdProcessText();

  private:
    int plotType;

  // Integral Curves
    QComboBox *sourceType;

    QLineEdit *pointSource;
    QLabel    *pointSourceLabel;
    QLineEdit *lineStart;
    QLabel    *lineStartLabel;
    QLineEdit *lineEnd;
    QLabel    *lineEndLabel;
    QListWidget *pointList;
    QLineEdit *planeOrigin;
    QLabel    *planeOriginLabel;
    QLineEdit *planeNormal;
    QLabel    *planeNormalLabel;
    QLineEdit *planeUpAxis;
    QLabel    *planeUpAxisLabel;
    QLabel    *sampleDensityLabel[2], *sampleDistanceLabel[2];
    QSpinBox  *sampleDensity[2];
    QLineEdit *sampleDistance[2];

    QGroupBox *samplingGroup;
    QLabel    *fillLabel;
    QButtonGroup *fillButtonGroup;
    QRadioButton *fillButtons[2];
    QLabel    *samplingTypeLabel;
    QButtonGroup *samplingTypeButtonGroup;
    QRadioButton *samplingTypeButtons[2];
    QSpinBox  *numberOfRandomSamples;
    QLabel    *numberOfRandomSamplesLabel;
    QSpinBox  *randomSeed;
    QLabel    *randomSeedLabel;

    QComboBox *fieldType;
    QLabel    *fieldConstantLabel;
    QLineEdit *fieldConstant;
    QLineEdit *velocitySource;
    QLabel    *velocitySourceLabel;
//    QCheckBox *forceNodal;

    QComboBox *directionType;
    QComboBox *integrationType;

    QCheckBox *limitMaxTimeStep;
    QLineEdit *maxTimeStep;
    QLineEdit *relTol;
    QLabel    *relTolLabel;
    QLineEdit *absTol;
    QComboBox *absTolSizeType;
    QLabel    *absTolLabel;
    QLineEdit *maxStepLength;
    QLabel    *maxStepLengthLabel;

    QLineEdit *maxSteps;
    QCheckBox *limitMaxTime;
    QLineEdit *maxTime;
    QCheckBox *limitMaxDistance;
    QLineEdit *maxDistance;

  // Appearance
    QLineEdit *cycleTolerance;
    QLineEdit *maxIterations;
    QCheckBox *showPartialResults;
    QCheckBox *showReturnDistances;

    QComboBox *dataValueComboBox;
    QvisVariableButton *dataVariable;
    QLabel    *correlationDistanceAngTolLabel, *correlationDistanceMinDistLabel;
    QLineEdit *correlationDistanceAngTolEdit, *correlationDistanceMinDistEdit;
    QComboBox *correlationDistanceMinDistType;

    QCheckBox *cropBeginFlag;
    QLineEdit *cropBegin;
    QCheckBox *cropEndFlag;
    QLineEdit *cropEnd;
    QComboBox *cropValueComboBox;

  // Advanced
    QLabel    *parallelAlgoLabel;
    QComboBox *parallelAlgo;
    QLabel    *maxSLCountLabel;
    QSpinBox  *maxSLCount;
    QLabel    *maxDomainCacheLabel;
    QSpinBox  *maxDomainCache;
    QLabel    *workGroupSizeLabel;
    QSpinBox  *workGroupSize;

    QButtonGroup *icButtonGroup;
    QCheckBox *pathlineOverrideStartingTimeFlag;
    QLineEdit *pathlineOverrideStartingTime;
    QLineEdit *pathlinePeriod;
    QButtonGroup *pathlineCMFEButtonGroup;

    QCheckBox *issueWarningForMaxSteps;
    QCheckBox *issueWarningForStepsize;
    QCheckBox *issueWarningForStiffness;
    QCheckBox *issueWarningForCriticalPoints;
    QLineEdit *criticalPointThreshold;
    QLabel    *criticalPointThresholdLabel;

    LimitCycleAttributes *atts;
};

#endif
