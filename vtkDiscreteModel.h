/*=========================================================================

Copyright (c) 1998-2005 Kitware Inc. 28 Corporate Drive, Suite 204,
Clifton Park, NY, 12065, USA.

All rights reserved. No part of this software may be reproduced,
distributed,
or modified, in any form or by any means, without permission in writing from
Kitware Inc.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES,
INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
"AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO
PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

=========================================================================*/
// .NAME vtkDiscreteModel - CMB's implementation of a vtkModel.
// .SECTION Description
// The implementation of vtkModel for discrete models.
// The server and the client
// will both have a vtkDiscreteModel with the one on the server having
// vtkPolyDatas for GEOMETRY, otherwise they are identical.  Use
// vtkDiscreteModelWrapper to access the model on the server from the
// client.

#ifndef __vtkDiscreteModel_h
#define __vtkDiscreteModel_h

#include "Model/vtkModel.h"

class vtkCharArray;
class vtkModelGridRepresentation;
class vtkModelMaterial;
class vtkDiscreteModelEdge;
class vtkDiscreteModelEntity;
class vtkDiscreteModelEntityGroup;
class vtkDiscreteModelFace;
class vtkDiscreteModelGeometricEntity;
class vtkModelNodalGroup;
class vtkModelUniqueNodalGroup;
class vtkIdTypeArray;
class vtkInformationDataObjectKey;
class vtkIntArray;
class vtkModelVertex;
class vtkModelVertexUse;

//BTX
#include <string>
enum vtkDiscreteModelEntityTypes
{
  vtkDiscreteModelEntityGroupType = 100,
  vtkModelMaterialType,
  vtkModelNodalGroupType
};

// Description:
// All the currently defined CMB Model events are listed here.
enum DiscreteModelEventIds {
  ModelEntityGroupCreated = 10000,
  ModelEntityGroupAboutToDestroy,
  ModelEntityGroupDestroyed,
  NodalGroupCreated,
  NodalGroupAboutToDestroy,
  NodalGroupDestroyed,
  DomainSetCreated,
  DomainSetAboutToDestroy,
  DomainSetDestroyed
};
//ETX

class VTK_EXPORT vtkDiscreteModel : public vtkModel
{
public:
  static vtkDiscreteModel* New();

//BTX
  vtkTypeRevisionMacro(vtkDiscreteModel,vtkModel);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Functions to get the vtkDiscreteModelGeometricEntity object that CellId
  // is classified on and the cell index of the copy of CellId in that
  // vtkPolyData.  Note that these should only be called on the server.
  vtkDiscreteModelGeometricEntity* GetCellModelGeometricEntity(vtkIdType cellId);
  vtkIdType GetCellModelGeometricEntityIndex(vtkIdType cellId);

  // Description:
  // Get the geometric representation of the model. Returns null on the
  // client.
  vtkObject* GetGeometry();

  // Description:
  // Get the name of the array that stores the point mapping information.
  // We want to rename it so that we have a unique name for it.
  static const char* GetPointMapArrayName();

  // Description:
  // Get the name of the arrays that stores the cell mapping information.
  // We want to rename it so that we have a unique name for it.
  static const char* GetCellMapArrayName();
  static const char* GetCanonicalSideArrayName();


  // Description:
  // Function to clear out the current model data.  This must be called
  // in order for a model to be deleted since this removes the
  // associations to the model of objects aggregated by the model.
  // NOTE: This function should be called before call Delete, otherwise,
  // there might be leaks due to model associations.
  virtual void Reset();

  // Description:
  // Build a model entity. The model is responsible for the management
  // of the built model entity.  The build model entity is deleted
  // from the model with the corresponding DestroyModel function.
  virtual vtkModelVertex* BuildModelVertex(vtkIdType pointId);
  virtual vtkModelVertex* BuildModelVertex(vtkIdType pointId,
                                           vtkIdType vertexId);
  virtual vtkModelEdge* BuildModelEdge(vtkModelVertex* vertex0,
                                       vtkModelVertex* vertex1);

