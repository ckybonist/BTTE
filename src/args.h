#ifndef _ARGS_H
#define _ARGS_H

#include <string>

class Args
{
  public:
    Args() {};
    void InitArgs(const std::string filename);

    /* getter */
    size_t get_num_peer() const;
    size_t get_num_seed() const;
    size_t get_num_leech() const;
    size_t get_num_peerlist() const;
    size_t get_num_choking() const;
    size_t get_num_ou() const;
    size_t get_num_piece() const;
    int get_type_peerselect() const;
    int get_type_pieceselect() const;
    float get_arrival_rate() const;

  private:
    size_t NUM_PEER;
    size_t NUM_SEED;
    size_t NUM_LEECH;
    size_t NUM_PEERLIST;
    size_t NUM_CHOKING;
    size_t NUM_OU;
    size_t NUM_PIECE;
    int TYPE_PEERSELECT;
    int TYPE_PIECESELECT;
    float ARRIVAL_RATE;
};

extern Args g_btte_args;

#endif // for #ifndef _ARGS_H
