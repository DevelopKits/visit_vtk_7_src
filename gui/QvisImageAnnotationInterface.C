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

#include <QvisImageAnnotationInterface.h>

#include <QCheckBox>
#include <QDir>
#include <QFileDialog>
#include <QWidget>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QToolTip>

#include <AnnotationObject.h>
#include <QvisColorButton.h>
#include <QvisDialogLineEdit.h>
#include <QvisOpacitySlider.h>
#include <QvisScreenPositionEdit.h>

// ****************************************************************************
// Method: QvisImageAnnotationInterface::QvisImageAnnotationInterface
//
// Purpose: 
//   Constructor for the QvisImageAnnotationInterface class.
//
// Arguments:
//   parent : This widget's parent widget.
//   name   : The name of this widget.
//
// Programmer: John C. Anderson
// Creation:   Fri Sep 03 09:22:46 PDT 2004
//
// Modifications:
//   Brad Whitlock, Tue Jun 28 15:46:08 PST 2005
//   Changed layout to make things fit better.
//
//   Brad Whitlock, Tue Apr  8 09:27:26 PDT 2008
//   Support for internationalization.
//
//   Brad Whitlock, Thu Jun 26 14:59:15 PDT 2008
//   Qt 4.
//
// ****************************************************************************

QvisImageAnnotationInterface::QvisImageAnnotationInterface(QWidget *parent) :
    QvisAnnotationObjectInterface(parent)
{
    // Set the title of the group box.
    this->setTitle(GetName());

    QGridLayout *cLayout = new QGridLayout(0);
    topLayout->addLayout(cLayout);
    cLayout->setSpacing(5);

    // Add controls for the image path.
    imageSource = new QvisDialogLineEdit(this);
    imageSource->setDialogMode(QvisDialogLineEdit::ChooseLocalFile);
    imageSource->setDialogFilter(tr("Images") + QString(" (*.bmp *.jpg *.pbm *.pgm *.png *.ppm *.tif)"));
    connect(imageSource, SIGNAL(textChanged(const QString &)),
            this, SLOT(imageSourceChanged(const QString &)));

    cLayout->addWidget(imageSource, 0, 1, 1, 3);
    QLabel *imageLabel = new QLabel(tr("Image source"), this);
    imageLabel->setBuddy(imageSource);
    imageLabel->setToolTip(tr("Name of the file containing the image"));
    cLayout->addWidget(imageLabel, 0, 0);

    // Add controls for the start position
    positionStartEdit = new QvisScreenPositionEdit(this);
    connect(positionStartEdit, SIGNAL(screenPositionChanged(double, double)),
            this, SLOT(positionStartChanged(double, double)));
    cLayout->addWidget(positionStartEdit, 1, 1, 1, 3);
    QLabel *lowerLeftLabel = new QLabel(tr("Lower left"), this);
    lowerLeftLabel->setBuddy(positionStartEdit);
    lowerLeftLabel->setToolTip(tr("Lower left corner of the image in "
        "screen coordinates [0,1]"));
    cLayout->addWidget(lowerLeftLabel, 1, 0);

    // Add controls for width.
    widthSpinBox = new QSpinBox(this);
    widthSpinBox->setMinimum(1);
    widthSpinBox->setMaximum(1000);
    widthSpinBox->setSuffix("%");
    widthSpinBox->setButtonSymbols(QSpinBox::PlusMinus);
    connect(widthSpinBox, SIGNAL(valueChanged(int)),
            this, SLOT(widthChanged(int)));
    cLayout->addWidget(widthSpinBox, 2, 1);
    QLabel *wLabel = new QLabel(tr("Width"), this);
    wLabel->setBuddy(widthSpinBox);
    cLayout->addWidget(wLabel, 2, 0);

    // Add controls for height.
    heightSpinBox = new QSpinBox(this);
    heightSpinBox->setSuffix("%");
    heightSpinBox->setMinimum(1);
    heightSpinBox->setMaximum(1000);
    heightSpinBox->setButtonSymbols(QSpinBox::PlusMinus);
    connect(heightSpinBox, SIGNAL(valueChanged(int)),
            this, SLOT(heightChanged(int)));
    cLayout->addWidget(heightSpinBox, 3, 1);
    QLabel *hLabel = new QLabel(tr("Height"), this);
    hLabel->setBuddy(heightSpinBox);
    cLayout->addWidget(hLabel, 3, 0);

    // Width/Height linked?
    linkedWH = new QCheckBox(tr("Lock aspect"), this);
    linkedWH->setChecked(true);
    connect(linkedWH, SIGNAL(toggled(bool)),
            this, SLOT(maintainAspectRatio(bool)));
    cLayout->addWidget(linkedWH, 2, 2, 1, 2);

    // Add controls for the opacity color.
    opacityCheck = new QCheckBox(tr("Transparent Color"), this);
    connect(opacityCheck, SIGNAL(toggled(bool)),
            this, SLOT(toggleOpacityColor(bool)));
    opacityColorButton = new QvisColorButton(this);
    connect(opacityColorButton, SIGNAL(selectedColor(const QColor &)),
            this, SLOT(opacityColorChanged(const QColor &)));
    opacityCheck->setChecked(false);
    cLayout->addWidget(opacityCheck, 4, 0, 1, 2);
    cLayout->addWidget(opacityColorButton, 4, 2);

    // Add controls for the overall opacity.
    int row = 5;
#ifdef OVERALL_OPACITY
    opacitySlider = new QvisOpacitySlider(0, 255, 10, 0, this);
    connect(opacitySlider, SIGNAL(valueChanged(int)),
            this, SLOT(opacityChanged(int)));
    cLayout->addWidget(new QLabel(tr("Opacity"), this), row, 0);
    cLayout->addMultiCellWidget(opacitySlider, row, row, 1, 3);
    ++row;
#endif

    // Added a visibility toggle
    visibleCheckBox = new QCheckBox(tr("Visible"), this);
    connect(visibleCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(visibilityToggled(bool)));
    cLayout->addWidget(visibleCheckBox, row, 0);
}

