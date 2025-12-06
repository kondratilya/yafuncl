#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cassert>

#include "tests.h"
#include "lexic.h"
#include "syntax.h"
#include "execute.h"

using namespace std::string_literals;

std::string numbers = "0={};1={++0;};2={++1;};3={++2;};4={++3;};5={++4;};6={++5;};7={++6;};8={++7;};9={++8;};10={++9;};";

std::string trim(std::string s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    return s;
}

std::string test(std::string test) {
    std::istringstream source(test);
    std::ostringstream result;

    std::cout << "---------------------------------------------" << std::endl;
    try {
        LexicAnalys lexic(source);
        SyntaxAnalys syntax(lexic);
        Execute exec(syntax, &result);
        exec.Run();
    } catch(std::runtime_error& e) {
        result << e.what();
    }

    std::cout << trim(result.str()) << std::endl;
    return trim(result.str());
}

void tests() {
    assert(test(numbers + "@+10;") == "'10'");
    assert(test(numbers + 
            "HelloWorld = ({7*10+2;}, 10*10+1, 10*10+8, 10*10+8, 10*10+10+1, 3*10+2, 10*10+10+9, 10*10+10+1, 10*10+10+4, 10*10+8, 10*10, 10+3, 10);"
            "@@@ HelloWorld;"
    ) == "Hello world" );
    assert(test(numbers + "10=3+3;") == "Exec: Modifying immutable value \"10\"");
    assert(test(numbers + 
        "@2; ; @+2; "s
        "@(1 == 1); @(1 != 1); @(1 == {++0;}); @(1 != {});"s
        "@(0 && 1 || 2); @(0 || {} || +0);"s
        "r={+@arguments;}; @(0=>r || (-3)=>r || 10=>r);"s
        "@(1 - - ++ 2);"s
        "5_3 = 2+3+3/10; @5_3; @(5_3 % 2); "s
    ) == 
        "'<20>' '2' '1' '0' '1' '1' '<20>' '0' '<3>' '-3' '<100>' '-3' '4' '5.3' '1.3'"s
    );
  
    assert(test(numbers + 
        "ret1 = { return ; @1; @2; return 10; @3; }; @=>ret1;"s
        "ret2 = { @1; @2; return 10; @3; }; @=>ret2;"s
        "@x={2+2+1;}; @y={2+2+arguments;}; @=>x; @(- 2=>y); @(-2)=>y; @( (1,2)=>{@arguments;} ); "s
    ) == 
        "'0' '<10>' '<20>' '<100>' '<163>' '<177>' '5' '-6' '2' '(<10>, <20>, )' '(<10>, <20>, )'"s
    );

    assert(test(numbers + "@=>(6+6);"s
    ) == 
        "Exec: Call to non-address"s
    );
  
    assert(test(numbers + 
        "@mutable a=+5;"
        "@mutable b=+2;"
        "=> mutable { @inner a=+3; @outer a; @outer b=+4; @a=+7; @a; @b; }; @a;@b;"
        "=> inner { @inner mutable a= +outer 8; @outer a; @outer b; @a= ++ ++ ++++a; }; @a;@b;"
    ) == 
        "'5' '2' '3' '5' '4' '7' '7' '4' '5' '4' '8' '5' '4' '12' '5' '4'"s
    );

    assert(test(numbers + 
        "@a=+6; => inner { outer a = 1; }; @a;"
    ) == 
        "'6' Exec: Modifying immutable value \"a\""s
    );

    assert(test(numbers +            //Замыкания
        "plus = inner { b=arguments; { arguments+b; }; };"
        "@plus2 = 2=>plus;" "@plus3 = 3=>plus;"
        "@10=>plus2;"
        "@10=>plus3;"
    ) == 
        "'<117>' '<117>' '12' '13'"s
    );

    assert(test(numbers +            //Рекурсии
        "recursion = { (a, ) = arguments; @a; "
        "    if a < 10: (a+1)=>recursion else: @?end;"
        "};"
        "1=>recursion;"
    ) == 
        "'<10>' '2' '3' '4' '5' '6' '7' '8' '9' '10' end"s
    );

    assert(test(numbers +            // Вложенные вызовы (и доступ по индексу)
        "i1 = { @?OuterFunction;"
        "    ("
        "        +2,+3,+4,"
        "        { "
        "            @?InnerFunction; "
        "            @arguments;@(arguments+0, arguments+1, arguments+2);"
        "        }"
        "    );"
        "};"
        "@=>i1;"
        "@(8=>((=>i1)[3]))[2];"
    ) == 
        "OuterFunction '(2, 3, 4, <121>, )' OuterFunction InnerFunction '<80>' '(8, 9, 10, )' '10'"s
    );

    assert(test(numbers + 
        "@mutable a=(  1,   (-2-3, ),   (),    ( 3, 3+1, (1,()) )    );"
        "@a=>length; @1=>length; @()=>length;; @(1, )=>length;"
        "@a[1]; @a[3][2];"
        "@a[3][-2] = 3+3; @a;"
        "@...10;"
        "@(); @(1,);"
        "@(a=(+1,+2,+3)+(+4,+5,+6));"

    ) == 
        "'(<10>, (-5, ), (), (<30>, 4, (<10>, (), ), ), )' '4' '1' '0' '1' '(-5, )' '(<10>, (), )' '6' '(<10>, (-5, ), (), (<30>, 6, (<10>, (), ), ), )' '(<100>, )' '()' '(<10>, )' '(1, 2, 3, 4, 5, 6, )'"s
    );

    assert(test(numbers + 
        "@immutable imm = (+1, +2, +3);"
        "imm[1]=6;"
    ) == 
        "'(1, 2, 3, )' Exec: Modifying immutable value \"imm\""s
    );

    assert(test(numbers +       // Unpacking
        "mutable a; mutable b; mutable c=+10;"
        "(a, b) = (+1, +2); @a;@b;@c;"
        "(a, b) = (b, a); @a;@b;@c;"
        "(a, b) = (+1, +2, +3); @(a,b,c);"
        "(a, b, c) = (+5, +6); @(a,b,c);"
        "(a, b, c) = +1; @(a,b,c);"
        ) == 
        "'1' '2' '10' '2' '1' '10' '(1, 2, 10, )' '(5, 6, ?, )' '(1, ?, ?, )'"s
    );

    assert(test(numbers + 
        "@mutable a=+2; @b=+3;"
        "(a, a) = (+4, +5); @a; @b;"
        "(a, b) = (+6, +7); @a; @b;"
    ) == 
        "'2' '3' '5' '3' Exec: Modifying immutable value \"b\""s
    );

    assert(test(numbers + 
        "immutable Y=10*8+9;immutable N=10*7+8;"
        "@if 1!=1: @+1 else: @+3;"
        "@a = if 0: +1;"
        "b= if 1: { @+2; 1+1; } else: { @1; 3+4; }; @b; @=>b;"
        "@((c = (if 1==1: 1)+2)+2);"
    ) == 
        "'3' '3' '?' '<157>' '2' '2' '5'"s
    );

    assert(test(numbers + 
        "@=>{ @+1; return if a; @+2; }; @=>{ @+1; return if !a; @+2; };"
        "@=>{ return +1 if !a; }; @=>{ +6; return +8 if a; };"
        "@=>{ return ; return +1; }; @=>{ return +1; return ; };"
        "@?beforeReturn; return; @?afterReturn; "
    ) == 
        "'1' '2' '2' '1' '1' '1' '8' '0' '1' beforeReturn"s
    );

    assert(test(numbers +                         // Циклы
        "mutable i=0;"
        "while { "
        "    i=++i;"
        "    if i%2 : @i else: @(i,);"
        "    return 1 if i == 3;\n"           // continue
        "    @? ret1;"
        "    return 0 if i > 7;"            // break
        "    @? ret2;"
        "    i<10;"
        "}; @i;"
    ) == 
        "'1' ret1 ret2 '(2, )' ret1 ret2 '3' '(4, )' ret1 ret2 '5' ret1 ret2 '(6, )' ret1 ret2 '7' ret1 ret2 '(8, )' ret1 '8'"s
    );

    assert(test(numbers +                         // if-else
        "immutable Y=10*8+9;immutable N=10*7+8;"
        "mutable a;"
        "@@@ (if (a,2) : Y else: N) + 0 ; "
        "@@@ (if () : Y else: N) + 0 ; "
        "a=(1,);"
        "@@@ (if a : Y else: N) + 0 ; "
        "a=();"
        "@@@ (if a : Y else: N) + 0 ; "
        "@@@ (if cffctf : Y else: N) + 0 ; "
    ) == 
        "YNYNN"s
    );

    assert(test(numbers +                         // complex
        "baseFactory = inner {"
        "    (base, ) = arguments;"
        "    0 = outer 0;"
        "    {"
        "        (digits = arguments);"
        "        mutable i = digits=>length;"
        "        mutable sum = 0;"
        "        mutable multiplyer = ++0;"
        "        while {"
        "            i = --i;"
        "            return 0 if i<0;"
        "            sum = sum + digits[i] * multiplyer;"
        "            multiplyer = multiplyer * base;" 
        "        };"
        "        sum;"
        "    };"
        "};"
        "base2  = (++1)=>baseFactory;"
        "base10 = ((1,0,1,0)=>base2)=>baseFactory;"
        "@(1,0,1,1)=>base2;"
        "@(1,2,9)=>base10;"
    ) == 
        "'11' '129'"s
    );


}
/*

'<30>' '?' '<848>' '3'
'1' '3' '5' '6'
'0' '1' '2' '3' '4' '5' '6' '7' '8' '9' '10'
'10' '9' '8' '7' '6' '5' '4' '3' '2' '1' '0' '0'
YNYNN'(<10>, <20>, <30>, <40>, <50>, <60>, )'
'11' '119'
'1' '0' '0'
Have a nice day !
*/