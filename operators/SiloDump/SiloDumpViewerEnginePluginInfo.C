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
//  File: SiloDumpViewerEnginePluginInfo.C
// ************************************************************************* //

#include <SiloDumpPluginInfo.h>
#include <SiloDumpAttributes.h>

//
// Storage for static data elements.
//
SiloDumpAttributes *SiloDumpViewerEnginePluginInfo::clientAtts = NULL;
SiloDumpAttributes *SiloDumpViewerEnginePluginInfo::defaultAtts = NULL;

// ****************************************************************************
//  Method:  SiloDumpViewerEnginePluginInfo::InitializeGlobalObjects
//
//  Purpose:
//    Initialize the operator atts.
//
//  Programmer: generated by xml2info
//  Creation:   omitted
//
// ****************************************************************************
void
SiloDumpViewerEnginePluginInfo::InitializeGlobalObjects()
{
    SiloDumpViewerEnginePluginInfo::clientAtts  = new SiloDumpAttributes;
    SiloDumpViewerEnginePluginInfo::defaultAtts = new SiloDumpAttributes;
}

// ****************************************************************************
//  Method: SiloDumpViewerEnginePluginInfo::GetClientAtts
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
SiloDumpViewerEnginePluginInfo::GetClientAtts()
{
    return clientAtts;
}

// ****************************************************************************
//  Method: SiloDumpViewerEnginePluginInfo::GetDefaultAtts
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
SiloDumpViewerEnginePluginInfo::GetDefaultAtts()
{
    return defaultAtts;
}

// ****************************************************************************
//  Method: SiloDumpViewerEnginePluginInfo::SetClientAtts
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
SiloDumpViewerEnginePluginInfo::SetClientAtts(AttributeSubject *atts)
{
    *clientAtts = *(SiloDumpAttributes *)atts;
    clientAtts->Notify();
}

// ****************************************************************************
//  Method: SiloDumpViewerEnginePluginInfo::GetClientAtts
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
SiloDumpViewerEnginePluginInfo::GetClientAtts(AttributeSubject *atts)
{
    *(SiloDumpAttributes *)atts = *clientAtts;
}

// ****************************************************************************
//  Method: SiloDumpViewerEnginePluginInfo::InitializeOperatorAtts
//
//  Purpose:
//    Initialize the operator attributes to the default attributes.
//
//  Arguments:
//    atts        The attribute subject to initialize.
//    plot        The viewer plot that owns the operator.
//    fromDefault True to initialize the attributes from the defaults. False
//                to initialize from the current attributes.
//
//  Programmer: generated by xml2info
//  Creation:   omitted
//
// ****************************************************************************

void
SiloDumpViewerEnginePluginInfo::InitializeOperatorAtts(AttributeSubject *atts,
                                              const avtPlotMetaData &plot,
                                              const bool fromDefault)
{
    if (fromDefault)
        *(SiloDumpAttributes*)atts = *defaultAtts;
    else
        *(SiloDumpAttributes*)atts = *clientAtts;

    UpdateOperatorAtts(atts, plot);
}

// ****************************************************************************
//  Method: SiloDumpViewerEnginePluginInfo::UpdateOperatorAtts
//
//  Purpose:
//    Update the operator attributes when using operator expressions.
//
//  Arguments:
//    atts        The attribute subject to update.
//    plot        The viewer plot that owns the operator.
//
//  Programmer: generated by xml2info
//  Creation:   omitted
//
// ****************************************************************************

void
SiloDumpViewerEnginePluginInfo::UpdateOperatorAtts(AttributeSubject *atts, const avtPlotMetaData &plot)
{
}

// ****************************************************************************
//  Method: SiloDumpViewerEnginePluginInfo::GetMenuName
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
SiloDumpViewerEnginePluginInfo::GetMenuName() const
{
    return "Silo Dump";
}