// ****************************************************************************
// Method: QvisImageAnnotationInterface::~QvisImageAnnotationInterface
//
// Purpose: 
//   Destructor for the QvisImageAnnotationInterface class.
//
// Programmer: John C. Anderson
// Creation:   Wed Nov 5 11:47:58 PDT 2003
//
// Modifications:
//   
// ****************************************************************************

QvisImageAnnotationInterface::~QvisImageAnnotationInterface()
{
}

// ****************************************************************************
// Method: QvisImageAnnotationInterface::GetMenuText
//
// Purpose: 
//   Returns the text to use in the annotation list box.
//
// Arguments:
//   annot : The annotation object to use for extra information.
//
// Returns:    The text to use in the annotation list box.
//
// Programmer: John C. Anderson
// Creation:   Wed Nov 5 16:06:47 PST 2003
//
// Modifications:
//   Brad Whitlock, Tue Apr  8 09:27:26 PDT 2008
//   Support for internationalization.
//
//   Brad Whitlock, Thu Jun 26 15:13:31 PDT 2008
//   Better implementation.
//
// ****************************************************************************

QString
QvisImageAnnotationInterface::GetMenuText(const AnnotationObject &annot) const
{
    QString retval;
    QString noImage(tr("no image"));
    if(annot.GetText().size() > 0)
    {
        if(annot.GetText()[0].size() > 0)
        {
            retval = QString("%1 - \"%2\"").
                     arg(GetName()).
                     arg(annot.GetText()[0].c_str());
        }
        else
            retval = GetName() + QString(" - [") + noImage + QString("]");
    }
    else
        retval = GetName() + QString(" - [") + noImage + QString("]");

    return retval;
}

// ****************************************************************************
// Method: QvisImageAnnotationInterface::UpdateControls
//
// Purpose: 
//   Updates the controls in the interface using the data in the Annotation
//   object pointed to by the annot pointer.
//
// Programmer: John C. Anderson
// Creation:   Wed Nov 5 11:48:15 PDT 2003
//
// Modifications:
//   
//   Hank Childs, Thu Jun  8 13:48:28 PDT 2006
//   Fix compiler warning for casting.
//
// ****************************************************************************

