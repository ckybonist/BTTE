#ifndef _PIECE_SELECTION_H
#define _PIECE_SELECTION_H

#include <iostream>
#include <map>
#include <set>
#include <vector>

#include "../args.h"
#include "../random.h"
#include "../peer.h"
#include "../neighbor.h"


namespace btte_piece_selection
{

typedef std::set<int> IntSet;
typedef IntSet::iterator IntSetIter;

typedef enum class TypePieceSelect
{
    RANDOM,
    RAREST_FIRST,
    USER_DEFINED
} PieceSelect_t;

class IPieceSelection
{
  public:
    IPieceSelection();
    virtual ~IPieceSelection();
    virtual MsgList StartSelection(const int client_pid) = 0;

  protected:
    typedef std::vector<int> IntVec;
    typedef std::map<int, IntVec> I2IVecMap;

    void CheckNeighborExist();
    void SetCandidates();
    bool HavePiece(const int pid, const int piece_no) const;
    //MsgList GetUndupDestReqMsgs(IntSet const& target_pieces, const int client_pid);
    MsgList GetUndupDestReqMsgs(std::vector<int> const& target_pieces, const int client_pid);

    void Debug(MsgList const& req_msgs, IntVec rf_candidates);

    IntVec candidates_;
    int selector_pid_;

  private:
    void CheckNeighborAlive();
    bool IsDupDest(const IntSet& dest_peers,
                   const int nid);
    IntSet GetPieceOwners(const int piece_no, const int client_pid);

    //IntVec GetNonReqPieceOwners(const int no, const int client_pid);
    IntSet GetNonReqPieceOwners(const int no, const int client_pid);

    PieceMsg MakeMsg(const int piece_no, const int dest_pid);

    std::map<int, IntSet> GetPieceOwnersMap(IntSet const& target_pieces, const int client_pid);
};

}

#endif // for #ifndef _PIECE_SELECTION_H
