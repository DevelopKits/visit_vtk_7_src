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

// ************************************************************************* //
//                    avtSurfaceAndWireframeMapper.h                         //
// ************************************************************************* //

#ifndef AVT_SURFACEANDWIREFRAMEMAPPER_H
#define AVT_SURFACEANDWIREFRAMEMAPPER_H

#include <plotter_exports.h>
#include <avtMapper.h>

#include <avtTerminatingDatasetSink.h>
#include <avtDrawable.h>

#include <vector>
#include <string>

class   vtkActor;
class   vtkAlgorithmOutput;
class   vtkDataObjectCollection;
class   vtkDataSetMapper;

class   avtTransparencyActor;
class   ColorAttribute;


// ****************************************************************************
//  Class:  avtSurfaceAndWireframeMapper
//
//  Purpose:
//      This takes geometry and makes a drawable by mapping the variable to 
//      colors.
//
//      Specializes avtMapper by allowing wireframe representation to be
//      drawn at same time as surface.
//
//  Programmer: Kathleen Biagas 
//  Creation:   May 11, 2016 
//
//  Modifications:
//
// ****************************************************************************

class PLOTTER_API avtSurfaceAndWireframeMapper : public avtMapper
{
  public:
                               avtSurfaceAndWireframeMapper();
    virtual                   ~avtSurfaceAndWireframeMapper();


    virtual bool               GetLighting(void);

    virtual void               SetSurfaceRepresentation(int rep);

    void                       SetIgnoreLighting(bool val)
                                   { ignoreLighting = val; }

    void                       SetSurfaceVisibility(bool val);
    void                       SetLineVisibility(bool val);

    void                       SetSurfaceColor(double rgb[3]);
    void                       SetLineColor(double rgb[3]);

    void                       SetOpacity(double val);
    void                       SetLineWidth(int lw);
    void                       SetLineStyle(int ls);

  protected:
    bool                       ignoreLighting;
    bool                       surfaceVis;
    bool                       lineVis;
    double                     surfaceColor[3];
    double                     lineColor[3];
    double                     opacity;
    int                        lineWidth;
    int                        lineStyle;

    virtual void               CustomizeMappers(void);
};


#endif


