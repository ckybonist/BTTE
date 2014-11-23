#ifndef _NEIGHBOR_H
#define _NEIGHBOR_H

/* Connection state
*
* A block is downloaded by the client when the client is interested in a peer,
* and that peer is not choking the client. A block is uploaded by a client when the client
* is not choking a peer, and that peer is interested in the client.
*
* Choking algorithm select four unchoked peers which have the best upload rate and are interested,
* these four peers referred as downloaders.
* Other Peers which have better upload rates(as compared to downloaders), but aren't interested get unchoked
* If they become interested, the downloader with the worst upload rate gets choked.
*
* For optimistic unchoking, at any one time there is a single peer
* which is unchoked regardless of its upload rate.
* * * * * * * * */
typedef struct ConnectionStates
{
    ConnectionStates() : am_choking(true), am_interested(false),
                         peer_choking(true), peer_interested(false) {};
    bool am_choking;
    bool am_interested;
    bool peer_choking;
    bool peer_interested;
} ConnStates;

/*
 * @pg_delay: Using range 0 to 100 to represent the propagation delay (pgdelay)
 * (if neighbor list had been changed, the pg_delay which peer ever had, need to
 * accumulate with current one).
 *
 * NOTE: Spliting the range to 4 class-intervals so that it could be use for determining
 *       that peers are in same cluster or not (CLUSTER-BASED PEER SELECTION algorithm).
 * * * * * * */
struct Neighbor
{
    Neighbor();
    Neighbor(const int id, const float pg_delay);
    bool IsConnected() const;

    int id;
    float pg_delay;
    // This var is use to check each neighbor is exist in peer list,
    // however, not the meaning of connection between neighbor by network
    bool exist;
    ConnStates conn_states;
};

#endif // for #ifndef _NEIGHBOR_H