void
QvisImageAnnotationInterface::UpdateControls()
{
    // Set the source name.
    imageSource->blockSignals(true);
    if(annot->GetText().size() > 0)
        imageSource->setText(annot->GetText()[0].c_str());
    imageSource->blockSignals(false);

    // Set the start position.
    positionStartEdit->setPosition(annot->GetPosition()[0],
                                   annot->GetPosition()[1]);
    
    // Set the spinbox values for the width and height 
    linkedWH->blockSignals(true);
    widthSpinBox->blockSignals(true);
    heightSpinBox->blockSignals(true);
    linkedWH->setChecked(annot->GetFontShadow());
    widthSpinBox->setValue((int)(annot->GetPosition2()[0]));
    heightSpinBox->setValue((int)(annot->GetPosition2()[1]));
    widthSpinBox->blockSignals(false);
    heightSpinBox->blockSignals(false);
    linkedWH->blockSignals(false);

    // Change color and opacity.
    opacityCheck->blockSignals(true);
    opacityColorButton->blockSignals(true);
#ifdef OVERALL_OPACITY
    opacitySlider->blockSignals(true);
#endif
    opacityCheck->setChecked(annot->GetIntAttribute1());
    if(annot->GetIntAttribute1())
    {
        QColor c(annot->GetColor1().Red(),
                 annot->GetColor1().Green(),
                 annot->GetColor1().Blue());
        opacityColorButton->setEnabled(true);
        opacityColorButton->setButtonColor(c);
    }
    else
    {
        opacityColorButton->setEnabled(false);
        opacityColorButton->setButtonColor(Qt::white);
    }
#ifdef OVERALL_OPACITY
    opacitySlider->setValue(annot->GetColor1().Alpha());
    opacitySlider->blockSignals(false);
#endif
    opacityColorButton->blockSignals(false);
    opacityCheck->blockSignals(false);

    // Set the visible check box.
    visibleCheckBox->blockSignals(true);
    visibleCheckBox->setChecked(annot->GetVisible());
    visibleCheckBox->blockSignals(false);
}

// ****************************************************************************
// Method: QvisImageAnnotationInterface::GetCurrentValues
//
// Purpose: 
//   Gets the current values for the text fields.
//
// Arguments:
//   which_widget : The widget for which we're getting the values. -1 for all.
//
// Programmer: John C. Anderson
// Creation:   Wed Nov 5 11:49:08 PDT 2003
//
// Modifications:
//   Brad Whitlock, Mon Mar 6 11:05:02 PDT 2006
//   I added code to make sure that the screen position is recorded.
//
//   Brad Whitlock, Thu Jun 26 15:21:02 PDT 2008
//   Qt 4.
//
// ****************************************************************************

void
QvisImageAnnotationInterface::GetCurrentValues(int which_widget)
{
    bool doAll = (which_widget == -1);
    double pos[3];

    if(which_widget == 0 || doAll)
    {
        // Get the new position
        GetScreenPosition(positionStartEdit, tr("Lower left"));
    }

    if(which_widget == 1 || doAll)
    {
        // Get its new current value and store it in the atts.
        ForceSpinBoxUpdate(widthSpinBox);
        int w = widthSpinBox->value();
        pos[0] = w;
        pos[1] = annot->GetPosition2()[1];
        pos[2] = 0.;
        annot->SetPosition2(pos);
        if(linkedWH->isChecked())
            heightSpinBox->setValue(w);
    }

    if(which_widget == 2 || doAll)
    {
        // Get its new current value and store it in the atts.
        ForceSpinBoxUpdate(heightSpinBox);
        int h = heightSpinBox->value();
        pos[0] = annot->GetPosition2()[0];
        pos[1] = h;
        pos[2] = 0.;
        annot->SetPosition2(pos);
        if(linkedWH->isChecked())
            widthSpinBox->setValue(h);
    }
}

//
// Qt Slot functions
//

// ****************************************************************************
// Method: QvisImageAnnotationInterface::imageSourceChanged
//
// Purpose:
//   This is a Qt slot that gets called when the user
//   selects a different image.
//
// Arguments:
//   s    The new image filename.
//
// Returns:
//
// Note:
//
// Programmer: John C. Anderson
// Creation:   Mon Aug 30 09:31:40 PDT 2004
//
// Modifications:
//
// ****************************************************************************

void
QvisImageAnnotationInterface::imageSourceChanged(const QString &s)
{
    stringVector sv;
    sv.push_back(s.toStdString());
    annot->SetText(sv);
    SetUpdate(false);
    Apply();
}

// ****************************************************************************
// Method: QvisImageAnnotationInterface::positionChanged
//
// Purpose: 
//   This is a Qt slot function that is called when return is pressed in the 
//   position line edit.
//
// Programmer: John C. Anderson
// Creation:   Wed Nov 5 11:49:46 PDT 2003
//
// Modifications:
//   
// ****************************************************************************

void
QvisImageAnnotationInterface::positionStartChanged(double x, double y)
{
    double pos[] = {x, y, 0.};
    annot->SetPosition(pos);
    SetUpdate(false);
    Apply();
}

// ****************************************************************************
// Method: QvisImageAnnotationInterface::widthChanged
//
// Purpose: 
//   This is a Qt slot function that is called when the value of the width
//   spin box changes.
//
// Arguments:
//   w : The new width in percent.
//
// Programmer: John C. Anderson
// Creation:   Wed Nov 5 11:49:46 PDT 2003
//
// Modifications:
//   Brad Whitlock, Thu Jun 26 15:21:18 PDT 2008
//   Qt 4.
//
// ****************************************************************************

