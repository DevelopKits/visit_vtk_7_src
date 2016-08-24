/*****************************************************************************
*
* Copyright (c) 2000 - 2016, Lawrence Livermore National Security, LLC
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


#include "vtkOpenGLPointMapper.h"

#include <vtkOpenGLHelper.h>

#include <vtkObjectFactory.h>
#include <vtkOpenGLActor.h>
#include <vtkOpenGLCamera.h>
#include <vtkOpenGLIndexBufferObject.h>
#include <vtkOpenGLPolyDataMapper.h>
#include <vtkOpenGLVertexArrayObject.h>
#include <vtkOpenGLVertexBufferObject.h>
#include <vtkMatrix4x4.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkShaderProgram.h>
#include <vtkUnsignedCharArray.h>

#include "vtkSphereMapperVS.h"

#include <vtk_glew.h>

// A helper class to draw points as sphere imposters

class vtkImposterHelper : public vtkOpenGLPolyDataMapper
{
public:
  static vtkImposterHelper* New();
  vtkTypeMacro(vtkImposterHelper, vtkOpenGLPolyDataMapper)

  vtkSetMacro(Invert, bool);
  vtkPointMapper *Owner;

protected:
  vtkImposterHelper();
  ~vtkImposterHelper() VTK_OVERRIDE;

  // Description:
  // Create the basic shaders before replacement
  void GetShaderTemplate(
    std::map<vtkShader::Type, vtkShader *> shaders,
    vtkRenderer *, vtkActor *) VTK_OVERRIDE;

  // Description:
  // Perform string replacments on the shader templates
  void ReplaceShaderValues(
    std::map<vtkShader::Type, vtkShader *> shaders,
    vtkRenderer *, vtkActor *) VTK_OVERRIDE;

  // Description:
  // Set the shader parameters related to the Camera
  void SetCameraShaderParameters(vtkOpenGLHelper &cellBO,
    vtkRenderer *ren, vtkActor *act) VTK_OVERRIDE;

  // Description:
  // Set the shader parameters related to the actor/mapper
  void SetMapperShaderParameters(vtkOpenGLHelper &cellBO,
    vtkRenderer *ren, vtkActor *act) VTK_OVERRIDE;

  // Description:
  // Does the VBO/IBO need to be rebuilt
  bool GetNeedToRebuildBufferObjects(vtkRenderer *ren,
    vtkActor *act) VTK_OVERRIDE;

  // Description:
  // Update the VBO to contain point based values
  void BuildBufferObjects(vtkRenderer *ren, vtkActor *act) VTK_OVERRIDE;

  void RenderPieceDraw(vtkRenderer *ren, vtkActor *act) VTK_OVERRIDE;

  bool  Invert;

private:
  vtkImposterHelper(const vtkImposterHelper&)
    VTK_DELETE_FUNCTION;
  void operator=(const vtkImposterHelper&) VTK_DELETE_FUNCTION;


  // Ensures the Colors array is set up, either from Scalars in input or
  // from the actor's color (via property).
  void SetColors();
};

//-----------------------------------------------------------------------------
vtkStandardNewMacro(vtkImposterHelper)

//-----------------------------------------------------------------------------
vtkImposterHelper::vtkImposterHelper()
{
  this->Owner = NULL;
  this->Invert = false;
}

//-----------------------------------------------------------------------------
vtkImposterHelper::~vtkImposterHelper()
{
}

//-----------------------------------------------------------------------------
void vtkImposterHelper::GetShaderTemplate(
  std::map<vtkShader::Type, vtkShader *> shaders,
  vtkRenderer *ren, vtkActor *actor)
{
  this->Superclass::GetShaderTemplate(shaders,ren,actor);
  shaders[vtkShader::Vertex]->SetSource(vtkSphereMapperVS);
}

void vtkImposterHelper::ReplaceShaderValues(
  std::map<vtkShader::Type, vtkShader *> shaders,
  vtkRenderer *ren, vtkActor *actor)
{
  std::string VSSource = shaders[vtkShader::Vertex]->GetSource();
  std::string FSSource = shaders[vtkShader::Fragment]->GetSource();

  vtkShaderProgram::Substitute(VSSource,
    "//VTK::Camera::Dec",
    "uniform mat4 VCDCMatrix;\n"
    "uniform mat4 MCVCMatrix;");

  vtkShaderProgram::Substitute(FSSource,
    "//VTK::PositionVC::Dec",
    "varying vec4 vertexVCVSOutput;");

  // we create vertexVC below, so turn off the default
  // implementation
  vtkShaderProgram::Substitute(FSSource,
    "//VTK::PositionVC::Impl",
    "vec4 vertexVC = vertexVCVSOutput;\n");

  // for lights kit and positional the VCDC matrix is already defined
  // so don't redefine it
  std::string replacement =
    "uniform float invertedDepth;\n"
    "uniform int cameraParallel;\n"
    "varying float radiusVCVSOutput;\n"
    "varying vec3 centerVCVSOutput;\n"
    "uniform mat4 VCDCMatrix;\n";
  vtkShaderProgram::Substitute(FSSource,"//VTK::Normal::Dec",replacement);

  vtkShaderProgram::Substitute(FSSource,"//VTK::Normal::Impl",
    // compute the eye position and unit direction
    "  vec3 EyePos;\n"
    "  vec3 EyeDir;\n"
    "  if (cameraParallel != 0) {\n"
    "    EyePos = vec3(vertexVC.x, vertexVC.y, vertexVC.z + 3.0*radiusVCVSOutput);\n"
    "    EyeDir = vec3(0.0,0.0,-1.0); }\n"
    "  else {\n"
    "    EyeDir = vertexVC.xyz;\n"
    "    EyePos = vec3(0.0,0.0,0.0);\n"
    "    float lengthED = length(EyeDir);\n"
    "    EyeDir = normalize(EyeDir);\n"
    // we adjust the EyePos to be closer if it is too far away
    // to prevent floating point precision noise
    "    if (lengthED > radiusVCVSOutput*3.0) {\n"
    "      EyePos = vertexVC.xyz - EyeDir*3.0*radiusVCVSOutput; }\n"
    "    }\n"

    // translate to Sphere center
    "  EyePos = EyePos - centerVCVSOutput;\n"
    // scale to radius 1.0
    "  EyePos = EyePos/radiusVCVSOutput;\n"
    // find the intersection
    "  float b = 2.0*dot(EyePos,EyeDir);\n"
    "  float c = dot(EyePos,EyePos) - 1.0;\n"
    "  float d = b*b - 4.0*c;\n"
    "  vec3 normalVCVSOutput = vec3(0.0,0.0,1.0);\n"
    "  if (d < 0.0) { discard; }\n"
    "  else {\n"
    "    float t = (-b - invertedDepth*sqrt(d))*0.5;\n"

    // compute the normal, for unit sphere this is just
    // the intersection point
    "    normalVCVSOutput = invertedDepth*normalize(EyePos + t*EyeDir);\n"
    // compute the intersection point in VC
    "    vertexVC.xyz = normalVCVSOutput*radiusVCVSOutput + centerVCVSOutput;\n"
    "    }\n"
    // compute the pixel's depth
    // " normalVCVSOutput = vec3(0,0,1);\n"
    "  vec4 pos = VCDCMatrix * vertexVC;\n"
    "  gl_FragDepth = (pos.z / pos.w + 1.0) / 2.0;\n"
    );

  if (ren->GetLastRenderingUsedDepthPeeling())
    {
    vtkShaderProgram::Substitute(FSSource,
      "//VTK::DepthPeeling::Impl",
      "float odepth = texture2D(opaqueZTexture, gl_FragCoord.xy/screenSize).r;\n"
      "  if (gl_FragDepth >= odepth) { discard; }\n"
      "  float tdepth = texture2D(translucentZTexture, gl_FragCoord.xy/screenSize).r;\n"
      "  if (gl_FragDepth <= tdepth) { discard; }\n"
      );
    }

  shaders[vtkShader::Vertex]->SetSource(VSSource);
  shaders[vtkShader::Fragment]->SetSource(FSSource);

  this->Superclass::ReplaceShaderValues(shaders,ren,actor);
}


//-----------------------------------------------------------------------------
void vtkImposterHelper::SetCameraShaderParameters(
  vtkOpenGLHelper &cellBO,
  vtkRenderer* ren, vtkActor *actor)
{
  vtkShaderProgram *program = cellBO.Program;

  vtkOpenGLCamera *cam = (vtkOpenGLCamera *)(ren->GetActiveCamera());

  vtkMatrix4x4 *wcdc;
  vtkMatrix4x4 *wcvc;
  vtkMatrix3x3 *norms;
  vtkMatrix4x4 *vcdc;
  cam->GetKeyMatrices(ren,wcvc,norms,vcdc,wcdc);
  if (program->IsUniformUsed("VCDCMatrix"))
    {
    program->SetUniformMatrix("VCDCMatrix", vcdc);
    }

  if (program->IsUniformUsed("MCVCMatrix"))
    {
    if (!actor->GetIsIdentity())
      {
      vtkMatrix4x4 *mcwc;
      vtkMatrix3x3 *anorms;
      ((vtkOpenGLActor *)actor)->GetKeyMatrices(mcwc,anorms);
      vtkMatrix4x4::Multiply4x4(mcwc, wcvc, this->TempMatrix4);
      program->SetUniformMatrix("MCVCMatrix", this->TempMatrix4);
      }
    else
      {
      program->SetUniformMatrix("MCVCMatrix", wcvc);
      }
    }

  if (program->IsUniformUsed("cameraParallel"))
    {
    cellBO.Program->SetUniformi("cameraParallel", cam->GetParallelProjection());
    }
}

//-----------------------------------------------------------------------------
void vtkImposterHelper::SetMapperShaderParameters(
  vtkOpenGLHelper &cellBO,
  vtkRenderer *ren, vtkActor *actor)
{
  if (cellBO.IBO->IndexCount && 
      (this->VBOBuildTime > cellBO.AttributeUpdateTime ||
       cellBO.ShaderSourceTime > cellBO.AttributeUpdateTime) &&
       cellBO.Program->IsAttributeUsed("offsetMC"))
    {
    cellBO.VAO->Bind();
    if (!cellBO.VAO->AddAttributeArray(cellBO.Program, this->VBO,
        "offsetMC", this->VBO->ColorOffset+sizeof(float),
        this->VBO->Stride, VTK_FLOAT, 2, false))
      {
      vtkErrorMacro(<< "Error setting 'offsetMC' in shader VAO.");
      }
    }

  if (cellBO.Program->IsUniformUsed("invertedDepth"))
    {
    cellBO.Program->SetUniformf("invertedDepth", this->Invert ? -1.0 : 1.0);
    }

  this->Superclass::SetMapperShaderParameters(cellBO,ren,actor);
}



//-----------------------------------------------------------------------------
namespace
{
// internal function called by CreateVBO
void vtkImposterHelperCreateVBO(
    float * points, vtkIdType numPts,
    unsigned char *colors, int colorComponents,
    float *sizes,
    vtkOpenGLVertexBufferObject *VBO)
{
  // Figure out how big each block will be, currently 6 or 7 floats.
  int blockSize = 3;
  VBO->VertexOffset = 0;
  VBO->NormalOffset = 0;
  VBO->TCoordOffset = 0;
  VBO->TCoordComponents = 0;
  VBO->ColorComponents = colorComponents;
  VBO->ColorOffset = sizeof(float) * blockSize;
  ++blockSize;

  // two more floats
  blockSize += 2;
  VBO->Stride = sizeof(float) * blockSize;

  // Create a buffer, and copy the data over.
  VBO->PackedVBO.resize(blockSize * numPts*3);
  std::vector<float>::iterator it = VBO->PackedVBO.begin();

  float *pointPtr;
  unsigned char *colorPtr;

  float cos30 = cos(vtkMath::RadiansFromDegrees(30.0));

  for (vtkIdType i = 0; i < numPts; ++i)
    {
    pointPtr = points + i*3;
    colorPtr = colors + i*colorComponents;
    float radius = sizes[i];

    // Vertices
    *(it++) = pointPtr[0];
    *(it++) = pointPtr[1];
    *(it++) = pointPtr[2];
    *(it++) = *reinterpret_cast<float *>(colorPtr);
    *(it++) = -2.0f*radius*cos30;
    *(it++) = -radius;

    *(it++) = pointPtr[0];
    *(it++) = pointPtr[1];
    *(it++) = pointPtr[2];
    *(it++) = *reinterpret_cast<float *>(colorPtr);
    *(it++) = 2.0f*radius*cos30;
    *(it++) = -radius;

    *(it++) = pointPtr[0];
    *(it++) = pointPtr[1];
    *(it++) = pointPtr[2];
    *(it++) = *reinterpret_cast<float *>(colorPtr);
    *(it++) = 0.0f;
    *(it++) = 2.0f*radius;
    }
  VBO->Upload(VBO->PackedVBO, vtkOpenGLBufferObject::ArrayBuffer);
  VBO->VertexCount = numPts*3;
  return;
}
}

//-------------------------------------------------------------------------
bool vtkImposterHelper::GetNeedToRebuildBufferObjects(
  vtkRenderer *ren, vtkActor *act)
{
  // picking state does not require a rebuild, unlike our parent
  if (this->VBOBuildTime < this->GetMTime() ||
      this->VBOBuildTime < act->GetMTime() ||
      this->VBOBuildTime < this->CurrentInput->GetMTime())
    {
    return true;
    }
  return false;
}

//-------------------------------------------------------------------------
void vtkImposterHelper::BuildBufferObjects(
  vtkRenderer *ren, vtkActor *act)
{
  vtkPolyData *poly = this->CurrentInput;

  if (poly == NULL)// || !poly->GetPointData()->GetNormals())
    {
    return;
    }
  
  if (this->ScalarVisibility && !this->Colors)
    {
    this->SetColors();
    }

  unsigned char *c;
  int nc = 3;
  if (!this->Colors)
    {
    vtkIdType np = poly->GetPoints()->GetNumberOfPoints();
    double dac[3];
    act->GetProperty()->GetColor(dac); 
    unsigned char ac[3];
    ac[0] = (unsigned char) (dac[0]*255.0);
    ac[1] = (unsigned char) (dac[1]*255.0);
    ac[2] = (unsigned char) (dac[2]*255.0);

    c = new unsigned char[np*3];
    for (vtkIdType i = 0; i < np; ++i)
      {
      c[3*i]   = ac[0];
      c[3*i+1] = ac[1];
      c[3*i+2] = ac[2];
      }
    nc = 3;
    }
  else
    {
    c = (unsigned char *)this->Colors->GetVoidPointer(0);
    nc = this->Colors->GetNumberOfComponents();
    }

  float *scales;
  bool hasScaleArray = this->Owner->GetScaleArray() != NULL &&
    poly->GetPointData()->HasArray(this->Owner->GetScaleArray());
 
  if (!hasScaleArray)
    {
    vtkIdType np = poly->GetPoints()->GetNumberOfPoints();
    scales = new float[np];
    for (vtkIdType i = 0; i < np; ++i)
      scales[i] = this->Owner->GetRadius();
    }
  else
    {
    scales = static_cast<float *>(poly->GetPointData()->GetArray(this->Owner->GetScaleArray())->GetVoidPointer(0));
    }

  // Iterate through all of the different types in the polydata, building
  //  OpenGLs and IBOs as appropriate for each type.
  vtkImposterHelperCreateVBO(
    static_cast<float *>(poly->GetPoints()->GetVoidPointer(0)),
    poly->GetPoints()->GetNumberOfPoints(),
    c,
    nc, 
    scales,
    this->VBO);

  if (!hasScaleArray)
    delete [] scales;
  if (!this->Colors)
    delete [] c;

  // create the IBO
  this->Points.IBO->IndexCount = 0;
  this->Lines.IBO->IndexCount = 0;
  this->TriStrips.IBO->IndexCount = 0;
  this->Tris.IBO->IndexCount = this->VBO->VertexCount;
  this->VBOBuildTime.Modified();
}



//-----------------------------------------------------------------------------
void vtkImposterHelper::RenderPieceDraw(vtkRenderer* ren, vtkActor *actor)
{
  // draw polygons
  if (this->Tris.IBO->IndexCount)
    {
    // First we do the triangles, update the shader, set uniforms, etc.
    this->UpdateShaders(this->Tris, ren, actor);
    glDrawArrays(GL_TRIANGLES, 0,
              static_cast<GLuint>(this->VBO->VertexCount));
    }
}


//-----------------------------------------------------------------------------
void
vtkImposterHelper::SetColors()
{
  vtkPolyData * input = this->GetInput();
  if ( this->Colors )
    {
    this->Colors->UnRegister(this);
    this->Colors = 0;
    }

  if (this->ScalarVisibility)
    {
    // Get rid of old colors
   vtkDataArray *colorsArray = input->GetPointData()->GetArray("Colors");

   if (colorsArray != NULL)
     {
     this->Colors = vtkUnsignedCharArray::SafeDownCast(colorsArray);
     this->Colors->Register(this);
     }
   else 
     {
     this->MapScalars(1.0);
     }
   }
}


// A helper class to draw points directly
class vtkPointHelper : public vtkOpenGLPolyDataMapper
{
public:
  static vtkPointHelper* New();
  vtkTypeMacro(vtkPointHelper, vtkOpenGLPolyDataMapper)

  vtkPointMapper *Owner;

protected:
  vtkPointHelper();
  ~vtkPointHelper() VTK_OVERRIDE;

private:
  vtkPointHelper(const vtkPointHelper&)
    VTK_DELETE_FUNCTION;
  void operator=(const vtkPointHelper&) VTK_DELETE_FUNCTION;
};

//-----------------------------------------------------------------------------
vtkStandardNewMacro(vtkPointHelper)

//-----------------------------------------------------------------------------
vtkPointHelper::vtkPointHelper()
{
}

//-----------------------------------------------------------------------------
vtkPointHelper::~vtkPointHelper()
{
}







//-----------------------------------------------------------------------------
vtkStandardNewMacro(vtkOpenGLPointMapper)

//-----------------------------------------------------------------------------
vtkOpenGLPointMapper::vtkOpenGLPointMapper()
{
  this->IHelper = vtkImposterHelper::New();
  this->IHelper->Owner = this;
  this->PHelper = vtkPointHelper::New();
  this->PHelper->Owner = this;
}

vtkOpenGLPointMapper::~vtkOpenGLPointMapper()
{
  this->IHelper->Delete();
  this->IHelper = 0;
  this->PHelper->Delete();
  this->PHelper = 0;
}

//----------------------------------------------------------------------------
void vtkOpenGLPointMapper::Render(vtkRenderer *ren, vtkActor *act)
{
  vtkProperty *prop = act->GetProperty();
  bool is_opaque = (prop->GetOpacity() >= 1.0);

  if (this->UseImposters)
    {
    // if we are transparent (and not backface culling) we have to draw twice
    if (!is_opaque && !prop->GetBackfaceCulling())
      {
      this->IHelper->SetInvert(true);
      this->Superclass::Render(ren,act);
      this->IHelper->SetInvert(false);
      }
    }
  this->Superclass::Render(ren,act);
}

void vtkOpenGLPointMapper::RenderPiece(vtkRenderer *ren, vtkActor *act)
{
  if (this->UseImposters)
    {
    if (this->GetMTime() > this->IHelperUpdateTime)
      {
      this->IHelper->vtkPolyDataMapper::ShallowCopy(this);
      this->IHelper->Modified();
      this->IHelperUpdateTime.Modified();
      }
    this->IHelper->RenderPiece(ren,act);
    }
  else 
    {
    if (this->GetMTime() > this->PHelperUpdateTime)
      {
      this->PHelper->vtkPolyDataMapper::ShallowCopy(this);
      this->PHelper->Modified();
      this->PHelperUpdateTime.Modified();
      }
    this->PHelper->RenderPiece(ren,act);
    }
}

//-----------------------------------------------------------------------------
void vtkOpenGLPointMapper::ReleaseGraphicsResources(vtkWindow* win)
{
  this->IHelper->ReleaseGraphicsResources(win);
  this->IHelper->SetInputData(0);
  this->PHelper->ReleaseGraphicsResources(win);
  this->PHelper->SetInputData(0);
  this->Modified();
}

//-----------------------------------------------------------------------------
void vtkOpenGLPointMapper::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
