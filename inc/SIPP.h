﻿#pragma once
#include <boost/functional/hash.hpp>
#include "SingleAgentSolver.h"
#include "ReservationTable.h"

class SIPPNode: public LLNode
{
public:
    // define a typedefs for handles to the heaps (allow up to quickly update a node in the heap)
    typedef boost::heap::pairing_heap< SIPPNode*, compare<SIPPNode::compare_node> >::handle_type open_handle_t;
    typedef boost::heap::pairing_heap< SIPPNode*, compare<SIPPNode::secondary_compare_node> >::handle_type focal_handle_t;
    open_handle_t open_handle;
    focal_handle_t focal_handle;
    int high_generation; // the upper bound with respect to generation
    int high_expansion; // the upper bound with respect to expansion
    bool collision_v;
    SIPPNode() : LLNode() {}
    SIPPNode(int loc, int g_val, int h_val, SIPPNode* parent, int timestep, int high_generation, int high_expansion,
             bool collision_v, int num_of_conflicts) :
            LLNode(loc, g_val, h_val, parent, timestep, num_of_conflicts), high_generation(high_generation),
            high_expansion(high_expansion), collision_v(collision_v) {}
    SIPPNode(const SIPPNode& other): LLNode(other), high_generation(other.high_generation), high_expansion(other.high_expansion),
                                     collision_v(collision_v) {}
    ~SIPPNode() {}

    void copy(const SIPPNode& other) // copy everything except for handles
    {
        LLNode::copy(other);
        high_generation = other.high_generation;
        high_expansion = other.high_expansion;
        collision_v = other.collision_v;
    }
    // The following is used by for generating the hash value of a nodes
    struct NodeHasher
    {
        std::size_t operator()(const SIPPNode* n) const
        {
            size_t seed = 0;
            boost::hash_combine(seed, n->location);
            boost::hash_combine(seed, n->high_generation);
            return seed;
        }
    };

    // The following is used for checking whether two nodes are equal
    // we say that two nodes, s1 and s2, are equal if
    // both are non-NULL and agree on the id and timestep
    struct eqnode
    {
        bool operator()(const SIPPNode* n1, const SIPPNode* n2) const
        {
            return (n1 == n2) ||
                   (n1 && n2 && n1->location == n2->location &&
                    n1->wait_at_goal == n2->wait_at_goal &&
                    n1->is_goal == n2->is_goal &&
                    n1->high_generation == n2->high_generation);
        }
    };
};

std::ostream& operator<<(std::ostream& os, const SIPPNode& node);

class SIPP: public SingleAgentSolver
{
public:
    SIPP(const Instance& instance, int agent): SingleAgentSolver(instance, agent) {}
    string getName() const { return "SIPP"; }

    Path findOptimalPath(const set<int>& higher_agents, const vector<Path*>& paths, int agent) override;
    Path findOptimalPath(const ConstraintTable& constraint_table) override;
    
    // return A path that minimizes collisions, breaking ties by cost
    Path findPath(const set<int>& higher_agents, const vector<Path*>& paths, int agent) override;
    Path findPath(const ConstraintTable& constraint_table) override;

    void saveSearchTree(const string& fileName="./tmp_search_tree.txt") const;

private:
    // define typedefs and handles for heap
    typedef boost::heap::pairing_heap< SIPPNode*, boost::heap::compare<LLNode::compare_node> > heap_open_t;
    typedef boost::heap::pairing_heap< SIPPNode*, boost::heap::compare<LLNode::secondary_compare_node> > heap_focal_t;
    heap_open_t open_list;
    heap_focal_t focal_list;

    // define typedef for hash_map
    typedef boost::unordered_map<SIPPNode*, list<SIPPNode*>, SIPPNode::NodeHasher, SIPPNode::eqnode> hashtable_t;
    hashtable_t allNodes_table;
    list<SIPPNode*> useless_nodes;
    // Path findNoCollisionPath(const ConstraintTable& constraint_table);

    void updatePath(const LLNode* goal, std::vector<PathEntry> &path);

    inline void pushNodeToOpen(SIPPNode* node);
    inline void pushNodeToOpenAndFocal(SIPPNode* node);
    inline void pushNodeToFocal(SIPPNode* node);
    inline void eraseNodeFromLists(SIPPNode* node);
    void updateFocalList();
    void releaseNodes();
    bool dominanceCheck(SIPPNode* new_node);
    void printSearchTree() const;
};