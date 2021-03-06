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
//                          avtAugDecompFileFormat.C                         //
// ************************************************************************* //

#include <avtAugDecompFileFormat.h>

#include <string>
#include <snprintf.h>
#include <visitstream.h>
#include <visit-config.h>

#include <vtkFloatArray.h>
#include <vtkRectilinearGrid.h>
#include <vtkUnstructuredGrid.h>

#include <avtDatabaseFactory.h>
#include <avtDatabaseMetaData.h>
#include <avtFileFormatInterface.h>
#include <avtGenericDatabase.h>

#include <InvalidFilesException.h>
#include <DatabasePluginInfo.h>
#include <DatabasePluginManager.h>
#include <DBOptionsAttributes.h>
#include <Expression.h>

#include <InvalidVariableException.h>


using     std::string;


class  avtBreakEncapsulationGenericDatabase : public avtGenericDatabase
{
  public:
                           avtBreakEncapsulationGenericDatabase(avtFileFormatInterface *ffi)
                                : avtGenericDatabase(ffi) {;};
    avtFileFormatInterface     *GetFileFormatInterface(void) { return Interface; };
};


// ****************************************************************************
//  Method: avtAugDecompFileFormat constructor
//
//  Programmer: childs -- generated by xml2avt
//  Creation:   Wed Feb 13 08:58:40 PDT 2008
//
//  Modifications:
//    Brad Whitlock, Tue Jun 24 16:35:26 PDT 2008
//    Pass a pointer to the plugin info. That's how we get the plugin manager
//    since it's no longer a singleton.
//
//    Jeremy Meredith, Thu Jan  7 12:09:53 EST 2010
//    Check to make sure we correctly read the number of subsets and domains.
//
// ****************************************************************************

avtAugDecompFileFormat::avtAugDecompFileFormat(const char *augd_filename,
    CommonDatabasePluginInfo *info)
    : avtMTMDFileFormat(augd_filename)
{
    ifstream ifile(augd_filename);
    ifile >> filename;
    if (filename[0] != VISIT_SLASH_CHAR)
    {
        int len = strlen(augd_filename);
        const char *last = augd_filename + (len-1);
        while (*last != VISIT_SLASH_CHAR && last > augd_filename)
        {
            last--;
        }

        if (*last == VISIT_SLASH_CHAR)
        {
            char str[1024];
            strcpy(str, augd_filename);
            str[last-augd_filename+1] = '\0';

            char str2[1024];
            SNPRINTF(str2, 1024, "%s%s", str, filename.c_str());
            filename = str2;
        }
    }

    int numSubsets;
    ifile >> numSubsets;
    if (!ifile)
    {
        EXCEPTION1(InvalidFilesException, filename.c_str());
    }        
    subset_names.resize(numSubsets);
    for (int i = 0 ; i < numSubsets ; i++)
        ifile >> subset_names[i];
#ifdef ENGINE
    int numDomains;
    ifile >>  numDomains;
    if (!ifile)
    {
        EXCEPTION1(InvalidFilesException, filename.c_str());
    }        
    zone_counts.resize(numDomains);
    subset_vals.resize(numDomains);
    for (int i = 0 ; i < numDomains ; i++)
    {
        ifile >> zone_counts[i];
        subset_vals[i].resize(zone_counts[i]);
    }
    for (int i = 0 ; i < numDomains ; i++)
        for (int j = 0 ; j < zone_counts[i] ; j++)
            ifile >> subset_vals[i][j];
#endif

    std::vector<std::string> pluginsUsed;
    const char *tmpname = filename.c_str();
    DatabasePluginManager *dbmgr = info->GetPluginManager();
    dbmgr->LoadPluginsNow();
    avtBreakEncapsulationGenericDatabase *begp = 
        (avtBreakEncapsulationGenericDatabase *) avtDatabaseFactory::FileList(
            dbmgr, &tmpname,1,0, pluginsUsed, NULL);
    interface = begp->GetFileFormatInterface();
    if (interface == NULL)
    {
        EXCEPTION1(InvalidFilesException, filename.c_str());
    }
}


// ****************************************************************************
//  Method: avtEMSTDFileFormat::GetNTimesteps
//
//  Purpose:
//      Tells the rest of the code how many timesteps there are in this file.
//
//  Programmer: childs -- generated by xml2avt
//  Creation:   Wed Feb 13 08:58:40 PDT 2008
//
// ****************************************************************************

int
avtAugDecompFileFormat::GetNTimesteps(void)
{
    //return interface->GetNTimesteps();
    return 1;
}


// ****************************************************************************
//  Method: avtAugDecompFileFormat::FreeUpResources
//
//  Purpose:
//      When VisIt is done focusing on a particular timestep, it asks that
//      timestep to free up any resources (memory, file descriptors) that
//      it has associated with it.  This method is the mechanism for doing
//      that.
//
//  Programmer: childs -- generated by xml2avt
//  Creation:   Wed Feb 13 08:58:40 PDT 2008
//
// ****************************************************************************

