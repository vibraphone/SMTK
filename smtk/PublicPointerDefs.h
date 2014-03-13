/*=========================================================================

Copyright (c) 1998-2012 Kitware Inc. 28 Corporate Drive,
Clifton Park, NY, 12065, USA.

All rights reserved. No part of this software may be reproduced, distributed,
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
// .NAME PublicPointerDefs.h -
// .SECTION Description
// .SECTION See Also

#ifndef __smtk_PublicPointerDefs_h
#define __smtk_PublicPointerDefs_h

#include "smtk/SharedPtr.h"
#include "smtk/util/SystemConfig.h"
#include <set>
#include <vector>

namespace cgmsmtk
{
  namespace cgm
  {
    class Bridge;
    typedef smtk::shared_ptr<cgmsmtk::cgm::Bridge> BridgePtr;
    class ImportSolid;
    class Engines;
    class ExportSolid;
  }
}

namespace smtk
{
  namespace model
  {
    class Arrangement;
    class ArrangementReference;
    typedef std::vector<ArrangementReference> ArrangementReferences;
    class AttributeListPhrase;
    typedef std::vector<Arrangement> Arrangements;
    class BRepModel;
    class BridgeBase;
    class CellEntity;
    class Chain;
    typedef std::vector<Chain> Chains;
    class Cursor;
    typedef std::set<Cursor> Cursors;
    typedef std::vector<Cursor> CursorArray;
    class DescriptivePhrase;
    class Edge;
    typedef std::vector<Edge> Edges;
    class EdgeUse;
    typedef std::vector<EdgeUse> EdgeUses;
    class EntityPhrase;
    class EntityListPhrase;
    class Face;
    typedef std::vector<Face> Faces;
    class FaceUse;
    typedef std::vector<FaceUse> FaceUses;
    class ExportJSON;
    class GridInfo;
    class GridInfo2D;
    class GridInfo3D;
    class GroupEntity;
    class GroupItem;
    class InstanceEntity;
    class Item;
    class ImportJSON;
    class Loop;
    typedef std::vector<Loop> Loops;
    class Entity;
    class Model;
    class ModelEntity;
    class Operator;
    class OperatorResult;
    class Parameter;
    class PropertyValuePhrase;
    class PropertyListPhrase;
    class Shell;
    typedef std::vector<Shell> Shells;
    class ShellEntity;
    typedef std::vector<ShellEntity> ShellEntities;
    class Storage;
    class SimpleModelSubphrases;
    class SubphraseGenerator;
    class ModelDomainItem;
    class Tessellation;
    class UseEntity;
    typedef std::vector<UseEntity> UseEntities;
    class Vertex;
    typedef std::vector<Vertex> Vertices;
    class VertexUse;
    typedef std::vector<VertexUse> VertexUses;
    class Volume;
    typedef std::vector<Volume> Volumes;
    class VolumeUse;
    typedef std::vector<VolumeUse> VolumeUses;
  }

  namespace attribute
  {
    class Attribute;
    class RefItem;
    class RefItemDefinition;
    class Definition;
    class DirectoryItem;
    class DirectoryItemDefinition;
    class DoubleItem;
    class DoubleItemDefinition;
    class FileItem;
    class FileItemDefinition;
    class GroupItem;
    class GroupItemDefinition;
    class IntItem;
    class IntItemDefinition;
    class Item;
    class ItemDefinition;
    class Manager;
    class StringItem;
    class StringItemDefinition;
    class ValueItem;
    class ValueItemDefinition;
    class VoidItem;
    class VoidItemDefinition;
  }

  namespace util
  {
    class Resource;
    class UserData;
    class UUID;
    class UUIDGenerator;
    typedef std::set<UUID> UUIDs;
    typedef std::vector<UUID> UUIDArray;
  }

  namespace view
  {
    class Attribute;
    class Base;
    class Group;
    class Instanced;
    class ModelEntity;
    class Root;
    class SimpleExpression;
  };

  //Shiboken requires that we use fully qualified namespaces for all
  //types that these shared_ptr and weak_ptr are holding
  namespace model
  {
    // Model Related Pointer Classes
    typedef smtk::shared_ptr< smtk::model::BridgeBase >            BridgeBasePtr;
    typedef smtk::shared_ptr< smtk::model::DescriptivePhrase >     DescriptivePhrasePtr;
    typedef smtk::weak_ptr< smtk::model::DescriptivePhrase >       WeakDescriptivePhrasePtr;
    typedef smtk::shared_ptr< smtk::model::EntityPhrase >          EntityPhrasePtr;
    typedef smtk::shared_ptr< smtk::model::EntityListPhrase >      EntityListPhrasePtr;
    typedef smtk::shared_ptr< smtk::model::PropertyValuePhrase >   PropertyValuePhrasePtr;
    typedef smtk::shared_ptr< smtk::model::PropertyListPhrase >    PropertyListPhrasePtr;
    typedef smtk::shared_ptr< smtk::model::SimpleModelSubphrases > SimpleModelSubphrasesPtr;
    typedef smtk::shared_ptr< smtk::model::SubphraseGenerator >    SubphraseGeneratorPtr;
    typedef smtk::shared_ptr< smtk::model::Model >                 ModelPtr;
    typedef smtk::weak_ptr< smtk::model::Model >                   WeakModelPtr;
    typedef smtk::shared_ptr< smtk::model::Operator >              OperatorPtr;
    typedef smtk::weak_ptr< smtk::model::Operator >                WeakOperatorPtr;
    typedef smtk::shared_ptr< smtk::model::Storage >               StoragePtr;
    typedef smtk::weak_ptr< smtk::model::Storage >                 WeakStoragePtr;
    typedef smtk::shared_ptr< smtk::model::Item >                  ItemPtr;
    typedef smtk::weak_ptr< smtk::model::Item >                    WeakItemPtr;
    typedef smtk::shared_ptr< smtk::model::GroupItem >             GroupItemPtr;
    typedef smtk::shared_ptr< smtk::model::Entity >                EntityPtr;
    typedef smtk::weak_ptr< smtk::model::Entity >                  WeakEntityPtr;
    typedef smtk::shared_ptr< smtk::model::Arrangement >           ArrangementPtr;
    typedef smtk::weak_ptr< smtk::model::Arrangement >             WeakArrangementPtr;
    typedef smtk::shared_ptr< smtk::model::Tessellation >          TessellationPtr;
    typedef smtk::weak_ptr< smtk::model::Tessellation >            WeakTessellationPtr;

    // class for making the analysis grid information available in SMTK
    typedef smtk::shared_ptr< smtk::model::GridInfo >          GridInfoPtr;
    typedef smtk::shared_ptr< smtk::model::GridInfo2D >        GridInfo2DPtr;
    typedef smtk::shared_ptr< smtk::model::GridInfo3D >        GridInfo3DPtr;

    // definition of the mask type for model entities. enum is in smtk/model/Item.h
    typedef unsigned long                                      MaskType;
  };

  namespace attribute
  {
    // Attribute Related Pointer Classes
    typedef smtk::shared_ptr< smtk::attribute::Definition >       DefinitionPtr;
    typedef smtk::shared_ptr< const smtk::attribute::Definition > ConstDefinitionPtr;
    typedef smtk::weak_ptr< smtk::attribute::Definition >         WeakDefinitionPtr;
    typedef smtk::shared_ptr< smtk::attribute::Attribute >        AttributePtr;
    typedef smtk::weak_ptr< smtk::attribute::Attribute >          WeakAttributePtr;

    typedef smtk::shared_ptr< smtk::attribute::RefItem >           RefItemPtr;
    typedef smtk::shared_ptr< smtk::attribute::RefItemDefinition > RefItemDefinitionPtr;

    typedef smtk::shared_ptr< smtk::attribute::Item >                 ItemPtr;
    typedef smtk::shared_ptr< const smtk::attribute::Item >           ConstItemPtr;
    typedef smtk::weak_ptr< smtk::attribute::Item >                   WeakItemPtr;
    typedef smtk::shared_ptr< smtk::attribute::ItemDefinition >       ItemDefinitionPtr;
    typedef smtk::shared_ptr< const smtk::attribute::ItemDefinition > ConstItemDefinitionPtr;
    typedef smtk::weak_ptr< smtk::attribute::ItemDefinition >         WeakItemDefinitionPtr;

    typedef smtk::shared_ptr< smtk::attribute::ValueItem >            ValueItemPtr;
    typedef smtk::shared_ptr< smtk::attribute::ValueItemDefinition >  ValueItemDefinitionPtr;

    typedef smtk::shared_ptr< smtk::attribute::DirectoryItem >            DirectoryItemPtr;
    typedef smtk::shared_ptr< smtk::attribute::DirectoryItemDefinition >  DirectoryItemDefinitionPtr;
    typedef smtk::shared_ptr< smtk::attribute::DoubleItem >               DoubleItemPtr;
    typedef smtk::shared_ptr< smtk::attribute::DoubleItemDefinition >     DoubleItemDefinitionPtr;
    typedef smtk::shared_ptr< smtk::attribute::FileItem >                 FileItemPtr;
    typedef smtk::shared_ptr< smtk::attribute::FileItemDefinition >       FileItemDefinitionPtr;
    typedef smtk::shared_ptr< smtk::attribute::GroupItem >                GroupItemPtr;
    typedef smtk::shared_ptr< smtk::attribute::GroupItemDefinition >      GroupItemDefinitionPtr;
    typedef smtk::shared_ptr< smtk::attribute::IntItem >                  IntItemPtr;
    typedef smtk::shared_ptr< smtk::attribute::IntItemDefinition >        IntItemDefinitionPtr;
    typedef smtk::shared_ptr< smtk::attribute::StringItem >               StringItemPtr;
    typedef smtk::shared_ptr< smtk::attribute::StringItemDefinition >     StringItemDefinitionPtr;
    typedef smtk::shared_ptr< smtk::attribute::VoidItem >                 VoidItemPtr;
    typedef smtk::shared_ptr< smtk::attribute::VoidItemDefinition >       VoidItemDefinitionPtr;

    // Note used by SMTK but added for completeness
    typedef smtk::shared_ptr< smtk::attribute::Manager >   ManagerPtr;
  };



  namespace util
  {
    //custom user data classes
    typedef smtk::shared_ptr< smtk::util::Resource > ResourcePtr;
    typedef smtk::shared_ptr< smtk::util::UserData > UserDataPtr;
  };

  namespace view
  {
  // View Related Pointer Classes
    typedef smtk::shared_ptr< smtk::view::Base >             BasePtr;
    typedef smtk::weak_ptr< smtk::view::Base >               WeakBasePtr;
    typedef smtk::shared_ptr< smtk::view::Attribute>         AttributePtr;
    typedef smtk::shared_ptr< smtk::view::Group>             GroupPtr;
    typedef smtk::shared_ptr< smtk::view::Instanced>         InstancedPtr;
    typedef smtk::shared_ptr< smtk::view::ModelEntity>       ModelEntityPtr;
    typedef smtk::shared_ptr< smtk::view::Root>              RootPtr;
    typedef smtk::shared_ptr< smtk::view::SimpleExpression>  SimpleExpressionPtr;
  };


#ifdef smtk_has_owner_less
  //special map and set typedefs for better safety with sets of weak pointers
  //since sets of weak pointers can be dangerous.
  namespace attribute
  {
    typedef std::set< attribute::WeakAttributePtr,
      smtk::owner_less<attribute::WeakAttributePtr > >        WeakAttributePtrSet;
    typedef std::set< attribute::WeakDefinitionPtr,
      smtk::owner_less< attribute::WeakDefinitionPtr > >      WeakDefinitionPtrSet;
    typedef std::set< attribute::WeakItemDefinitionPtr,
      smtk::owner_less< attribute::WeakItemDefinitionPtr > >  WeakItemDefinitionPtrSet;
    typedef std::set< attribute::WeakItemPtr,
      smtk::owner_less< attribute::WeakItemPtr > >   WeakItemPtrSet;
  };

  namespace model
  {
    typedef std::set< WeakItemPtr,
      smtk::owner_less<WeakItemPtr > >        WeakItemPtrSet;
    typedef std::set< WeakModelPtr,
      smtk::owner_less<WeakModelPtr > >       WeakModelPtrSet;
  };

  namespace view
  {
    typedef std::set< view::WeakBasePtr,
      smtk::owner_less<view::WeakBasePtr > >            WeakViewPtrSet;
  };

#else
  //we can use less than operator
  namespace attribute
  {
    typedef std::set< attribute::WeakAttributePtr  >      WeakAttributePtrSet;
    typedef std::set< attribute::WeakDefinitionPtr  >     WeakDefinitionPtrSet;
    typedef std::set< attribute::WeakItemDefinitionPtr >  WeakItemDefinitionPtrSet;
    typedef std::set< attribute::WeakItemPtr  >           WeakItemPtrSet;
  };

  namespace model
  {
    typedef std::set< model::WeakItemPtr  >               WeakItemPtrSet;
    typedef std::set< model::WeakModelPtr  >              WeakModelPtrSet;
  };

  namespace view
  {
    typedef std::set< view::WeakBasePtr  >              WeakBasePtrSet;
  };
#endif

  // These are used internally by SMTK
  namespace internal
  {
    template <typename T >
    struct is_shared_ptr
    {
      enum {type=false};
    };
    template<typename T >
    struct is_shared_ptr< smtk::shared_ptr< T >  >
    {
      enum{type=true};
    };

    template<typename T, int Enabled = is_shared_ptr< T >::type  >
    struct shared_ptr_type
    {
      typedef smtk::shared_ptr< T > SharedPointerType;
      typedef T RawPointerType;
    };

    template<typename T >
    struct shared_ptr_type<T,true >
    {
      typedef T SharedPointerType;
      typedef typename T::element_type RawPointerType;
    };

  }
}
#endif /* __smtk_PublicPointerDefs_h */