  virtual vtkModelEdge* BuildModelEdge(
    vtkModelVertex* vertex0, vtkModelVertex* vertex1, vtkIdType edgeId);

  virtual vtkModelFace* BuildModelFace(int numEdges, vtkModelEdge** edges,
                                       int* edgeDirections, vtkModelMaterial* material);
  virtual vtkModelFace* BuildModelFace(int numEdges, vtkModelEdge** edges,
                                       int* edgeDirections);
  virtual vtkModelFace* BuildModelFace(int NumEdges, vtkModelEdge** Edges,
                                       int* edgeDirections,
                                       vtkIdType modelFaceId);
  virtual vtkModelRegion* BuildModelRegion();
  virtual vtkModelRegion* BuildModelRegion(vtkIdType modelRegionId);
  virtual vtkModelRegion* BuildModelRegion(
    int numFaces, vtkModelFace** faces, int* faceSides);
  virtual vtkModelRegion* BuildModelRegion(
    int numFaces, vtkModelFace** faces, int* faceSides,
    vtkIdType modelRegionId);
  virtual vtkModelRegion* BuildModelRegion(
    int numFaces, vtkModelFace** faces, int* faceSides,
    vtkModelMaterial* material);
  virtual vtkModelRegion* BuildModelRegion(
    int numFaces, vtkModelFace** faces, int* faceSides,
    vtkIdType modelRegionId, vtkModelMaterial* material);

  virtual bool DestroyModelGeometricEntity(
    vtkDiscreteModelGeometricEntity* geomEntity);

  virtual void GetBounds(double bounds[6]);

  virtual vtkModelMaterial* BuildMaterial();
  virtual vtkModelMaterial* BuildMaterial(vtkIdType id);

  // Description:
  // Remove an existing material from the model.  Note that no model entities
  // should be associated with this material.
  virtual bool DestroyMaterial(vtkModelMaterial* material);

  // Description:
  // Build a vtkDiscreteModelEntityGroup and initialize it with some some objects.
  virtual vtkDiscreteModelEntityGroup* BuildModelEntityGroup(
    int itemType, int numEntities, vtkDiscreteModelEntity** entities);
  virtual vtkDiscreteModelEntityGroup* BuildModelEntityGroup(
    int itemType, int numEntities, vtkDiscreteModelEntity** entities, vtkIdType id);

  // Description:
  // Destroy EntityGroup.  Returns true if successful.
  virtual bool DestroyModelEntityGroup(vtkDiscreteModelEntityGroup* entityGroup);

  // Description:
  // Build a vtkModelNodalGroup and populate it with the vtkPoint Ids
  // in PointIds. If Type is 0/BASE_NODAL_GROUP a vtkModelNodalGroup will
  // be created and if Type is 1/UNIQUE_NODAL_GROUP a vtkModelUniqueNodalGroup
  // will be created. Id is the unique persistent Id to be assigned
  // to the nodal group.
  virtual vtkModelNodalGroup* BuildNodalGroup(int type, vtkIdList* pointIds);
  virtual vtkModelNodalGroup* BuildNodalGroup(int type, vtkIdList* pointIds,
                                            vtkIdType id);

  // Description:
  // Destroy EntityGroup.  Returns true if successful.
  virtual bool DestroyNodalGroup(vtkModelNodalGroup* nodalGroup);

  // Description:
  // Build/Destroy a floating vtkDiscreteModelEdge.
  virtual vtkModelEdge* BuildFloatingRegionEdge(
    double point1[3], double point2[3],
    int resolution, vtkIdType regionId)
    {return this->BuildFloatingRegionEdge(this->GetNextUniquePersistentId(),
      point1, point2, resolution, regionId);}
  virtual vtkModelEdge* BuildFloatingRegionEdge(vtkIdType edgeId,
    double point1[3], double point2[3],
    int resolution, vtkIdType regionId);
  virtual bool DestroyModelEdge(vtkDiscreteModelEdge* modelEdge);

  vtkGetMacro(AnalysisGridInfo, vtkModelGridRepresentation*);
  void SetAnalysisGridInfo(vtkModelGridRepresentation*);

