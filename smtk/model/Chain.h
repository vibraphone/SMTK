#ifndef __smtk_model_Chain_h
#define __smtk_model_Chain_h

#include "smtk/model/ShellEntity.h"

namespace smtk {
  namespace model {

class Edge;
class Chain;
class VertexUse;
typedef std::vector<Chain> Chains;
typedef std::vector<VertexUse> VertexUses;

/**\brief A cursor subclass with methods specific to vertex-chains.
  *
  * A chain is a collection of vertex-uses that form a
  * subset of the boundary of an edge cell --
  * and usually that subset is the entire boundary.
  * In theory, a chain may contain other chains --
  * in which case the child chains would act as holes
  * leaving the edge interrupted.
  * In practice this should be an exceptional event,
  * and you should not rely on SMTK to support this
  * convention.
  */
class SMTKCORE_EXPORT Chain : public ShellEntity
{
public:
  SMTK_CURSOR_CLASS(Chain,ShellEntity,isChain);

  Edge edge() const;
  VertexUses vertexUses() const;
  Chain containingChain() const;
  Chains containedChains() const;
};

  } // namespace model
} // namespace smtk

#endif // __smtk_model_Chain_h