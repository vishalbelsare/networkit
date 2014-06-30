/*
 * ClusterContracter.cpp
 *
 *  Created on: 30.10.2012
 *      Author: Christian Staudt (christian.staudt@kit.edu)
 */

#include "ClusterContracter.h"

namespace NetworKit {

ClusterContracter::ClusterContracter() {
	// TODO Auto-generated constructor stub

}

ClusterContracter::~ClusterContracter() {
	// TODO Auto-generated destructor stub
}

std::pair<Graph, NodeMap<node> > ClusterContracter::run(Graph& G, Clustering& zeta) {

	Graph Gcon(0); // empty graph
	Gcon.markAsWeighted(); // Gcon will be a weighted graph

	IndexMap<cluster, node> clusterToSuperNode(zeta.upperBound(), none); // there is one supernode for each cluster

	// populate map cluster -> supernode
	G.forNodes([&](node v){
		cluster c = zeta.clusterOf(v);
		if (! clusterToSuperNode.hasBeenSet(c)) {
			clusterToSuperNode[c] = Gcon.addNode(); // TODO: probably does not scale well, think about allocating ranges of nodes
		}
	});


	int64_t n = G.numberOfNodes();
	NodeMap<node> nodeToSuperNode(n);

	// set entries node -> supernode
	G.parallelForNodes([&](node v){
		nodeToSuperNode[v] = clusterToSuperNode[zeta.clusterOf(v)];
	});


	// iterate over edges of G and create edges in Gcon or update edge and node weights in Gcon
	G.parallelForWeightedEdges([&](node u, node v, edgeweight ew) {
		TRACE("reading supernodes");
		node su = nodeToSuperNode[u];
		node sv = nodeToSuperNode[v];
		TRACE("modifying edge (", su, ", ", sv, ")");
		Gcon.increaseWeight(su, sv, ew);
	});

	return std::make_pair(Gcon, nodeToSuperNode);

}

}