  // Description:
  // Reads the state of an instance from an archive OR
  // writes the state of an instance to an archive. See
  // the documentation for this class for details.
  virtual void Serialize(vtkSerializer* ser);

  // Description:
  // Static function for getting the information object
  // used to (optionally) store the mapping from
  // entities in this grid to another grid.
  static vtkInformationObjectBaseKey* POINTMAPARRAY();
  static vtkInformationObjectBaseKey* CELLMAPARRAY();

  // Description:
  // Flag to whether invoke event
  vtkSetMacro(BlockEvent, bool);
  vtkGetMacro(BlockEvent, bool);
  vtkBooleanMacro(BlockEvent, bool);

protected:
  vtkDiscreteModel();
  virtual ~vtkDiscreteModel();

  // Description:
  // Function to get the next default name for a model entity.  It
  // is used for giving a default value when creating a model entity
  // that the user can change.
  void GetModelEntityDefaultName(int entityType, const char* baseName,
                                 std::string & defaultEntityName);

  // Description:
  // Function to set the vtkDiscreteModelGeometricEntity object and Geometric
  // Entity grid Id (GeomeEntityCellId) that MasterCellId is classified on.
  // This does not set the reverse classification information though.
  // Note that this should only be called on the server as it won't do
  // anything on the client.
  void SetCellClassification(vtkIdType masterCellId,vtkIdType geomEntityCellId,
                             vtkDiscreteModelGeometricEntity* geomEntity);
//BTX
  friend class vtkDiscreteModelGeometricEntity;
  friend class vtkCMBParserBase;
  friend class vtkDiscreteModelFace;
  friend class vtkDiscreteModelWrapper;
  friend class vtkCMBModelBuilder;
  friend class vtkDiscreteModelEdge;
  friend class CmbSceneBuilderCore;
  friend class CmbGeologyBuilderCore;
  friend class vtkCmbMapToCmbModel;
  friend class vtkModelUniqueNodalGroup;
  friend class vtkCmbIncorporateMeshOperator;
//ETX

  // Description:
  // Set/get the vtkCMBUniqueNodalGroups that a point is assigned to.
  // For setting, it removes the point from its current vtkCMBUniqueNodalGroups
  // if it belongs to one.
  void SetPointUniqueNodalGroup(vtkModelUniqueNodalGroup* nodalGroup,
                                vtkIdType pointId);
  vtkModelUniqueNodalGroup* GetPointUniqueNodalGroup(vtkIdType pointId);

  // Description:
  // Set the vtkObject that is used to represent the Geometry.  This should
  // only be called on the server. Note: Sometimes UpdateGeometry() needs to
  // be called if the master polydata is modified.
  void SetGeometry(vtkObject* geometry);
  void UpdateGeometry();

  // Description:
  // The mappings from a cell on the master geometry to the geometric model
  // entity it is classified on (CellClassification) as well as the index
  // of the cell on the geometric model entity geometric representation.
  std::vector<vtkDiscreteModelGeometricEntity*> CellClassification;
  std::vector<vtkIdType> ClassifiedCellIndex;

  // Description:
  // The vector of vtkCMBUniqueNodalGroups that grid points are assigned to.
  // A point can be assigned to at most one vtkCMBUniqueNodalGroups but
  // is not required to be assigned to any.
  std::vector<vtkModelUniqueNodalGroup*> UniquePointGroup;

  // Description:
  // The bounds of the model; set (on the server) when doing a SetGeometry,
  // and then passed to the client during serialization
  double ModelBounds[6];

  // Description:
  // Object to get analysis grid information.  Currently used for
  // model entity and nodal groups.
  vtkModelGridRepresentation* AnalysisGridInfo;

  // Description:
  // Flag to whether invoke event
  bool BlockEvent;
  void InternalInvokeEvent(unsigned long event, void *callData);

private:
  vtkDiscreteModel(const vtkDiscreteModel&);  // Not implemented.
  void operator=(const vtkDiscreteModel&);  // Not implemented.
//ETX
};

#endif
