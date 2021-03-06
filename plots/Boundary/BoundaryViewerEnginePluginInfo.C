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
//  File: BoundaryViewerEnginePluginInfo.C
// ************************************************************************* //

#include <BoundaryPluginInfo.h>
#include <avtBoundaryPlot.h>
#include <BoundaryAttributes.h>

//
// Storage for static data elements.
//
BoundaryAttributes *BoundaryViewerEnginePluginInfo::clientAtts = NULL;
BoundaryAttributes *BoundaryViewerEnginePluginInfo::defaultAtts = NULL;

// ****************************************************************************
//  Method:  BoundaryViewerEnginePluginInfo::InitializeGlobalObjects
//
//  Purpose:
//    Initialize the plot atts.
//
//  Programmer: generated by xml2info
//  Creation:   omitted
//
// ****************************************************************************
void
BoundaryViewerEnginePluginInfo::InitializeGlobalObjects()
{
    BoundaryViewerEnginePluginInfo::clientAtts  = new BoundaryAttributes;
    BoundaryViewerEnginePluginInfo::defaultAtts = new BoundaryAttributes;
}

// ****************************************************************************
//  Method: BoundaryViewerEnginePluginInfo::GetClientAtts
//
//  Purpose:
//    Return a pointer to the viewer client attributes.
//
//  Returns:    A pointer to the viewer client attributes.
//
//  Programmer: generated by xml2info
//  Creation:   omitted
//
// ****************************************************************************

AttributeSubject *
BoundaryViewerEnginePluginInfo::GetClientAtts()
{
    return clientAtts;
}

// ****************************************************************************
//  Method: BoundaryViewerEnginePluginInfo::GetDefaultAtts
//
//  Purpose:
//    Return a pointer to the viewer default attributes.
//
//  Returns:    A pointer to the viewer default attributes.
//
//  Programmer: generated by xml2info
//  Creation:   omitted
//
// ****************************************************************************

AttributeSubject *
BoundaryViewerEnginePluginInfo::GetDefaultAtts()
{
    return defaultAtts;
}

// ****************************************************************************
//  Method: BoundaryViewerEnginePluginInfo::SetClientAtts
//
//  Purpose:
//    Set the viewer client attributes.
//
//  Arguments:
//    atts      A pointer to the new client attributes.
//
//  Programmer: generated by xml2info
//  Creation:   omitted
//
// ****************************************************************************

void
BoundaryViewerEnginePluginInfo::SetClientAtts(AttributeSubject *atts)
{
    *clientAtts = *(BoundaryAttributes *)atts;
    clientAtts->Notify();
}

// ****************************************************************************
//  Method: BoundaryViewerEnginePluginInfo::GetClientAtts
//
//  Purpose:
//    Get the viewer client attributes.
//
//  Arguments:
//    atts      A pointer to return the client default attributes in.
//
//  Programmer: generated by xml2info
//  Creation:   omitted
//
// ****************************************************************************

void
BoundaryViewerEnginePluginInfo::GetClientAtts(AttributeSubject *atts)
{
    *(BoundaryAttributes *)atts = *clientAtts;
}

// ****************************************************************************
//  Method: BoundaryViewerEnginePluginInfo::AllocAvtPlot
//
//  Purpose:
//    Return a pointer to a newly allocated avt plot.
//
//  Returns:    A pointer to the newly allocated avt plot.
//
//  Programmer: generated by xml2info
//  Creation:   omitted
//
// ****************************************************************************

avtPlot *
BoundaryViewerEnginePluginInfo::AllocAvtPlot()
{
    return new avtBoundaryPlot;
}

// ****************************************************************************
//  Method: BoundaryViewerEnginePluginInfo::InitializePlotAtts
//
//  Purpose:
//    Initialize the plot attributes to the default attributes.
//
//  Arguments:
//    atts      The attribute subject to initialize.
//
//  Programmer: whitlocb -- generated by xml2info
//  Creation:   Tue Mar 18 09:01:21 PDT 2003
//
//  Modifications:
//    Kathleen Bonnell, Tue Nov  6 08:19:45 PST 2001
//    Create labels only from mesh related to varName, not from all
//    meshes.
//
//    Hank Childs, Wed Aug 14 11:30:18 PDT 2002
//    Only use the labels from the material we actually have.
//
//    Kathleen Bonnell, Thu Sep  5 10:55:47 PDT 2002  
//    Moved bulk of code to PrivateSetPlotAtts to aid in maintenance, as it is
//    shared with ResetPlotAtts. 
//
//    Brad Whitlock, Fri Mar 26 15:15:38 PST 2004
//    I made it use passed in metadata.
//
//    Brad Whitlock, Wed Feb 21 14:27:15 PST 2007
//    Changed API.
//
// ****************************************************************************

void
BoundaryViewerEnginePluginInfo::InitializePlotAtts(AttributeSubject *atts,
    const avtPlotMetaData &plot)
{
    *(BoundaryAttributes*)atts = *defaultAtts;

    PrivateSetPlotAtts(atts, plot);
}

