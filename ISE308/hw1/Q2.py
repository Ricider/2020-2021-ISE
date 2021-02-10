#!/usr/bin/env python3
from copy import deepcopy
from time import time
import heapq
from sys import argv

class State:
    moves = [["policeman",person] for person in ["son","daughter","mother","father","thief"]]+[
        ["policeman"],
        ["father","son"],
        ["mother","father"],
        ["mother","daughter"],
        ["mother"],
        ["father"],
        ]
    
    def __init__(self,initial_state,predefined_heuristic=None):
        self.starting_island=initial_state[0]
        self.end_island=initial_state[1]
        self.raft_pos="start"
        self.parent=None
        self.traversed_nodes=0
        if predefined_heuristic!=None:
            self.heuristic_value=predefined_heuristic # for the final node to avoid infinite loops
        else:
            self.heuristic_value=guess_heuristic(self)
        
    def is_legal(self):
        for group in [self.starting_island,self.end_island]:
            if -1 in group.values():
                return False          
            if (not group["mother"]) and group["daughter"] and group["father"]:
                return False
            if (not group["father"]) and group["son"] and group["mother"]:
                return False
            if group["thief"] and (not group["policeman"]) and (group["daughter"] or group["father"] or group["son"] or group["mother"]):
                return False
        return True
    
    def move(movement,src,dest):
        for person in movement:
            dest[person]+=1
            src[person]-=1
            
    def generate_next(self):
        new_frontier=[]
        for movement in State.moves:
            current=deepcopy(self)
            current.parent=self
            current.traversed_nodes+=1
            current.heuristic_value=guess_heuristic(current)
            if self.raft_pos=="start": 
                State.move(movement,current.starting_island,current.end_island)
                current.raft_pos="end"
            elif self.raft_pos=="end": 
                State.move(movement,current.end_island,current.starting_island)
                current.raft_pos="start"
            new_frontier.append(current)
        
        new_frontier=list(filter(State.is_legal,new_frontier)) #remove illegal
        return new_frontier
            
    def __eq__(self,other):
        return self.starting_island==other.starting_island and self.end_island==other.end_island and self.raft_pos==other.raft_pos
    
    def __lt__(self,other):
        return self.heuristic_value+self.traversed_nodes < other.heuristic_value+other.traversed_nodes
    
    def __str__(self):
        return  "starting island: "+str(self.starting_island) +'\n'+"ending island: "+str(self.end_island)+'\n'+str(self.raft_pos)
    __repr__=__str__
        
def bfs(goal,frontier):
    traversed=[]
    nodes_generated=1 #starting node
    while frontier:
        current=frontier.pop(0)
        if current == goal:
            return current,nodes_generated
        
        new_frontier=current.generate_next()
        traversed.append(current)
        for i in range(len(new_frontier)):
            if new_frontier[i] in traversed:
                new_frontier[i]=None
        new_frontier=[node for node in new_frontier if node] #eliminate traversed nodes
        nodes_generated+=len(new_frontier)
        frontier.extend(new_frontier)
    return None,nodes_generated   

def a_star(goal,frontier):
    traversed=[]
    nodes_generated=1 #starting node
    heapq.heapify(frontier) #just for a more general solution
    while frontier:
        current=heapq.heappop(frontier)
        if current == goal:
            return current,nodes_generated
        
        new_frontier=current.generate_next()
        traversed.append(current)
        for i in range(len(new_frontier)):
            if new_frontier[i] in traversed:
                new_frontier[i]=None
        new_frontier=[node for node in new_frontier if node] #eliminate traversed nodes
        nodes_generated+=len(new_frontier)
        for node in new_frontier:
            heapq.heappush(frontier,node)
    return None,nodes_generated

def guess_heuristic(state):
    people_left=8
    for key in state.end_island:
        people_left-=state.end_island[key]
    return (people_left-1)*2-1

end_state=State([{"son":0,"daughter":0,"father":0,"mother":0,"policeman":0,"thief":0},
                {"son":2,"daughter":2,"father":1,"mother":1,"policeman":1,"thief":1}],predefined_heuristic=0)

starting_state=State([{"son":2,"daughter":2,"father":1,"mother":1,"policeman":1,"thief":1},
                      {"son":0,"daughter":0,"father":0,"mother":0,"policeman":0,"thief":0}])

if __name__ == "__main__":
    end_state.raft_pos="end"
    start_time=time()
    if argv[1]=="bfs":
        solution,nodes_generated=bfs(end_state,[starting_state])
    elif argv[1] == "a_star":
        solution,nodes_generated=a_star(end_state,[starting_state])
    total_time=time()-start_time
    
    passes=0
    while solution.parent:
        print(solution,'\n')
        solution=solution.parent
        passes+=1
        
    print(starting_state)
    print("total raft passes = ",passes,"calculated in",total_time,"seconds and generated",nodes_generated,"nodes")