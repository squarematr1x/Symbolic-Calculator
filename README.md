# YAASC
YAASC (Yet Another Aspiring Symbolic Calculator) is an aspiring [CAS](https://en.wikipedia.org/wiki/Computer_algebra_system) made from scratch. It is designed for performing simplification operations on different algebraic expressions. All the expressions are stored in a tree (datastructure).

An expression tree is constructed from an input string given by the user. Expression nodes in the tree can be binary, generic, function or atomic. A binary node has two children, where as a generic node can have arbitrary number of children. A function node has only one child (parameter) and an atomic node doesn't have any children. By modifying the expression tree with different algorithms, the mathematical expression given by the user can be simplified.


### Example 1: Different nodes
![nodes](https://github.com/squarematr1x/YAASC/blob/master/Resources/img3.png?raw=true)

### Example 2: an expression tree constructed from input a^2+b
Infix input string is first converted to postfix (a ^ 2 + b --> a 2 ^ b +). Next, the expression tree is constructed from the postfix input. The scanner knows how to seperate between function input and variables. For example: sin(x) will be converted to x #sin, where as san(x) will be converted to s a * n * x * . At the moment hashtag symbol (#) acts as a function token.

![simple tree](https://github.com/squarematr1x/YAASC/blob/master/Resources/img1.png?raw=true)

### Example 3: simplification from a(a+b+c) to a^2+ab+ac
![simplification](https://github.com/squarematr1x/YAASC/blob/master/Resources/img2.png?raw=true)

### Some examples with different input strings:

At this point some algebraic expression can be simplified with the CLI:

```
>>> 4.5+x^2+6+2x^2
         simplified: 10.5+3x^2
>>> 15+x-15.7
         simplified: -0.7+x
>>> xx+yyy+zz
         simplified: x^2+y^3+z^2
>>> (xyz)(5/(xyz))
         simplified: 5
>>> (xy^2z)^2
         simplified: x^2y^4z^2
>>> log(xy)
         simplified: log(x)+log(y)
>>> log2(8)+log10(100)
         simplified: 5
>>> log(x^y)
         simplified: ylog(x)
>>> ln(x^2x^-4)
         simplified: -2ln(x)
>>> ln(x^2x^3)+ln(x)
         simplified: 6ln(x)
>>> ln(xe)
         simplified: ln(x)+1
>>> x^3^5
         simplified: x^15
>>> x(xy+z+2y)
         simplified: 2xy+x^2y+xz   
>>> 1xy
         simplified: xy
>>> x
         couldn't simplify further: x
>>> (xyz)^0+5
         simplified: 6
>>> 0-2xy+0
         simplified: -2xy
>>> 4!+x
         simplified: 24+x
>>> 3!+x^2x+4!
         simplified: 30+x^3       
>>> 2xyz+3yxz+4zxy
         simplified: 9xyz   
>>> x2yz+zyx+zyx5
         simplified: 8xyz        
>>> (x^2yz/(x^2y))
         simplified: z
>>> x^2yx^3zy^5
         simplified: x^5y^6z
>>> (x+y)(x+y)
         simplified: 2xy+x^2+y^2
>>> (xy)^1
         simplified: xy
>>> -4!+(x+y)^3+(xy)^0
         simplified: -23+3x^2y+3xy^2+x^3+y^3
>>> (x+y)^9
         simplified: x^9+9x^8y+36x^7y^2+84x^6y^3+126x^5y^4+126x^4y^5+84x^3y^6+36x^2y^7+9xy^8+y^9
>>> D(x^3)
         simplified: 3x^2
>>> D(x^3+x^4)
         simplified: 3x^2+4x^3
>>> D(x^3sin(x)x^4)
         simplified: 7sin(x)x^6+cos(x)x^7
>>> D(ln(x+y+z))
         simplified: (x+y+z)^-1
>>> D(log2(x))
         simplified: (xln(2))^-1
>>> D(2.4)
         simplified: 0   
```

Multiplication sign ( * ) and spaces are optional, but however, those can be added into the input string:

```
>>> x*x^2 + y + y
         simplified: 2y+x^3
>>> (x*y)*(x + 2*y + z)
         simplified: 2xy^2+x^2y+xyz         
```

Future plans include: fractions (and roots), trigonometric functions and calculus.