// ****************************************************************************
//  Method: BoundaryViewerEnginePluginInfo::ReInitializePlotAtts
//
//  Purpose:
//    ReInitialize the plot attributes.
//
//  Arguments:
//    atts      The attribute subject to initialize.
//
//  Programmer: Kathleen Bonnell 
//  Creation:   December 5, 2002 
//
//  Modifications:
//    Brad Whitlock, Fri Mar 26 15:15:38 PST 2004
//    I made it use passed in metadata.
//
//    Brad Whitlock, Wed Feb 21 14:31:20 PST 2007
//    Changed API.
//
// ****************************************************************************

void
BoundaryViewerEnginePluginInfo::ReInitializePlotAtts(AttributeSubject *atts,
    const avtPlotMetaData &plot)
{
    PrivateSetPlotAtts(atts, plot);
}

// ****************************************************************************
//  Method: BoundaryViewerEnginePluginInfo::ResetPlotAtts
//
//  Purpose:
//    Initialize the plot attributes to the default attributes.
//
//  Arguments:
//    atts      The attribute subject to initialize.
//
//  Programmer: meredith -- generated by xml2info
//  Creation:   Fri Sep 7 10:53:06 PDT 2001
//
//  Modifications:
//    Kathleen Bonnell, Mon Apr 29 13:37:14 PDT 2002  
//    Create labels only from mesh related to varName, not from all
//    meshes.
//
//    Hank Childs, Wed Aug 14 11:30:18 PDT 2002
//    Only use the labels from the material we actually have.
//
//    Kathleen Bonnell, Thu Sep  5 10:55:47 PDT 2002  
//    Moved code to PrivateSetPlotAtts to aid in maintenance, as the code is
//    shared with InitializePlotAtts. 
//
//    Brad Whitlock, Fri Mar 26 15:15:38 PST 2004
//    I made it use passed in metadata.
//
//    Brad Whitlock, Wed Feb 21 14:27:15 PST 2007
//    Changed API.
//
// ****************************************************************************

void
BoundaryViewerEnginePluginInfo::ResetPlotAtts(AttributeSubject *atts,
    const avtPlotMetaData &plot)
{
    PrivateSetPlotAtts(atts, plot);
}

// ****************************************************************************
//  Method: BoundaryViewerEnginePluginInfo::GetMenuName
//
//  Purpose:
//    Return a pointer to the name to use in the viewer menus.
//
//  Returns:    A pointer to the name to use in the viewer menus.
//
//  Programmer: generated by xml2info
//  Creation:   omitted
//
// ****************************************************************************

const char *
BoundaryViewerEnginePluginInfo::GetMenuName() const
{
    return "Boundary";
}

// ****************************************************************************
//  Method: BoundaryViewerEnginePluginInfo::PrivateSetPlotAtts
//
//  Purpose:
//    Initialize the plot attributes. 
//
//  Arguments:
//    atts          The attribute subject to initialize.
//    hostName      The host name of the plot. 
//    databaseName  The database name of the plot.
//    varName       The variable name of the plot.
//
//  Notes:  
//    This code was pulled from ResetPlotAtts and InitializePlotAtts to
//    aid in maintenance, and reworkd to support groups.
//
//  Programmer: Kathleen Bonnell 
//  Creation:   September 5, 2002 
//
//  Modifications:
//    Kathleen Bonnell, Thu Dec  5 16:53:22 PST 2002 
//    Changed exception from ImproperUse to InvalidVariable.
//
//    Brad Whitlock, Wed Nov 20 14:12:03 PST 2002
//    I added support for discrete color tables.
//
//    Kathleen Bonnell, Thu Sep  4 16:08:46 PDT 2003 
//    Set colors, subsetNames for defaultAtts so that "Reset" won't zero
//    out the colors in the gui.
//
//    Brad Whitlock, Fri Mar 26 15:15:38 PST 2004
//    I made it use passed in metadata.
//
//    Brad Whitlock, Wed Feb 21 14:27:15 PST 2007
//    Changed API.
//
// ****************************************************************************

#include <stdio.h>

#include <avtColorTables.h>
#include <avtDatabaseMetaData.h>
#include <avtBoundaryPlot.h>
#include <avtTypes.h>
#include <set>

#include <avtPlotMetaData.h>

#include <DebugStream.h>
#include <InvalidVariableException.h>

