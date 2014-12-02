#ifndef _PEER_H
#define _PEER_H

#include <set>
#include <map>
#include <list>
#include <vector>

#include "piece.h"
#include "neighbor.h"
#include "bandwidth.h"


typedef std::map<int, Neighbor> NeighborMap;
typedef std::list<PieceMsg> MsgList;


class Peer
{
  public:
    enum Type { SEED, LEECH, NORMAL };

    Peer();
    Peer(const Type type,
         const int pid,
         const int cid,
         const Bandwidth bw);

    void InitPieces(const Type type,
                    const int NUM_PIECE,
                    const double main_prob = 0.0);

  public:
    /* setter */
    void set_in_swarm(const bool st);
    void set_pid(const int pid);
    void set_cid(const int cid);
    void to_seed();
    void download_piece(const int n);
    void set_neighbors(NeighborMap const& ns);
    void clear_neighbors();
    void set_neighbor_counts(const int c);
    void incr_neighbor_counts(const int n);
    void set_join_time(const float t);
    void set_leave_time(const float t);

    void insert_on_req_peer(const int pid);
    void erase_on_req_peer(const int pid);
    void push_recv_msg(PieceMsg const& msg);
    void sort_recv_msg();
    void erase_recv_msg(MsgList::iterator it);
    void pop_recv_msg();

    void destroy_pieces();

    /* getter */
    int get_pid() const;
    int get_cid() const;
    int get_neighbor_counts() const;
    float get_trans_time() const;
    float get_neighbor_pgdelay(const int nid) const;
    float get_join_time() const;
    float get_leave_time() const;
    bool check_in_swarm() const;
    bool* get_piece_info() const;
    bool get_nth_piece_info(const int n) const;
    Type get_type() const;
    NeighborMap const& get_neighbors() const;
    Neighbor const& get_nth_neighbor(const int n) const;
    Bandwidth const& get_bandwidth() const;
    std::set<int> const& get_on_req_peer_set() const;
    std::list<PieceMsg> const& get_recv_msg_buf() const;


  private:
    bool in_swarm;

    int pid;
    int cid;
    Type type;

    int neighbor_counts;  // counts of peer being served(selected) as neighbor

    bool* pieces;
    NeighborMap neighbors;  // <nid, nei_info>
    //Neighbor* neighbors;

    std::set<int> on_req_peer_set;
    MsgList recv_msg_buf;

    Bandwidth bandwidth;

    float join_time;     // start time of peer run the routine
    float leave_time;      // end time of all pieces have been downloaded
};

//extern Peer* g_peers;
extern std::vector<Peer> g_peers;
extern bool* g_in_swarm_set;

#endif // for #ifndef _PEER_H