void
QvisImageAnnotationInterface::widthChanged(int w)
{
    double pos[] = {w, annot->GetPosition2()[1], 0};
    annot->SetPosition2(pos);
    SetUpdate(false);
    Apply();

    if(linkedWH->isChecked())
        heightSpinBox->setValue(w);
}

// ****************************************************************************
// Method: QvisImageAnnotationInterface::heightChanged
//
// Purpose: 
//   This is a Qt slot function that is called when the value of the height
//   spin box changes.
//
// Arguments:
//   w : The new width in percent.
//
// Programmer: John C. Anderson
// Creation:   Wed Nov 5 11:49:46 PDT 2003
//
// Modifications:
//   Brad Whitlock, Thu Jun 26 15:21:31 PDT 2008
//   Qt 4.
//
// ****************************************************************************

void
QvisImageAnnotationInterface::heightChanged(int h)
{
    double pos[] = {annot->GetPosition2()[0], h, 0};
    annot->SetPosition2(pos);
    SetUpdate(false);
    Apply();

    if(linkedWH->isChecked())
        widthSpinBox->setValue(h);
}

// ****************************************************************************
// Method: QvisImageAnnotationInterface::maintainAspectRatio
//
// Purpose:
//   This is a Qt slot that is called when the user checks/unchecks
//   the maintain aspect ratio check box.
//
// Arguments:
//   b   Whether or not maintain aspect ratio is enabled.
//
// Returns:
//
// Note:
//
// Programmer: John C. Anderson
// Creation:   Thu Sep 02 08:50:08 PDT 2004
//
// Modifications:
//
// ****************************************************************************

void
QvisImageAnnotationInterface::maintainAspectRatio(bool b)
{
    annot->SetFontShadow(b);
    SetUpdate(false);
    Apply();
}

// ****************************************************************************
// Method: QvisImageAnnotationInterface::toggleOpacityColor
//
// Purpose:
//   This is a Qt slot that is called whenever the user checks/unchecks
//   the opacity check box.
//
// Arguments:
//   b    Whether or not opacity color is enabled.
//
// Returns:
//
// Note:
//
// Programmer: John C. Anderson
// Creation:   Mon Aug 30 09:32:04 PDT 2004
//
// Modifications:
//
// ****************************************************************************

void
QvisImageAnnotationInterface::toggleOpacityColor(bool b)
{
    if(b)
    {
        opacityColorButton->setEnabled(true);
        annot->SetIntAttribute1(1);
    }
    else
    {
        opacityColorButton->setEnabled(false);
        annot->SetIntAttribute1(0);
    }
    opacityColorChanged(opacityColorButton->buttonColor());
}

// ****************************************************************************
// Method: QvisImageAnnotationInterface::opacityColorChanged
//
// Purpose: This is a Qt slot function that is called when a new
// opacity color is set.
//
// Arguments:
//   c : The new start color.
//
// Programmer: John C. Anderson
// Creation:   Wed Nov 5 11:49:46 PDT 2003
//
// Modifications:
//   
// ****************************************************************************

void
QvisImageAnnotationInterface::opacityColorChanged(const QColor &c)
{
    int a = annot->GetColor1().Alpha();
    ColorAttribute tc(c.red(), c.green(), c.blue(), a);
    annot->SetColor1(tc);
    Apply();
}

// ****************************************************************************
// Method: QvisImageAnnotationInterface::opacityChanged
//
// Purpose: 
//   This is a Qt slot function that is called when a new opacity is
//   selected.
//
// Arguments:
//   opacity : The new start opacity.
//
// Programmer: John C. Anderson
// Creation:   Wed Nov 5 11:49:46 PDT 2003
//
// Modifications:
//   
// ****************************************************************************

void
QvisImageAnnotationInterface::opacityChanged(int opacity)
{
    ColorAttribute tc(annot->GetColor1());
    tc.SetAlpha(opacity);
    annot->SetColor1(tc);
    SetUpdate(false);
    Apply();
}

// ****************************************************************************
// Method: QvisImageAnnotationInterface::visibilityToggled
//
// Purpose: 
//   This is a Qt slot function that is called when the visibility toggle is
//   changed.
//
// Arguments:
//   val : The visibility flag.
//
// Programmer: John C. Anderson
// Creation:   Wed Nov 5 11:49:46 PDT 2003
//
// Modifications:
//   
// ****************************************************************************

void
QvisImageAnnotationInterface::visibilityToggled(bool val)
{
    annot->SetVisible(val);
    SetUpdate(false);
    Apply();
}