void
BoundaryViewerEnginePluginInfo::PrivateSetPlotAtts(AttributeSubject *atts, 
    const avtPlotMetaData &plot)
{
    BoundaryAttributes *boundaryAtts = (BoundaryAttributes *)atts;

    //
    // Get the meta-data and initialize the boundary names and colors in the
    // new BoundaryAttributes object.
    //
    const avtDatabaseMetaData *md = plot.GetMetaData();
    if (md == NULL)
    {
        return;
    }

    avtDatabaseMetaData *nonConstmd = const_cast <avtDatabaseMetaData *>(md);

    std::string vn(plot.GetVariableName());

    const avtMaterialMetaData *mat = NULL;

    std::string meshName = nonConstmd->MeshForVar(vn);
    avtMeshMetaData *mesh = 
        const_cast <avtMeshMetaData *> (md->GetMesh(meshName));


    stringVector       sv;
    stringVector::const_iterator pos;
    std::set<int> groupSet;
    std::vector<int> gIDS;
    char temp[512];

    // 
    // Create boundary names, based on Boundary Type 
    // 
    avtSubsetType subT = nonConstmd->DetermineSubsetType(vn);
    switch (subT)
    {
      case AVT_DOMAIN_SUBSET : 
          debug5 << "Variable for boundary plot is a domain Mesh." << endl; 
          boundaryAtts->SetBoundaryType(BoundaryAttributes::Domain);
          defaultAtts->SetBoundaryType(BoundaryAttributes::Domain);
          if (mesh->blockNames.empty())
          {
              for (int i = 0; i < mesh->numBlocks; i++)
              { 
                  sprintf(temp, "%d", i+mesh->blockOrigin);
                  sv.push_back(temp);
              }
          }
          else
          {
              for(pos = mesh->blockNames.begin();
                  pos != mesh->blockNames.end(); ++pos)
              {
                  sv.push_back(*pos);
              }
          }
          break;

      case AVT_GROUP_SUBSET :
          debug5 << "Variable for boundary plot is a group Mesh." << endl; 
          boundaryAtts->SetBoundaryType(BoundaryAttributes::Group);
          defaultAtts->SetBoundaryType(BoundaryAttributes::Group);
          for (size_t i = 0; i < mesh->groupIds.size(); i++)
          {
              if (groupSet.count(mesh->groupIds[i]) == 0)
              {
                  groupSet.insert(mesh->groupIds[i]);
                  gIDS.push_back(mesh->groupIds[i]);
              }
          }
          for (size_t i = 0; i < gIDS.size(); i++)
          {
              sprintf(temp, "%d", gIDS[i]);
              sv.push_back(temp);
          }
          break;

      case AVT_MATERIAL_SUBSET :
          debug5 << "Variable for boundary plot is a Material." << endl; 
          boundaryAtts->SetBoundaryType(BoundaryAttributes::Material);
          defaultAtts->SetBoundaryType(BoundaryAttributes::Material);
          mat = md->GetMaterial(vn);
          if (mat != NULL)
          {
              for(pos = mat->materialNames.begin();
                  pos != mat->materialNames.end(); ++pos)
              {
                  sv.push_back(*pos);
              }
          }
          break;

      default:
          EXCEPTION1(InvalidVariableException, vn);
          break;
    }
    
    // 
    // Add a color for each boundary name.
    //
    ColorAttribute *ca = new ColorAttribute[sv.size() + 1];
    avtColorTables *ct = avtColorTables::Instance();
    if(ct->IsDiscrete(ct->GetDefaultDiscreteColorTable()))
    {
        // The CT is discrete, get its color color control points.
        for(int i = 0; i < (int)sv.size(); ++i)
        {
            unsigned char rgb[3] = {0,0,0};
            ct->GetControlPointColor(ct->GetDefaultDiscreteColorTable(), i, rgb);
            ca[i].SetRed(int(rgb[0]));
            ca[i].SetGreen(int(rgb[1]));
            ca[i].SetBlue(int(rgb[2]));
        }
    }
    else
    {
        // The CT is continuous, sample the CT so we have a unique color
        // for each element in sv.
        unsigned char *rgb = ct->GetSampledColors(
            ct->GetDefaultDiscreteColorTable(), (int)sv.size());
        if(rgb)
        {
            for(size_t i = 0; i < sv.size(); ++i)
            {
                ca[i].SetRed(int(rgb[i*3]));
                ca[i].SetGreen(int(rgb[i*3+1]));
                ca[i].SetBlue(int(rgb[i*3+2]));
            }
            delete [] rgb;
        }
    }

    ColorAttributeList cal;
    int idx = 0;
    for(pos = sv.begin(); pos != sv.end(); ++pos)
    {
        if (idx < boundaryAtts->GetMultiColor().GetNumColors())
        {
            // The meshIndex is within the defaultAtts' color
            // vector size.
            cal.AddColors(boundaryAtts->GetMultiColor()[idx]);
        }
        else
        {
            // The meshIndex is greater than the size of the
            // defaultAtts' color vector. Use colors from the
            // default discrete color table.
            cal.AddColors(ca[idx]);
        }
        ++idx;
    }

    delete [] ca;

    // Set the boundary names and colors in the boundaryAtts.
    boundaryAtts->SetBoundaryNames(sv);
    boundaryAtts->SetMultiColor(cal);
    defaultAtts->SetBoundaryNames(sv);
    defaultAtts->SetMultiColor(cal);
}

