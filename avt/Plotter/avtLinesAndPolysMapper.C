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
//                   avtLinesAndPolysMapper.C                          //
// ************************************************************************* //

#include <avtLinesAndPolysMapper.h>

#include <vtkActor.h>
#include <vtkDataSet.h>
#include <vtkDataSetMapper.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>


// ****************************************************************************
//  Method: avtLinesAndPolysMapper constructor
//
//  Programmer: Kathleen Biagas 
//  Creation:   May 11, 2016 
//
//  Modifications:
//    Kathleen Biagas, Wed May 11 14:20:20 PDT 2016
//    Derive from avtSimpleMapper.
//
// ****************************************************************************

avtLinesAndPolysMapper::avtLinesAndPolysMapper() : avtSimpleMapper()
{
    ignoreLighting = true;
    surfaceVis = true;
}


// ****************************************************************************
//  Method: avtLinesAndPolysMapper destructor
//
//  Programmer: Kathleen Biagas 
//  Creation:   May 11, 2016 
//
//  Modifications:
//
// ****************************************************************************

avtLinesAndPolysMapper::~avtLinesAndPolysMapper()
{
}


// ****************************************************************************
//  Method: avtLinesAndPolysMapper::GetLighting
//
//  Purpose:
//      Returns the lighting state.
//
//  Returns:
//      True if lighting for this mapper is ON, false otherwise. 
//
//  Programmer: Kathleen Biagas 
//  Creation:   May 11, 2016
//
// ****************************************************************************

bool
avtLinesAndPolysMapper::GetLighting()
{
   return !ignoreLighting; 
}


// ****************************************************************************
//  Method: avtLinesAndPolysMapper::SetSurfaceRepresentation
//
//  Purpose:
//      Sets the drawable's surface representation.
//
//  Arguments:
//      rep : The new surface representation.
//
//  Programmer: Brad Whitlock
//  Creation:   Mon Sep 23 15:58:48 PST 2002
//
//  Modifications:
//    Kathleen Bonnell, Sat Oct 19 15:07:04 PDT 2002 
//    Disable lighting for Wireframe and Points representation.
//
//    Kathleen Bonnell, Thu Sep  2 11:44:09 PDT 2004 
//    Moved from avtGeometryDrawable so that derived mappers may override. 
//
// ****************************************************************************

void
avtLinesAndPolysMapper::SetSurfaceRepresentation(int globalRep)
{
  // This is global rep, don't allow it to set anything for now.
}


// ****************************************************************************
//  Method: avtLinesAndPolysMapper::CustomizeMappers
//
//  Purpose:
//      A hook from the base class that allows the surface and wireframe
//      mapper to make any calls take effect that were made before the base
//      class set up the vtk mappers.
//
//  Programmer: Kathleen Biagas
//  Creation:   May 11, 2016 
//
//  Modifications:
//
// ****************************************************************************

void
avtLinesAndPolysMapper::CustomizeMappers()
{
    avtDataObject_p input = GetInput();
    if (*input == NULL)
        return;

    avtDataTree_p tree  = GetInputDataTree();
    if (*tree == NULL)
        return;

    int nDs = 0;
    vtkDataSet **ds = tree->GetAllLeaves(nDs);

    for (int i = 0 ; i < nMappers ; i++)
    {
        vtkPolyData *pd = vtkPolyData::SafeDownCast(ds[i]);
        if (pd == NULL)
        {
            continue;
        }
        bool hasVerts  = pd->GetNumberOfVerts()  > 0;
        bool hasLines  = pd->GetNumberOfLines()  > 0;
        bool hasPolys  = pd->GetNumberOfPolys()  > 0;
        bool hasStrips = pd->GetNumberOfStrips() > 0;
        if (mappers[i] != NULL)
        {
            mappers[i]->SetScalarVisibility(false);
        }
        if (actors[i] != NULL)
        {
            vtkProperty *prop = actors[i]->GetProperty();
            if (hasLines)
            {
                if (edgeVis)
                {
                    // Not sure of the cost for doing this if we are not 
                    // rendering the polys.  Could add a test for surfaceVis ??
                    mappers[i]->SetResolveCoincidentTopologyToPolygonOffset();
                    mappers[i]->SetResolveCoincidentTopologyLineOffsetParameters(1., 1.);
                    prop->SetColor(edgeColor);
                    prop->SetOpacity(opacity);
                    prop->SetLineWidth(lineWidth);
                    // This currently isn't supported with vtk's opengl2 backend
                    prop->SetLineStipplePattern(lineStyle);
                }
                else
                {
                    prop->SetOpacity(0.0);
                }
            }
            if (hasPolys || hasStrips) 
            {
                if (surfaceVis)
                {
                    prop->SetColor(surfaceColor);
                    prop->SetOpacity(opacity);
                }
                else
                {
                    prop->SetOpacity(0.0);
                }
            }
            if (ignoreLighting)
            {
                prop->SetAmbient(1.);
                prop->SetDiffuse(0.);
            }
        }
    }
}


// ****************************************************************************
//  Method: avtLinesAndPolysMapper::SetSurfaceVisibility
//
//  Purpose:
//      Toggles surface visibility.
//
//  Programmer: Kathleen Biagas
//  Creation:   May 11, 2016 
//
//  Modifications:
//
// ****************************************************************************

void
avtLinesAndPolysMapper::SetSurfaceVisibility(bool val)
{
    if (surfaceVis != val)
    {
        surfaceVis = val;
        CustomizeMappers();
        NotifyTransparencyActor();
    }
}

