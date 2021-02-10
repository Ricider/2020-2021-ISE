import sys

RED=True
BLACK=False
score_types=["Rebs","Assists","Points"]

class Red_Black_Node:
    def __init__(self, name:str, color:bool, stats:[int,int,int], parent):
        self.name=name
        self.stats=stats
        self.max_stats=[i for i in stats] #initially the bottom node so max stats equals stats
        self.max_stats_owner=[name for i in range(3)] #owners of all max stats
        
        self.color=color
        
        self.parent=parent
        self.left=None
        self.right=None
    
    def __str__(self):
        return ("(RED)" if self.color else "(BLACK)")+self.name
    
    __repr__=__str__
    
    def get_grandparent(self):
        return self.parent.parent
    
    def get_uncle(self):
        grandparent= self.get_grandparent()
        if grandparent.left==self.parent: return grandparent.right
        else: return grandparent.left
    
    def traverse(self,padding=""):
        if self:
            if self.parent: 
                assert not (self.parent.color == RED and self.color==RED) #just checking to be sure
                for i in range(3):
                    assert self.max_stats[i] <= self.parent.max_stats[i]
            print(padding+str(self))
            Red_Black_Node.traverse(self.left,padding+"-")
            Red_Black_Node.traverse(self.right,padding+"-")
   
  
def maybe_update(node): #returns true if successfully updates else false
    if not node or not node.parent: return False #no update happens
    updated=False
    for i in range(3): #for every type of stat
        if node.max_stats[i] > node.parent.max_stats[i]:
            node.parent.max_stats[i]=node.max_stats[i]
            node.parent.max_stats_owner[i]=node.max_stats_owner[i]
            updated=True
    return updated
            
def update_max(node): #update up until root
    updated = maybe_update(node)
    if updated:
        update_max(node.parent) #go one level up

def left_rotation(node):
    child = node.right
    
    if node.parent: #if node has a parent swap it
        if node.parent.left == node : node.parent.left=child
        else: node.parent.right=child    
    
    if child.left != None:
        child.left.parent=node    
    
    node.right=child.left #swapping children
    child.left=node
    
    child.parent=node.parent #swapping parents
    node.parent=child
    
    node.max_stats=[stat for stat in node.stats] #update max stats
    child.max_stats=[stat for stat in node.stats]
    maybe_update(node.left)
    maybe_update(node.right)
    maybe_update(child.left)
    maybe_update(child.right)
    
def right_rotation(node):
    
    child = node.left
    
    if node.parent: #if node has a parent swap it
        if node.parent.left == node : node.parent.left=child
        else: node.parent.right=child
    
    if child.right != None:
        child.right.parent=node
    
    node.left=child.right #swapping children
    child.right=node
    
    child.parent=node.parent #swapping parents
    node.parent=child

    node.max_stats=[stat for stat in node.stats] #update max stats
    child.max_stats=[stat for stat in node.stats]
    maybe_update(node.left)
    maybe_update(node.right)
    maybe_update(child.left)
    maybe_update(child.right)
    
def enforce_rb(node):
    while node.parent and node.parent.color==RED:
            
        u=node.get_uncle()
        if u and u.color==RED:
            u.color=BLACK
            node.parent.color=BLACK
            node.get_grandparent().color=RED
            node=node.get_grandparent()
            
        else:#if the uncle is black or nil (which is black)
            p=node.parent
            gp=node.get_grandparent()
            
            if node==p.left and p==gp.left: #LL
                right_rotation(gp)
                p.color=BLACK
                gp.color=RED  
                
            elif node==p.right and p==gp.left: #LR
                left_rotation(p)
                right_rotation(gp)
                node.color=BLACK
                gp.color=RED
                node=p #continue from parent
                
            elif node==p.right and p==gp.right: #RR
                left_rotation(gp)
                p.color=BLACK
                gp.color=RED   
                
                
            elif node==p.left and p==gp.right: #RL
                right_rotation(p)
                left_rotation(gp)
                node.color=BLACK
                gp.color=RED  
                node=p #continue from parent
                
    if not node.parent:
        node.color=BLACK
    
def insert_player(root, player_season_info):
    
    last_parent=None
    direction=None
    
    while root!=None:
        if root.name == player_season_info[1]: #player is the current node
            root.stats=[root.stats[i]+player_season_info[i+3] for i in range(3)] #add to old stats if already present
            
            for i in range(3): #update maximum stats
                if root.max_stats[i] < root.stats[i]:
                    root.max_stats[i]=root.stats[i]
                    root.max_stats_owner[i]=root.name
            
            update_max(root)
            return
        
        elif root.name > player_season_info[1]: #name greater than root name
            last_parent=root
            root=root.left
            direction=True
    
        else: #name smaller than root name
            last_parent=root
            root=root.right
            direction=False
        
    new_node=Red_Black_Node(player_season_info[1],RED,player_season_info[3:],last_parent)
    if direction:
        last_parent.left=new_node
    else:
        last_parent.right=new_node
        
    update_max(new_node)
    enforce_rb(new_node)
  
def insert_one_season(root,players,current=0):

    current_season=players[current][0] #first players first season
    
    while current < len(players) and players[current][0]==current_season: #while in this season
        insert_player(root,players[current])
        current+=1
        while root.parent: root=root.parent #if the root have been moved fix it back to the top
        
    return root,current
        
if __name__ == "__main__":
    lines= open(sys.argv[1],"r").readlines()[1:]
    players=[player.strip().split(",") for player in lines]
    players=[player[:3]+list(map(int,player[3:])) for player in players] #convert last 3 values to integers
    
    root=Red_Black_Node(players[0][1],BLACK,players[0][3:],None)
    
    root,current=insert_one_season(root,players) #get first season
    print("End of the "+players[current-1][0]+" Season")
    list(map(print,["Max "+score_types[i]+": "+str(root.max_stats[i])+" - Player Name: "+root.max_stats_owner[i] for i in range(2,-1,-1)])) #print stats
    root.traverse() #print first season in tree form
    
    while current < len(players): #rest of the seasons
        root,current=insert_one_season(root,players,current)  
        print("End of the "+players[current-1][0]+" Season")
        list(map(print,["Max "+score_types[i]+": "+str(root.max_stats[i])+" - Player Name: "+root.max_stats_owner[i] for i in range(2,-1,-1)])) #print stats