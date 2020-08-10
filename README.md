# JASC
JASC (Just Another Symbolic Calculator) is an aspiring [CAS](https://en.wikipedia.org/wiki/Computer_algebra_system) for performing simplification operations on different algebraic expressions. An expression tree is constructed from an input string given by the user. Expression nodes in the tree can be binary, generic or atomic. A binary node has only two children, where as a generic node can have arbitrary number of children. An atomic node doesn't have any children. By modifying the expression tree with different algorithms, the mathematical expression given by the user can be simplified.


### Example 1: Different nodes
![nodes](https://github.com/squarematr1x/JASC/blob/develop/Resources/img3.png?raw=true)

### Example 2: an expression tree constructed from input a^2+b
![simple tree](https://github.com/squarematr1x/JASC/blob/master/Resources/img1.png?raw=true)

### Example 3: simplification from a(a+b+c) to a^2+ab+ac
![simplification](https://github.com/squarematr1x/JASC/blob/master/Resources/img2.png?raw=true)
