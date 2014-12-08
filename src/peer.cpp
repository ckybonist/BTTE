#include "error.h"
#include "random.h"
#include "piece.h"
#include "peer.h"


std::vector<Peer> g_peers;
bool* g_peers_reg_info = nullptr;
//Peer *g_peers = nullptr;


Peer::Peer()
{
    in_swarm = false;
    pid = -1;
    cid = -1;
    pieces = nullptr;
    //neighbors = nullptr;
    join_time = 0.0;
    leave_time = 0.0;
    neighbor_counts = 0;
}

Peer::Peer(const Type type,
           const int pid,
           const int cid,
           const Bandwidth bandwidth)
{
    this->type = type;
    this->pid = pid;
    this->cid = cid;
    this->bandwidth = bandwidth;

    join_time = 0.0;
    complete_time = 0.0;
    leave_time = 0.0;

    neighbor_counts = 0;

    pieces = nullptr;

    //neighbors = nullptr;
    in_swarm = true;
}

/* setter */
void Peer::set_in_swarm(const bool st)       { in_swarm = st; }
void Peer::set_pid(const int pid)            { this->pid = pid; }
void Peer::set_cid(const int cid)            { this->cid = cid; }
void Peer::to_seed()                         { type = SEED; }
void Peer::download_piece(const int n)       { pieces[n] = true; }
void Peer::set_neighbor_counts(const int c)  { neighbor_counts = c; }
void Peer::incr_neighbor_counts(const int n) { neighbor_counts += n; }
void Peer::set_join_time(const float t)      { join_time = t; }
void Peer::set_complete_time(const float t)  { complete_time = t; }
void Peer::set_leave_time(const float t)     { leave_time = t; }

void Peer::set_neighbors(NeighborMap const& ns)
{
    //if (n == nullptr) { ExitError("No neighbors, It's NULL"); }
    if (ns.size() == 0) { ExitError("No neighbors, It's empty"); }
    neighbors = ns;
}

void Peer::clear_neighbors()                                    { neighbors.clear(); }
void Peer::push_send_msg(PieceMsg const& msg)                   { send_msg_buf.push_back(msg); }
void Peer::remove_send_msg(PieceMsg const& msg)
{
    auto msg_match = [&msg](PieceMsg const& this_msg) {
        bool match = false;
        if (msg.src_pid == this_msg.src_pid &&
            msg.dest_pid == this_msg.dest_pid &&
            msg.piece_no == this_msg.piece_no) { match = true; }

        return match;
    };

    send_msg_buf.remove_if(msg_match);
}

void Peer::push_recv_msg(PieceMsg const& msg)                   { recv_msg_buf.push_back(msg); }
void Peer::sort_recv_msg()                                      { recv_msg_buf.sort(); }
void Peer::erase_recv_msg(MsgList::iterator it)                 { recv_msg_buf.erase(it); }
void Peer::pop_recv_msg()                                       { recv_msg_buf.pop_front(); }

void Peer::destroy_pieces()
{
    if (pieces != nullptr)
    {
        delete [] pieces;
        pieces = nullptr;
    }
}


/* getter */
int Peer::get_pid() const                                  { return pid; }
int Peer::get_cid() const                                  { return cid; }
int Peer::get_neighbor_counts() const                      { return neighbor_counts; }
float Peer::get_trans_time() const                         { return g_kPieceSize / bandwidth.uplink; }
float Peer::get_neighbor_pgdelay(const int nid) const      { return neighbors.at(nid).pg_delay; }
float Peer::get_join_time() const                          { return join_time; }
float Peer::get_complete_time() const                      { return complete_time; }
float Peer::get_leave_time() const                         { return leave_time; }
bool Peer::check_in_swarm() const                          { return in_swarm; }

bool Peer::get_nth_piece_info(const int n) const
{
    if (nullptr == pieces) ExitError("Pieces is NULL");
    return pieces[n];
}

bool* Peer::get_piece_info() const
{
    if (nullptr == pieces) ExitError("Pieces is NULL");
    return pieces;
}

Peer::Type Peer::get_type() const { return type; }
NeighborMap const& Peer::get_neighbors() const
{
    //if (nullptr == neighbors) ExitError("Neighbors is NULL");
    return neighbors;
}

Neighbor const& Peer::get_nth_neighbor(const int n) const         { return neighbors.at(n); }
Bandwidth const& Peer::get_bandwidth() const                      { return bandwidth; }
MsgList const& Peer::get_send_msg_buf() const                     { return send_msg_buf; }
MsgList const& Peer::get_recv_msg_buf() const                     { return recv_msg_buf; }

void Peer::InitPieces(const Type type, const int NUM_PIECE, const double main_prob)
{
    pieces = MakePieces(NUM_PIECE);

    switch (type)
    {
        case SEED:
            for (int i = 0; i < NUM_PIECE; i++)
                pieces[i] = true;
            break;

        case LEECH:
            for (int i = 0; i < NUM_PIECE; i++)
            {
                double sub_prob = uniformrand::Rand(RSC::PROB_PIECE) / (double)RAND_MAX;
                pieces[i] = (sub_prob < main_prob);
            }
            break;

        case NORMAL:
            for (int i = 0; i < NUM_PIECE; i++)
                pieces[i] = false;
            break;

        default:
            ExitError("Unknown type of peer");
            break;
    }
}
