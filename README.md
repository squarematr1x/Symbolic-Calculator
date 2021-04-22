# YAASC
YAASC (Yet Another Aspiring Symbolic Calculator) is a primitive symbolic calculator made from scratch. It is designed for performing simplification operations on different algebraic expressions. All the expressions are stored in a tree (data structure).

An expression tree is constructed from an input string given by the user. Expression nodes in the tree can be binary, generic, function or atomic. A binary node has two children, where as a generic node can have arbitrary number of children. A function node has only one child (parameter) and an atomic node doesn't have any children. By modifying the expression tree with different algorithms, the mathematical expression given by the user can be simplified.


### Example 1: Different nodes
![nodes](https://github.com/squarematr1x/YAASC/blob/master/Resources/img3.png?raw=true)

### Example 2: an expression tree constructed from input a^2+b
Infix input string is first converted to postfix (a ^ 2 + b --> a 2 ^ b +). Next, the expression tree is constructed from the postfix input. The scanner knows how to seperate between function input and variables. For example: sin(x) will be converted to x #sin, where as san(x) will be converted to s a * n * x * . At the moment hashtag symbol (#) acts as a function token.

![simple tree](https://github.com/squarematr1x/YAASC/blob/master/Resources/img1.png?raw=true)

### Example 3: simplification from a(a+b+c) to a^2+ab+ac
![simplification](https://github.com/squarematr1x/YAASC/blob/master/Resources/img2.png?raw=true)

### Features:

Basic numerical calculations:

```
yaasc:1> 4.5+7*2
         simplified: 18.5
yaasc:2> 4-6+1.1
         simplified: -0.9
yaasc:3> 16/(2^3)
         simplified: 2
yaasc:4> 9^2+15.25
         simplified: 96.25
yaasc:5> sin(4)
         simplified: -0.756802
```

Fractions:

```
yaasc:1> 3/4+5
         simplified: 23/4
yaasc:2> 1/2+3/2
         simplified: 2
yaasc:3> 1/5-4+3/2
         simplified: -23/10
```

Factorials:

```
yaasc:1> 4!
         simplified: 24
yaasc:2> -5!+4!
         simplified: -96
yaasc:3> (x+y)!
         couldn't simplify further: (x+y)!
```

Addition of variables:

```
yaasc:1> 3xyz+4yzx+2zxy
         simplified: 9xyz
yaasc:2> 2xy-4yx-y2x
         simplified: -4xy
yaasc:3> x^2+y+2x^2
         simplified: 3x^2+y
yaasc:4> 3/2x+y+5x
         simplified: 13/2x+y
yaasc:5> 3!+(x+yz)!+4!
         simplified: (x+yz)!+30
```

Exponents:

```
yaasc:1> x^2x^3
         simplified: x^5
yaasc:2> x/x^4
         simplified: x^-3
yaasc:3> x^2yxy^(1/2)
         simplified: x^3y^3/2
yaasc:4> x^3^4
         simplified: x^12
yaasc:5> (xy^3)^2
         simplified: x^2y^6
yaasc:6> xxx+yxy+zz
         simplified: x^3+xy^2+z^2
```

Expand:

```
yaasc:1> 3(x+y+z)
         simplified: 3x+3y+3z
yaasc:2> xy(2x+4+z)
         simplified: 2x^2y+4xy+xyz
yaasc:3> y^2(x-4)
         simplified: xy^2-4y^2
yaasc:4> (x+y)(x+y)
         simplified: x^2+2xy+y^2
```

Binomial theorem:

```
yaasc:1> (x+y)^2
         simplified: x^2+2xy+y^2
yaasc:2> (x-y)^2
         simplified: x^2-2xy+y^2
yaasc:3> (x+y)^4
         simplified: x^4+4x^3y+6x^2y^2+4xy^3+y^4
yaasc:4> (x+y)^7
         simplified: x^7+7x^6y+21x^5y^2+35x^4y^3+35x^3y^4+21x^2y^5+7xy^6+y^7
```

Logarithms:

```
yaasc:1> log(xyz)
         simplified: log(x)+log(y)+log(z)
yaasc:2> log(y/x)
         simplified: log(y)-log(x)
yaasc:3> ln(xe)
         simplified: ln(x)+1
yaasc:4> log2(2^x)
         simplified: x
yaasc:5> log10(1)+log2(16)+ln(x)
         simplified: 4+ln(x)
yaasc:6> ln(x^(3yz))
         simplified: 3ln(x)yz
```

Derivative (currently only with respect to x):

```
yaasc:1> D(x^3)
         simplified: 3x^2
yaasc:2> D(x^n)
         simplified: nx^(n-1)
yaasc:3> D(cos(x))
         simplified: -sin(x)
yaasc:4> D(x^2+x+sin(x))
         simplified: 1+2x+cos(x)
yaasc:5> D(5+y)
         simplified: 0
yaasc:6> D(sin(x)x^3)
         simplified: cos(x)x^3+3sin(x)x^2
yaasc:7> D(ln(x+1))
         simplified: (x+1)^-1
yaasc:8> D(y^x)
         simplified: y^xln(y)
yaasc:9> D(ln(sin(x)))
         simplified: cos(x)(sin(x))^-1
yaasc:10> D(sin(sin(sin(x))))
         simplified: cos(sin(sin(x)))cos(sin(x))cos(x)
```

Multiplication sign ( * ) and spaces are optional. However, those can be added into the input string:

```
yaasc:1> x*x^2 + y + y
         simplified: 2y+x^3
yaasc:2> (x*y)*(x + 2*y + z)
         simplified: 2xy^2+x^2y+xyz         
```

Future plans include: Integration, trigonometric identities and complex algebra.
