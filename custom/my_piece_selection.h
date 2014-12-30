#include "../src/algorithm/piece_selection.h"


using namespace btte_piece_selection;


class MyPieceSelection : public IPieceSelection
{
    public:
        MyPieceSelection() : IPieceSelection() {};
        ~MyPieceSelection();

    private:
        IntSet StartSelection(const int client_pid) override;
        /* Declare By Yourself */
};
