### Quick Start

```shell
# Maybe you need add this line in Makefile if you use macOS Big Sur
# LDFLAGS := -L/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/lib
git clone https://github.com/undefinedboy/alien.git
cd alien && make
```

### Run

```shell
./alien examples/callStatement.alien
```

### Json Generator

```shell
./jsongen test/testjsongen.alien test/testjsongen.json
```

### Regex

```regex
NUMBER     = DIGIT+(\.DIGIT+)?
STRING     = "anything"
IDENTIFIER = ALPHA(ALPHA|DIGIT)*
ALPHA      = [a-zA-Z_]
DIGIT      = [0-9]
```

### Context-free grammar

```ebnf
program     = declaration* EOF
declaration = classDecl | funcDecl | varDecl | constDecl
classDecl   = "class" IDENTIFIER "{" funcDecl* "}"
funcDecl    = "func" IDENTIFIER "(" parameters? ")" block
varDecl     = "var" IDENTIFIER ( "=" expression )? ";"
constDecl   = "const" IDENTIFIER "=" expression ";"
parameters  = IDENTIFIER ( "," IDENTIFIER )*
block       = "{" ( varDecl | statement )* "}"
expression  = assignment
assignment  = ( call "." )? IDENTIFIER "=" assignment
            | logicOr
logicOr     = logicAnd ( "or" logicAnd )* 
logicAnd    = equality ( "and" equality )* 
equality    = comparison ( ( "!=" | "==" ) comparison )* 
comparison  = term ( ( ">" | ">=" | "<" | "<=" ) term )* 
term        = factor ( ( "-" | "+" ) factor )* 
factor      = unary ( ( "/" | "*" ) unary )* 
unary       = ( "!" | "-" ) unary | call 
call        = primary ( "(" arguments? ")" | "." IDENTIFIER )* 
primary     = "true" | "false" | "nil" | "this"
            | NUMBER | STRING | IDENTIFIER | "(" expression ")"
statement   = exprStmt | ifStmt | whileStmt | forStmt | printStmt
            | returnStmt
exprStmt    = expression ";"
ifStmt      = "if" "(" expression ")" block ("else" block)?
whileStmt   = "while" "(" expression ")" block
forStmt     = "for" "(" ( varDecl | exprStmt | ";" )
                          expression? ";"
                          expression? 
                     ")" block
printStmt   = "print" expression ";"
returnStmt  = "return" expression? ";"
arguments   = expression ("," expression)*
```

### Optimizer

_Comming soon_
