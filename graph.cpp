// File: graph.cpp
// -- simple graph handling source file
//-----------------------------------------------------------------------------
// Community detection
// Based on the article "Fast unfolding of community hierarchies in large networks"
// Copyright (C) 2008 V. Blondel, J.-L. Guillaume, R. Lambiotte, E. Lefebvre
//
// This file is part of Louvain algorithm.
// 
// Louvain algorithm is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Louvain algorithm is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Louvain algorithm.  If not, see <http://www.gnu.org/licenses/>.
//-----------------------------------------------------------------------------
// Author   : E. Lefebvre, adapted by J.-L. Guillaume
// Email    : jean-loup.guillaume@lip6.fr
// Location : Paris, France
// Time	    : February 2008
//-----------------------------------------------------------------------------
// see readme.txt for more details

#include "graph.h"

using namespace std;

Graph::Graph(char *filename, uint32_t type) {
  ifstream finput;
  finput.open(filename,fstream::in | fstream::binary);
  if (!finput.is_open())
  {
    std::cerr << "failed to open file\n";
    return;
  }

  uint32_t nb_links=0;

  while (true) {
    uint32_t src, dest;
    double weight=1.;


    if (type==WEIGHTED) 
    {
      finput.read((char*) &src, sizeof(src));
      if (finput.eof())
        break;
      assert(finput.rdstate() == ios::goodbit);
      finput.read((char*) &dest, sizeof(dest));
      assert(finput.rdstate() == ios::goodbit);
      finput.read((char*) &weight, sizeof(weight));
      assert(finput.rdstate() == ios::goodbit);
    } 
    else
    {
      finput.read((char*) &src, sizeof(src));
      if (finput.eof())
        break;
      assert(finput.rdstate() == ios::goodbit);
      finput.read((char*) &dest, sizeof(dest));
      assert(finput.rdstate() == ios::goodbit);
    }
    
    
    if (links.size()<=max(src,dest)+1) {
      links.resize(max(src,dest)+1);
    }
      
    links[src].push_back(make_pair(dest,weight));
    if (src!=dest)
      links[dest].push_back(make_pair(src,weight));

    nb_links++;
     
  }

  finput.close();
}

void
Graph::renumber(uint32_t type) {
  vector<uint32_t> linked(links.size(),-1);
  vector<uint32_t> renum(links.size(),-1);
  uint32_t nb=0;
  
  for (uint32_t i=0 ; i<links.size() ; i++) {
    for (uint32_t j=0 ; j<links[i].size() ; j++) {
      linked[i]=1;
      linked[links[i][j].first]=1;
    }
  }
  
  for (uint32_t i=0 ; i<links.size() ; i++) {
    if (linked[i]==1)
      renum[i]=nb++;
  }

  for (uint32_t i=0 ; i<links.size() ; i++) {
    if (linked[i]==1) {
      for (uint32_t j=0 ; j<links[i].size() ; j++) {
	links[i][j].first = renum[links[i][j].first];
      }
      links[renum[i]]=links[i];
    }
  }
  links.resize(nb);
}

void
Graph::clean(uint32_t type) {
  for (uint32_t i=0 ; i<links.size() ; i++) {
    map<uint32_t, float> m;
    map<uint32_t, float>::iterator it;

    for (uint32_t j=0 ; j<links[i].size() ; j++) {
      it = m.find(links[i][j].first);
      if (it==m.end())
	m.insert(make_pair(links[i][j].first, links[i][j].second));
      else if (type==WEIGHTED)
      	it->second+=links[i][j].second;
    }
    
    vector<pair<uint32_t,float> > v;
    for (it = m.begin() ; it!=m.end() ; it++)
      v.push_back(*it);
    links[i].clear();
    links[i]=v;
  }
}

void
Graph::display(uint32_t type) {
  for (uint32_t i=0 ; i<links.size() ; i++) {
    for (uint32_t j=0 ; j<links[i].size() ; j++) {
      int dest   = links[i][j].first;
      float weight = links[i][j].second;
      if (type==WEIGHTED)
	cout << i << " " << dest << " " << weight << endl;
      else
	cout << i << " " << dest << endl;
    }
  }
}

void
Graph::display_binary(char *filename, char *filename_w, uint32_t type) {
  ofstream foutput;
  foutput.open(filename, fstream::out | fstream::binary);

  uint32_t s = links.size();

  // outputs number of nodes
  foutput.write((char *)(&s),sizeof(s));

  // outputs cumulative degree sequence
  long tot=0;
  for (uint32_t i=0 ; i<s ; i++) {
    tot+=(long)links[i].size();
    foutput.write((char *)(&tot),sizeof(tot));
  }

  // outputs links
  for (uint32_t i=0 ; i<s ; i++) {
    for (uint32_t j=0 ; j<links[i].size() ; j++) {
      uint32_t dest = links[i][j].first;
      foutput.write((char *)(&dest),sizeof(dest));
    }
  }
  foutput.close();

  // outputs weights in a separate file
  if (type==WEIGHTED) {
    ofstream foutput_w;
    foutput_w.open(filename_w,fstream::out | fstream::binary);
    for (uint32_t i=0 ; i<s ; i++) {
      for (uint32_t j=0 ; j<links[i].size() ; j++) {
	float weight = links[i][j].second;
	foutput_w.write((char *)(&weight),sizeof(weight));
      }
    }
    foutput_w.close();
  }
}

