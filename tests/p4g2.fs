start
let a : 0
let b : 1
let c : 2
let d : 3
let e : 4
let f : 5
main {
    if [ 1 == 2 ] then {
        printf [ a ] .
    } .
    if [ 1 == 1 ] then {
        printf [ b ] .
    } .
    if [ 2 =< 1 ] then {
        printf [ c ] .
    } .
    if [ 2 => 1 ] then {
        printf [ d ] .
    } .
    if [ 2 == 1 ] then {
        printf [ e ] .
    } .
    if [ 2 : : 1 ] then {
        printf [ f ] .
    } .
} stop