void
avtAugDecompFileFormat::FreeUpResources(void)
{
    interface->FreeUpResources(-1, -1);
}


// ****************************************************************************
//  Method: avtAugDecompFileFormat::PopulateDatabaseMetaData
//
//  Purpose:
//      This database meta-data object is like a table of contents for the
//      file.  By populating it, you are telling the rest of VisIt what
//      information it can request from you.
//
//  Programmer: childs -- generated by xml2avt
//  Creation:   Wed Feb 13 08:58:40 PDT 2008
//
// ****************************************************************************

void
avtAugDecompFileFormat::PopulateDatabaseMetaData(avtDatabaseMetaData *md, int timeState)
{
    interface->SetDatabaseMetaData(md, timeState);
    int nm = md->GetNumMaterials();
    for (int i = nm-1 ; i >= 0 ; i--)
        md->RemoveMaterials(i);

    string mesh_for_mat = md->GetMeshes(0).name;
    string matname = "decomposition";
    int nmats = subset_names.size();
    AddMaterialToMetaData(md, matname, mesh_for_mat, nmats, subset_names);
}


// ****************************************************************************
//  Method: avtAugDecompFileFormat::GetMesh
//
//  Purpose:
//      Gets the mesh associated with this file.  The mesh is returned as a
//      derived type of vtkDataSet (ie vtkRectilinearGrid, vtkStructuredGrid,
//      vtkUnstructuredGrid, etc).
//
//  Arguments:
//      timestate   The index of the timestate.  If GetNTimesteps returned
//                  'N' time steps, this is guaranteed to be between 0 and N-1.
//      domain      The index of the domain.  If there are NDomains, this
//                  value is guaranteed to be between 0 and NDomains-1,
//                  regardless of block origin.
//      meshname    The name of the mesh of interest.  This can be ignored if
//                  there is only one mesh.
//
//  Programmer: childs -- generated by xml2avt
//  Creation:   Wed Feb 13 08:58:40 PDT 2008
//
// ****************************************************************************

vtkDataSet *
avtAugDecompFileFormat::GetMesh(int timestate, int domain, const char *meshname)
{
    return interface->GetMesh(timestate, domain, meshname);
}


// ****************************************************************************
//  Method: avtAugDecompFileFormat::GetVar
//
//  Purpose:
//      Gets a scalar variable associated with this file.  Although VTK has
//      support for many different types, the best bet is vtkFloatArray, since
//      that is supported everywhere through VisIt.
//
//  Arguments:
//      timestate  The index of the timestate.  If GetNTimesteps returned
//                 'N' time steps, this is guaranteed to be between 0 and N-1.
//      domain     The index of the domain.  If there are NDomains, this
//                 value is guaranteed to be between 0 and NDomains-1,
//                 regardless of block origin.
//      varname    The name of the variable requested.
//
//  Programmer: childs -- generated by xml2avt
//  Creation:   Wed Feb 13 08:58:40 PDT 2008
//
// ****************************************************************************

vtkDataArray *
avtAugDecompFileFormat::GetVar(int timestate, int domain, const char *varname)
{
    return interface->GetVar(timestate, domain, varname);
}


// ****************************************************************************
//  Method: avtAugDecompFileFormat::GetVectorVar
//
//  Purpose:
//      Gets a vector variable associated with this file.  Although VTK has
//      support for many different types, the best bet is vtkFloatArray, since
//      that is supported everywhere through VisIt.
//
//  Arguments:
//      timestate  The index of the timestate.  If GetNTimesteps returned
//                 'N' time steps, this is guaranteed to be between 0 and N-1.
//      domain     The index of the domain.  If there are NDomains, this
//                 value is guaranteed to be between 0 and NDomains-1,
//                 regardless of block origin.
//      varname    The name of the variable requested.
//
//  Programmer: childs -- generated by xml2avt
//  Creation:   Wed Feb 13 08:58:40 PDT 2008
//
// ****************************************************************************

vtkDataArray *
avtAugDecompFileFormat::GetVectorVar(int timestate, int domain,const char *varname)
{
    return interface->GetVectorVar(timestate, domain, varname);
}


void      *
avtAugDecompFileFormat::GetAuxiliaryData(const char *var, int timestep,
                                        int domain, const char *type, void *args,
                                        DestructorFunction &df)
{
    void *rv = NULL;
    if (strcmp(type, AUXILIARY_DATA_MATERIAL) == 0)
    {
        avtMaterial *mat = new avtMaterial((int)subset_names.size(), subset_names, 
                                           zone_counts[domain], &(subset_vals[domain][0]),
                                           0, NULL, NULL, NULL, NULL);
        df = avtMaterial::Destruct;
        rv = mat;
    }
    else
    {
        rv = interface->GetAuxiliaryData(var, timestep, domain, type, args, df);
    }

    return rv;
}


