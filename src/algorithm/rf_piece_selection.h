#ifndef _RF_PIECE_SELECTION_H
#define _RF_PIECE_SELECTION_H

#include "piece_selection.h"


namespace btte_piece_selection
{

/*
 * Current Implementation for example:
 *
 * Suppose a peer need these pieces: {1, 2, 3, 4, 5, 6, 7},
 *
 * and each piece own by number-of-neighbors (represent as count):
 * <piece>    <count>
 * 1          3
 * 2          4
 * 3          4
 * 4          4
 * 5          5
 * 6          5
 * 7          5
 *
 * Then it will first select piece #1 which have least count,
 * then choose the second less (count is 4), but piece #{2,3,4}
 * have same count, so we randomly choose one. Finally, we randomly
 * choose one piece from {5,6,7}.
 *
 * So the piece selection result will probably be pieces #{1, 3, 6}.
 *
 * */
class RarestFirst : public IPieceSelection
{
  public:
    RarestFirst() : IPieceSelection() {};
    ~RarestFirst();

  private:
    typedef struct PieceOwnersCount
    {
        int piece_no;
        int count;
    } POC;

    //IntSet StartSelection(const int client_pid) override;
    MsgList StartSelection(const int client_pid) override;
    MsgList GenrMsgs();

    void ProcessCountInfo();
    void SetCountInfo();
    int CountNumPeerOwnPiece(const int piece_no);
    void ShuffleCountInfo(const RSC rsc, POC *arr);
    void SortByPieceCounts();
    void RefreshInfo();

    POC* count_info_;
};

}

#endif // for #ifndef _RF_PIECE_SELECTION_H
