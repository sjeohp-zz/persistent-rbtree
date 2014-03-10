A red-black tree based on the data structures described at: http://bartoszmilewski.com/2013/11/13/functional-data-structures-in-c-lists/

I use the persistent list for the contents of the red-black tree, so it can hold more than one item per value.

The red-black tree described there has no remove methods, so I added them. This required: 
	remove methods for list
	methods for replacing the contents of a node with its min or max child
	additional temporary colours: negative-black and double-black for coping with removed nodes
	rewritten balance function
	bubble function for eliminating negative/double-black

I also added a size method to list, using foldr and a lambda function.