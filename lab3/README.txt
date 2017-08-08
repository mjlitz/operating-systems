Honor pledge: mjlitz and kiweber have neither given nor received aid
on this assignment.

We completed exercises 1-3, and implemented a better print, with 
attempts at exercises 4 and 5. Part of our slow progress stemmed from
a misunderstanding of how to implement drop_one_node; we were
traversing the tree and calling _delete on the pointer, rather than
reconstructing a string and passing that as an argument to delete. 

We used our remaining 51 late hours on this, for a total of 72

At a high level, ex4 would be implemented by calling locks on each
node as we reach it in the method; as the method unwinds recursively,
we would unlock each node. The trick was figuring out where to put the 
locks and unlocks. 

Print2 is an unused method; that is the original print method that
came with the code in case you would rather read print statements
from that. 

We never got the drop_one_node method ironclad; it still hangs 
sometimes in the sequential case and often in the mutex case. The vast 
majority of our time on this assignment was spent trying 
to resolve this issue. 
