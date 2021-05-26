/**
 * @file backtrack.cc
 *
 */

#include "backtrack.h"
#include <queue>
#include <map>
Backtrack::Backtrack() {}
Backtrack::~Backtrack() {}
using namespace std;

  vector<bool> u_visited; //u가 방문되었는지를 확인
  vector <bool> v_visited; //vertex v가 방문되었는지를 확인
  vector<int> each; //각각의 u에 해당되는 vertex를 저장
  map<int, set<int>> candidate_set;
  set<int> children; //이미 방문한 node 들의 children 후보
  int n;
  vector<vector<int>> parent(n, vector<int>(n, 0));
  vector<vector<int>> child(n, vector<int>(n, 0));
void Backtrack::PrintAllMatches(const Graph &data, const Graph &query,
    
                                const CandidateSet &cs) {
                                  
  std::cout << "t " << query.GetNumVertices() << "\n";

  n=query.GetNumVertices();
  //find root
 
  int root=0;
  int initial_candidate=0;
  float value=0;
  parent.resize(n, vector<int>(0,0));
  child.resize(n, vector<int>(0,0));
  u_visited.resize(query.GetNumVertices());
  v_visited.resize(data.GetNumVertices());
  each.resize(query.GetNumVertices());
  value=cs.GetCandidateSize(0)/query.GetDegree(0);
  for(int j=1; j<n; j++){
      if ((cs.GetCandidateSize(j)/query.GetDegree(j))<value){
        root=j;
        value=initial_candidate/query.GetDegree(j);
      }
  }
  
  vector<bool> query_visited(n, false); //to check parent & child relationship using BFS
  queue<int> q;
  q.push(root);
  query_visited.at(root)=true;
   
  while(!q.empty()){
    int current= q.front();
    q.pop();
    for(unsigned int i=query.GetNeighborStartOffset(current); i<query.GetNeighborEndOffset(current); i++){
     
     auto it=find(child[query.adj_array_[i]].begin(),child[query.adj_array_[i]].end(), current );
     if (it==child[query.adj_array_[i]].end()){ //child 후보에 안 넣었음
        child[current].emplace_back((int)query.adj_array_[i]); 
        parent[(int)query.adj_array_[i]].emplace_back(current); 
     }
      if (!query_visited[(int)query.adj_array_[i]]){ 
        query_visited[(int)query.adj_array_[i]]=true;
        q.push((int)query.adj_array_[i]);
      }
    }
  }

  set<int> root_candidate;
  for(unsigned int i=0; i<cs.GetCandidateSize(root); i++){
    root_candidate.insert(cs.GetCandidate(root, i)); //root에 mapping되는 vertex를 root_candidate에 넣음 
  }
  
  candidate_set.insert(make_pair(root, root_candidate)); 
  children.insert(root);
  FindMatches(data, query, cs,  root, 1); //루트부터 시작
  
}
  
    
void Backtrack::FindMatches(const Graph &data, const Graph &query, const CandidateSet &cs,  int u, unsigned int count){
    // u의 parent를 돌면서 가능한 candidate set을 찾기
    vector<int>::iterator ptr;
    set<int> tmp;
    set<int> possible_set;//현재 u에서 가능한 v들의 집합
    if (candidate_set.find(u)!=candidate_set.end()){
      possible_set=candidate_set.find(u)->second; //현재 가능한 vertex들을 불러옴
    }
    

    if (count== query.GetNumVertices()){
        for(set<int> :: iterator it=possible_set.begin(); it!=possible_set.end(); it++){ 
           each[u]=*it; //mapping
           v_visited[*it]=true;
          std::cout <<"a";
          vector<int>::iterator iterate;
          for (iterate = each.begin(); iterate != each.end(); ++iterate){
          std:: cout<<" "<< *iterate;
        }
        std::cout<<endl;
        v_visited[*it]=false;
        u_visited[u]=false;
        }
        return;
    }
   

      for(set<int> :: iterator it=possible_set.begin(); it!=possible_set.end(); it++){ //가능한 애들(v)을 하나씩 돌면서 그 다음 가능한 u를 탐색
 //parent가 이미 mapping이 다 된 애들 중, candidate set이 가장 작은 다음 u를 선택 
      u_visited[u]=true;
     
        for(vector<int>::iterator sub=child[u].begin(); sub!=child[u].end(); sub++){
          children.insert(*sub); //children: 다음 갈 수 있는 u 넣어줌 
        }
         if (children.find(u)!=children.end()){
              children.erase(children.find(u)); // 현재 u는 빼줌 
        }
     //  std::cout<<"children 개수: "<<children.size()<<endl;
  unsigned int min_cand=0;
  int min_val=-1; //가장 candidate이 적은 set으로 이동 
  each[u]=*it; // 현재 u와 v를 mapping
  
  if (children.size()==0){
      //더 이상 갈 곳이 없음
      return; 
  }

        for(set<int>:: iterator u_it=children.begin(); u_it!=children.end(); u_it++){
    
         bool all_mapped=true;
          //다음 u를 찾는 과정 
      
          for(vector<int> :: iterator parent_it=parent[*u_it].begin(); parent_it!=parent[*u_it].end(); parent_it++ ){
              if (!u_visited[*parent_it]){ //parent가 하나라도 mapping 되지 않은 것이 있다면
                all_mapped=false;
                break;
              }
            
        }
          
        set<int> child_possible_candidate;
    if (all_mapped ){ //parent가 다 맵핑이 되어 있을 때 
    for(unsigned int i=0; i<cs.GetCandidateSize(*u_it); i++){
      bool connected=true;
      //parent와 연결되어 있는지 검사
      if (v_visited[cs.GetCandidate(*u_it, i)]){ //이미 방문된 것은 candidate set에 넣지 않음
          connected=false;
      }
      else{ //방문은 안 된 것
      for(vector<int>::iterator p_it=parent[*u_it].begin(); p_it!=parent[*u_it].end(); p_it++){
          if (!data.IsNeighbor(each[*p_it], cs.GetCandidate(*u_it, i))){ //연결되어 있지 않음
              connected=false;
              break;
          }   
      }
      }
      if (connected) {
        child_possible_candidate.insert( cs.GetCandidate(*u_it, i));
      }
    }
    if (child_possible_candidate.size()==1){
       min_cand=child_possible_candidate.size();
       min_val=*u_it;
       candidate_set[*u_it]=child_possible_candidate;
       break;
    }
    else if (child_possible_candidate.size()!=0) { //하나라도 가능한 것이 있으면 
        if (min_val==-1||(child_possible_candidate.size()<min_cand)){
          min_cand=child_possible_candidate.size();
          min_val=*u_it;
          candidate_set[*u_it]=child_possible_candidate;
          
        }
    }
  
}

        }

    
FindMatches(data, query, cs,min_val, count+1 );
v_visited[*it]=false;
u_visited[u]=false;
children.insert(u);
      }
    
    

}
